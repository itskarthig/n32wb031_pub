#ifndef __P2PSS_H__
#define __P2PSS_H__

#include "rwip_config.h"

#if (BLE_APP_P2PS)

#include <stdint.h>
#include "prf_types.h"
#include "prf.h"
#include "attm.h"

#define P2PS_IDX_MAX  (1)

struct p2ps_val_elmt
{
    struct co_list_hdr hdr;
    uint8_t  att_idx;
    uint8_t  length;
    uint8_t  data[__ARRAY_EMPTY];
};

struct p2ps_env_tag
{
    prf_env_t      prf_env;
    uint16_t       shdl;
    uint8_t        max_nb_att;
    struct ke_msg *operation;
    uint8_t        cursor;
    uint8_t        ccc_idx;
    struct co_list values;
    ke_state_t     state[P2PS_IDX_MAX];
};

const struct prf_task_cbs *p2ps_prf_itf_get(void);
void p2ps_task_init(struct ke_task_desc *p_task_desc);

#endif /* BLE_APP_P2PS */
#endif /* __P2PSS_H__ */
