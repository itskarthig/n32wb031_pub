/*
 * @file application.c
 * @brief Application entry point implementation.
 */

#include "application.h"
#include "app_log.h"
#include "n32_seq.h"
#include "n32_timer.h"
#include "n32_systime.h"
#include "n32_lpm.h"
#include "app_config.h"
#include "adc.h"
#include "nvm_ram.h"

 /* Periodic task -- proves UTIL_SEQ_Init -> RegTask -> SetTask -> Run (driven
 * every APP_Process() iteration) actually executes a registered task.
 * TASK_PERIODIC_ID / CFG_SEQ_Prio_0 come from app_config.h (shared task/priority
 * enum, STM32 utilities_def.h pattern). */

/* Fires every 5 s via s_periodic_timer (see OnPeriodicTimerEvent) -- proves the full
 * n32_timer chain (timer_if -> RTC WUT) is actually driving the sequencer,
 * not just linking. */
#define TASK_PERIODIC_PERIOD_MS (60000U)

/* Wake-window: how long StopMode stays blocked after boot or a PB3 press,
 * giving a guaranteed RUN-mode reflash opportunity (N32WB031 has no
 * DBGMCU-style "keep debug alive during STOP" register -- see CLAUDE.md
 * LPM section). */
#define WAKE_WINDOW_MS (20000U)

static void APP_PeriodicTask(void);
static void OnPeriodicTimerEvent(void *context);
static void UserApp_PulseCounter(void);
static void WakeWindow_Start(void);
static void OnWakeWindowExpiry(void *context);
static void UserApp_WakeWindowTask(void);

static UTIL_TIMER_Object_t s_periodic_timer;
static UTIL_TIMER_Object_t s_wake_window_timer;

 void UserApp_Init(void){

    UTIL_SEQ_RegTask((1U << TASK_PERIODIC_ID), CFG_SEQ_Prio_0, APP_PeriodicTask);
    UTIL_SEQ_RegTask((1U << TASK_PULSE_COUNTER_ID), CFG_SEQ_Prio_0, UserApp_PulseCounter);
    UTIL_SEQ_RegTask((1U << TASK_WAKE_WINDOW_ID), CFG_SEQ_Prio_0, UserApp_WakeWindowTask);
    UTIL_TIMER_Create(&s_periodic_timer, TASK_PERIODIC_PERIOD_MS, UTIL_TIMER_ONESHOT, OnPeriodicTimerEvent, NULL);
     UTIL_TIMER_Start(&s_periodic_timer);
    UTIL_TIMER_Create(&s_wake_window_timer, WAKE_WINDOW_MS, UTIL_TIMER_ONESHOT, OnWakeWindowExpiry, NULL);

    /* Unconditional -- every boot/reset gets the same guaranteed 60 s
     * RUN window before LPM can first engage, not just a PB3 press. */
    WakeWindow_Start();
 }


 static void APP_PeriodicTask(void)
{
    APP_LOG(TS_OFF, VLEVEL_M, "[APP_PeriodicTask] periodic task running\r\n");
    SysTime_t boot_time = SysTimeGet();
     APP_LOG(TS_OFF, VLEVEL_M, "[APP_PeriodicTask] epoch=%lu.%03dms\r\n", (unsigned long)boot_time.Seconds, boot_time.SubSeconds);

    int32_t vref_mv = APP_ADC_ReadVrefMv();
    APP_LOG(TS_OFF, VLEVEL_M, "[APP_PeriodicTask] vref=%ldmV\r\n", (long)vref_mv);

    int32_t temp_c = APP_ADC_ReadTemperatureC();
    APP_LOG(TS_OFF, VLEVEL_M, "[APP_PeriodicTask] temp=%ldC\r\n", (long)temp_c);
}

/**
 * @brief 5 s timer callback -- STM32 LoRaWAN_End_Node's OnTxTimerEvent
 * convention (lora_app.c): create UTIL_TIMER_ONESHOT, then re-arm from
 * inside the callback itself (rather than UTIL_TIMER_PERIODIC) so the
 * period could be changed dynamically on a later fire if ever needed.
 */
static void OnPeriodicTimerEvent(void *context)
{
    (void)context;

    UTIL_SEQ_SetTask((1U << TASK_PERIODIC_ID), CFG_SEQ_Prio_0);

    /* Wait for next slot */
    UTIL_TIMER_Start(&s_periodic_timer);
}

/**
 * @brief PB1 button-press handler -- scheduled (not called directly) from
 * EXTI0_1_IRQHandler via UTIL_SEQ_SetTask(TASK_PULSE_COUNTER_ID), matching
 * STM32CubeWL's HAL_GPIO_EXTI_Callback -> UTIL_SEQ_SetTask convention so the
 * increment/print run in task context, not interrupt context.
 *
 * pulse_count lives in .noinit RAM (nvm_ram.c) so it survives a warm reset
 * instead of restarting at 0/1 on every NVIC_SystemReset().
 */
static void UserApp_PulseCounter(void)
{
    NvmRam_t *nvm = NvmRam_Get();
    nvm->pulse_count++;
    APP_LOG(TS_OFF, VLEVEL_M, "[PULSE] count=%lu\r\n", (unsigned long)nvm->pulse_count);
}

/**
 * @brief Blocks StopMode via CFG_LPM_WAKE_BUTTON_Id and (re)starts the 60 s
 * wake-window timer. Called unconditionally at boot and again from
 * UserApp_WakeWindowTask() on every PB3 press -- UTIL_TIMER_Start() on an
 * already-running UTIL_TIMER_ONESHOT restarts it from zero, so a press
 * mid-window simply extends the awake period rather than stacking timers.
 */
static void WakeWindow_Start(void)
{
    UTIL_LPM_SetStopMode((1U << CFG_LPM_WAKE_BUTTON_Id), UTIL_LPM_DISABLE);
    UTIL_TIMER_Start(&s_wake_window_timer);
    APP_LOG(TS_OFF, VLEVEL_M, "[LPM] wake window started (%lu ms)\r\n", (unsigned long)WAKE_WINDOW_MS);
}

static void OnWakeWindowExpiry(void *context)
{
    (void)context;
    UTIL_LPM_SetStopMode((1U << CFG_LPM_WAKE_BUTTON_Id), UTIL_LPM_ENABLE);
    APP_LOG(TS_OFF, VLEVEL_M, "[LPM] wake window expired -- StopMode allowed\r\n");
}

/**
 * @brief PB3 wake-button handler -- scheduled from EXTI2_3_IRQHandler via
 * UTIL_SEQ_SetTask(TASK_WAKE_WINDOW_ID), same ISR-defers-to-task convention
 * as UserApp_PulseCounter() above.
 */
static void UserApp_WakeWindowTask(void)
{
    WakeWindow_Start();
}