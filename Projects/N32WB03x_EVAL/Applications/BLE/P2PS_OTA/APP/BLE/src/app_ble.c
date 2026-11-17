/**
 * @file app_ble.c
 * @brief BLE bring-up for P2PS_OTA/APP. See app_ble.h for the design
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
 *
 * 2026-11-17 UPDATE -- the StopMode-gating paragraphs above (BLE voter, app_env.adv_mode polling, RW_DEEP_SLEEP
 * stuck-latch watchdog, TraceIf_Flush before sleep) describe a superseded design. APP_BLE_ProcessTask() is now the
 * vendor rwip_schedule()/ns_sleep() shape validated for 87 h on LORAWAN_WMBUS_AMR_V20: one PRIMASK section,
 * rwip_sleep() consulted in every BLE state (no BLE voter), vendor post-wake fix-up afterwards. See CLAUDE.md
 * "V20 sleep path ported to the other projects".
 */

#include "n32wb03x.h"
#include "gapm_task.h"
#include "app_ble.h"
#include "ns_sec.h"
#include "app_dis.h"
#include "app_p2ps.h"
#include "app_fuota.h"
#include "app_user_config.h"
#include "user_config.h" /* BLE_SESSION_TIMEOUT_SEC */
#include "app_log.h"
#include "n32_seq.h"
#include "n32_timer.h"
#include "n32_lpm.h"
#include "app_config.h"
#include "rwip.h"
#include "rwip_int.h" /* rwip_env / RW_DEEP_SLEEP -- see file header */
#include "lpm_if.h"    /* LpmIf_EntryIdle() */
#include "ke_event.h"  /* ke_event_set() -- post-wake KE timer re-arm */
#include "co_utils.h"  /* CLK_DIFF */
#include "iwdg_if.h"   /* IwdgIf_Feed() -- hardware watchdog, see app_config.h IWDG_ENABLE */
#include "trace_if.h" /* TraceIf_Flush() -- see the rwip_sleep() call site below */

/**
 * @brief BLE connection state (IsConnected() only; no LPM voter -- rwip_sleep() governs sleeping).
 * Shape mirrors app_ble_lorawan_wmbus/app_entry.h's APP_BLE_State_t, but only
 * IDLE and CONNECTED are ever actually assigned in this project -- FAST_ADV/
 * SLOW_ADV are kept for shape-parity with the reference but unreachable here,
 * since the SDK callback that would set them is confirmed dead (see file
 * header). CONNECTED: BLE link active, WFI CI-pacing required -> STOP
 * blocked. Everything else: STOP allowed, gated purely by rwip_sleep().
 */
typedef enum
{
    APP_BLE_STATE_IDLE = 0,
    APP_BLE_STATE_FAST_ADV,
    APP_BLE_STATE_SLOW_ADV,  /* never assigned -- folds into IDLE, see comment above */
    APP_BLE_STATE_CONNECTED,
} APP_BLE_State_t;

static volatile APP_BLE_State_t s_ble_state = APP_BLE_STATE_IDLE;
/* Forces a disconnect after BLE_SESSION_TIMEOUT_SEC (user_config.h) of an
 * open connection -- ported from app_ble_cellular_wmbus's app_ble.c, fully
 * generic (bounds how long an authenticated P2PS session can stay open). */
static UTIL_TIMER_Object_t s_ble_session_timer;

static void APP_BLE_GapParamsInit(void);
static void APP_BLE_SecInit(void);
static void APP_BLE_AdvInit(void);
static void APP_BLE_MsgHandler(struct ble_msg_t const *p_ble_msg);
static void APP_BLE_UserMsgHandler(ke_msg_id_t const msgid, void const *p_param);
static void APP_BLE_AdvMsgHandler(enum app_adv_mode adv_mode);
static void APP_BLE_ProcessTask(void);
static void APP_BLE_SetState(APP_BLE_State_t state);
static void APP_BLE_AdvRestartTask(void);
static void APP_BLE_SessionTimeoutTask(void);
static void APP_BLE_SessionTimerEvent(void *context);

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
#if (BLE_APP_P2PS)
    ns_ble_add_prf_func_register(APP_P2PS_AddP2ps);
#endif
#if (BLE_APP_FUOTA)
    ns_ble_add_prf_func_register(APP_FUOTA_AddFuota);
#endif

    ns_ble_adv_start();

    UTIL_SEQ_RegTask((1U << TASK_BLE_STACK_ID), CFG_SEQ_Prio_1, APP_BLE_ProcessTask);
    UTIL_SEQ_SetTask((1U << TASK_BLE_STACK_ID), CFG_SEQ_Prio_1);

    UTIL_SEQ_RegTask((1U << TASK_BLE_ADV_RESTART), CFG_SEQ_Prio_1, APP_BLE_AdvRestartTask);

    UTIL_SEQ_RegTask((1U << TASK_BLE_SESSION_TIMEOUT), CFG_SEQ_Prio_0, APP_BLE_SessionTimeoutTask);
    UTIL_TIMER_Create(&s_ble_session_timer, (uint32_t)BLE_SESSION_TIMEOUT_SEC * 1000U,
                       UTIL_TIMER_ONESHOT, APP_BLE_SessionTimerEvent, NULL);
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
 * @brief UTIL_TIMER callback -- defers to TASK_BLE_SESSION_TIMEOUT so the
 * actual disconnect runs in sequencer context, not inside the timer's own
 * (interrupt-adjacent) callstack.
 */
static void APP_BLE_SessionTimerEvent(void *context)
{
    (void)context;
    UTIL_SEQ_SetTask((1U << TASK_BLE_SESSION_TIMEOUT), CFG_SEQ_Prio_0);
}

/**
 * @brief Bounds how long an authenticated P2PS session can stay open --
 * ported from app_ble_cellular_wmbus's app_ble.c. Timer is started on
 * connect and stopped on disconnect (see APP_BLE_MsgHandler()).
 */
static void APP_BLE_SessionTimeoutTask(void)
{
    APP_LOG(TS_OFF, VLEVEL_M, "[BLE] session timeout (%u s) -- disconnecting\r\n",
            (unsigned)BLE_SESSION_TIMEOUT_SEC);
    ns_ble_disconnect();
}

/* Iteration budget of the vendor's post-wake wait (ns_sleep.c "2*800us"). */
#define POST_WAKE_WAIT_ITERS 12800U

/**
 * @brief ns_sleep() step 5 (vendor ns_library/sleep/ns_sleep.c), run with interrupts enabled.
 * (a) While a BB sleep cycle is still completing (WAKE_UP_ONGOING|DEEP_SLEEP set) spin, bounded, so
 *     the BLE_SLP / slot ISRs it waits for can run. (b) Vendor fix V1.3.0 #18: if the 1 ms kernel
 *     timer target expired while the BB slept, force KE_EVENT_KE_TIMER (that compare IRQ can be missed).
 */
static void APP_BLE_PostWakeFixup(void)
{
    volatile uint16_t *const p_prevent = (volatile uint16_t *)&rwip_env.prevent_sleep;
    uint32_t n = POST_WAKE_WAIT_ITERS;

    while (((*p_prevent) & (RW_WAKE_UP_ONGOING | RW_DEEP_SLEEP)) != 0U)
    {
        if (--n == 0U)
        {
            break;
        }
    }

    const uint32_t target_hs = rwip_env.timer_1ms_target.hs;
    if (target_hs != RWIP_INVALID_TARGET_TIME)
    {
        const rwip_time_t now = rwip_time_get();
        if (CLK_DIFF(now.hs, target_hs) < 0)
        {
            ke_event_set(KE_EVENT_KE_TIMER);
        }
    }
}

/**
 * @brief BLE pump task, vendor shape: rwip_schedule(); ns_sleep();
 *
 * ns_sleep() equivalent, in ONE PRIMASK section so that "may we sleep?" and "enter sleep" cannot be
 * separated by an interrupt (no flush window, no commit-then-decline):
 *   - app voters (UART TX in flight, wake window, FUOTA, LOW_POWER_DISABLE) block sleeping entirely:
 *     rwip_sleep() is NOT called (it commits RW_DEEP_SLEEP + the BB wake-up as a side effect), plain WFI.
 *   - otherwise rwip_sleep() decides, in EVERY BLE state (FAST_ADV / CONNECTED included, as the OEM
 *     examples do -- no BLE voter): DEEP_SLEEP -> BB + MCU deep sleep (LpmIf_EnterStopMode via the LPM
 *     engine), CPU_SLEEP -> entry_idle(), ACTIVE -> nothing (loop again).
 * After PRIMASK is restored the vendor post-wake step runs (APP_BLE_PostWakeFixup()).
 * No trace flush: a trace TX in flight holds the UART_TX voter, which keeps us out of deep sleep.
 */
static void APP_BLE_ProcessTask(void)
{
    IwdgIf_SelfTestPoll(); /* no-op unless IWDG_SELFTEST */

    rwip_schedule();

    uint32_t primask;
    __asm volatile("mrs %0, primask\n\tcpsid i" : "=r"(primask) : : "memory");

    bool slept = false;
    if (UTIL_LPM_GetMode() == UTIL_LPM_STOPMODE)
    {
        switch (rwip_sleep())
        {
            case RWIP_DEEP_SLEEP:
                UTIL_LPM_EnterLowPower(); /* voters unchanged under PRIMASK -> STOPMODE tier */
                slept = true;
                break;
            case RWIP_CPU_SLEEP:
                LpmIf_EntryIdle();
                slept = true;
                break;
            default: /* RWIP_ACTIVE: kernel has work, go round again */
                break;
        }
    }
    else
    {
        __WFI();
    }

    __asm volatile("msr primask, %0" : : "r"(primask) : "memory");

    /* Hardware watchdog feed: once per pump pass, i.e. after every wake. Only the pump feeds (not timers
     * or ISRs), so a dead pump or a hang in the sleep sequence cannot be hidden. */
    IwdgIf_Feed();

    if (slept)
    {
        APP_BLE_PostWakeFixup();
    }

    UTIL_SEQ_SetTask((1U << TASK_BLE_STACK_ID), CFG_SEQ_Prio_1);
}

/**
 * @brief Update BLE state.
 * See the enum's own doc comment and the file header for the rationale.
 */
static void APP_BLE_SetState(APP_BLE_State_t state)
{
    /* OEM model: no BLE StopMode voter -- rwip_sleep() alone governs sleeping in every BLE state. */
    s_ble_state = state;
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
            UTIL_TIMER_StartWithPeriod(&s_ble_session_timer, (uint32_t)BLE_SESSION_TIMEOUT_SEC * 1000U);
            break;

        case APP_BLE_GAP_DISCONNECTED:
            APP_LOG(TS_OFF, VLEVEL_M, "[BLE] disconnected -- restarting advertising\r\n");
            UTIL_TIMER_Stop(&s_ble_session_timer);
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
 * and structurally reachable -- see the file header's 2026-09-27 correction
 * (the earlier "dead code" claim checked the wrong file; the real call site
 * is ns_ble_task.c's gapm_cmp_evt_handler). Whether it actually fires on
 * this hardware has not been re-verified since that correction. Deliberately
 * does NOT drive any LPM voter -- see APP_BLE_ProcessTask()/file header for
 * why rwip_sleep() alone is trusted for that regardless of whether this
 * callback fires.
 */
static void APP_BLE_AdvMsgHandler(enum app_adv_mode adv_mode)
{
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
