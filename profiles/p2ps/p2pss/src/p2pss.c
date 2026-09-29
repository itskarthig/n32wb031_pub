/**
 * @file p2pss.c
 * @brief P2P Server profile — service registration and lifecycle callbacks.
 */

#include "rwip_config.h"

#if (BLE_APP_P2PS)

#include "p2pss.h"
#include "p2pss_task.h"
#include "app_p2ps.h"

#include "prf_utils.h"
#include "prf.h"
#include "attm_db.h"
#include "ke_mem.h"
#include "att.h"
#include "co_utils.h"
#include "attm.h"
#include "ke_task.h"
#include "gapc.h"
#include "gapc_task.h"
#include "gattc_task.h"
#include "ns_log.h"

static uint8_t p2ps_init(struct prf_task_env *env, uint16_t *start_hdl,
                          uint16_t app_task, uint8_t sec_lvl,
                          struct p2ps_db_cfg *params)
{
    uint8_t  status   = ATT_ERR_NO_ERROR;
    uint8_t  uuid_128[16] = SERVICE_P2PS;
    uint32_t cfg_flag = ((1U << P2PS_IDX_NB) - 1U);

    status = attm_svc_create_db_128(
        start_hdl, uuid_128, (uint8_t *)&cfg_flag,
        P2PS_IDX_NB, NULL, env->task, p2ps_att_db,
        (sec_lvl & PERM_MASK_SVC_AUTH) | (sec_lvl & PERM_MASK_SVC_EKS) |
        PERM(SVC_SECONDARY, DISABLE) | PERM_VAL(SVC_UUID_LEN, 0x2));

    if (status == ATT_ERR_NO_ERROR)
    {
        struct p2ps_env_tag *p2ps_env =
            (struct p2ps_env_tag *)ke_malloc(sizeof(struct p2ps_env_tag), KE_MEM_ATT_DB);

        env->env = (prf_env_t *)p2ps_env;
        p2ps_env->shdl       = *start_hdl;
        p2ps_env->max_nb_att = P2PS_IDX_NB;
        p2ps_env->prf_env.app_task = app_task |
            (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        p2ps_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        env->id = TASK_ID_P2PS;
        p2ps_task_init(&(env->desc));
        co_list_init(&(p2ps_env->values));
        p2ps_init_ccc_values(p2ps_att_db, P2PS_IDX_NB);

        ns_ble_p2ps_task = env->task;
        ke_state_set(ns_ble_p2ps_task, P2PS_IDLE);
    }

    return status;
}

static void p2ps_destroy(struct prf_task_env *env)
{
    struct p2ps_env_tag *p2ps_env = (struct p2ps_env_tag *)env->env;

    while (!co_list_is_empty(&(p2ps_env->values)))
    {
        struct co_list_hdr *hdr = co_list_pop_front(&(p2ps_env->values));
        ke_free(hdr);
    }
    env->env = NULL;
    ke_free(p2ps_env);
}

static void p2ps_create(struct prf_task_env *env, uint8_t conidx)
{
    for (int i = 1; i < P2PS_IDX_NB; i++)
    {
        if (PERM_GET(p2ps_att_db[i].perm, UUID_LEN) == 0 &&
            (p2ps_att_db[i].uuid[0] + (p2ps_att_db[i].uuid[1] << 8)) == ATT_DESC_CLIENT_CHAR_CFG)
        {
            p2ps_set_ccc_value(conidx, i, 0);
        }
    }
}

static void p2ps_cleanup(struct prf_task_env *env, uint8_t conidx, uint8_t reason)
{
    struct p2ps_env_tag *p2ps_env = (struct p2ps_env_tag *)env->env;

    for (int i = 1; i < P2PS_IDX_NB; i++)
    {
        if (PERM_GET(p2ps_att_db[i].perm, UUID_LEN) == 0 &&
            (p2ps_att_db[i].uuid[0] + (p2ps_att_db[i].uuid[1] << 8)) == ATT_DESC_CLIENT_CHAR_CFG)
        {
            p2ps_set_ccc_value(conidx, i, 0);
        }
    }
    ke_state_set(prf_src_task_get(&(p2ps_env->prf_env), conidx), P2PS_IDLE);
}

static const struct prf_task_cbs p2ps_itf =
{
    (prf_init_fnct)p2ps_init,
    p2ps_destroy,
    p2ps_create,
    p2ps_cleanup,
};

const struct prf_task_cbs *p2ps_prf_itf_get(void)
{
    return &p2ps_itf;
}

#endif /* BLE_APP_P2PS */
