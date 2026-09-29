/**
 * @file app_p2ps.c
 * @brief Peer-to-Peer Server (P2PS) application layer.
 *
 * Exposes two characteristics:
 *   Write (AD42)  -- client sends data (write without response); received here
 *   Notify (AD43) -- MCU pushes data via APP_P2PS_NotifySend()
 *
 * Ported from app_ble_cellular_wmbus's app_p2ps.c, restructured against
 * P2PS_OTA's own minimal user_config_t (see user_config.h) instead of the
 * old project's 234-byte cellular/WMBus-heavy userConfig_t:
 *   - Owns a self-contained static user_config_t cache (s_cfg), loaded once
 *     via User_Config_Load() -- no UserApp_GetConfig()/UpdateConfig()
 *     abstraction layer, since nothing else in this template needs to share
 *     config state.
 *   - USER_CFG_FRAME_TYPE read/write only touches fields that actually exist
 *     in the new struct (frame_type/device_type/magic/version/
 *     firmware_version/hardware_version/epoch_systime/vcc_mv/die_temp_c) --
 *     every WMBus/cellular-specific field (slave_dev_addr, apn/server_host,
 *     forward_volume, cap_mv, cellular_rssi, ...) from the old struct is gone.
 *   - vcc_mv/die_temp_c are read live via adc.c's APP_ADC_ReadVrefMv()/
 *     APP_ADC_ReadTemperatureC(), reusing the old project's proven encode
 *     scheme (vcc_mv = (actual_mv/10) - 200, die_temp_c = direct signed cast)
 *     so a companion mobile app's existing decode logic still applies.
 *   - Config write now zeros epoch_systime/vcc_mv/die_temp_c before saving
 *     (the old file had this commented out, leaving a stale epoch_systime
 *     baked into flash after every mobile-initiated time set -- a real
 *     inconsistency against the field's own "Live/RW" doc comment; fixed
 *     here rather than carried forward).
 *   - Dropped entirely: CELL_LOG_FRAME_TYPE cellular-session-log streaming
 *     (CellularLog_*, TASK_CELL_LOG_NOTIFY) and the LpuartComm_SlaveIsActive()
 *     config-write deferral guard -- neither concept exists in this project.
 *   - USER_PWD (see user_config.h) is a shared placeholder value across every
 *     project that has it -- flagged there as must-change-per-deployment.
 *
 * Kept as-is (generic, already proven): AD40/AD42/AD43 GATT layout, the
 * session-password gate, queue_notify()/APP_P2PS_NotifySend() indirection
 * (sends outside BLE kernel-callback context, CFG_SEQ_Prio_1 re-arm
 * discipline matching app_ble_cellular_wmbus bug #39's starvation fix),
 * USER_TIMESTAMP_EPOCH_FRAME_TYPE read/set.
 */

#include "rwip_config.h"

#if (BLE_APP_P2PS)

#include "app_p2ps.h"
#include <string.h>
#include "p2pss.h"
#include "p2pss_task.h"
#include "attm.h"
#include "ke_task.h"
#include "gapc.h"
#include "gapc_task.h"
#include "gattc_task.h"
#include "prf_utils.h"
#include "ke_mem.h"
#include "ke_msg.h"
#include "ns_ble.h"
#include "app_log.h"
#include "n32_systime.h"
#include "n32_seq.h"
#include "n32_timer.h"
#include "user_config.h"
#include "adc.h"

/* ---- GATT attribute database ---------------------------------------------- */

struct attm_desc_128 p2ps_att_db[P2PS_IDX_NB] =
    {
        /* Primary service declaration */
        [P2PS_IDX_SVC] = {{0x00, 0x28}, PERM(RD, ENABLE), 0, 0},

        /* Write without response characteristic */
        [P2PS_IDX_WR_CHAR] = {{0x03, 0x28}, PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE), 0, 0},
        [P2PS_IDX_WR_VAL] = {CHAR_P2PS_WR,
                             PERM(WRITE_COMMAND, ENABLE),
                             PERM_VAL(UUID_LEN, 0x02), 244},

        /* Notify characteristic */
        [P2PS_IDX_NTF_CHAR] = {{0x03, 0x28}, PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE), 0, 0},
        [P2PS_IDX_NTF_VAL] = {CHAR_P2PS_NTF,
                              PERM(NTF, ENABLE),
                              PERM(RI, ENABLE) | PERM_VAL(UUID_LEN, 0x02), 244},
        [P2PS_IDX_NTF_CFG] = {{0x02, 0x29}, PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE), 0, 0},
};

/* ---- Private state --------------------------------------------------------- */

static bool     s_ntf_enable   = false;
static bool     s_session_auth = false;
static uint8_t  s_ntf_buf[244];
static uint16_t s_ntf_len      = 0;
static uint32_t s_pending_epoch = 0;

/* Self-contained RAM config cache -- loaded once in APP_P2PS_Init(). */
static user_config_t s_cfg;
static user_config_t s_pending_cfg;

/* ---- Post-config-save reset timer ------------------------------------------ */
static UTIL_TIMER_Object_t s_cfg_reset_timer;
static void cfg_reset_cb(void *arg)
{
    (void)arg;
    APP_LOG(TS_OFF, VLEVEL_M, "[P2PS] config saved -- resetting\r\n");
    NVIC_SystemReset();
}

/* ---- Internal helper: queue a notification for TASK_P2PS_NOTIFY ----------- */

static void queue_notify(const uint8_t *data, uint16_t len)
{
    if (len > (uint16_t)sizeof(s_ntf_buf))
        len = (uint16_t)sizeof(s_ntf_buf);
    memcpy(s_ntf_buf, data, len);
    s_ntf_len = len;
    UTIL_SEQ_SetTask(1U << TASK_P2PS_NOTIFY, CFG_SEQ_Prio_0);
}

/* ---- Write indication handler ---------------------------------------------- */

static int p2ps_val_write_ind_handler(ke_msg_id_t const msgid,
                                      void const *param,
                                      ke_task_id_t const dest_id,
                                      ke_task_id_t const src_id)
{
    const struct p2ps_val_write_ind *p = (const struct p2ps_val_write_ind *)param;

    switch (p->handle)
    {
    case P2PS_IDX_NTF_CFG:
    {
        if (p->length == 2)
        {
            uint16_t cfg = (uint16_t)(p->value[0] | (p->value[1] << 8));
            s_ntf_enable = (cfg == PRF_CLI_START_NTF);
            APP_LOG(TS_OFF, VLEVEL_M, "P2PS notify: %s\r\n", s_ntf_enable ? "enabled" : "disabled");
        }
        break;
    }

    case P2PS_IDX_WR_VAL:
    {
        if (p->length < 2)
            break;

        uint16_t ft = (uint16_t)(p->value[0] | (p->value[1] << 8));

        /* --- Password auth --- */
        if (ft == USER_PWD_FRAME_TYPE && p->length == USER_PWD_FRAME_LEN)
        {
            static const uint8_t s_expected_pwd[16] = USER_PWD;
            s_session_auth = (memcmp(&p->value[2], s_expected_pwd, 16U) == 0);
            APP_LOG(TS_OFF, VLEVEL_M, "P2PS auth: %s\r\n", s_session_auth ? "success" : "FAILED");
            uint8_t resp[3] = { (uint8_t)(USER_PWD_FRAME_TYPE),
                                (uint8_t)(USER_PWD_FRAME_TYPE >> 8),
                                s_session_auth ? 0x01U : 0x00U };
            queue_notify(resp, sizeof(resp));
        }
        /* --- Config read request: [frame_type, 0x01] --- */
        else if (ft == USER_CFG_FRAME_TYPE && p->length == 3U && p->value[2] == 0x01U)
        {
            if (!s_session_auth) { APP_LOG(TS_OFF, VLEVEL_M, "P2PS cfg read: not auth\r\n"); break; }
            user_config_t snap = s_cfg;
            snap.epoch_systime = SysTimeGet().Seconds;
            snap.vcc_mv        = (uint8_t)((APP_ADC_ReadVrefMv() / 10) - 200);
            snap.die_temp_c    = (int8_t)APP_ADC_ReadTemperatureC();
            APP_LOG(TS_OFF, VLEVEL_M, "P2PS cfg read: Queried\r\n");
            queue_notify((const uint8_t *)&snap, (uint16_t)sizeof(snap));
        }
        /* --- Config write: full user_config_t --- */
        else if (ft == USER_CFG_FRAME_TYPE && p->length == (uint16_t)sizeof(user_config_t))
        {
            if (!s_session_auth) { APP_LOG(TS_OFF, VLEVEL_M, "P2PS cfg write: not auth\r\n"); break; }
            memcpy(&s_pending_cfg, p->value, sizeof(user_config_t));
            APP_LOG(TS_OFF, VLEVEL_M, "P2PS cfg write: Queued (%u bytes)\r\n", (unsigned)sizeof(s_pending_cfg));
            UTIL_SEQ_SetTask(1U << TASK_P2PS_CFG_WRITE, CFG_SEQ_Prio_0);
        }
        /* --- Epoch read request: [frame_type, 0x01] --- */
        else if (ft == USER_TIMESTAMP_EPOCH_FRAME_TYPE && p->length == 3U && p->value[2] == 0x01U)
        {
            if (!s_session_auth) { APP_LOG(TS_OFF, VLEVEL_M, "P2PS epoch read: not auth\r\n"); break; }
            SysTime_t now = SysTimeGet();
            uint8_t buf[6] = { (uint8_t)(USER_TIMESTAMP_EPOCH_FRAME_TYPE),
                               (uint8_t)(USER_TIMESTAMP_EPOCH_FRAME_TYPE >> 8),
                               (uint8_t)(now.Seconds),
                               (uint8_t)(now.Seconds >> 8),
                               (uint8_t)(now.Seconds >> 16),
                               (uint8_t)(now.Seconds >> 24) };
            queue_notify(buf, sizeof(buf));
        }
        /* --- Epoch write/set: [frame_type, b0..b3] --- */
        else if (ft == USER_TIMESTAMP_EPOCH_FRAME_TYPE && p->length == 6U)
        {
            if (!s_session_auth) { APP_LOG(TS_OFF, VLEVEL_M, "P2PS epoch set: not auth\r\n"); break; }
            s_pending_epoch = (uint32_t)p->value[2]
                            | ((uint32_t)p->value[3] << 8)
                            | ((uint32_t)p->value[4] << 16)
                            | ((uint32_t)p->value[5] << 24);
            UTIL_SEQ_SetTask(1U << TASK_P2PS_TIME_SET, CFG_SEQ_Prio_0);
        }
        else
        {
            APP_LOG(TS_OFF, VLEVEL_L, "P2PS write: unknown ft=0x%04X len=%u\r\n",
                    (unsigned)ft, (unsigned)p->length);
        }
        break;
    }

    default:
        break;
    }

    return KE_MSG_CONSUMED;
}

/* ---- Disconnect handler ---------------------------------------------------- */

static int p2ps_disconnect_handler(ke_msg_id_t const msgid,
                                   void const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
    s_ntf_enable   = false;
    s_session_auth = false;
    s_ntf_len      = 0;
    return KE_MSG_CONSUMED;
}

/* ---- Message handler table ------------------------------------------------- */

const struct ke_msg_handler p2ps_app_msg_handler_list[] =
    {
        {P2PS_VAL_WRITE_IND, p2ps_val_write_ind_handler},
        {P2PS_DISCONNECT, p2ps_disconnect_handler},
    };

const struct app_subtask_handlers p2ps_app_handlers = APP_HANDLERS(p2ps_app);

/* ---- Sequencer tasks ------------------------------------------------------ */

void APP_P2PS_NotifyTask(void)
{
    if (s_ntf_len == 0)
        return;
    APP_P2PS_NotifySend(s_ntf_buf, s_ntf_len);
    s_ntf_len = 0;
}

void APP_P2PS_CfgWriteTask(void)
{
    if (s_pending_cfg.magic != USER_CFG_MAGIC || s_pending_cfg.version != USER_CFG_VERSION)
    {
        APP_LOG(TS_OFF, VLEVEL_M, "P2PS cfg write: bad magic/version - rejected\r\n");
        return;
    }

    /* Restore read-only fields -- mobile must not overwrite these */
    s_pending_cfg.frame_type       = s_cfg.frame_type;
    s_pending_cfg.device_type      = s_cfg.device_type;
    s_pending_cfg.magic            = s_cfg.magic;
    s_pending_cfg.version          = s_cfg.version;
    s_pending_cfg.firmware_version = s_cfg.firmware_version;
    s_pending_cfg.hardware_version = s_cfg.hardware_version;

    APP_LOG(TS_OFF, VLEVEL_M, "Received epoch: %lu\r\n", (unsigned long)s_pending_cfg.epoch_systime);
    /* Apply epoch to RTC if mobile provided one, then zero the live fields
     * before flash save -- a stale write-time snapshot would otherwise
     * persist in flash and diverge from the RTC/ADC on every later read. */
    if (s_pending_cfg.epoch_systime != 0U)
    {
        SysTime_t t = {.Seconds = s_pending_cfg.epoch_systime, .SubSeconds = 0};
        SysTimeSet(t);
    }
    SysTime_t t = SysTimeGet();
    APP_LOG(TS_OFF, VLEVEL_M, "RTC now: %lu\r\n", (unsigned long)t.Seconds);
    s_pending_cfg.epoch_systime = 0U;
    s_pending_cfg.vcc_mv        = 0U;
    s_pending_cfg.die_temp_c    = 0;

    s_cfg = s_pending_cfg;
    User_Config_Save(&s_cfg);
    APP_LOG(TS_OFF, VLEVEL_M, "P2PS cfg write: saved (%u bytes) - reset in 5 s\r\n",
            (unsigned)sizeof(s_cfg));
    uint8_t ack[3] = { (uint8_t)(USER_CFG_FRAME_TYPE),
                       (uint8_t)(USER_CFG_FRAME_TYPE >> 8),
                       0xACU };
    queue_notify(ack, sizeof(ack));
    UTIL_TIMER_Create(&s_cfg_reset_timer, 5000U, UTIL_TIMER_ONESHOT, cfg_reset_cb, NULL);
    UTIL_TIMER_Start(&s_cfg_reset_timer);
}

/**
 * @brief Apply pending epoch to RTC then notify client with the confirmed
 *        time. Runs in sequencer context -- never blocks the BLE message
 *        handler.
 */
void APP_P2PS_TimeSetTask(void)
{
    SysTime_t t = {.Seconds = s_pending_epoch, .SubSeconds = 0};
    SysTimeSet(t);
    APP_LOG(TS_OFF, VLEVEL_M, "P2PS time set: %lu\r\n", (unsigned long)s_pending_epoch);

    t = SysTimeGet();
    uint8_t buf[6];
    buf[0] = (uint8_t)(USER_TIMESTAMP_EPOCH_FRAME_TYPE);
    buf[1] = (uint8_t)(USER_TIMESTAMP_EPOCH_FRAME_TYPE >> 8);
    buf[2] = (uint8_t)(t.Seconds);
    buf[3] = (uint8_t)(t.Seconds >> 8);
    buf[4] = (uint8_t)(t.Seconds >> 16);
    buf[5] = (uint8_t)(t.Seconds >> 24);

    APP_P2PS_NotifySend(buf, sizeof(buf));
}

/* ---- Public API ------------------------------------------------------------ */

bool APP_P2PS_IsAuthenticated(void)
{
    return s_session_auth;
}

void APP_P2PS_NotifySend(const uint8_t *data, uint16_t length)
{
    if (!s_ntf_enable)
        return;

    uint8_t state = ke_state_get(ns_ble_p2ps_task);
    if (state == P2PS_BUSY)
        return;

    ke_state_set(ns_ble_p2ps_task, P2PS_BUSY);

    struct p2ps_env_tag *p2ps_env = PRF_ENV_GET(P2PS, p2ps);
    struct gattc_send_evt_cmd *req = KE_MSG_ALLOC_DYN(
        GATTC_SEND_EVT_CMD,
        KE_BUILD_ID(TASK_GATTC, app_env.conidx),
        p2ps_env->prf_env.prf_task,
        gattc_send_evt_cmd,
        length);

    req->operation = GATTC_NOTIFY;
    req->handle = p2ps_env->shdl + P2PS_IDX_NTF_VAL;
    req->length = length;
    memcpy(req->value, data, length);
    ke_msg_send(req);
}

void APP_P2PS_AddP2ps(void)
{
    struct p2ps_db_cfg *db_cfg;
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(
        GAPM_PROFILE_TASK_ADD_CMD,
        TASK_GAPM,
        TASK_APP,
        gapm_profile_task_add_cmd,
        sizeof(struct p2ps_db_cfg));

    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH);
    req->prf_task_id = TASK_ID_P2PS;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    db_cfg = (struct p2ps_db_cfg *)req->param;
    db_cfg->att_tbl = NULL;
    db_cfg->cfg_flag = 0;
    db_cfg->features = 0;

    ke_msg_send(req);
    APP_P2PS_Init();
}

void APP_P2PS_Init(void)
{
    (void)User_Config_Load(&s_cfg);

    struct prf_task_t prf;
    prf.prf_task_id = TASK_ID_P2PS;
    prf.prf_task_handler = &p2ps_app_handlers;
    ns_ble_prf_task_register(&prf);

    struct prf_get_func_t get_func;
    get_func.task_id = TASK_ID_P2PS;
    get_func.prf_itf_get_func = p2ps_prf_itf_get;
    prf_get_itf_func_register(&get_func);
}

#endif /* BLE_APP_P2PS */
