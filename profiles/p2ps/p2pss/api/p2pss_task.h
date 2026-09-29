#ifndef __P2PSS_TASK_H__
#define __P2PSS_TASK_H__

#include "rwip_config.h"

#if (BLE_APP_P2PS)

#include "prf_types.h"
#include "prf.h"
#include "attm.h"

/* Task ID for P2P Server — defined here so rwip_task.h (SDK) stays unmodified.
 * Free range is 77-199. Update this value if a future SDK version claims 77. */
#define TASK_ID_P2PS  77

struct p2ps_db_cfg
{
    uint8_t  max_nb_att;
    uint8_t *att_tbl;
    uint8_t *cfg_flag;
    uint16_t features;
};

enum
{
    P2PS_CREATE_DB_REQ = TASK_FIRST_MSG(TASK_ID_P2PS),
    P2PS_VALUE_REQ_IND,
    P2PS_VAL_WRITE_IND,
    P2PS_ATT_INFO_REQ,
    P2PS_DISCONNECT,
};

enum p2ps_state
{
    P2PS_IDLE,
    P2PS_BUSY,
    P2PS_STATE_MAX,
};

struct p2ps_val_write_ind
{
    uint8_t  conidx;
    uint16_t handle;
    uint16_t length;
    uint8_t  value[__ARRAY_EMPTY];
};

struct p2ps_value_req_ind
{
    uint8_t  conidx;
    uint16_t att_idx;
};

struct p2ps_att_info_req
{
    uint8_t  conidx;
    uint16_t att_idx;
};

struct p2ps_disconnect
{
    uint8_t conidx;
};

extern ke_task_id_t ns_ble_p2ps_task;

void p2ps_set_ccc_value(uint8_t conidx, uint8_t att_idx, uint16_t ccc);
void p2ps_init_ccc_values(const struct attm_desc_128 *att_db, int max_nb_att);

#endif /* BLE_APP_P2PS */
#endif /* __P2PSS_TASK_H__ */
