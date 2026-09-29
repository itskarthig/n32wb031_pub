/**
 * @file app_dis.h
 * @brief Device Information Service (DIS, GATT UUID 0x180A) application
 * layer -- ported directly from the N32 SDK's own `ble/dis` example project
 * (`inc/app_profile/app_dis.h`), renamed to this project's `APP_DIS_*`
 * PascalCase convention (matches `APP_BLE_Init`/`APP_BLE_GapParamsInit`/...).
 *
 * Only the string characteristics are populated (Manufacturer Name, Model
 * Number, Serial Number, Hardware/Firmware/Software Revision) -- System ID,
 * IEEE Regulatory Cert list, and PnP ID are skipped (no natural value to
 * source them from for this project; the SDK reference only fills them in
 * for HID/USB-style vendor identification, not applicable here).
 */

#ifndef APP_DIS_H_
#define APP_DIS_H_

#include "rwip_config.h" /* BLE_APP_DIS -- set via rwapp_config.h/CFG_APP_DIS */

#if (BLE_APP_DIS)

#include <stdint.h>

/* -------------------------------------------------------------------------
 * DIS characteristic values -- edit these for the real product identity.
 * ------------------------------------------------------------------------- */

#define APP_DIS_MANUFACTURER_NAME      "Adarko"
#define APP_DIS_MANUFACTURER_NAME_LEN  (6)

#define APP_DIS_MODEL_NB_STR           "P2PS_OTA_CB"
#define APP_DIS_MODEL_NB_STR_LEN       (11)

#define APP_DIS_SERIAL_NB_STR          "0000000001"
#define APP_DIS_SERIAL_NB_STR_LEN      (10)

#define APP_DIS_HARD_REV_STR           "1.0.0"
#define APP_DIS_HARD_REV_STR_LEN       (5)

#define APP_DIS_FIRM_REV_STR           "1.0.0"
#define APP_DIS_FIRM_REV_STR_LEN       (5)

#define APP_DIS_SW_REV_STR             "1.0.0"
#define APP_DIS_SW_REV_STR_LEN         (5)

/** Feature bitmask passed to DISS -- string characteristics only, matching
 * the values actually populated above. Bits from diss_task.h. */
#define APP_DIS_FEATURES              (DIS_MANUFACTURER_NAME_CHAR_SUP_BIT | \
                                        DIS_MODEL_NB_STR_CHAR_SUP_BIT      | \
                                        DIS_SERIAL_NB_STR_CHAR_SUP_BIT     | \
                                        DIS_HARD_REV_STR_CHAR_SUP_BIT      | \
                                        DIS_FIRM_REV_STR_CHAR_SUP_BIT      | \
                                        DIS_SW_REV_STR_CHAR_SUP_BIT)

/** Message-handler table for DISS_VALUE_REQ_IND -- registered with the BLE
 * profile framework by APP_DIS_Init(). */
extern const struct app_subtask_handlers APP_DIS_Handlers;

/**
 * @brief Registers the DIS subtask handler table and profile-interface
 * getter with the BLE profile framework. Called from APP_DIS_AddDis(),
 * never called directly by app_ble.c.
 */
void APP_DIS_Init(void);

/**
 * @brief Adds a DIS instance to the GATT database. Registered via
 * ns_ble_add_prf_func_register() in APP_BLE_Init(), runs during BLE
 * profile-registration (after ns_ble_adv_init(), before ns_ble_adv_start()).
 */
void APP_DIS_AddDis(void);

#endif /* BLE_APP_DIS */

#endif /* APP_DIS_H_ */
