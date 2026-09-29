/**
 * @file app_ble.c
 * @brief BLE bring-up for BEACON/APP. See app_ble.h for the design
 * rationale. Init sequence, struct field names, and the advertising-mode
 * enum/message-handler shape are all taken directly from ns_library's own
 * public headers (ns_ble.h/ns_ble_task.h) -- independently verified against
 * N32WB03x_SDK_V2.0.0/projects/n32wb03x_EVAL/ble/{beacon,rdtss}/src/app_ble.c
 * (genuine SDK examples) for the calling convention, not copied from either.
 *
 * BLE pump scheduling (APP_BLE_ProcessTask()): STM32CubeWB0/WBA can defer
 * rwip_schedule()-equivalent pumping to a UTIL_SEQ task because their radio
 * ISR is genuine app-linked code (confirmed: STM32CubeWBA's RADIO_IRQHandler
 * lives in stm32wbaxx_it.c and calls back into UTIL_SEQ_SetTask() via
 * ll_sys_schedule_bg_process_isr()). N32's BLE_FIFO_IRQn/BLE_SLP_IRQn are
 * redirected via a ROM-owned RAM table (p_IrqFun[], set up inside
 * ns_ble_stack_init()) straight to ROM stubs -- confirmed no app-visible
 * "schedule me" hook exists (struct ns_stack_cfg_t has no such callback,
 * unlike WB0's BLE_STACK_Init()). So APP_BLE_ProcessTask() self-perpetuates
 * (re-arms itself every dispatch) instead of being IRQ-triggered. Registered
 * at CFG_SEQ_Prio_1 (lower than CFG_SEQ_Prio_0), matching WB0's own explicit
 * "keep BLE Stack Process priority low" convention, so PB1/PB3/periodic-timer
 * work already pending in the same UTIL_SEQ_Run() pass isn't starved.
 *
 * StopMode gating (per explicit user direction: "refer app_ble_* for ble
 * pump only" -- ported from the proven, hardware-validated ble_pump_task()
 * in app_ble_lorawan_wmbus/app_ble_cellular_wmbus's app_entry.c, the one
 * exception to this repo's standing "don't reference app_*" rule). Earlier
 * revisions called UTIL_LPM_EnterLowPower() unconditionally after
 * rwip_schedule() -- this forces the BLE hardware into its deep-sleep
 * register sequence (LpmIf_EnterStopMode()) even when the BLE kernel's own
 * software scheduler isn't expecting it, desyncing the RW controller from
 * its host-side state permanently (confirmed on hardware: ADV died silently
 * after the first real StopMode cycle, MCU otherwise healthy).
 *
 * CORRECTION -- ble_adv_msg_handler (the field wired to APP_BLE_AdvMsgHandler
 * below) is NOT dead code. The original "confirmed DEAD CODE" claim below
 * was based on an incomplete investigation: it grepped only ns_ble.c and
 * found no call site there. The real dispatch is in a *different* file,
 * ns_ble_stack/../ns_library/ble/ns_ble_task.c (genuinely compiled into
 * this project), inside gapm_cmp_evt_handler()'s `#if (BLE_APP_PRF)` block:
 * on GAPM_START_ACTIVITY/GAPM_DELETE_ACTIVITY completion with current_op==
 * CURRENT_OP_START_ADV/DELETE_ADV, it calls
 * `adv_env.ble_adv_msg_handler(app_env.adv_mode)`. ns_ble_adv_init()
 * (ns_ble.c) does `memcpy(&adv_env, p_init, ...)` from the user_adv struct
 * APP_BLE_AdvInit() populates, so adv_env.ble_adv_msg_handler genuinely is
 * our APP_BLE_AdvMsgHandler pointer -- the wiring is structurally correct
 * end to end. BLE_APP_PRF is confirmed 1 in this build (rwapp_config.h), so
 * this code path is compiled in, not stripped.
 *
 * Hardware-confirmed: a RAM flight-recorder (s_adv_msg_mode_count[] below)
 * proved the callback fires for FAST/SLOW/STOP alike; its own synchronous
 * APP_LOG() call was simply losing the race for the shared trace ring
 * buffer during the dense BLE ADV-FSM logging burst it fires inside of --
 * not a dead callback. APP_BLE_AdvMsgHandler() now also calls
 * APP_BLE_SetState() directly as an event-driven companion to the
 * app_env.adv_mode polling in APP_BLE_ProcessTask() below (never a sole
 * source of truth -- see that polling loop's own comment for why a single
 * update path here is deliberately avoided).
 *
 * The design decision below (APP_BLE_ProcessTask() polling app_env.adv_mode
 * directly, not relying solely on this callback) remains correct regardless
 * of the above -- kept for the reasons already documented: read
 * rwip_sleep()'s own implementation (rwip_driver.c) -- it's a fully self-
 * contained, duration-aware decision function that computes real time-to-
 * next-scheduled-BLE-event against rwip_env.lp_cycle_wakeup_delay, and only
 * returns RWIP_DEEP_SLEEP when that gap genuinely justifies it. It needs no
 * app-level FAST/SLOW hint to behave correctly during FAST_ADV's tight
 * 100 ms cadence -- it already refuses deep sleep on its own then, the same
 * way it handles CI pacing for the proven app_ble_* reference projects. So
 * APP_BLE_ProcessTask() below only special-cases CONNECTED (a signal that
 * DOES reliably fire, via APP_BLE_MsgHandler -- matches the reference's own
 * established CI-pacing rationale, CLAUDE.md app_ble_lorawan_wmbus bugs
 * 19/32): CONNECTED skips rwip_sleep() entirely and goes straight to plain
 * __WFI(). Every other case -- IDLE, FAST_ADV, SLOW_ADV, all indistinguishable
 * to us now and that's fine -- always consults rwip_sleep() as the sole
 * authority, entering real StopMode (UTIL_LPM_EnterLowPower(), still
 * lpm_if.c's unchanged entry_sleep()-faithful sequence) only when it agrees.
 */

#include "n32wb03x.h"
#include "gapm_task.h"
#include "app_ble.h"
#include "ns_sec.h"
#include "app_dis.h"
#include "app_user_config.h"
#include "app_log.h"
#include "n32_seq.h"
#include "n32_lpm.h"
#include "app_config.h"
#include "rwip.h"
#include "rwip_int.h" /* rwip_env / RW_DEEP_SLEEP -- see file header */
#include "trace_if.h" /* TraceIf_Flush() -- see the rwip_sleep() call site below */

/**
 * @brief BLE state visible to APP_BLE_ProcessTask(), drives CFG_LPM_BLE_Id.
 * Shape mirrors app_ble_lorawan_wmbus/app_entry.h's APP_BLE_State_t.
 * FAST_ADV is reachable -- set from app_env.adv_mode polling in
 * APP_BLE_ProcessTask() below, and as an event-driven companion from
 * APP_BLE_AdvMsgHandler() (see file header) -- SLOW_ADV is never assigned
 * separately, folding into IDLE (APP_BLE_SetState()'s own switch already
 * treats them identically: voter ENABLE). CONNECTED: BLE link active, WFI
 * CI-pacing required -> STOP blocked. Everything else: STOP allowed, gated
 * purely by rwip_sleep().
 */
typedef enum
{
    APP_BLE_STATE_IDLE = 0,
    APP_BLE_STATE_FAST_ADV,
    APP_BLE_STATE_SLOW_ADV,  /* never assigned -- folds into IDLE, see comment above */
    APP_BLE_STATE_CONNECTED,
} APP_BLE_State_t;

static volatile APP_BLE_State_t s_ble_state = APP_BLE_STATE_IDLE;
static uint16_t s_deepsleep_stuck_ctr = 0U;

/* [LPM] diagnostic log state -- ported from app_ble_cellular_wmbus's
 * ble_pump_task() (ns_sleep()/rwip_sleep() visibility was previously omitted
 * when this function was first ported; added to investigate an elevated
 * sleep-current report with no other code-level cause found -- see the
 * plan/CLAUDE.md history for this round). */
static uint16_t s_sleep_ctr = 0U;
static uint8_t s_last_sleep = 0xFFU;

/* APP_BLE_AdvMsgHandler diagnostic: records every invocation via pure RAM
 * writes (safe from any context, including BLE-kernel dispatch) instead of
 * relying solely on the synchronous APP_LOG() call already inside the
 * handler. Proved the callback fires for FAST/SLOW/STOP alike while the
 * synchronous log silently drops most invocations (see file header,
 * "CORRECTION"). Indexed by enum app_adv_mode (IDLE=0, ENABLE=1,
 * DIRECTED=2, FAST=3, SLOW=4, STOP=5); sized 8 for headroom, out-of-range
 * writes clamp to the last slot rather than corrupting memory. Drained and
 * printed from APP_BLE_ProcessTask() (already proven-safe APP_LOG()
 * context) on the next pump cycle after a new call is recorded. */
static volatile uint32_t s_adv_msg_call_count = 0U;
static volatile uint32_t s_adv_msg_mode_count[8] = {0};
static uint32_t s_adv_msg_reported_count = 0U;

static void APP_BLE_GapParamsInit(void);
static void APP_BLE_SecInit(void);
static void APP_BLE_AdvInit(void);
static void APP_BLE_MsgHandler(struct ble_msg_t const *p_ble_msg);
static void APP_BLE_UserMsgHandler(ke_msg_id_t const msgid, void const *p_param);
static void APP_BLE_AdvMsgHandler(enum app_adv_mode adv_mode);
static void APP_BLE_ProcessTask(void);
static void APP_BLE_SetState(APP_BLE_State_t state);
static void APP_BLE_AdvRestartTask(void);

void APP_BLE_Init(void)
{
    struct ns_stack_cfg_t app_handler = {0};

    app_handler.ble_msg_handler  = APP_BLE_MsgHandler;
    app_handler.user_msg_handler = APP_BLE_UserMsgHandler;
    /* LSE, not the internal LSI RC oscillator -- BLE's own advertising-
     * duration timing runs from this clock, and was measured on hardware
     * running ~1.6-1.7x slower than configured (a 60 s fast_adv window took
     * ~100 s) while on BLE_LSC_LSI_32000HZ, identically whether or not real
     * host-CPU StopMode was even being entered (ruling out our own LPM code
     * as the cause -- the BLE radio scheduler is autonomous hardware on its
     * own clock reference). rtc.c's RTC_USE_LSI=0 default already proves
     * this board's 32.768 kHz LSE crystal is populated and precise (RTC has
     * never drifted all session) -- LSE is already running continuously for
     * RTC regardless, so BLE becoming a second consumer of it costs nothing. */
    app_handler.lsc_cfg          = BLE_LSC_LSE_32768HZ;

    ns_ble_stack_init(&app_handler);

    /* Makes the SDK's own silent static default (g_rf_tx_power =
     * TX_POWER_0_DBM, rwip_driver.c) explicit and project-configurable --
     * see BLE_TX_POWER in app_user_config.h. */
    ns_ble_radio_power_set(BLE_TX_POWER);

    APP_BLE_GapParamsInit();
    APP_BLE_SecInit();
    APP_BLE_AdvInit();

#if (BLE_APP_DIS)
    ns_ble_add_prf_func_register(APP_DIS_AddDis);
#endif

    ns_ble_adv_start();

    UTIL_SEQ_RegTask((1U << TASK_BLE_STACK_ID), CFG_SEQ_Prio_1, APP_BLE_ProcessTask);
    UTIL_SEQ_SetTask((1U << TASK_BLE_STACK_ID), CFG_SEQ_Prio_1);

    UTIL_SEQ_RegTask((1U << TASK_BLE_ADV_RESTART), CFG_SEQ_Prio_1, APP_BLE_AdvRestartTask);
}

/**
 * @brief Restarts advertising outside BLE kernel-callback context. Scheduled
 * (never called directly) from APP_BLE_MsgHandler()'s APP_BLE_GAP_DISCONNECTED
 * case -- see that case's comment and the file header for why.
 */
static void APP_BLE_AdvRestartTask(void)
{
    ns_ble_adv_start();
}

/**
 * @brief BLE pump task -- self-perpetuating, see file header for why. Pumps
 * the BLE host+link-layer kernel event queue, then makes the BLE-aware sleep
 * decision for this pass -- ported from app_ble_lorawan_wmbus/
 * app_ble_cellular_wmbus's proven ble_pump_task() (see file header for why
 * this project's earlier unconditional UTIL_LPM_EnterLowPower() call was
 * wrong) -- then re-arms itself.
 */
static void APP_BLE_ProcessTask(void)
{
    rwip_schedule();

    /* Defensive re-arm, every cycle, unconditionally -- matches
     * app_ble_cellular_wmbus's ble_pump_task() exactly ("belt-and-
     * suspenders" per its own comment there). A third, independent re-arm
     * site alongside the two already inside lpm_if.c's LpmIf_EnterStopMode()/
     * ExitStopMode() -- those only run during an actual StopMode transition,
     * so they can't protect EXTI->IMASK from being cleared/corrupted by
     * anything happening outside that narrow window (e.g. while this task
     * is spending a cycle in the CONNECTED branch below, which never
     * touches lpm_if.c at all). */
    EXTI->IMASK |= (EXTI_LINE1 | EXTI_LINE3 | EXTI_LINE9);

    /* Deferred-print drain for APP_BLE_AdvMsgHandler's call recorder --
     * APP_LOG() here runs in the same proven-safe task context already used
     * for the [LPM] diagnostics below, ruling out the "synchronous log call
     * inside the BLE-kernel-dispatch callback silently fails" hypothesis.
     * See the static state's own comment (top of file) for full context. */
    if (s_adv_msg_call_count != s_adv_msg_reported_count)
    {
        s_adv_msg_reported_count = s_adv_msg_call_count;
        APP_LOG(TS_OFF, VLEVEL_M,
                "[BLE] AdvMsgHandler DEFERRED: total=%lu IDLE=%lu ENABLE=%lu DIRECTED=%lu FAST=%lu SLOW=%lu STOP=%lu\r\n",
                (unsigned long)s_adv_msg_call_count,
                (unsigned long)s_adv_msg_mode_count[APP_ADV_MODE_IDLE],
                (unsigned long)s_adv_msg_mode_count[APP_ADV_MODE_ENABLE],
                (unsigned long)s_adv_msg_mode_count[APP_ADV_MODE_DIRECTED],
                (unsigned long)s_adv_msg_mode_count[APP_ADV_MODE_FAST],
                (unsigned long)s_adv_msg_mode_count[APP_ADV_MODE_SLOW],
                (unsigned long)s_adv_msg_mode_count[APP_ADV_MODE_STOP]);
    }

    /* Sync FAST_ADV/IDLE from app_env.adv_mode (SDK's own public global,
     * ns_ble.h) each cycle. Skipped entirely while CONNECTED: that
     * transition is driven by the real, reliable APP_BLE_MsgHandler()
     * connect/disconnect signal and must not be second-guessed by
     * adv_mode, which can read a stale FAST/SLOW value for a cycle or two
     * around a fresh connection. SLOW_ADV/other modes all fold into IDLE
     * here -- APP_BLE_SetState()'s own switch already treats SLOW_ADV and
     * IDLE identically (voter ENABLE), so no finer distinction is needed
     * for behavior, only FAST_ADV is a distinct bucket (voter DISABLE,
     * WFI-only, matching CONNECTED -- see APP_BLE_SetState()'s existing
     * switch-case). Ported from LORAWAN/APP's identical fix -- see
     * CLAUDE.md LORAWAN section, "s_ble_state never actually reached
     * APP_BLE_STATE_FAST_ADV" -- this file's own header comment above
     * argued rwip_sleep() alone is sufficient and no app-level hint is
     * needed; kept for consistency with the sibling projects regardless. */
    if (s_ble_state != APP_BLE_STATE_CONNECTED)
    {
        APP_BLE_State_t adv_derived_state = (app_env.adv_mode == APP_ADV_MODE_FAST)
                                                 ? APP_BLE_STATE_FAST_ADV
                                                 : APP_BLE_STATE_IDLE;
        if (adv_derived_state != s_ble_state)
        {
            APP_BLE_SetState(adv_derived_state);
        }
    }

    if (s_ble_state == APP_BLE_STATE_CONNECTED || s_ble_state == APP_BLE_STATE_FAST_ADV)
    {
        /* STOP already blocked via CFG_LPM_BLE_Id -- calling rwip_sleep()
         * here would set RW_DEEP_SLEEP while we cannot complete a real BLE
         * hardware sleep cycle, latching it. Plain WFI for CI pacing. */
        __WFI();
    }
    else
    {
        uint32_t primask_saved;
        __asm volatile("MRS %0, PRIMASK" : "=r"(primask_saved));
        __asm volatile("CPSID I");

        if (UTIL_LPM_GetMode() == UTIL_LPM_STOPMODE)
        {
            /* Second, independent guarantee (on top of the CFG_LPM_UART_TX_Id
             * voter above) that the trace UART is genuinely idle before any
             * StopMode decision is made -- mirrors the proven old-structure
             * app_ble_cellular_wmbus/app_ble_lorawan_wmbus ble_pump_task()
             * exactly: temporarily restore PRIMASK so the TX-complete ISR can
             * actually run and drain the FIFO while this spins, then re-mask
             * before proceeding. Ported from LORAWAN/APP's identical fix --
             * see trace_if.h's TraceIf_Flush() doc comment and CLAUDE.md
             * LORAWAN section ("garbled trace UART output" investigation)
             * for why the voter alone was not sufficient there. */
            __asm volatile("MSR PRIMASK, %0" : : "r"(primask_saved));
            TraceIf_Flush();
            __asm volatile("CPSID I");

            /* Voter allows STOP -- now safe to ask the BLE kernel itself
             * whether it is actually ready for a real hardware sleep cycle. */
            uint8_t sleep_type = rwip_sleep();

            /* RW_DEEP_SLEEP (0x0008) stuck-latch watchdog -- normally cleared
             * by rwip_wakeup() after the BLE HW sleep cycle completes. If the
             * BLE wakeup IRQ is ever missed, it stays set forever and every
             * later rwip_sleep() call returns RWIP_CPU_SLEEP permanently.
             * Force-clear after ~30 ms of that state (10 iterations). */
            if (((sleep_type == RWIP_CPU_SLEEP) || (sleep_type == RWIP_ACTIVE))
                && (rwip_env.prevent_sleep & RW_DEEP_SLEEP))
            {
                if (++s_deepsleep_stuck_ctr >= 10U)
                {
                    s_deepsleep_stuck_ctr = 0U;
                    rwip_prevent_sleep_clear(RW_DEEP_SLEEP);
                    /* Commented out -- serial log flood. Uncomment for LPM/BLE
                     * sleep debugging if the stuck-latch watchdog needs tracing. */
                    // APP_LOG(TS_OFF, VLEVEL_M, "[BLE] cleared stuck RW_DEEP_SLEEP prev=0x%04x\r\n",
                    //         (unsigned)rwip_env.prevent_sleep);
                }
            }
            else
            {
                s_deepsleep_stuck_ctr = 0U;
            }

            if (sleep_type == RWIP_DEEP_SLEEP)
            {
                /* Both voter and BLE kernel agree: real StopMode. */
                UTIL_LPM_EnterLowPower();
            }
            else
            {
                /* BLE kernel not ready yet -- light WFI only. */
                __WFI();
            }

            /* Restore PRIMASK HERE, immediately after the sleep decision --
             * matches app_ble_cellular_wmbus's ble_pump_task() exactly.
             * Deliberately NOT a single shared restore after the whole
             * if/else (as this function had before): the [LPM] diagnostic
             * log below calls APP_LOG(), which starts an interrupt-driven
             * trace UART TX via PreSendHook() -- with PRIMASK still 1, that
             * TX's completion interrupt cannot be serviced. During the
             * rwip_sleep()==2/1 alternation this log fires on *every*
             * cycle (sleep_type always differs from the previous one),
             * hitting that race far more often than the rare, occasional
             * case CFG_LPM_UART_TX_Id's force-release (LpmIf_ExitStopMode())
             * and the stuck-latch watchdog above were sized for -- confirmed
             * on hardware: StopMode logging stopped permanently within ~3 s
             * of the wake window expiring, current never dropping. */
            __asm volatile("MSR PRIMASK, %0" : : "r"(primask_saved));

            /* [LPM] diagnostic: log on first call in a new sleep_type, or
             * every 200 calls -- see the static state's comment above for why. */
            if ((sleep_type != s_last_sleep) || (++s_sleep_ctr >= 200U))
            {
                s_last_sleep = sleep_type;
                s_sleep_ctr = 0U;
                /* Commented out -- serial log flood. Uncomment for LPM/BLE
                 * sleep debugging. */
                // APP_LOG(TS_OFF, VLEVEL_M,
                //         "[LPM] rwip_sleep=%d lpm=%d state=%d prev=0x%04x\r\n",
                //         (int)sleep_type, (int)UTIL_LPM_GetMode(),
                //         (int)s_ble_state, (unsigned)rwip_env.prevent_sleep);
            }
        }
        else
        {
            /* Voter blocks STOP (FAST_ADV/CONNECTED already handled above --
             * this covers e.g. the 60 s wake window). Plain WFI, no
             * rwip_sleep() call. Restore PRIMASK immediately after, matching
             * the reference -- see the comment above for why this can't be
             * a single shared restore after the whole if/else. */
            __WFI();
            __asm volatile("MSR PRIMASK, %0" : : "r"(primask_saved));
        }
    }

    UTIL_SEQ_SetTask((1U << TASK_BLE_STACK_ID), CFG_SEQ_Prio_1);
}

/**
 * @brief Update BLE state and the CFG_LPM_BLE_Id StopMode voter accordingly.
 * See the enum's own doc comment and the file header for the rationale.
 */
static void APP_BLE_SetState(APP_BLE_State_t state)
{
    s_ble_state = state;
    s_deepsleep_stuck_ctr = 0U;
    s_last_sleep = 0xFFU; /* force a [LPM] diagnostic log on the first pump cycle in the new state */
    s_sleep_ctr = 0U;

    switch (state)
    {
        case APP_BLE_STATE_FAST_ADV:
        case APP_BLE_STATE_CONNECTED:
            UTIL_LPM_SetStopMode((1U << CFG_LPM_BLE_Id), UTIL_LPM_DISABLE);
            break;

        case APP_BLE_STATE_SLOW_ADV:
        case APP_BLE_STATE_IDLE:
            UTIL_LPM_SetStopMode((1U << CFG_LPM_BLE_Id), UTIL_LPM_ENABLE);
            break;

        default:
            break;
    }
}

static void APP_BLE_GapParamsInit(void)
{
    struct ns_gap_params_t dev_info = {0};
    uint8_t *p_mac = SystemGetMacAddr();

    if (p_mac != NULL)
    {
        memcpy(dev_info.mac_addr.addr, p_mac, BD_ADDR_LEN);
    }
    else
    {
        memcpy(dev_info.mac_addr.addr, "\x01\x02\x03\x04\x05\x06", BD_ADDR_LEN);
    }

    dev_info.mac_addr_type = GAPM_STATIC_ADDR;
    dev_info.appearance    = 0;
    dev_info.dev_role      = GAP_ROLE_PERIPHERAL;

    dev_info.dev_name_len = sizeof(BLE_DEVICE_NAME) - 1;
    memcpy(dev_info.dev_name, BLE_DEVICE_NAME, dev_info.dev_name_len);

    dev_info.dev_conn_param.intv_min = MSECS_TO_UNIT(BLE_MIN_CONN_INTERVAL_MS, MSECS_UNIT_1_25_MS);
    dev_info.dev_conn_param.intv_max = MSECS_TO_UNIT(BLE_MAX_CONN_INTERVAL_MS, MSECS_UNIT_1_25_MS);
    dev_info.dev_conn_param.latency  = BLE_SLAVE_LATENCY;
    dev_info.dev_conn_param.time_out = MSECS_TO_UNIT(BLE_CONN_SUP_TIMEOUT_MS, MSECS_UNIT_10_MS);
    dev_info.conn_param_update_delay = BLE_FIRST_CONN_PARAMS_UPDATE_DELAY_MS;

    ns_ble_gap_init(&dev_info);
}

static void APP_BLE_SecInit(void)
{
    struct ns_sec_init_t sec_init = {0};

    sec_init.rand_pin_enable = false;
    sec_init.pin_code        = 0;

    sec_init.pairing_feat.auth      = (BLE_SEC_BOND | (BLE_SEC_MITM << 2) | (BLE_SEC_LESC << 3) | (BLE_SEC_KEYPRESS << 4));
    sec_init.pairing_feat.iocap     = BLE_SEC_IO_CAPABILITIES;
    sec_init.pairing_feat.key_size  = BLE_SEC_KEY_SIZE;
    sec_init.pairing_feat.oob       = BLE_SEC_OOB;
    sec_init.pairing_feat.ikey_dist = BLE_SEC_IKEY_DIST;
    sec_init.pairing_feat.rkey_dist = BLE_SEC_RKEY_DIST;
    sec_init.pairing_feat.sec_req   = BLE_SEC_REQ_LEVEL;

    sec_init.bond_enable   = BLE_BOND_STORE_ENABLE;
    sec_init.bond_db_addr  = BLE_BOND_DATA_BASE_ADDR;
    sec_init.bond_max_peer = BLE_MAX_BOND_PEER;
    sec_init.bond_sync_delay = 2000;

    sec_init.ns_sec_msg_handler = NULL;

    ns_sec_init(&sec_init);
}

static void APP_BLE_AdvInit(void)
{
    struct ns_adv_params_t user_adv = {0};

    user_adv.adv_data_len = BLE_ADV_DATA_LEN;
    memcpy(user_adv.adv_data, BLE_ADV_DATA, BLE_ADV_DATA_LEN);
    user_adv.scan_rsp_data_len = BLE_ADV_SCAN_RSP_DATA_LEN;

    user_adv.attach_appearance = false;
    user_adv.ex_adv_enable     = false;
    user_adv.adv_phy           = PHY_1MBPS_VALUE;

#if (BLE_ADV_CONNECTABLE == 1)
    user_adv.beacon_enable = false;
    user_adv.attach_name   = true;
#else
    user_adv.beacon_enable = true;
    user_adv.attach_name   = false;
#endif

    user_adv.directed_adv.enable = false;

    /* .duration is raw seconds, NOT pre-converted to 10ms units -- despite
     * struct adv_time_t's own field comment claiming "in unit of 10ms",
     * ns_ble.c's app_start_advertising() (the only reader of this field)
     * applies its own SECS_TO_UNIT(..., SECS_UNIT_10MS) conversion before
     * writing the uint16_t GAPM duration field. Pre-converting here as well
     * double-converts and silently truncates on the uint16_t assignment
     * (60s -> 6000 -> 600000 -> truncated to 10176 -> 101.76s, exactly the
     * ~101s fast_adv window measured on hardware across three rounds,
     * completely independent of BLE's LSC clock source -- confirmed not a
     * clock-accuracy issue). Matches the vendor's own ble/beacon and
     * ble/rdtss examples, which assign a raw-seconds macro directly with no
     * conversion (CUSTOM_ADV_FAST_DURATION, documented "units of 1 seconds,
     * maximum is 655 seconds" -- 655*100=65500, just under the uint16_t
     * ceiling, confirming the SDK's single internal conversion is the only
     * one that should ever happen). */
    user_adv.fast_adv.enable   = true;
    user_adv.fast_adv.duration = BLE_ADV_FAST_DURATION_SEC;
    user_adv.fast_adv.adv_intv = BLE_ADV_FAST_INTERVAL;

    user_adv.slow_adv.enable   = true;
    user_adv.slow_adv.duration = BLE_ADV_SLOW_DURATION_SEC; /* 0 = forever, unaffected either way */
    user_adv.slow_adv.adv_intv = BLE_ADV_SLOW_INTERVAL;

    user_adv.ble_adv_msg_handler = APP_BLE_AdvMsgHandler;

    ns_ble_adv_init(&user_adv);
}

static void APP_BLE_MsgHandler(struct ble_msg_t const *p_ble_msg)
{
    switch (p_ble_msg->msg_id)
    {
        case APP_BLE_OS_READY:
            APP_LOG(TS_OFF, VLEVEL_M, "[BLE] stack ready\r\n");
            break;

        case APP_BLE_GAP_CONNECTED:
            APP_LOG(TS_OFF, VLEVEL_M, "[BLE] connected\r\n");
            APP_BLE_SetState(APP_BLE_STATE_CONNECTED);
            break;

        case APP_BLE_GAP_DISCONNECTED:
            APP_LOG(TS_OFF, VLEVEL_M, "[BLE] disconnected -- restarting advertising\r\n");
            /* Symmetric with CONNECTED's SetState() above -- without this,
             * s_ble_state stays stuck at CONNECTED forever after the first
             * disconnect, permanently blocking CFG_LPM_BLE_Id AND permanently
             * routing APP_BLE_ProcessTask() into its plain-WFI branch (never
             * calling rwip_sleep() again) -- confirmed on hardware: StopMode
             * worked correctly pre-connection (~1.2 uA) but stayed stuck at
             * ~1 mA (WFI-only) for every ADV state after the first connect+
             * disconnect cycle. */
            APP_BLE_SetState(APP_BLE_STATE_IDLE);
            /* Deferred, NOT called directly -- ns_ble_adv_start() from inside
             * this kernel-context message handler is a confirmed anti-pattern
             * (app_ble_cellular_wmbus bugs 23/29/31: controller resources not
             * yet freed here, corrupting state for the *next* connection --
             * an intermittent failure mode, not every time, matching what was
             * observed on hardware). TASK_BLE_ADV_RESTART runs
             * APP_BLE_AdvRestartTask() on the next UTIL_SEQ_Run() pass,
             * fully outside this callstack. */
            UTIL_SEQ_SetTask((1U << TASK_BLE_ADV_RESTART), CFG_SEQ_Prio_1);
            break;

        default:
            break;
    }
}

static void APP_BLE_UserMsgHandler(ke_msg_id_t const msgid, void const *p_param)
{
    (void)msgid;
    (void)p_param;
}

/**
 * @brief Registered as ns_adv_params_t.ble_adv_msg_handler. Correctly wired
 * and structurally reachable -- see the file header's CORRECTION (the
 * earlier "dead code" claim checked the wrong file; the real call site is
 * ns_ble_task.c's gapm_cmp_evt_handler). Hardware-confirmed genuinely
 * firing (see file header). Deliberately does NOT drive the CFG_LPM_BLE_Id
 * voter directly (only APP_BLE_SetState() does that) -- see
 * APP_BLE_ProcessTask()/file header for why rwip_sleep() alone is trusted
 * as the StopMode authority regardless of whether this callback fires.
 */
static void APP_BLE_AdvMsgHandler(enum app_adv_mode adv_mode)
{
    /* Unconditional, every invocation -- the switch below only prints for
     * FAST/SLOW/STOP; if adv_mode is DIRECTED or some other value it falls
     * through to default: break with zero output, indistinguishable from
     * the callback never having been called at all. */
    APP_LOG(TS_OFF, VLEVEL_M, "[BLE] AdvMsgHandler \r\n");

    /* Deferred-print recording -- pure RAM writes, safe from any context.
     * See the static state's own comment for why. */
    {
        uint8_t idx = ((uint8_t)adv_mode < 8U) ? (uint8_t)adv_mode : 7U;
        s_adv_msg_mode_count[idx]++;
        s_adv_msg_call_count++;
    }

    /* Event-driven companion to APP_BLE_ProcessTask()'s app_env.adv_mode
     * polling above -- not a replacement. The polling loop's own
     * `if (adv_derived_state != s_ble_state)` guard means this call and the
     * poll's own call naturally de-duplicate; if this callback's dispatch
     * ever became unreliable again, the poll alone still keeps s_ble_state
     * correct, unchanged from today. Never overrides CONNECTED, mirroring
     * the polling loop's own documented reason exactly. APP_LOG()-free
     * inside APP_BLE_SetState() itself -- safe to call from this
     * BLE-kernel-dispatch context (pure RAM writes + voter bitmask
     * update). */
    if (s_ble_state != APP_BLE_STATE_CONNECTED)
    {
        APP_BLE_SetState((adv_mode == APP_ADV_MODE_FAST) ? APP_BLE_STATE_FAST_ADV : APP_BLE_STATE_IDLE);
    }

    switch (adv_mode)
    {
        case APP_ADV_MODE_FAST:
            APP_LOG(TS_OFF, VLEVEL_M, "[BLE] advertising: FAST (%u ms interval)\r\n", (unsigned)(BLE_ADV_FAST_INTERVAL * 625U / 1000U));
            break;

        case APP_ADV_MODE_SLOW:
            APP_LOG(TS_OFF, VLEVEL_M, "[BLE] advertising: SLOW (%u ms interval)\r\n", (unsigned)(BLE_ADV_SLOW_INTERVAL * 625U / 1000U));
            break;

        case APP_ADV_MODE_STOP:
            APP_LOG(TS_OFF, VLEVEL_M, "[BLE] advertising: stopped\r\n");
            break;

        default:
            break;
    }
}
