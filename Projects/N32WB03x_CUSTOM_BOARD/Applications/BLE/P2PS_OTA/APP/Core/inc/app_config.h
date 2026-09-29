/**
 * @file app_config.h
 * @brief Shared sequencer task/priority IDs for P2PS_OTA/APP.
 *
 * Centralized enum pattern confirmed against STM32CubeWL's
 * LoRaWAN_End_Node reference (Core/Inc/utilities_def.h: CFG_SEQ_Prio_Id_t /
 * CFG_SEQ_Task_Id_t) -- plain bit-position enums, shifted explicitly by
 * callers via (1U << TASK_xxx). Same structural pattern already used by
 * this repo's other projects (app_ble_lorawan_wmbus/app_ble_cellular_wmbus/
 * app_ble_oms_wmbus's app_config.h SeqTaskId_t/LpmVoterId_t), naming kept
 * consistent with those rather than STM's literal names.
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/** Sequencer task priority levels (must stay <= UTIL_SEQ_CONF_PRIO_NBR, see
 * utilities_conf.h -- already 2, confirmed). CFG_SEQ_Prio_0 is dispatched
 * before CFG_SEQ_Prio_1 within the same UTIL_SEQ_Run() pass. */
typedef enum
{
    CFG_SEQ_Prio_0,
    CFG_SEQ_Prio_1,
    CFG_SEQ_Prio_NBR
} CFG_SEQ_Prio_Id_t;

/** Sequencer task IDs -- bit position, used as (1U << TASK_xxx). */
typedef enum
{
    TASK_PERIODIC_ID,
    TASK_PULSE_COUNTER_ID,
    TASK_WAKE_WINDOW_ID,
    TASK_BLE_ADV_RESTART,        /* app_ble.c -- restarts advertising outside BLE kernel-callback context after a disconnect (see APP_BLE_MsgHandler()); must keep a lower bit than TASK_BLE_STACK_ID so it dispatches first within the same CFG_SEQ_Prio_1 pass -- matches app_ble_cellular_wmbus bug 29 */
    TASK_BLE_SESSION_TIMEOUT,    /* app_ble.c -- forces a disconnect after BLE_SESSION_TIMEOUT_SEC (user_config.h) of an open connection */
    TASK_P2PS_TIME_SET,          /* app_p2ps.c -- applies a mobile-provided epoch to the RTC, then notifies */
    TASK_P2PS_NOTIFY,            /* app_p2ps.c -- drains the queued P2PS notify buffer outside kernel context */
    TASK_P2PS_CFG_WRITE,         /* app_p2ps.c -- validates + saves a pending user_config_t write */
    TASK_FUOTA_WORK,             /* app_fuota.c -- deferred flash erase/write/finish, outside kernel context */
    TASK_BLE_STACK_ID,           /* app_ble.c -- BLE pump (rwip_schedule), CFG_SEQ_Prio_1, self-perpetuating */
    TASK_NS_LOG_DRAIN_ID,        /* ns_log_if.c -- drains one deferred NS_LOG_x message per pump cycle into APP_LOG(), spreading the BLE SDK's own bursty NS_LOG_DEBUG sequences out instead of contending for the shared trace FIFO all at once (see ns_log_if.h) */
    TASK_NBR
} SeqTaskId_t;

/**
 * @brief Low power manager voter IDs -- bit position, used as
 * (1U << CFG_LPM_xxx_Id) with UTIL_LPM_SetStopMode()/SetOffMode().
 * Pattern matches the other projects' app_config.h LpmVoterId_t (see
 * CLAUDE.md "LPM Voter Bits").
 */
typedef enum
{
    CFG_LPM_APPLI_Id,       /* app_entry.c -- permanent OffMode block, and StopMode block under LOW_POWER_DISABLE */
    CFG_LPM_UART_TX_Id,     /* trace UART TX -- blocks StopMode while bytes are in flight */
    CFG_LPM_WAKE_BUTTON_Id, /* PB3 wake button / boot -- blocks StopMode for a 60 s reflash window, see application.c */
    CFG_LPM_BLE_Id,         /* app_ble.c -- blocks StopMode during FAST_ADV/CONNECTED; StopMode only entered when rwip_sleep() agrees, see APP_BLE_ProcessTask() */
    CFG_LPM_FUOTA_Id,       /* app_fuota.c -- blocks StopMode for the duration of an active FUOTA transfer (write-without-response has no retransmit) */
    CFG_LPM_NBR
} LpmVoterId_t;

/**
 * @brief 0 = LPM active (StopMode allowed once no voter blocks it); 1 =
 * StopMode permanently blocked -- full RUN-mode development/flashing build.
 * Matches STM32CubeExpansion_LRWAN's sys_conf.h LOW_POWER_DISABLE pattern --
 * the STM32 reference whose flat (non-tiered) PWR modes actually match this
 * chip's PWR_EnterIDLE/SLEEP/PDMode() shape (see CLAUDE.md LPM section for
 * why STM32CubeWBA's multi-level CFG_LPM_LEVEL/STOP1/STOP2/STANDBY_SUPPORTED
 * scheme was not adopted instead -- N32WB031 has no matching sub-tiers).
 * Unaffected by BLE -- PWR_EnterSLEEPMode() stays the sole StopMode
 * dispatch, BLE-safe or not, see lpm_if.c.
 */
#ifndef LOW_POWER_DISABLE
#define LOW_POWER_DISABLE  0
#endif

#ifdef __cplusplus
}
#endif

#endif /* APP_CONFIG_H */
