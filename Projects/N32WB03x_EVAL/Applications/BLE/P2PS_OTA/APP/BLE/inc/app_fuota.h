/**
 * @file app_fuota.h
 * @brief FUOTA (Firmware Update Over The Air) application layer -- public API.
 *
 * Ported from app_ble_cellular_wmbus's app_fuota.c/.h essentially unchanged
 * -- already fully generic (no cellular/WMBus coupling beyond its intentional
 * dependency on app_p2ps.h's APP_P2PS_IsAuthenticated()). CLAUDE.md bugs #7
 * ("silently-incomplete image" drop-count guard) and #8 ("stale install flag"
 * clear-on-every-rejection/cancel/start) are already fixed in the source this
 * was ported from -- confirmed by reading app_fuota.c directly, not assumed.
 *
 * ST-inspired OTA service layout:
 *   Service UUID:        0000AD20-CC7A-482A-984A-0A0D0A060700
 *   Base Address (AD22): client writes action + offset + sectors (write-without-response)
 *   Confirmation (AD23): device indicates status to client (indicate)
 *   Raw Data (AD24):     client streams firmware chunks (write-without-response, max 240 B)
 *
 * Protocol flow:
 *   1. Client enables Confirmation indicate (CCC = 0x0002)
 *   2. Client writes AD22: action=0x02 (START App), address, sectors
 *   3. Device erases Download Slot, indicates AD23=0x02 (Ready)
 *   4. Client streams firmware via AD24 (240-byte chunks, multiple of 4)
 *   5. Client writes AD22: action=0x07 (File Upload Finish)
 *   6. Device verifies magic keyword, marks Download Slot (APP2) as pending install,
 *      indicates AD23=0x01 (Reboot)
 *   7. Device resets -- P2PS_OTA_INSTALLER copies APP2 -> APP1, then boots APP1
 */

#ifndef _APP_FUOTA_H_
#define _APP_FUOTA_H_

#include "rwip_config.h"

#if (BLE_APP_FUOTA)

#include <stdint.h>
#include "attm.h"
#include "ke_msg.h"
#include "ns_ble_task.h"
#include "app_config.h"

/* ---- 128-bit UUIDs (little-endian byte arrays for attm_desc_128) ---------- */

#define SERVICE_FUOTA {0x00, 0x07, 0x06, 0x0A, 0x0D, 0x0A, 0x4A, 0x98, 0x2A, 0x48, 0x7A, 0xCC, 0x20, 0xAD, 0x00, 0x00}
#define CHAR_FUOTA_BASE {0x00, 0x07, 0x06, 0x0A, 0x0D, 0x0A, 0x4A, 0x98, 0x2A, 0x48, 0x7A, 0xCC, 0x22, 0xAD, 0x00, 0x00}
#define CHAR_FUOTA_CFM {0x00, 0x07, 0x06, 0x0A, 0x0D, 0x0A, 0x4A, 0x98, 0x2A, 0x48, 0x7A, 0xCC, 0x23, 0xAD, 0x00, 0x00}
#define CHAR_FUOTA_DATA {0x00, 0x07, 0x06, 0x0A, 0x0D, 0x0A, 0x4A, 0x98, 0x2A, 0x48, 0x7A, 0xCC, 0x24, 0xAD, 0x00, 0x00}

/* ---- Attribute index ------------------------------------------------------- */

enum
{
    FUOTA_IDX_SVC,       /* Primary service declaration                         */
    FUOTA_IDX_BASE_CHAR, /* Base Address characteristic declaration             */
    FUOTA_IDX_BASE_VAL,  /* Base Address: 5 bytes (action+addr), write-without-
                          * response -- 9 bytes on newer mobile app builds,
                          * with a trailing little-endian uint32 declared file
                          * size (see app_fuota.c's START_APP handler)         */
    FUOTA_IDX_CFM_CHAR,  /* Confirmation characteristic declaration             */
    FUOTA_IDX_CFM_VAL,   /* Confirmation: 1 byte, indicate                      */
    FUOTA_IDX_CFM_CFG,   /* Client Characteristic Configuration descriptor      */
    FUOTA_IDX_DATA_CHAR, /* Raw Data characteristic declaration                 */
    FUOTA_IDX_DATA_VAL,  /* Raw Data: 240 bytes max, write-without-response     */
    FUOTA_IDX_NB,        /* Total attribute count = 8                           */
};

/* ---- Confirmation status codes (AD23) -------------------------------------- */

#define FUOTA_CFM_REBOOT 0x01 /**< Device is rebooting to activate new image */
#define FUOTA_CFM_READY 0x02  /**< Download Slot ready to receive data       */
#define FUOTA_CFM_ERROR 0x03  /**< Error (overflow, magic mismatch, etc.)    */

/* ---- Base Address action codes (AD22 byte[0]) ------------------------------ */

#define FUOTA_ACTION_STOP 0x00      /**< Stop all upload                           */
#define FUOTA_ACTION_START_APP 0x02 /**< Start Application File Upload             */
#define FUOTA_ACTION_EOF 0x06       /**< End Of File Transfer                      */
#define FUOTA_ACTION_FINISH 0x07    /**< File Upload Finish -- verify + activate   */
#define FUOTA_ACTION_CANCEL 0x08    /**< Cancel upload                             */

/* ---- Download Slot layout ------------------------------------------------- */
/* App always runs from APP1 (0x01004000).                                    */
/* APP2 (0x01040000) is the fixed Download Slot -- P2PS_OTA_INSTALLER copies  */
/* it to APP1 on next boot (STM32-style single-bank approach).                */

#define FUOTA_APP1_ADDR 0x01004000U /* Application Bank (always executes here) */
#define FUOTA_APP2_ADDR 0x01040000U /* Download Slot   (receives OTA image)    */
#define FUOTA_BANK_SIZE 0x0003C000U /* 240 KB                                  */

#define FUOTA_DL_ADDR FUOTA_APP2_ADDR /* fixed: app always runs from APP1 */

/* ---- Sequencer task ------------------------------------------------------- */

/**
 * @brief Deferred OTA work task. Handles erase, flash write, and
 *        transfer-finish in sequencer context. Registered with
 *        UTIL_SEQ_RegTask(1U << TASK_FUOTA_WORK, ...) in application.c.
 */
void APP_FUOTA_WorkTask(void);

/* ---- External symbols (defined in app_fuota.c) ---------------------------- */

extern struct attm_desc_128 fuota_att_db[FUOTA_IDX_NB];
extern const struct app_subtask_handlers fuota_app_handlers;

/* ---- Public API ------------------------------------------------------------ */

/**
 * @brief Send a Confirmation indication to the connected client.
 * @param status_code  FUOTA_CFM_READY / FUOTA_CFM_REBOOT / FUOTA_CFM_ERROR
 */
void APP_FUOTA_IndicateSend(uint8_t status_code);

/**
 * @brief Called by the shared profiles/fuota/fuotas/src/fuotas_task.c on
 *        GATTC_CMP_EVT for GATTC_INDICATE -- name is a hardcoded external
 *        reference from that shared, non-project-local file (confirmed by
 *        direct read), so it deliberately keeps the lowercase
 *        app_fuota_indicate_complete name rather than this file's own
 *        APP_FUOTA_* convention. Triggers NVIC_SystemReset() if a reboot
 *        was requested after the FUOTA_CFM_REBOOT indication.
 */
void app_fuota_indicate_complete(void);

/**
 * @brief Register and add the FUOTA GATT service. Called via
 *        ns_ble_add_prf_func_register() in APP_BLE_Init().
 */
void APP_FUOTA_AddFuota(void);

/**
 * @brief Register the FUOTA profile task and interface. Called internally
 *        from APP_FUOTA_AddFuota().
 */
void APP_FUOTA_Init(void);

#endif /* BLE_APP_FUOTA */
#endif /* _APP_FUOTA_H_ */
