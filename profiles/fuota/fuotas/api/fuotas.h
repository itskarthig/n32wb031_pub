#ifndef __FUOTAS_H__
#define __FUOTAS_H__

#include "rwip_config.h"

#if (BLE_APP_FUOTA)

#include <stdint.h>
#include "prf_types.h"
#include "prf.h"
#include "attm.h"

#define FUOTAS_IDX_MAX  (1)

struct fuotas_val_elmt
{
    struct co_list_hdr hdr;
    uint8_t  att_idx;
    uint8_t  length;
    uint8_t  data[__ARRAY_EMPTY];
};

struct fuotas_env_tag
{
    prf_env_t      prf_env;
    uint16_t       shdl;
    uint8_t        max_nb_att;
    struct ke_msg *operation;
    uint8_t        cursor;
    uint8_t        ccc_idx;
    struct co_list values;
    ke_state_t     state[FUOTAS_IDX_MAX];
};

const struct prf_task_cbs *fuotas_prf_itf_get(void);
void fuotas_task_init(struct ke_task_desc *p_task_desc);

#endif /* BLE_APP_FUOTA */
#endif /* __FUOTAS_H__ */
