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
#include "iwdg_if.h"
#include "app_ble.h"
#include "nvm_ram.h"

void APP_Init(void)
{
     /* Remember whether this reset came from the IWDG before anything can clear the flag (the vendor's
      * ns_peripheral_config() clears RCC_CTRLSTS' reset flags when BLE initialises). No-op unless IWDG_ENABLE. */
     IwdgIf_CaptureResetCause();

   /* SystemInit() (called from Reset_Handler before main()) unconditionally
      * sets PWR->VTOR_REG = 0x81000000, pointing the active interrupt-vector
      * table at BEACON_INSTALLER's base (0x01000000) -- since installer
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
     APP_LOG(TS_OFF, VLEVEL_M, "-- START BLE APP OTA--\r\n");

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
      * so UTIL_LPM_EnterLowPower() (called from APP_BLE_ProcessTask()'s sleep decision, app_ble.c) never
      * picks it by default. StopMode stays allowed unless a voter (e.g.
      * CFG_LPM_UART_TX_Id, see PreSendHook/PostSendHook below) blocks it. */
     UTIL_LPM_Init();
     UTIL_LPM_SetOffMode((1U << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);

#if (LOW_POWER_DISABLE == 1)
     /* Full RUN-mode development/flashing build -- StopMode never entered. */
     UTIL_LPM_SetStopMode((1U << CFG_LPM_APPLI_Id), UTIL_LPM_DISABLE);
#endif

     UserApp_Init();

     /* Hardware watchdog (IWDG_ENABLE, default 0): after UserApp_Init() and before APP_BLE_Init(). Fed from the
      * BLE pump (app_ble.c). */
     IwdgIf_Init();

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
    /* Unreachable once APP_BLE_Init() has armed TASK_BLE_STACK_ID (app_ble.c):
     * that self-perpetuating task is always pending again when UTIL_SEQ_Run()
     * re-reads the task set, so the sequencer never finds itself empty, and the
     * sleep decision lives in APP_BLE_ProcessTask() (see app_ble.c's file header).
     *
     * It must NOT call UTIL_LPM_EnterLowPower() (it used to): that would enter
     * LpmIf_EnterStopMode() without rwip_sleep()'s agreement, i.e. with the BLE
     * baseband never told to sleep, and the unbounded `while (BLE_OSC_EN)` wait
     * would then spin forever under PRIMASK=1 -- a total freeze at ~1.2 mA
     * (CLAUDE.md LORAWAN_WMBUS_AMR, sleep-path audit 2026-11-10, F5.2). A plain
     * WFI is the only thing that is ever safe here (PRIMASK is 1: it wakes on
     * any pending IRQ and the sequencer then re-evaluates). */
    __WFI();
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