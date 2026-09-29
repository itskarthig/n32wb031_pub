#include "app_entry.h"
#include "n32wb03x.h"
#include "app_log.h"
#include "n32_seq.h"
#include "rtc.h"
#include "n32_systime.h"
#include "n32_timer.h"
#include "application.h"
#include "gpio.h"
#include "n32_lpm.h"
#include "app_config.h"
#include "app_ble.h"
#include "nvm_ram.h"

void APP_Init(void)
{
   /* SystemInit() (called from Reset_Handler before main()) unconditionally
      * sets PWR->VTOR_REG = 0x81000000, pointing the active interrupt-vector
      * table at P2PS_OTA_CB_INSTALLER's base (0x01000000) -- since installer
      * hands off to this APP via a plain function-pointer call (no CPU
      * reset), that stale value is still in effect here. Re-point it at this
      * image's own vector table (0x01004000) so IRQs (e.g. USART1 from
      * trace_if.c) resolve to our handlers instead of the installer's
      * Default_Handler stubs. Encoding confirmed against N32WB03x_SDK's own
      * peripheral_alone example (PWR->VTOR_REG = 0xA0000000 for RUN_FROM_RAM
      * == 0x80000000 enable bit | RAM base 0x20000000). */
     PWR->VTOR_REG = 0x81004000U;

     //Initilize RTC
     APP_RTC_Init();

     /* .noinit-resident persistent state (e.g. pulse_count, and -- since
      * BEACON/APP Bug #20 -- systime_if.c's SysTimeSet() delta) -- validates/
      * zero-fills on cold boot only, leaves a warm-reset value untouched.
      * MUST run before anything could read OR write NvmRam_Get() -- moved
      * ahead of the SysTimeGet() call below (previously came after it).
      * Hardware-confirmed real bug from the old ordering: SysTimeGet() ->
      * BKUPRead_Seconds()/BKUPRead_SubSeconds() read NvmRam_Get()'s
      * systime_delta_sec/systime_delta_subsec_ms fields directly -- with
      * NvmRam_Init() not yet run, those fields held raw uninitialized
      * .noinit content on the very first read of a genuine cold boot,
      * producing a wildly garbled epoch (huge nonsensical seconds +
      * out-of-0-999-range sub-second ms, a different garbage value each
      * time) on this one log line only -- self-correcting on every
      * subsequent SysTimeGet() call once NvmRam_Init() had run. Harmless
      * under the pre-Bug-#20 design, where these read callbacks were
      * hardcoded stubs always returning 0 regardless of NvmRam_Init()'s
      * state -- only became a real bug once they started genuinely
      * depending on it. */
     NvmRam_Init();

     // Initialize the hardware and peripherals
     UTIL_ADV_TRACE_Init();
     UTIL_ADV_TRACE_SetVerboseLevel(VERBOSE_LEVEL);
     APP_LOG(TS_OFF, VLEVEL_M, "-- START P2PS APP OTA--\r\n");

     /* Proves the full n32_systime chain (systime_if -> rtc.c -> RTC hw)
      * compiles, links, and reads back a value -- calendar hasn't been set
      * yet (no APP_RTC_SetDateTime()/SysTimeSet() call), so this just
      * reflects the RTC's power-on-reset default until the app or a BLE/NTP
      * write sets a real date/time. */
     SysTime_t boot_time = SysTimeGet();
     APP_LOG(TS_OFF, VLEVEL_M, "[SYSTIME] epoch=%lu.%03dms\r\n", (unsigned long)boot_time.Seconds, boot_time.SubSeconds);
     APP_LOG(TS_OFF, VLEVEL_M, "[NVM] pulse_count=%lu\r\n", (unsigned long)NvmRam_Get()->pulse_count);

     UTIL_SEQ_Init();

     /* Registered as early as possible -- NS_LOG_x (ns_log_if.h) can fire
      * from any BLE-stack/SDK code from this point on, and each call
      * unconditionally arms TASK_NS_LOG_DRAIN_ID regardless of whether
      * it's registered yet (the push itself doesn't need the task to
      * exist -- only draining does, and messages simply queue up until
      * this line runs). */
     UTIL_SEQ_RegTask((1U << TASK_NS_LOG_DRAIN_ID), CFG_SEQ_Prio_1, NsLogIf_DrainTask);

     /* UTIL_TIMER_Init() requires APP_RTC_Init() (above) to have already
      * configured the RTC clock source/prescaler -- see timer_if.h. */
     UTIL_TIMER_Init();

     APP_GPIO_ButtonInit();
     APP_GPIO_WakeButtonInit();

     /* OffMode resets the MCU on wake (see lpm_if.c) -- permanently block it
      * so UTIL_LPM_EnterLowPower() (called from UTIL_SEQ_Idle() below) never
      * picks it by default. StopMode stays allowed unless a voter (e.g.
      * CFG_LPM_UART_TX_Id, see PreSendHook/PostSendHook below) blocks it. */
     UTIL_LPM_Init();
     UTIL_LPM_SetOffMode((1U << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);

#if (LOW_POWER_DISABLE == 1)
     /* Full RUN-mode development/flashing build -- StopMode never entered. */
     UTIL_LPM_SetStopMode((1U << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);
#endif

     UserApp_Init();

     /* CFG_LPM_BLE_Id needs no boot-time init -- an unset voter bit already
      * means "not blocking" (see n32_lpm.c), which is correct before any BLE
      * connection exists. APP_BLE_SetState() (app_ble.c) sets/clears it purely
      * on CONNECTED/DISCONNECTED from here on; see app_ble.c's file header for
      * why FAST_ADV/SLOW_ADV are deliberately not tracked at the voter level. */
     APP_BLE_Init();
}

void APP_Process(void)
{
    UTIL_SEQ_Run(UTIL_SEQ_DEFAULT);
}



void UTIL_SEQ_PreIdle( void )
{
    /* Unless specified by the application, there is nothing to be done */
    return;
}

void UTIL_SEQ_Idle( void )
{
    /* Reachable only before APP_BLE_Init() arms TASK_BLE_STACK_ID (app_ble.c)
     * -- once that self-perpetuating task is pending, UTIL_SEQ_Run() never
     * finds the sequencer empty again, and the sleep decision happens inside
     * APP_BLE_ProcessTask() instead (see app_ble.c's file header for why).
     * Harmless to keep calling here too either way. */
    UTIL_LPM_EnterLowPower();
}

/* UTIL_ADV_TRACE_PreSendHook()/PostSendHook() are weak in n32_adv_trace.c,
 * called from UTIL_ADV_TRACE_Send() around each async TX burst. Block STOP
 * for the full burst duration -- STOP gates the USART1 APB clock mid-byte
 * otherwise, corrupting the trace peripheral (see CLAUDE.md's sibling
 * projects' bugs 13/51/52 for the hardware-confirmed failure mode). */
void UTIL_ADV_TRACE_PreSendHook(void)
{
    UTIL_LPM_SetStopMode((1U << CFG_LPM_UART_TX_Id), UTIL_LPM_DISABLE);
}

void UTIL_ADV_TRACE_PostSendHook(void)
{
    UTIL_LPM_SetStopMode((1U << CFG_LPM_UART_TX_Id), UTIL_LPM_ENABLE);
}