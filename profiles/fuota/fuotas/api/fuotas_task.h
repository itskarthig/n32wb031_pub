#ifndef __FUOTAS_TASK_H__
#define __FUOTAS_TASK_H__

#include "rwip_config.h"

#if (BLE_APP_FUOTA)

#include "prf_types.h"
#include "prf.h"
#include "attm.h"

/* Task ID for FUOTA Server — defined here so rwip_task.h (SDK) stays unmodified.
 * Free range is 77-199. TASK_ID_P2PS=77. Update if a future SDK version claims 78. */
#define TASK_ID_FUOTA  78

struct fuotas_db_cfg
{
    uint8_t  max_nb_att;
    uint8_t *att_tbl;
    uint8_t *cfg_flag;
    uint16_t features;
};

enum
{
    FUOTA_CREATE_DB_REQ = TASK_FIRST_MSG(TASK_ID_FUOTA),
    FUOTA_VAL_WRITE_IND,
    FUOTA_ATT_INFO_REQ,
    FUOTA_DISCONNECT,
};

enum fuotas_state
{
    FUOTAS_IDLE,
    FUOTAS_BUSY,
    FUOTAS_STATE_MAX,
};

struct fuotas_val_write_ind
{
    uint8_t  conidx;
    uint16_t handle;    /* attribute index (not raw handle) */
    uint16_t length;
    uint8_t  value[__ARRAY_EMPTY];
};

struct fuotas_att_info_req
{
    uint8_t  conidx;
    uint16_t att_idx;
};

struct fuotas_disconnect
{
    uint8_t conidx;
};

extern ke_task_id_t ns_ble_fuotas_task;

void fuotas_set_ccc_value(uint8_t conidx, uint8_t att_idx, uint16_t ccc);
void fuotas_init_ccc_values(const struct attm_desc_128 *att_db, int max_nb_att);

#endif /* BLE_APP_FUOTA */
#endif /* __FUOTAS_TASK_H__ */
