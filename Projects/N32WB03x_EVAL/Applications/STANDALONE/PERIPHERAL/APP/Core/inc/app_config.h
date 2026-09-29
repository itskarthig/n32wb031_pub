/**
 * @file app_config.h
 * @brief Shared sequencer task/priority IDs for PERIPHERAL/APP.
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

/** Sequencer task priority levels (must stay <= UTIL_SEQ_CONF_PRIO_NBR, see utilities_conf.h). */
typedef enum
{
    CFG_SEQ_Prio_0,
    CFG_SEQ_Prio_NBR
} CFG_SEQ_Prio_Id_t;

/** Sequencer task IDs -- bit position, used as (1U << TASK_xxx). */
typedef enum
{
    TASK_PERIODIC_ID,
    TASK_PULSE_COUNTER_ID,
    TASK_WAKE_WINDOW_ID,
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
 */
#ifndef LOW_POWER_DISABLE
#define LOW_POWER_DISABLE  0
#endif

#ifdef __cplusplus
}
#endif

#endif /* APP_CONFIG_H */
