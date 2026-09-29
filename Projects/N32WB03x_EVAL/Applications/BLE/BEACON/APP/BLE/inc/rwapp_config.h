/**
 * @file rwapp_config.h
 * @brief Required per-project BLE profile-feature-flag config -- every
 * N32WB03x_SDK BLE project supplies its own copy of this file (included by
 * ns_ble_stack/modules/rwip/api/rwip_config.h). No profiles are registered
 * by this project (see app_ble.c), so every flag below resolves to 0.
 */

#ifndef RWAPP_CONFIG_H
#define RWAPP_CONFIG_H

#include "app_user_config.h"
#include "rwip_config.h"

#if defined(CFG_APP_PRF)
#define BLE_APP_PRF          1
#else
#define BLE_APP_PRF          0
#endif

#if defined(CFG_APP_HT)
#define BLE_APP_HT           1
#else
#define BLE_APP_HT           0
#endif

#if defined(CFG_APP_HID)
#define BLE_APP_HID          1
#else
#define BLE_APP_HID          0
#endif

#if defined(CFG_APP_DIS)
#define BLE_APP_DIS          1
#else
#define BLE_APP_DIS          0
#endif

#if (CFG_APP_BATT)
#define BLE_APP_BATT         1
#else
#define BLE_APP_BATT         0
#endif

#if (defined(CFG_APP_SEC) || BLE_APP_HID)
#define BLE_APP_SEC          1
#else
#define BLE_APP_SEC          0
#endif

#define AM0_APP_OPTIONAL_CHARACTERISTICS        0

#endif /* RWAPP_CONFIG_H */
