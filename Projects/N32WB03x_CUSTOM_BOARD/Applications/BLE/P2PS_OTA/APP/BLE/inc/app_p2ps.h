/**
 * @file app_p2ps.h
 * @brief Peer-to-Peer Server (P2PS) application layer -- public API.
 *
 * Ported from app_ble_cellular_wmbus's app_p2ps.c/.h, restructured against
 * P2PS_OTA's own minimal user_config_t (see user_config.h) -- see app_p2ps.c
 * for exactly what was kept, dropped, and rewritten. Renamed to this
 * project's APP_<MODULE>_<Verb> PascalCase convention (matches app_dis.c).
 *
 * Service UUID:   0000AD40-CC7A-482A-984A-0A0D0A060700
 *   Write char:   0000AD42-CC7A-482A-984A-0A0D0A060700  (client writes, no response)
 *   Notify char:  0000AD43-CC7A-482A-984A-0A0D0A060700  (MCU pushes to client)
 */

#ifndef _APP_P2PS_H_
#define _APP_P2PS_H_

#include "rwip_config.h"

#if (BLE_APP_P2PS)

#include <stdint.h>
#include <stdbool.h>
#include "attm.h"
#include "ke_msg.h"
#include "ns_ble_task.h"
#include "app_config.h"

/* ---- 128-bit UUIDs (little-endian byte arrays for attm_desc_128) ---------- */

#define SERVICE_P2PS {0x00, 0x07, 0x06, 0x0A, 0x0D, 0x0A, 0x4A, 0x98, 0x2A, 0x48, 0x7A, 0xCC, 0x40, 0xAD, 0x00, 0x00}
#define CHAR_P2PS_WR {0x00, 0x07, 0x06, 0x0A, 0x0D, 0x0A, 0x4A, 0x98, 0x2A, 0x48, 0x7A, 0xCC, 0x42, 0xAD, 0x00, 0x00}
#define CHAR_P2PS_NTF {0x00, 0x07, 0x06, 0x0A, 0x0D, 0x0A, 0x4A, 0x98, 0x2A, 0x48, 0x7A, 0xCC, 0x43, 0xAD, 0x00, 0x00}

/* ---- Attribute index ------------------------------------------------------- */

enum
{
    P2PS_IDX_SVC,      /* Primary service declaration                          */
    P2PS_IDX_WR_CHAR,  /* Write-without-response characteristic declaration    */
    P2PS_IDX_WR_VAL,   /* Write value: PERM(WRITE_COMMAND,ENABLE), max 244 B   */
    P2PS_IDX_NTF_CHAR, /* Notify characteristic declaration                    */
    P2PS_IDX_NTF_VAL,  /* Notify value: PERM(NTF,ENABLE), max 244 bytes        */
    P2PS_IDX_NTF_CFG,  /* Client Characteristic Configuration descriptor       */
    P2PS_IDX_NB,       /* Total attribute count = 6                            */
};

/* ---- Sequencer tasks -------------------------------------------------------
 * Registered with UTIL_SEQ_RegTask() in UserApp_Init() (application.c). */

void APP_P2PS_TimeSetTask(void);   /* applies pending epoch to RTC, then notifies */
void APP_P2PS_NotifyTask(void);    /* drains the queued notify buffer            */
void APP_P2PS_CfgWriteTask(void);  /* validates + saves a pending config write   */

bool APP_P2PS_IsAuthenticated(void);

/* ---- External symbols (defined in app_p2ps.c) ----------------------------- */

extern struct attm_desc_128 p2ps_att_db[P2PS_IDX_NB];
extern const struct app_subtask_handlers p2ps_app_handlers;

/* ---- Public API ------------------------------------------------------------ */

/**
 * @brief Send a notification to a subscribed client.
 * @note  Call only after APP_BLE_OS_READY and when the client has subscribed
 *        (CCC notification enabled). Does nothing if not subscribed or busy.
 * @param data   Pointer to the notification payload.
 * @param length Number of bytes (max 244).
 */
void APP_P2PS_NotifySend(const uint8_t *data, uint16_t length);

/**
 * @brief Register and add the P2PS GATT service. Called via
 *        ns_ble_add_prf_func_register() in APP_BLE_Init().
 */
void APP_P2PS_AddP2ps(void);

/**
 * @brief Register the P2PS profile task and interface. Called internally
 *        from APP_P2PS_AddP2ps().
 */
void APP_P2PS_Init(void);

#endif /* BLE_APP_P2PS */
#endif /* _APP_P2PS_H_ */
