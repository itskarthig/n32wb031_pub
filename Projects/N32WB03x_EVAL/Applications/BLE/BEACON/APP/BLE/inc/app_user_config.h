/**
 * @file app_user_config.h
 * @brief BLE bring-up config for BEACON/APP -- device identity,
 * advertising timing, and security baseline for ns_ble_gap_init()/
 * ns_ble_adv_init()/ns_sec_init() (see app_ble.c).
 *
 * File name/shape matches the N32 SDK's own per-project convention (every
 * ble/* example carries an inc/app_user_config.h alongside app_ble.c) --
 * independently written for this project, not copied from any app_* project.
 */

#ifndef APP_USER_CONFIG_H
#define APP_USER_CONFIG_H

#include "ns_adv_data_def.h"

/* -------------------------------------------------------------------------
 * Device identity
 * ------------------------------------------------------------------------- */
#define BLE_DEVICE_NAME                     "N32_BEACON"

/* Enables BLE_APP_DIS in rwapp_config.h -- registers the Device Information
 * Service (see app_dis.c/.h). CFG_PRF_DISS is a separate, lower-level gate
 * (BLE_DIS_SERVER in the SDK's own rwprf_config.h) that diss.c/diss_task.c
 * themselves require to compile -- both must be defined, matching the SDK's
 * own ble/dis example's app_user_config.h. */
#define CFG_APP_DIS
#define CFG_PRF_DISS

/* -------------------------------------------------------------------------
 * Connectable vs. beacon-only -- single-field toggle, confirmed by diffing
 * the SDK's own ble/beacon (beacon_enable=true) vs ble/rdtss (unset/false,
 * attach_name=true) app_ble_adv_init(). 1 = connectable/general-discoverable
 * (default -- matches the "OTA" in the project name; no GATT profile is
 * added by this task either way, this only sets the GAPM advertising
 * property flags). 0 = pure non-connectable broadcast beacon.
 * ------------------------------------------------------------------------- */
#ifndef BLE_ADV_CONNECTABLE
#define BLE_ADV_CONNECTABLE                 1
#endif

/* -------------------------------------------------------------------------
 * Advertising timing -- ns_library's own built-in fast->slow FSM
 * (ns_ble_adv_init()'s adv_time_t fields), controller-duration-driven, no
 * app-level timer needed. Units per struct adv_time_t (ns_ble.h): adv_intv
 * in 625 us units, duration in 10 ms units (0 = advertise forever).
 * ------------------------------------------------------------------------- */
#define BLE_ADV_FAST_INTERVAL                160     /* 100 ms */
#define BLE_ADV_FAST_DURATION_SEC            60      /* fast ADV window */

#define BLE_ADV_SLOW_INTERVAL                8000    /* 5 s */
#define BLE_ADV_SLOW_DURATION_SEC            0       /* forever */

/* -------------------------------------------------------------------------
 * BLE radio TX power -- ns_ble_radio_power_set() (ns_ble.h), rf_tx_power_t
 * (ns_ble_stack/stack_common/global_var.h). Available values (enum's own
 * inline comments, note two are mislabeled vs. the SDK's own doc comment
 * for ns_ble_radio_power_set -- Neg15_DBM is actually -12 dBm and
 * Neg20_DBM is actually -16 dBm):
 *   TX_POWER_Neg20_DBM, TX_POWER_Neg15_DBM, TX_POWER_Neg8_DBM,
 *   TX_POWER_Neg4_DBM, TX_POWER_Neg2_DBM, TX_POWER_0_DBM (default),
 *   TX_POWER_Pos2_DBM, TX_POWER_Pos3_DBM, TX_POWER_Pos4_DBM,
 *   TX_POWER_Pos6_DBM (max)
 * ------------------------------------------------------------------------- */
#define BLE_TX_POWER                         TX_POWER_0_DBM

/* Must NOT include an AD Type Flags structure (0x01) -- GAPM auto-generates
 * and reserves that field itself (app_set_adv_data()'s own comment: "GAP
 * will use 3 bytes for the AD Type"); an app-supplied one is rejected with
 * GAP_ERR_ADV_DATA_INVALID (0x4a), confirmed on hardware. The SDK's own
 * connectable ble/rdtss example's advertise data has no Flags AD type
 * either, for the same reason. Empty is valid -- the device name still
 * reaches the scan response via BLE_ADV_CONNECTABLE's attach_name path. */
#define BLE_ADV_DATA ""

#define BLE_ADV_DATA_LEN (sizeof(BLE_ADV_DATA)-1)

#define BLE_ADV_SCAN_RSP_DATA                ""
#define BLE_ADV_SCAN_RSP_DATA_LEN            (sizeof(BLE_ADV_SCAN_RSP_DATA)-1)

/* -------------------------------------------------------------------------
 * Connection parameters (only meaningful when BLE_ADV_CONNECTABLE=1)
 * ------------------------------------------------------------------------- */
#define BLE_MIN_CONN_INTERVAL_MS             15
#define BLE_MAX_CONN_INTERVAL_MS             30
#define BLE_SLAVE_LATENCY                    0
#define BLE_CONN_SUP_TIMEOUT_MS               5000
#define BLE_FIRST_CONN_PARAMS_UPDATE_DELAY_MS 5000

/* -------------------------------------------------------------------------
 * Security -- minimal/no-bonding baseline. No profile/pairing UI is added
 * by this task; ns_sec_init() is still called unconditionally, matching
 * every N32 SDK BLE example (confirmed: ble/beacon and ble/rdtss both call
 * it regardless of connectable/beacon-only mode).
 * ------------------------------------------------------------------------- */
#define BLE_SEC_IO_CAPABILITIES               GAP_IO_CAP_NO_INPUT_NO_OUTPUT
#define BLE_SEC_OOB                           0
#define BLE_SEC_KEY_SIZE                      16
/* ns_sec.c references this exact literal macro name directly (not just via
 * the struct field populated in app_ble.c) -- confirmed by a real link/
 * compile error, not a style choice. Must stay defined under this name. */
#define SEC_PARAM_KEY_SIZE                    BLE_SEC_KEY_SIZE
#define BLE_SEC_BOND                          0
#define BLE_SEC_MITM                          0
#define BLE_SEC_LESC                          0
#define BLE_SEC_KEYPRESS                      0
#define BLE_SEC_IKEY_DIST                     GAP_KDIST_NONE
#define BLE_SEC_RKEY_DIST                     GAP_KDIST_NONE
#define BLE_SEC_REQ_LEVEL                     GAP_NO_SEC

#define BLE_BOND_STORE_ENABLE                 0
#define BLE_BOND_DATA_BASE_ADDR               0x0103B000U /* inert while BLE_BOND_STORE_ENABLE=0 */
#define BLE_MAX_BOND_PEER                     1

#endif /* APP_USER_CONFIG_H */
