/**
 * @file app_dis.c
 * @brief Device Information Service (DIS) application layer -- ported
 * directly from the N32 SDK's own `ble/dis` example project
 * (`src/app_profile/app_dis.c`), renamed to this project's `APP_DIS_*`
 * PascalCase convention and using this project's own APP_LOG() (app_log.h)
 * in place of the SDK reference's NS_LOG_DEBUG(). See app_dis.h for the
 * populated-characteristics rationale.
 */

#include "rwip_config.h"

#if (BLE_APP_DIS)

#include "ns_ble.h"
#include "app_dis.h"
#include "diss_task.h"
#include "prf_types.h"
#include "ke_task.h"
#include "gapm_task.h"
#include "diss.h"
#include "app_log.h"
#include <string.h>

/**
 * @brief DISS_VALUE_REQ_IND handler -- DISS asks the app for the value of
 * one of the characteristics advertised in APP_DIS_FEATURES; reply with
 * DISS_VALUE_CFM built from the static strings in app_dis.h.
 */
static int APP_DIS_ValueReqIndHandler(ke_msg_id_t const msgid,
                                       struct diss_value_req_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    uint8_t len = 0U;
    uint8_t *data = NULL;

    switch (param->value)
    {
        case DIS_MANUFACTURER_NAME_CHAR:
            len = APP_DIS_MANUFACTURER_NAME_LEN;
            data = (uint8_t *)APP_DIS_MANUFACTURER_NAME;
            break;

        case DIS_MODEL_NB_STR_CHAR:
            len = APP_DIS_MODEL_NB_STR_LEN;
            data = (uint8_t *)APP_DIS_MODEL_NB_STR;
            break;

        case DIS_SERIAL_NB_STR_CHAR:
            len = APP_DIS_SERIAL_NB_STR_LEN;
            data = (uint8_t *)APP_DIS_SERIAL_NB_STR;
            break;

        case DIS_HARD_REV_STR_CHAR:
            len = APP_DIS_HARD_REV_STR_LEN;
            data = (uint8_t *)APP_DIS_HARD_REV_STR;
            break;

        case DIS_FIRM_REV_STR_CHAR:
            len = APP_DIS_FIRM_REV_STR_LEN;
            data = (uint8_t *)APP_DIS_FIRM_REV_STR;
            break;

        case DIS_SW_REV_STR_CHAR:
            len = APP_DIS_SW_REV_STR_LEN;
            data = (uint8_t *)APP_DIS_SW_REV_STR;
            break;

        default:
            /* DISS only ever requests a char advertised as supported in
             * APP_DIS_FEATURES -- reaching here means a mismatch between
             * that bitmask and this switch. */
            ASSERT_ERR(0);
            break;
    }

    struct diss_value_cfm *cfm_value = KE_MSG_ALLOC_DYN(DISS_VALUE_CFM,
            src_id, dest_id,
            diss_value_cfm,
            len);

    cfm_value->value = param->value;
    cfm_value->length = len;
    if (len != 0U)
    {
        memcpy(&cfm_value->data[0], data, len);
    }

    ke_msg_send(cfm_value);

    return (KE_MSG_CONSUMED);
}

void APP_DIS_Init(void)
{
    struct prf_task_t prf;
    prf.prf_task_id = TASK_ID_DISS;
    prf.prf_task_handler = &APP_DIS_Handlers;
    ns_ble_prf_task_register(&prf);

    struct prf_get_func_t get_func;
    get_func.task_id = TASK_ID_DISS;
    get_func.prf_itf_get_func = diss_prf_itf_get;
    prf_get_itf_func_register(&get_func);

    APP_LOG(TS_OFF, VLEVEL_H, "[DIS] initialized, features=0x%04x\r\n", (unsigned)APP_DIS_FEATURES);
}

void APP_DIS_AddDis(void)
{
    struct diss_db_cfg *db_cfg;
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
            TASK_GAPM, TASK_APP,
            gapm_profile_task_add_cmd, sizeof(struct diss_db_cfg));

    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH);
    req->prf_task_id = TASK_ID_DISS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    db_cfg = (struct diss_db_cfg *)req->param;
    db_cfg->features = APP_DIS_FEATURES;

    ke_msg_send(req);

    APP_DIS_Init();
}

static const struct ke_msg_handler app_dis_msg_handler_list[] =
{
    {DISS_VALUE_REQ_IND, (ke_msg_func_t)APP_DIS_ValueReqIndHandler},
};

const struct app_subtask_handlers APP_DIS_Handlers = APP_HANDLERS(app_dis);

#endif /* BLE_APP_DIS */
