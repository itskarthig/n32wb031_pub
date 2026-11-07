/**
 * @file timer_if.c
 * @brief RTC WakeUp-Timer-backed hardware interface for n32_timer (UTIL_TimerDriver).
 *
 * 2026-11-06 TIMER AUDIT (CLAUDE.md "Timer implementation audit") -- READ FIRST.
 * It SUPERSEDES every statement further down this header that says
 * n32_timer.c's UTIL_TIMER_IRQ_Handler() "re-checks real elapsed time and
 * silently re-arms the remainder" after an early/clamped WUT fire, or that
 * long requests "chain for free". That is FALSE:
 *   - UTIL_TIMER_IRQ_Handler() (a faithful port of STM32CubeWL's) only re-arms
 *     the head when head->IsPending == 0. TimerSetTimeout() sets IsPending=1 and
 *     only a pop / head-replacement clears it, so after an early or clamped
 *     fire NOTHING re-arms the hardware. ST never hits this because its RTC
 *     Alarm is one-shot and never fires early.
 *   - What actually made early fires "work" was the WUT hardware AUTO-RELOAD
 *     (User Manual S20.3.5): it simply fired again one period later. Same
 *     accident made a 120 s TxTimer (> the 65000-tick clamp) fire at 127 s, so
 *     the TX cycle was 4.5 + 2 x 63.5 = 131.5 s, and left the WUT firing
 *     forever after the last timer expired.
 * Fix implemented here: the WUT is ONE-SHOT (disabled at the start of
 * TimerIf_ProcessTask()), the head's IsPending is cleared before the engine
 * runs when the fire was early/clamped (so the engine re-arms the remainder),
 * and that remainder is re-armed at the rate MEASURED on the fire that just
 * happened (so a real-2048 Hz WUT costs exactly two fires, not a halving
 * chain). First hops of every new arm still assume the slow 1024 Hz rate
 * (early is recoverable, late is not). Also: coherent SUBS->TSH->DATE
 * calendar read, bounded + spaced WUT register handshake, WKUPT+1 semantics.
 *
 * 2026-11-07: HARDWARE-CONFIRMED on LORAWAN_WMBUS_AMR (two boards): TX-to-TX
 * 120 s (was 131 s), RX1/RX2 correct, no spurious fires. Ported to every
 * project from that file. The deferral to TASK_RTC_TIMER_SVC exists for the
 * LoRaWAN radio-SPI reason described further below; projects without a radio
 * use it too, only to keep a single code path (the one-shot/early-fire logic
 * runs in task context). The WUT rate (1024 vs 2048 Hz) still alternates for
 * an unknown reason (NOT StopMode-related, see CLAUDE.md) -- this design
 * measures it per fire and does not depend on knowing it.
 *
 * Tick = 1 millisecond, via the RTC WakeUp Timer's RTCCLK/16 clock mode
 * (32768/16 = 2048 Hz, ~488 us/tick) combined with the RTC's sub-second
 * register for millisecond-resolution elapsed-time tracking -- but see the
 * WUT-rate notes below: the 2048 Hz figure is NOT reliable.
 *
 * 2026-11-06 UPDATE (supersedes the voter-based rate guess described in the
 * next paragraph -- see CLAUDE.md LORAWAN_WMBUS_AMR section, "RX1 opens ~2x
 * late / RX2 never opens"): two boards, same firmware, showed the WUT's real
 * rate is NOT predictable from UTIL_LPM_GetMode()/voters or StopMode history.
 * The same 3000 ms boot timer armed identically (assumed 2048 Hz, zero
 * StopMode entries) took 3000 ms on one boot (true 2048 Hz) and 5998-6001 ms
 * on others (true 1024 Hz); the calendar agreed with host wall-clock in
 * every case, so it is the WUT that differs, by an exact 2x. Assuming the
 * wrong rate has an asymmetric cost: a WUT that runs FASTER than assumed
 * fires early -- recoverable, but ONLY because the WUT auto-reloaded and fired
 * again (the engine does not re-arm it itself; see the audit note above, and
 * TimerIf_ProcessTask() for the real recovery now); a WUT that runs SLOWER
 * than assumed fires late and nothing recovers it (RX1 at +2 s instead of
 * +1 s, RX2 skipped). So the FIRST hop of every arm in
 * TimerIf_StartTimerEvt() assumes the slower rate
 * (TIMER_IF_WUT_TICK_HZ / 2 = 1024 Hz), at
 * the cost of an occasional extra wake-up. The root cause of the rate change
 * itself is not established (see the [TIMER] / [RTC] diagnostics, which print
 * RCC_LSCTRL / RTC_CTRL / RTC_PRE to rule the 32 kHz clock chain in or out).
 *
 * WUT RATE DEPENDS ON WHETHER STOPMODE IS CONTINUOUSLY CYCLING (root cause
 * of the whole "RX1/RX2 always time out during FAST_ADV" investigation,
 * hardware-confirmed 2026-09-20 via Projects/.../STANDALONE/LORAWAN -- a
 * BLE-free isolation copy of this project built specifically to test this):
 * whenever the MCU is NOT continuously cycling through genuine StopMode
 * (e.g. this project's own FAST_ADV state, which takes a plain-WFI-only
 * path in app_ble.c's APP_BLE_ProcessTask() and never calls
 * UTIL_LPM_EnterLowPower()), the WUT counts at a fixed real rate of
 * ~1024 Hz, not the documented 2048 Hz -- confirmed in STANDALONE/LORAWAN
 * by holding wut_ticks constant across two arms (9069 and 9946) and
 * finding real_elapsed_ms scales with wut_ticks at a constant ~1024 Hz in
 * both cases, not with the requested delay_ms. A one-time StopMode
 * "training" cycle at boot did NOT fix it there -- the effect requires
 * StopMode to be continuously active, not just entered once. Once StopMode
 * genuinely is cycling (this project's SLOW_ADV/IDLE state), the WUT
 * settles to the documented 2048 Hz and every RX window opens on time.
 *
 * This is an empirical, hardware-measured finding, not one derived from a
 * documented first-principles explanation -- neither the N32 User Manual's
 * WUT section (S20.3.5) nor any N32 SDK example states the WUT's own rate
 * varies by power mode (the closest documented adjacent behavior is
 * S20.3.3's calendar-shadow-register note: "No replication is performed in
 * Standby and Sleep modes" -- the same *class* of mode-dependent RTC
 * behavior, not a precise match), and neither STM32CubeWL nor
 * STM32CubeExpansion_LRWAN's RTC/timer code has anything analogous to
 * check against -- their reference design uses a free-running binary-mode
 * sub-second counter + Alarm compare, never reprogrammed from a
 * decrementing value the way this chip's WUT is, so it is structurally
 * immune to this whole class of issue by design rather than by having
 * solved it. GetTimerValue() below (which measures both this behavior and
 * everything else in this file) reads the very same calendar/sub-second
 * register the S20.3.3 note describes, so whether the true mechanism is
 * "the WUT counter itself ticks slower" or "the calendar read-back used to
 * measure elapsed time is itself skewed during continuous non-StopMode
 * operation," the compensation in TimerIf_StartTimerEvt() below is tuned
 * empirically against this exact same measurement chain and is confirmed
 * fixed end-to-end on hardware in STANDALONE/LORAWAN.
 *
 * Unlike STANDALONE/LORAWAN (which selects a fixed rate at compile time via
 * its LOW_POWER_DISABLE build flag, since that project is either always or
 * never StopMode-cycling for its whole life), this project genuinely needs
 * both regimes within one boot session -- so TimerIf_StartTimerEvt() below
 * checks UTIL_LPM_GetMode() live, at arm time, instead: it is the dynamic
 * equivalent of that static flag, true exactly when no voter (including
 * CFG_LPM_BLE_Id, held DISABLE during CONNECTED/FAST_ADV) blocks StopMode.
 *
 * FIX (see CLAUDE.md LORAWAN section, "RX never detects downlink preamble"
 * investigation, Experiment 5): this file previously ran the WUT in
 * RTC_WKUPCLK_CK_SPRE_16BITS mode (~1-2 real seconds per tick, whole-second
 * GetTimerValue()) -- fine for this file's original use (BEACON/APP-style
 * multi-second heartbeats/wake-windows, inherited unmodified via the
 * BEACON->P2PS_OTA->LORAWAN copy lineage), but LoRaMac.c's own internally
 * -scheduled RX1/RX2 window-open timers (join-accept delay 5000/6000 ms)
 * go through this exact same generic UTIL_TIMER backend and need the
 * LoRaWAN MAC layer's own design tolerance of ~10 ms (SystemMaxRxError) --
 * a ~1-2 second scheduling error is 50-100x beyond that budget, causing a
 * deterministic 100%-of-attempts RX window miss. Rewritten to match
 * utils/st/timer/n32_timer_if.c (the confirmed-working, hardware-proven
 * N32WB031 reference used by app_ble_lorawan_wmbus): RTCCLK/16 WUT clock +
 * millisecond GetTimerValue() via the RTC sub-second register, the same
 * RTC_GetSubSecond() mechanism already used correctly by this project's own
 * systime_if.c (SystimeIf_GetCalendarTime()) and by utils/st/misc/st_systime.c
 * (ST_SysTime_GetMcuTime()).
 *
 * That mode's counter is hard-capped at 16 bits -- StartTimerEvt() clamps
 * to 65000 ticks (63.5 s at the assumed 1024 Hz). UTIL_TIMER_IRQ_Handler()
 * does NOT chain longer requests by itself (see the 2026-11-06 audit note at
 * the top of this header: it re-checks the real elapsed time to decide whether
 * a timer is due, but only re-arms the hardware if the head's IsPending is 0).
 * TimerIf_ProcessTask() therefore clears IsPending after an early/clamped fire
 * so the engine re-arms the remainder -- one hop per clamp period.
 *
 * GetTimerValue() deliberately reads the *raw* RTC calendar + sub-second
 * register directly (not SysTimeGet()/SysTimeGetMcuTime()) -- so the tick
 * base is never touched by a SysTimeSet() correction at all.
 *
 * Structurally immune to SysTimeSet() by construction (ported from the
 * BEACON/P2PS_OTA/STANDALONE-PERIPHERAL redesign, CLAUDE.md's BEACON/APP
 * Bug #20, hardware-confirmed 2026-09-18): systime_if.c's SysTimeSet()
 * path stores its correction as a delta in .noinit RAM (nvm_ram.c) and
 * NEVER rewrites the RTC calendar -- so the calendar this file reads for
 * s_timer_context_ms below can never jump because of a SysTimeSet() call,
 * and no compensation entry point (this file previously exported
 * TimerIf_AdjustContext() for exactly that purpose -- see CLAUDE.md Bug 49)
 * is needed or present anymore.
 *
 * RTC_IRQHandler() defers UTIL_TIMER_IRQ_Handler() to task context via the
 * sequencer instead of calling it directly from ISR context (see
 * RTC_IRQHandler()'s own comment below and CLAUDE.md's LORAWAN section,
 * "RX1/RX2 always time out" investigation) -- required because this
 * project's lpm_if.c deinitializes the radio's SPI1 peripheral/GPIO before
 * StopMode and only restores it afterward, in LpmIf_ExitStopMode(), which
 * always runs strictly after the RTC WUT ISR that woke the CPU has already
 * returned. Calling UTIL_TIMER_IRQ_Handler() (and therefore any expired
 * timer's callback -- for LoRaWAN's RxWindowTimer1/2, real Radio.Standby()/
 * RegionRxConfig()/Radio.Rx() SPI transactions) directly from that ISR ran
 * those SPI commands before SPI1/GPIO had been restored, every time --
 * confirmed as the root cause of RX1/RX2 consistently timing out despite
 * opening at the correct, scheduled frequency/DR.
 */

/** 1 = per-fire [TIMER] log, [RTC] boot register dumps and StopMode-entry
 *  counters (needs lpm_if.h's LpmIf_GetStopEnterCount()). Only
 *  LORAWAN_WMBUS_AMR enables it (investigation tooling, not part of the fix). */
#ifndef TIMER_IF_DIAG
#define TIMER_IF_DIAG 0
#endif

#include "timer_if.h"
#include "n32wb03x.h"
#include "rtc.h"
#include "n32_systime.h"
#include "n32_seq.h"
#include "n32_lpm.h"
#include "app_config.h"
#include "app_log.h"
#if TIMER_IF_DIAG
#include "lpm_if.h"   /* LpmIf_GetStopEnterCount() -- diagnostic only */
#endif
#include <stdbool.h>
#include <string.h>

/** WUT clock: RTCCLK/16 = 32768/16 = 2048 Hz, 1 tick ~= 488 us. */
#define TIMER_IF_WUT_TICK_HZ   2048U

/** WUT counter ceiling -- leave headroom below 65535 (16-bit register). */
#define TIMER_IF_WUT_MAX_TICKS 65000U   /* ~= 31.74 s */

/** RTC synchronous prescaler value (rtc.c: RTC_SynchPrediv = 0x3FF, 1024
 * steps/s, ~0.98 ms resolution) -- must track rtc.c's RTC_SynchPrediv
 * exactly, and stay in sync with systime_if.c's own independent copy of
 * this same value (SYSTIME_RTC_SYNC_PREDIV), same hardware register. */
#define TIMER_IF_RTC_SYNC_PREDIV 0x3FFU

static uint32_t s_timer_context_ms;

/* Defined in the SDK's system_n32wb03x.c, not declared in any header. */
extern void system_delay_n_10us(uint32_t value);

/** User Manual S20.3.10: after WTWF reads 1 "it needs to be delayed by about
 * 2 RTCCLK clock cycles (due to clock synchronization)" before WKUPT /
 * WKUPSEL may be written. 2 cycles = ~61 us at 32.768 kHz -> 7 x 10 us. */
#define TIMER_IF_WTWF_SETTLE_10US 7U

/** First hop of every arm assumes the SLOWER of the two observed WUT rates:
 * an early fire is recoverable (re-armed at the measured rate by
 * TimerIf_ProcessTask()), a late fire is not. See file header. */
#define TIMER_IF_WUT_RATE_SLOW_HZ (TIMER_IF_WUT_TICK_HZ / 2U)

/** A fire is only used to MEASURE the real WUT rate if the armed interval was
 * at least this long: the calendar timestamp has ~1 ms granularity, so shorter
 * intervals give a useless ratio. */
#define TIMER_IF_RATE_MIN_MEASURE_MS 16U

/** Measured rate is accepted only inside this window (anything else is a
 * glitch / stale read -> fall back to the slow assumption). */
#define TIMER_IF_RATE_MIN_HZ  512U
#define TIMER_IF_RATE_MAX_HZ  4096U

/* Per-arm bookkeeping. The WUT is ONE-SHOT: StartTimerEvt() arms it,
 * TimerIf_ProcessTask() disables it on the fire. s_arm_* describe the arm that
 * is currently outstanding (last arm wins -- the WUT is simply reprogrammed). */
static volatile uint32_t s_arm_t0_ms;       /* GetTimerValue() right after WTEN=1 */
static volatile uint32_t s_arm_ticks;       /* ticks armed (period = WKUPT+1 = this) */
static volatile uint32_t s_arm_hz;          /* rate the ticks were computed with */
static volatile uint32_t s_arm_delay_ms;    /* requested delay */
static volatile uint8_t  s_wut_armed;       /* an arm is outstanding */

/* Rate used for the NEXT StartTimerEvt() only while TimerIf_ProcessTask() is
 * re-arming a remainder after an early/clamped fire (0 = none -> slow rate). */
static uint32_t s_rate_override_hz;

/* Last successfully measured rate, reused ONLY for remainder re-arms whose own
 * interval was too short to measure (never for a fresh arm). */
#define TIMER_IF_RATE_REUSE_MS 2000U
static uint32_t s_last_meas_hz;
static uint32_t s_last_meas_ms;

/* Diagnostics (2026-11-06 timer audit, Step 0). Counters are plain RAM so the
 * hot path never logs; TimerIf_ProcessTask() reports them. */
#define TIMER_IF_DIAG_MAX_DELAY_MS 6000U /* only log LoRaMac RX-window-sized timers */
#if TIMER_IF_DIAG
static volatile uint32_t s_arm_stop_cnt;
static volatile int      s_arm_mode;
#endif
static volatile uint32_t s_wkupt_mismatch_cnt;  /* WKUPT readback != value written */
static volatile uint32_t s_wtwf_timeout_cnt;    /* bounded WTWF wait gave up */
static volatile uint32_t s_fire_cnt;            /* WUT fires processed */
static volatile uint32_t s_spurious_cnt;        /* fires with no outstanding arm */

/* Optional scope hook: define TIMER_IF_DEBUG_GPIO_PORT / TIMER_IF_DEBUG_GPIO_PIN
 * (a free pin you wired a probe to) and the pin goes HIGH at every WUT arm and
 * LOW at RTC_IRQHandler() entry -> pulse width = arm->fire time, measured
 * independently of the RTC calendar read chain behind every "real=" number.
 * Off by default (no pin is assumed free on this board). */
#if defined(TIMER_IF_DEBUG_GPIO_PORT) && defined(TIMER_IF_DEBUG_GPIO_PIN)
#define TIMER_IF_DBG_PIN_INIT()                                                   \
    do {                                                                          \
        GPIO_InitType gi = {0};                                                   \
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB |   \
                                RCC_APB2_PERIPH_AFIO, ENABLE);                    \
        gi.Pin       = TIMER_IF_DEBUG_GPIO_PIN;                                   \
        gi.GPIO_Mode = GPIO_MODE_OUTPUT_PP;                                       \
        gi.GPIO_Pull = GPIO_NO_PULL;                                              \
        GPIO_InitPeripheral(TIMER_IF_DEBUG_GPIO_PORT, &gi);                       \
        GPIO_ResetBits(TIMER_IF_DEBUG_GPIO_PORT, TIMER_IF_DEBUG_GPIO_PIN);        \
    } while (0)
#define TIMER_IF_DBG_PIN_HIGH() GPIO_SetBits(TIMER_IF_DEBUG_GPIO_PORT, TIMER_IF_DEBUG_GPIO_PIN)
#define TIMER_IF_DBG_PIN_LOW()  GPIO_ResetBits(TIMER_IF_DEBUG_GPIO_PORT, TIMER_IF_DEBUG_GPIO_PIN)
#else
#define TIMER_IF_DBG_PIN_INIT() do { } while (0)
#define TIMER_IF_DBG_PIN_HIGH() do { } while (0)
#define TIMER_IF_DBG_PIN_LOW()  do { } while (0)
#endif

/* ---------------------------------------------------------------------------
 * Tick source -- raw RTC calendar + sub-second register, in milliseconds
 * (see file header for why not SysTimeGet()/SysTimeGetMcuTime())
 * ------------------------------------------------------------------------- */

static uint32_t TimerIf_GetTimerValue(void)
{
    RTC_DateType date;
    RTC_TimeType time;
    struct tm calendar_tm;
    uint32_t raw_subsec;
    uint32_t whole_sec_epoch;
    uint32_t subsec_ms;

    /* Coherent SUBS -> TSH -> DATE snapshot (User Manual S20.3.11); the old
     * TSH -> DATE -> SUBS order could mix two shadow snapshots across a second
     * rollover and step the timestamp back ~1000 ms (audit finding 2). */
    APP_RTC_GetDateTimeSubSec(&date, &time, &raw_subsec);

    memset(&calendar_tm, 0, sizeof(calendar_tm));
    calendar_tm.tm_year = (int)date.Year + 100; /* RTC 2-digit year (00-99) -> struct tm since-1900, 20xx assumed */
    calendar_tm.tm_mon  = (int)date.Month - 1;  /* struct tm months are 0-11 */
    calendar_tm.tm_mday = (int)date.Date;
    calendar_tm.tm_hour = (int)time.Hours;
    calendar_tm.tm_min  = (int)time.Minutes;
    calendar_tm.tm_sec  = (int)time.Seconds;

    whole_sec_epoch = SysTimeMkTime(&calendar_tm);

    /* User Manual S20.4.10 (RTC_SUBS): "SS can be greater than DIVS only
     * after performing [a shift] operation... the correct time/date is one
     * second behind" -- clamp before subtracting to avoid uint32_t
     * underflow/overflow garbage (same fix as systime_if.c's
     * SystimeIf_GetCalendarTime(), hardware-confirmed necessary there --
     * see CLAUDE.md's BEACON/APP Bug #20 addendum). */
    if (raw_subsec > TIMER_IF_RTC_SYNC_PREDIV)
    {
        raw_subsec = TIMER_IF_RTC_SYNC_PREDIV;
    }

    /* SSR counts down from the sync prescaler reload value each second --
     * (PREDIV - raw_subsec) is elapsed sub-second ticks since the last
     * whole-second rollover. Same conversion as systime_if.c's
     * SystimeIf_GetCalendarTime(). */
    subsec_ms = ((TIMER_IF_RTC_SYNC_PREDIV - raw_subsec) * 1000U) / (TIMER_IF_RTC_SYNC_PREDIV + 1U);

    return (whole_sec_epoch * 1000U) + subsec_ms;
}

static uint32_t TimerIf_SetTimerContext(void)
{
    s_timer_context_ms = TimerIf_GetTimerValue();
    return s_timer_context_ms;
}

static uint32_t TimerIf_GetTimerContext(void)
{
    return s_timer_context_ms;
}

static uint32_t TimerIf_GetTimerElapsedTime(void)
{
    /* Unsigned subtraction -- wraparound-safe, matching n32_timer.c's own
     * "intentional wrap around" design. */
    return TimerIf_GetTimerValue() - s_timer_context_ms;
}

/* ---------------------------------------------------------------------------
 * Tick <-> ms conversion -- 1 tick = 1 ms (identity), now that
 * GetTimerValue() itself returns real milliseconds.
 * ------------------------------------------------------------------------- */

static uint32_t TimerIf_ms2Tick(uint32_t timeMs)
{
    return timeMs;
}

static uint32_t TimerIf_Tick2ms(uint32_t tick)
{
    return tick;
}

static uint32_t TimerIf_GetMinimumTimeout(void)
{
    return 1U; /* 1 ms */
}

/* ---------------------------------------------------------------------------
 * Driver functions
 * ------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * WUT register handshake helpers (User Manual S20.3.9 / S20.3.10)
 * ------------------------------------------------------------------------- */

/** Bounded WTWF wait (iterations). The SDK's RTC_EnableWakeUp(DISABLE) bounds
 * it at 0x2000; the old unbounded loops here could hang the MCU inside the
 * engine's critical section if the RTC clock ever stopped. */
#define TIMER_IF_WTWF_GUARD 200000U

/**
 * @brief Clear WTEN and wait until the hardware acknowledges (WTWF=1) plus the
 *        ~2 RTCCLK synchronisation delay the manual requires before WKUPT may
 *        be written (S20.3.10).
 * @note  RTC write protection MUST already be disabled by the caller
 *        (RTC_EnableWriteProtection(DISABLE)) -- one unlock per operation
 *        instead of the ~9 RTC_CTRL changes the stacked SDK setters made
 *        (S20.3.9: "each change of RTC_CTRL needs ~2 RTCCLK").
 * @return true if WTWF was seen, false if the bounded wait gave up.
 */
static bool TimerIf_WutDisableLocked(void)
{
    uint32_t guard = TIMER_IF_WTWF_GUARD;

    RTC->CTRL &= ~RTC_CTRL_WTEN;
    while (((RTC->INITSTS & RTC_INITSTS_WTWF) == 0U) && (guard != 0U))
    {
        guard--;
    }
    system_delay_n_10us(TIMER_IF_WTWF_SETTLE_10US);

    if (guard == 0U)
    {
        s_wtwf_timeout_cnt++;
        return false;
    }
    return true;
}

/**
 * @brief Measure the real WUT rate from a fire: armed ticks / elapsed time.
 * @return rate in Hz, snapped to the two nominal values (2048 / 1024) when
 *         within 12 %, or 0 if the interval was too short / the result is
 *         implausible (caller then keeps the slow assumption).
 */
static uint32_t TimerIf_MeasureRateHz(uint32_t ticks, uint32_t real_ms)
{
    uint32_t hz;

    if ((ticks == 0U) || (real_ms < TIMER_IF_RATE_MIN_MEASURE_MS))
    {
        return 0U;
    }

    hz = ((ticks * 1000U) + (real_ms / 2U)) / real_ms; /* ticks <= 65000 -> no overflow */

    if ((hz < TIMER_IF_RATE_MIN_HZ) || (hz > TIMER_IF_RATE_MAX_HZ))
    {
        return 0U;
    }
    if ((hz * 100U >= TIMER_IF_WUT_TICK_HZ * 88U) && (hz * 100U <= TIMER_IF_WUT_TICK_HZ * 112U))
    {
        return TIMER_IF_WUT_TICK_HZ;
    }
    if ((hz * 100U >= TIMER_IF_WUT_RATE_SLOW_HZ * 88U) && (hz * 100U <= TIMER_IF_WUT_RATE_SLOW_HZ * 112U))
    {
        return TIMER_IF_WUT_RATE_SLOW_HZ;
    }
    return hz;
}

/**
 * @brief Sequencer task handler -- runs UTIL_TIMER_IRQ_Handler() (and
 *        therefore any expired timer's callback) in ordinary task context,
 *        set pending from RTC_IRQHandler() below instead of calling it
 *        directly from ISR context. See file header for why.
 *
 * 2026-11-06 audit: also makes the WUT ONE-SHOT and re-arms the remainder after
 * an early/clamped fire, which the engine does not do by itself (it skips the
 * re-arm while the head's IsPending is 1).
 */
static void TimerIf_ProcessTask(void)
{
    /* Snapshot the arm that just FIRED before anything re-arms (the handler's
     * callbacks / the engine's re-arm call TimerIf_StartTimerEvt() and
     * overwrite s_arm_*). Timestamp first, log after the handler: a callback
     * may open an RX window and must not be delayed by a log call. */
    const uint32_t now_ms    = TimerIf_GetTimerValue();
    const uint32_t real_ms   = now_ms - s_arm_t0_ms;
    const uint8_t  armed     = s_wut_armed;
    const uint32_t arm_ticks = s_arm_ticks;
#if TIMER_IF_DIAG
    const uint32_t arm_d     = s_arm_delay_ms;
    const uint32_t arm_hz    = s_arm_hz;
    const int      arm_mode  = s_arm_mode;
    const uint32_t arm_stops = s_arm_stop_cnt;
#endif
    uint32_t       meas_hz   = 0U;
    bool           early     = false;
    UTIL_TIMER_Object_t *head;

    s_fire_cnt++;
    s_wut_armed = 0U;
    if (armed == 0U)
    {
        s_spurious_cnt++;
    }

    /* One-shot: stop the WUT now. It auto-reloads (UM S20.3.5) and would
     * otherwise keep firing every <= 63 s forever after the last timer. */
    RTC_EnableWriteProtection(DISABLE);
    (void)TimerIf_WutDisableLocked();
    RTC_EnableWriteProtection(ENABLE);
    RTC_ClrIntPendingBit(RTC_INT_WUT);
    EXTI_ClrITPendBit(EXTI_LINE9);
    NVIC_ClearPendingIRQ(RTC_IRQn);

    /* Is this fire early/clamped (head NOT due yet)? Mirrors the engine's own
     * test: after SetTimerContext() it treats the head as due iff
     * Timestamp <= elapsed-since-old-context. Slightly pessimistic by <1 ms
     * (the engine's "now" is a hair later), which only ever errs toward
     * clearing IsPending on a head that is about to be popped anyway. */
    head = UTIL_TIMER_GetTimerList();
    if ((head != NULL) && (head->Timestamp > (now_ms - s_timer_context_ms)))
    {
        /* Not due yet. This is also the right recovery if the fire had no
         * recorded arm (armed==0): the WUT is disabled now, so a head still
         * marked IsPending would otherwise never fire. */
        early = true;

        if (armed != 0U)
        {
            /* Re-arm the remainder at the rate MEASURED on this very fire, so
             * a real-2048 Hz WUT costs two fires, not a halving chain. */
            meas_hz = TimerIf_MeasureRateHz(arm_ticks, real_ms);
            if (meas_hz != 0U)
            {
                s_last_meas_hz = meas_hz;
                s_last_meas_ms = now_ms;
            }
            else if ((s_last_meas_hz != 0U) && ((now_ms - s_last_meas_ms) < TIMER_IF_RATE_REUSE_MS))
            {
                /* Interval too short to measure (< 16 ms): reuse a measurement
                 * from the last couple of seconds rather than re-halving. */
                meas_hz = s_last_meas_hz;
            }
        }
        s_rate_override_hz = meas_hz;       /* 0 -> slow assumption */
        head->IsPending    = 0U;            /* let the engine's re-arm run */
    }

    UTIL_TIMER_IRQ_Handler();

    s_rate_override_hz = 0U;

#if TIMER_IF_DIAG
    if (armed != 0U)
    {
        /* vs. requested: "early" = fired before the requested delay (the engine
         * re-armed the remainder); "late" = after it. +-3/5 ms covers the
         * ceil, WKUPT+1 and ~1 ms calendar granularity. */
        const char *verdict = ((real_ms + 3U) < arm_d) ? "early" : ((real_ms > (arm_d + 5U)) ? "LATE" : "ok");

        APP_LOG(TS_OFF, VLEVEL_M,
                "[TIMER] fire#%lu d=%lu hz=%lu ticks=%lu real=%lu %s measHz=%lu mode=%d stop(arm/now)=%lu/%lu "
                "spur=%lu wkMis=%lu wtwfTO=%lu\r\n",
                (unsigned long)s_fire_cnt, (unsigned long)arm_d, (unsigned long)arm_hz,
                (unsigned long)arm_ticks, (unsigned long)real_ms, verdict,
                (unsigned long)(early ? meas_hz : 0U), arm_mode,
                (unsigned long)arm_stops, (unsigned long)LpmIf_GetStopEnterCount(),
                (unsigned long)s_spurious_cnt, (unsigned long)s_wkupt_mismatch_cnt,
                (unsigned long)s_wtwf_timeout_cnt);
    }
#else
    (void)early;
    (void)meas_hz;
#endif
}

static UTIL_TIMER_Status_t TimerIf_InitTimer(void)
{
    EXTI_InitType EXTI_InitStructure = {0};
    NVIC_InitType NVIC_InitStructure = {0};

    s_timer_context_ms = 0U;

    UTIL_SEQ_RegTask((1U << TASK_RTC_TIMER_SVC), UTIL_SEQ_RFU, TimerIf_ProcessTask);

    TIMER_IF_DBG_PIN_INIT();

    /* AFIO clock is also enabled by APP_RTC_Init() (rtc.c), which must run
     * before this -- re-enabling here is a harmless idempotent safeguard. */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

    /* WUT clock source: RTCCLK/16 (nominally 2048 Hz) -- see file header.
     * User Manual S20.3.10: WKUPSEL[2:0] may only be written with WTEN=0 AND
     * WTWF=1 (+ ~2 RTCCLK of synchronisation). After a warm reset in which the
     * WUT was running (WTEN=1 survives in the always-on RTC domain) a write
     * without this handshake is silently dropped and WKUPSEL keeps whatever a
     * previous run/firmware left there. Bounded -- runs once at boot. */
    RTC_EnableWriteProtection(DISABLE);
    (void)TimerIf_WutDisableLocked();
    RTC_EnableWriteProtection(ENABLE);
    RTC_ConfigWakeUpClock(RTC_WKUPCLK_RTCCLK_DIV16);
    system_delay_n_10us(TIMER_IF_WTWF_SETTLE_10US);

    /* WTIEN is enabled ONCE here and left on: with the WUT disabled no flag can
     * set, and it removes a second RTC_CTRL change from every re-arm (S20.3.9). */
    RTC_ConfigInt(RTC_INT_WUT, ENABLE);
    system_delay_n_10us(TIMER_IF_WTWF_SETTLE_10US);
    RTC_ClrIntPendingBit(RTC_INT_WUT);

#if TIMER_IF_DIAG
    /* 32 kHz clock-chain snapshot, to tell a clock/config difference between
     * boards or boots apart from a pure WUT-counting effect. LSCTRL:
     * bit3 LSERD (LSE ready), bit5 LSXSEL (1=LSE, 0=LSI), bit2 LSEEN,
     * bit24 RTCEN. CTRL[2:0] WKUPSEL (0 = RTCCLK/16), bit10 WTEN.
     * PRE = prescalers (expect 0x001F03FF). */
    APP_LOG(TS_OFF, VLEVEL_M, "[RTC] LSCTRL=%08lX CTRL=%08lX PRE=%08lX INITSTS=%08lX\r\n",
            (unsigned long)RCC->LSCTRL, (unsigned long)RTC->CTRL,
            (unsigned long)RTC->PRE, (unsigned long)RTC->INITSTS);
    /* Audit Step 0: registers outside the set above that the BLE stack / ROM
     * touch around sleep (undocumented AFEC block + PWR + RCC oscillator
     * counters). Compare a "1024 Hz" boot against a "2048 Hz" boot. */
    APP_LOG(TS_OFF, VLEVEL_M,
            "[RTC] AFEC0=%08lX AFEC4=%08lX AFEC8=%08lX OSCFCCR=%08lX OSCFCSR=%08lX PWRCR1=%08lX PWRCR2=%08lX CFG=%08lX\r\n",
            (unsigned long)(*(volatile uint32_t *)0x40011000UL), (unsigned long)(*(volatile uint32_t *)0x40011004UL),
            (unsigned long)(*(volatile uint32_t *)0x40011008UL), (unsigned long)RCC->OSCFCCR,
            (unsigned long)RCC->OSCFCSR, (unsigned long)PWR->CR1, (unsigned long)PWR->CR2,
            (unsigned long)RCC->CFG);
#endif

    /* RTC WUT wakeup routes through EXTI_LINE9 on this MCU. */
    EXTI_ClrITPendBit(EXTI_LINE9);
    EXTI_InitStructure.EXTI_Line = EXTI_LINE9;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel         = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    return UTIL_TIMER_OK;
}

static UTIL_TIMER_Status_t TimerIf_DeInitTimer(void)
{
    NVIC_InitType NVIC_InitStructure = {0};

    RTC_ConfigInt(RTC_INT_WUT, DISABLE);
    RTC_EnableWriteProtection(DISABLE);
    (void)TimerIf_WutDisableLocked();
    RTC_EnableWriteProtection(ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel    = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);

    return UTIL_TIMER_OK;
}

static UTIL_TIMER_Status_t TimerIf_StartTimerEvt(uint32_t timeout_ms)
{
    uint32_t elapsed_ms = TimerIf_GetTimerElapsedTime();
    uint32_t delay_ms = (timeout_ms > elapsed_ms) ? (timeout_ms - elapsed_ms) : 1U;
#if TIMER_IF_DIAG
    UTIL_LPM_Mode_t lpm_mode_at_arm = UTIL_LPM_GetMode(); /* diagnostic only */
#endif
    uint32_t wut_hz;
    uint32_t max_delay_ms;
    uint32_t wut_ticks;
    uint32_t wkupt;

    /* Rate: the FIRST hop of every arm assumes the slower observed rate
     * (1024 Hz) -- an early fire is recovered by TimerIf_ProcessTask(), a late
     * one is not. Only the remainder re-arm that TimerIf_ProcessTask() drives
     * right after an early fire gets the rate measured on that fire. */
    wut_hz = (s_rate_override_hz != 0U) ? s_rate_override_hz : TIMER_IF_WUT_RATE_SLOW_HZ;

    /* ms -> WUT ticks, ceiling so the WUT fires AT or AFTER the requested
     * delay; clamped to TIMER_IF_WUT_MAX_TICKS (16-bit counter). The clamp test
     * is done on the delay first so delay_ms * wut_hz can never overflow 32
     * bits (the old code overflowed above ~69.9 min). A clamped arm is a
     * normal "early" fire: TimerIf_ProcessTask() re-arms the remainder. */
    max_delay_ms = (TIMER_IF_WUT_MAX_TICKS * 1000U) / wut_hz;
    if (delay_ms >= max_delay_ms)
    {
        wut_ticks = TIMER_IF_WUT_MAX_TICKS;
    }
    else
    {
        wut_ticks = ((delay_ms * wut_hz) + 999U) / 1000U;
        if (wut_ticks == 0U)
        {
            wut_ticks = 1U;
        }
    }

    /* UM S20.4.7: the WUT period is WKUPT + 1 ticks (first WTF WKUPT+1 ticks
     * after WTEN). Writing wut_ticks directly made every arm >= 1 tick late. */
    wkupt = wut_ticks - 1U;

    /* ONE unlock for the whole sequence, and no interleaved SDK setters: the
     * old sequence made ~9 RTC_CTRL changes within microseconds (UM S20.3.9
     * wants ~2 RTCCLK between changes) and its WTWF poll could pass instantly
     * on a stale WTWF=1 right after a previous enable. Here: clear WTEN ->
     * bounded WTWF + 2 RTCCLK -> WKUPT -> WTEN -> 2 RTCCLK before anything
     * else touches RTC_CTRL. */
    RTC_EnableWriteProtection(DISABLE);
    (void)TimerIf_WutDisableLocked();

    /* WUT-startup glitch guard: the N32WB031 RTC transiently asserts WUTF
     * around WTEN going high. With EXTI_LINE9 armed in IMASK this propagates
     * WUTF -> EXTI PR -> NVIC pending (spurious immediate RTC_IRQn). Mask the
     * line until the glitch window has passed, then clear and re-arm. */
    EXTI->IMASK &= ~EXTI_LINE9;

    RTC->WKUPT = wkupt;
    if ((RTC->WKUPT & RTC_WKUPT_WKUPT) != wkupt) /* did the write land? */
    {
        s_wkupt_mismatch_cnt++;
        RTC->WKUPT = wkupt;                       /* one retry */
    }

    RTC_ClrIntPendingBit(RTC_INT_WUT);
    EXTI_ClrITPendBit(EXTI_LINE9);

    RTC->CTRL |= RTC_CTRL_WTEN;
    TIMER_IF_DBG_PIN_HIGH();

    /* Timestamp right after WTEN=1 (the true start of the count), and record
     * what this arm assumed. Last arm wins -- the WUT is simply reprogrammed. */
    s_arm_t0_ms    = TimerIf_GetTimerValue();
    s_arm_ticks    = wut_ticks;
    s_arm_hz       = wut_hz;
    s_arm_delay_ms = delay_ms;
#if TIMER_IF_DIAG
    s_arm_mode     = (int)lpm_mode_at_arm;
    s_arm_stop_cnt = LpmIf_GetStopEnterCount();
#endif
    s_wut_armed    = 1U;

    /* >= 2 RTCCLK before the next RTC_CTRL access (also covers the delayed
     * WUTF glitch), then clear whatever the glitch left and unmask. */
    system_delay_n_10us(TIMER_IF_WTWF_SETTLE_10US);
    RTC_EnableWriteProtection(ENABLE);

    RTC_ClrIntPendingBit(RTC_INT_WUT);
    EXTI_ClrITPendBit(EXTI_LINE9);
    EXTI->IMASK |= EXTI_LINE9;

    return UTIL_TIMER_OK;
}

static UTIL_TIMER_Status_t TimerIf_StopTimerEvt(void)
{
    RTC_EnableWriteProtection(DISABLE);
    (void)TimerIf_WutDisableLocked();
    RTC_EnableWriteProtection(ENABLE);

    s_wut_armed = 0U;
    RTC_ClrIntPendingBit(RTC_INT_WUT);
    EXTI_ClrITPendBit(EXTI_LINE9);

    return UTIL_TIMER_OK;
}

/* ---------------------------------------------------------------------------
 * Driver table
 * ------------------------------------------------------------------------- */

const UTIL_TIMER_Driver_s UTIL_TimerDriver = {
    TimerIf_InitTimer,
    TimerIf_DeInitTimer,

    TimerIf_StartTimerEvt,
    TimerIf_StopTimerEvt,

    TimerIf_SetTimerContext,
    TimerIf_GetTimerContext,

    TimerIf_GetTimerElapsedTime,
    TimerIf_GetTimerValue,
    TimerIf_GetMinimumTimeout,

    TimerIf_ms2Tick,
    TimerIf_Tick2ms,
};

/* ---------------------------------------------------------------------------
 * Interrupt handler -- overrides the weak default in startup_n32wb03x_gcc.s
 * ------------------------------------------------------------------------- */

void RTC_IRQHandler(void)
{
    TIMER_IF_DBG_PIN_LOW(); /* scope hook: pulse width = arm -> ISR entry (off by default) */

    /* Cleared unconditionally, before the RTC_GetITStatus() check -- a
     * spurious EXTI_LINE9 edge (a clock-domain-crossing glitch on RTC's
     * APB sync logic during Standby-mode transitions) with RTC_INT_WUT
     * never actually set used to leave this pending bit set forever,
     * causing NVIC to re-dispatch this ISR instantly in an infinite
     * storm -- see CLAUDE.md BEACON/APP bugs for the full diagnosis. */
    EXTI_ClrITPendBit(EXTI_LINE9);

    if (RTC_GetITStatus(RTC_INT_WUT) != RESET)
    {
        RTC_ClrIntPendingBit(RTC_INT_WUT);

        /* Deferred to task context via TimerIf_ProcessTask() -- NOT called
         * directly here. This ISR is frequently the very interrupt that
         * wakes the CPU out of StopMode's __WFI() (lpm_if.c), and on
         * Cortex-M an ISR runs to completion before the instruction after
         * __WFI() executes -- i.e. before LpmIf_ExitStopMode()'s
         * N32WB031_RADIO_IoInit() has restored the radio's SPI1 clock/GPIO
         * (deinitialized before __WFI() by N32WB031_RADIO_IoDeInit()).
         * Calling UTIL_TIMER_IRQ_Handler() (and any expired timer's
         * callback -- for LoRaWAN, Radio.Standby()/RegionRxConfig()/
         * Radio.Rx() SPI transactions to open RX1/RX2) directly from here
         * would run those SPI commands before SPI1/GPIO were restored,
         * every time -- confirmed as the root cause of RX1/RX2 always
         * timing out. UTIL_SEQ_SetTask() is ISR-safe to call here (already
         * proven by OnTxTimerEvent(), itself invoked from inside this same
         * ISR's callback chain). See file header. */
        UTIL_SEQ_SetTask((1U << TASK_RTC_TIMER_SVC), CFG_SEQ_Prio_0);
    }
}
