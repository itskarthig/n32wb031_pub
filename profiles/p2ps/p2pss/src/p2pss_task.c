/**
 * @file p2pss_task.c
 * @brief P2P Server profile — GATT ATT message handlers.
 */

#include "rwip_config.h"

#if (BLE_APP_P2PS)

#include "p2pss_task.h"
#include "p2pss.h"
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

ke_task_id_t ns_ble_p2ps_task;

/* ---- Value list helpers ---------------------------------------------------- */

static int p2ps_att_set_value(uint8_t att_idx, uint16_t length, const uint8_t *data)
{
    struct p2ps_env_tag *p2ps_env = PRF_ENV_GET(P2PS, p2ps);
    struct p2ps_val_elmt *val =
        (struct p2ps_val_elmt *)co_list_pick(&(p2ps_env->values));

    while (val != NULL)
    {
        if (val->att_idx == att_idx)
        {
            if (length != val->length)
            {
                co_list_extract(&p2ps_env->values, &val->hdr);
                ke_free(val);
                val = NULL;
            }
            break;
        }
        val = (struct p2ps_val_elmt *)val->hdr.next;
    }

    if (val == NULL)
    {
        val = (struct p2ps_val_elmt *)ke_malloc(
            sizeof(struct p2ps_val_elmt) + length, KE_MEM_ATT_DB);
        co_list_push_back(&p2ps_env->values, &val->hdr);
    }
    val->att_idx = att_idx;
    val->length  = length;
    memcpy(val->data, data, length);
    return 0;
}

static int p2ps_att_get_value(uint8_t att_idx, uint16_t *length, const uint8_t **data)
{
    struct p2ps_env_tag *p2ps_env = PRF_ENV_GET(P2PS, p2ps);
    struct p2ps_val_elmt *val =
        (struct p2ps_val_elmt *)co_list_pick(&p2ps_env->values);

    while (val != NULL)
    {
        if (val->att_idx == att_idx)
        {
            *length = val->length;
            *data   = val->data;
            break;
        }
        val = (struct p2ps_val_elmt *)val->hdr.next;
    }

    if (val == NULL)
    {
        *length = 0;
        *data   = NULL;
    }
    return val ? 0 : ATT_ERR_ATTRIBUTE_NOT_FOUND;
}

/* ---- CCC helpers ----------------------------------------------------------- */

void p2ps_init_ccc_values(const struct attm_desc_128 *att_db, int max_nb_att)
{
    uint8_t ccc_values[BLE_CONNECTION_MAX] = {0};

    for (int i = 1; i < max_nb_att; i++)
    {
        if (PERM_GET(att_db[i].perm, UUID_LEN) == PERM_UUID_16 &&
            (att_db[i].uuid[0] + (att_db[i].uuid[1] << 8)) == ATT_DESC_CLIENT_CHAR_CFG)
        {
            p2ps_att_set_value(i, sizeof(ccc_values), ccc_values);
        }
    }
}

void p2ps_set_ccc_value(uint8_t conidx, uint8_t att_idx, uint16_t ccc)
{
    uint16_t       length;
    const uint8_t *value;
    uint8_t        new_value[BLE_CONNECTION_MAX];

    ASSERT_ERR(conidx < BLE_CONNECTION_MAX);
    p2ps_att_get_value(att_idx, &length, &value);
    ASSERT_ERR(length);
    ASSERT_ERR(value);
    memcpy(new_value, value, length);
    new_value[conidx] = (uint8_t)ccc;
    p2ps_att_set_value(att_idx, length, new_value);
}

static uint16_t p2ps_get_ccc_value(uint8_t conidx, uint8_t att_idx)
{
    uint16_t       length;
    const uint8_t *value;

    p2ps_att_get_value(att_idx, &length, &value);
    return (uint16_t)value[conidx];
}

/* ---- Attribute index helper ----------------------------------------------- */

static uint8_t p2ps_get_att_idx(uint16_t handle, uint8_t *att_idx)
{
    struct p2ps_env_tag *p2ps_env = PRF_ENV_GET(P2PS, p2ps);
    uint8_t status = PRF_APP_ERROR;

    if ((handle >= p2ps_env->shdl) && (handle < p2ps_env->shdl + p2ps_env->max_nb_att))
    {
        *att_idx = handle - p2ps_env->shdl;
        status   = ATT_ERR_NO_ERROR;
    }
    return status;
}

static uint16_t get_value_handle(uint16_t cfg_handle)
{
    uint8_t  uuid[ATT_UUID_128_LEN];
    uint8_t  uuid_len;
    uint16_t handle = cfg_handle;
    struct attm_svc *srv = attmdb_get_service(handle);

    while ((handle >= srv->svc.start_hdl) && (handle <= srv->svc.end_hdl))
    {
        struct attm_elmt elmt;
        attmdb_get_attribute(handle, &elmt);
        attmdb_get_uuid(&elmt, &uuid_len, uuid, false, false);
        if (*(uint16_t *)&uuid[0] == ATT_DECL_CHARACTERISTIC)
            return handle + 1;
        handle--;
    }
    return 0;
}

static int check_client_char_cfg(bool is_notification, const struct gattc_write_req_ind *param)
{
    uint8_t  status  = GAP_ERR_NO_ERROR;
    uint16_t ntf_cfg = 0;

    if (param->length != sizeof(uint16_t))
    {
        status = ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN;
    }
    else
    {
        ntf_cfg = *((uint16_t *)param->value);
        if (is_notification)
        {
            if ((ntf_cfg != PRF_CLI_STOP_NTFIND) && (ntf_cfg != PRF_CLI_START_NTF))
                status = PRF_ERR_INVALID_PARAM;
        }
        else
        {
            if ((ntf_cfg != PRF_CLI_STOP_NTFIND) && (ntf_cfg != PRF_CLI_START_IND))
                status = PRF_ERR_INVALID_PARAM;
        }
    }
    return status;
}

/* ---- ATT message handlers -------------------------------------------------- */

static int gattc_read_req_ind_handler(ke_msg_id_t const msgid,
                                       void const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    const struct gattc_read_req_ind *p_param = (const struct gattc_read_req_ind *)param;

    if (ke_state_get(dest_id) != P2PS_IDLE)
        return KE_MSG_SAVED;

    struct p2ps_env_tag *p2ps_env = PRF_ENV_GET(P2PS, p2ps);
    uint8_t  att_idx = 0;
    uint8_t  conidx  = KE_IDX_GET(src_id);
    uint8_t  status  = p2ps_get_att_idx(p_param->handle, &att_idx);
    uint16_t length  = 0;
    uint16_t ccc_val = 0;

    if (status == GAP_ERR_NO_ERROR)
    {
        if (PERM_GET(p2ps_att_db[att_idx].perm, UUID_LEN) == PERM_UUID_16 &&
            (p2ps_att_db[att_idx].uuid[0] + (p2ps_att_db[att_idx].uuid[1] << 8)) ==
                ATT_DESC_CLIENT_CHAR_CFG)
        {
            /* CCC: respond directly from stored value */
            ccc_val = p2ps_get_ccc_value(conidx, att_idx);
            length  = 2;
        }
        else
        {
            /* Non-CCC value: ask the application layer to supply it */
            struct p2ps_value_req_ind *req_ind = KE_MSG_ALLOC(
                P2PS_VALUE_REQ_IND,
                prf_dst_task_get(&(p2ps_env->prf_env), KE_IDX_GET(src_id)),
                dest_id,
                p2ps_value_req_ind);
            req_ind->conidx  = KE_IDX_GET(src_id);
            req_ind->att_idx = att_idx;
            ke_msg_send(req_ind);
            ke_state_set(dest_id, P2PS_BUSY);
            return KE_MSG_CONSUMED;
        }
    }

    struct gattc_read_cfm *cfm = KE_MSG_ALLOC_DYN(
        GATTC_READ_CFM, src_id, dest_id, gattc_read_cfm, length);
    cfm->handle = p_param->handle;
    cfm->status = status;
    cfm->length = length;
    if (status == GAP_ERR_NO_ERROR)
        memcpy(cfm->value, &ccc_val, length);
    ke_msg_send(cfm);
    return KE_MSG_CONSUMED;
}

static int gattc_write_req_ind_handler(ke_msg_id_t const msgid,
                                        void const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    const struct gattc_write_req_ind *p_param = (const struct gattc_write_req_ind *)param;
    struct p2ps_env_tag *p2ps_env = PRF_ENV_GET(P2PS, p2ps);
    uint8_t att_idx = 0;
    uint8_t conidx  = KE_IDX_GET(src_id);
    uint8_t status  = p2ps_get_att_idx(p_param->handle, &att_idx);

    if (status == ATT_ERR_NO_ERROR)
    {
        if (PERM_GET(p2ps_att_db[att_idx].perm, UUID_LEN) == PERM_UUID_16 &&
            (p2ps_att_db[att_idx].uuid[0] + (p2ps_att_db[att_idx].uuid[1] << 8)) ==
                ATT_DESC_CLIENT_CHAR_CFG)
        {
            uint16_t perm;
            struct attm_elmt elem = {0};
            uint16_t value_hdl = get_value_handle(p_param->handle);
            attmdb_att_get_permission(value_hdl, &perm, PERM_MASK_ALL, 0, &elem);
            status = check_client_char_cfg(PERM_IS_SET(perm, NTF, ENABLE), p_param);
            if (status == ATT_ERR_NO_ERROR)
                p2ps_set_ccc_value(conidx, att_idx, *(uint16_t *)p_param->value);
        }

        if (status == ATT_ERR_NO_ERROR)
        {
            struct p2ps_val_write_ind *req_id = KE_MSG_ALLOC_DYN(
                P2PS_VAL_WRITE_IND,
                prf_dst_task_get(&(p2ps_env->prf_env), KE_IDX_GET(src_id)),
                dest_id,
                p2ps_val_write_ind,
                p_param->length);
            memcpy(req_id->value, p_param->value, p_param->length);
            req_id->conidx = conidx;
            req_id->handle = att_idx;
            req_id->length = p_param->length;
            ke_msg_send(req_id);
        }
    }

    struct gattc_write_cfm *cfm = KE_MSG_ALLOC(GATTC_WRITE_CFM, src_id, dest_id, gattc_write_cfm);
    cfm->handle = p_param->handle;
    cfm->status = status;
    ke_msg_send(cfm);
    return KE_MSG_CONSUMED;
}

static int gattc_att_info_req_ind_handler(ke_msg_id_t const msgid,
                                           void const *param,
                                           ke_task_id_t const dest_id,
                                           ke_task_id_t const src_id)
{
    const struct gattc_att_info_req_ind *p_param = (const struct gattc_att_info_req_ind *)param;

    if (ke_state_get(dest_id) == P2PS_IDLE)
    {
        struct p2ps_env_tag *p2ps_env = PRF_ENV_GET(P2PS, p2ps);
        struct p2ps_att_info_req *req = KE_MSG_ALLOC(
            P2PS_ATT_INFO_REQ, TASK_APP, dest_id, p2ps_att_info_req);
        req->conidx  = KE_IDX_GET(src_id);
        req->att_idx = p_param->handle - p2ps_env->shdl;
        ke_msg_send(req);
    }
    return KE_MSG_CONSUMED;
}

static int gattc_cmp_evt_handler(ke_msg_id_t const msgid,
                                  void const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id)
{
    const struct gattc_cmp_evt *p_param = (const struct gattc_cmp_evt *)param;

    if (p_param->operation == GATTC_NOTIFY)
        ke_state_set(ns_ble_p2ps_task, P2PS_IDLE);

    return KE_MSG_CONSUMED;
}

/* ---- Message handler table ------------------------------------------------- */

KE_MSG_HANDLER_TAB(p2ps)
{
    {GATTC_READ_REQ_IND,     gattc_read_req_ind_handler},
    {GATTC_WRITE_REQ_IND,    gattc_write_req_ind_handler},
    {GATTC_ATT_INFO_REQ_IND, gattc_att_info_req_ind_handler},
    {GATTC_CMP_EVT,          gattc_cmp_evt_handler},
};

void p2ps_task_init(struct ke_task_desc *p_task_desc)
{
    struct p2ps_env_tag *p_p2ps_env = PRF_ENV_GET(P2PS, p2ps);

    p_task_desc->msg_handler_tab = p2ps_msg_handler_tab;
    p_task_desc->msg_cnt         = ARRAY_LEN(p2ps_msg_handler_tab);
    p_task_desc->state           = p_p2ps_env->state;
    p_task_desc->idx_max         = P2PS_IDX_MAX;
}

#endif /* BLE_APP_P2PS */
