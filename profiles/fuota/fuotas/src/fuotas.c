/**
 * @file fuotas.c
 * @brief FUOTA Server profile — service registration and lifecycle callbacks.
 */

#include "rwip_config.h"

#if (BLE_APP_FUOTA)

#include "fuotas.h"
#include "fuotas_task.h"
#include "app_fuota.h"

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

static uint8_t fuotas_init(struct prf_task_env *env, uint16_t *start_hdl,
                            uint16_t app_task, uint8_t sec_lvl,
                            struct fuotas_db_cfg *params)
{
    uint8_t  status   = ATT_ERR_NO_ERROR;
    uint8_t  uuid_128[16] = SERVICE_FUOTA;
    uint32_t cfg_flag = ((1U << FUOTA_IDX_NB) - 1U);

    status = attm_svc_create_db_128(
        start_hdl, uuid_128, (uint8_t *)&cfg_flag,
        FUOTA_IDX_NB, NULL, env->task, fuota_att_db,
        (sec_lvl & PERM_MASK_SVC_AUTH) | (sec_lvl & PERM_MASK_SVC_EKS) |
        PERM(SVC_SECONDARY, DISABLE) | PERM_VAL(SVC_UUID_LEN, 0x2));

    if (status == ATT_ERR_NO_ERROR)
    {
        struct fuotas_env_tag *fuotas_env =
            (struct fuotas_env_tag *)ke_malloc(sizeof(struct fuotas_env_tag), KE_MEM_ATT_DB);

        env->env = (prf_env_t *)fuotas_env;
        fuotas_env->shdl       = *start_hdl;
        fuotas_env->max_nb_att = FUOTA_IDX_NB;
        fuotas_env->prf_env.app_task = app_task |
            (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        fuotas_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        env->id = TASK_ID_FUOTA;
        fuotas_task_init(&(env->desc));
        co_list_init(&(fuotas_env->values));
        fuotas_init_ccc_values(fuota_att_db, FUOTA_IDX_NB);

        ns_ble_fuotas_task = env->task;
        ke_state_set(ns_ble_fuotas_task, FUOTAS_IDLE);
    }

    return status;
}

static void fuotas_destroy(struct prf_task_env *env)
{
    struct fuotas_env_tag *fuotas_env = (struct fuotas_env_tag *)env->env;

    while (!co_list_is_empty(&(fuotas_env->values)))
    {
        struct co_list_hdr *hdr = co_list_pop_front(&(fuotas_env->values));
        ke_free(hdr);
    }
    env->env = NULL;
    ke_free(fuotas_env);
}

static void fuotas_create(struct prf_task_env *env, uint8_t conidx)
{
    for (int i = 1; i < FUOTA_IDX_NB; i++)
    {
        if (PERM_GET(fuota_att_db[i].perm, UUID_LEN) == 0 &&
            (fuota_att_db[i].uuid[0] + (fuota_att_db[i].uuid[1] << 8)) == ATT_DESC_CLIENT_CHAR_CFG)
        {
            fuotas_set_ccc_value(conidx, i, 0);
        }
    }
}

static void fuotas_cleanup(struct prf_task_env *env, uint8_t conidx, uint8_t reason)
{
    struct fuotas_env_tag *fuotas_env = (struct fuotas_env_tag *)env->env;

    for (int i = 1; i < FUOTA_IDX_NB; i++)
    {
        if (PERM_GET(fuota_att_db[i].perm, UUID_LEN) == 0 &&
            (fuota_att_db[i].uuid[0] + (fuota_att_db[i].uuid[1] << 8)) == ATT_DESC_CLIENT_CHAR_CFG)
        {
            fuotas_set_ccc_value(conidx, i, 0);
        }
    }
    ke_state_set(prf_src_task_get(&(fuotas_env->prf_env), conidx), FUOTAS_IDLE);
}

static const struct prf_task_cbs fuotas_itf =
{
    (prf_init_fnct)fuotas_init,
    fuotas_destroy,
    fuotas_create,
    fuotas_cleanup,
};

const struct prf_task_cbs *fuotas_prf_itf_get(void)
{
    return &fuotas_itf;
}

#endif /* BLE_APP_FUOTA */
