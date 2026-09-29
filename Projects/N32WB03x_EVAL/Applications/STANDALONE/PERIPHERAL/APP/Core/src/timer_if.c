/**
 * @file timer_if.c
 * @brief RTC WakeUp-Timer-backed hardware interface for n32_timer (UTIL_TimerDriver).
 *
 * Tick = 1 millisecond, via the RTC WakeUp Timer's RTCCLK/16 clock mode
 * (32768/16 = 2048 Hz, ~488 us/tick) combined with the RTC's sub-second
 * register for millisecond-resolution elapsed-time tracking -- but see the
 * WUT-rate-vs-StopMode note below: this 2048 Hz figure only holds while
 * StopMode is continuously cycling.
 *
 * Ported from the LORAWAN project's timer_if.c, which itself
 * hardware-confirmed (see CLAUDE.md LORAWAN section, "RX1/RX2 always time
 * out" investigation) that this same WUT clock mode counts at a fixed real
 * rate of ~1024 Hz -- not the documented 2048 Hz -- whenever the MCU is not
 * continuously cycling through genuine StopMode. This project has exactly
 * such a window itself: the 60 s PB3 wake-button reflash period
 * (CFG_LPM_WAKE_BUTTON_Id, see lpm_if.c/application.c), during which
 * StopMode is blocked and this same halving would apply if left
 * uncompensated. Once StopMode is genuinely allowed and cycling (after the
 * wake window expires, assuming nothing else blocks it), the WUT settles to
 * the documented 2048 Hz.
 *
 * This was previously an open, untested question for this project (see the
 * prior revision of this file's header) -- whether the StopMode-dependent
 * halving applies to the CK_SPRE_16BITS mode this file used to run in.
 * That question is now moot: this file no longer uses CK_SPRE_16BITS at
 * all, has switched to the same RTCCLK_DIV16 mode LORAWAN uses, and
 * carries the same live UTIL_LPM_GetMode()-based compensation as that
 * project's proven fix, rather than trying to separately characterize the
 * old mode's own behavior.
 *
 * TimerIf_StartTimerEvt() below checks UTIL_LPM_GetMode() live, at arm
 * time -- true exactly when no voter (CFG_LPM_APPLI_Id under
 * LOW_POWER_DISABLE, CFG_LPM_UART_TX_Id, CFG_LPM_WAKE_BUTTON_Id) currently
 * blocks StopMode. This project moves between both regimes once per boot
 * (StopMode-blocked during the wake window, then StopMode-cycling
 * afterward), so a live check is used rather than a compile-time flag.
 *
 * That mode's counter is hard-capped at 16 bits -- StartTimerEvt() clamps
 * to ~65000 ticks (~31.7 s) -- and n32_timer.c's own UTIL_TIMER_IRQ_Handler()
 * naturally "chains" through longer requests for free: it always re-checks
 * the *real* elapsed time (via GetTimerElapsedTime(), based on the RTC
 * calendar, independent of the WUT) before treating a timer as actually
 * due, so an early/clamped WUT fire just causes a silent re-arm for the
 * remaining time instead of a spurious callback. No explicit multi-arm
 * bookkeeping needed here.
 *
 * GetTimerValue() deliberately reads the *raw* RTC calendar + sub-second
 * register directly (not SysTimeGet()/SysTimeGetMcuTime()) -- so the tick
 * base is never touched by a SysTimeSet() correction at all.
 *
 * Structurally immune to SysTimeSet() by construction: systime_if.c's
 * SysTimeSet() path stores its correction as a delta in .noinit RAM
 * (nvm_ram.c) and NEVER rewrites the RTC calendar -- so the calendar this
 * file reads for s_timer_context_ms below can never jump because of a
 * SysTimeSet() call, and no compensation entry point (this file previously
 * exported TimerIf_AdjustContext() for exactly that purpose) is needed or
 * present.
 *
 * Deliberately NOT ported from LORAWAN's timer_if.c: the RTC_IRQHandler()
 * deferred-to-sequencer-task dispatch (TASK_RTC_TIMER_SVC). That mechanism
 * exists there because LORAWAN's lpm_if.c deinitializes the LoRa radio's
 * SPI1 peripheral/GPIO before StopMode and only restores it afterward, so
 * calling UTIL_TIMER_IRQ_Handler() (and therefore a timer callback that
 * issues SPI transactions) directly from the waking ISR would race that
 * restore. This project has no radio/SPI to protect -- RTC_IRQHandler()
 * below keeps calling UTIL_TIMER_IRQ_Handler() directly, exactly as before
 * this port, avoiding the extra task-ID/sequencer-registration complexity
 * that mechanism would otherwise add for no benefit here.
 */

#include "timer_if.h"
#include "n32wb03x.h"
#include "rtc.h"
#include "n32_systime.h"
#include "n32_lpm.h"
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

    APP_RTC_GetDateTime(&date, &time);
    raw_subsec = RTC_GetSubSecond();

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
     * SystimeIf_GetCalendarTime(), hardware-confirmed necessary there). */
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

static UTIL_TIMER_Status_t TimerIf_InitTimer(void)
{
    EXTI_InitType EXTI_InitStructure = {0};
    NVIC_InitType NVIC_InitStructure = {0};

    s_timer_context_ms = 0U;

    /* AFIO clock is also enabled by APP_RTC_Init() (rtc.c), which must run
     * before this -- re-enabling here is a harmless idempotent safeguard. */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

    /* WUT clock source: RTCCLK/16 = 2048 Hz -- see file header. */
    RTC_EnableWakeUp(DISABLE);
    RTC_ConfigWakeUpClock(RTC_WKUPCLK_RTCCLK_DIV16);

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
    RTC_EnableWakeUp(DISABLE);

    NVIC_InitStructure.NVIC_IRQChannel    = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);

    return UTIL_TIMER_OK;
}

static UTIL_TIMER_Status_t TimerIf_StartTimerEvt(uint32_t timeout_ms)
{
    uint32_t elapsed_ms = TimerIf_GetTimerElapsedTime();
    uint32_t delay_ms = (timeout_ms > elapsed_ms) ? (timeout_ms - elapsed_ms) : 1U;

    /* Live check, at arm time -- see file header. The WUT only counts at
     * its documented 2048 Hz while StopMode is continuously cycling
     * (UTIL_LPM_GetMode() == UTIL_LPM_STOPMODE, i.e. no voter currently
     * blocks it); otherwise it counts at a fixed real ~1024 Hz. Checked
     * fresh on every arm since this project moves between both regimes
     * once per boot (the PB3 wake window, then normal StopMode cycling). */
    uint32_t wut_hz_effective = (UTIL_LPM_GetMode() == UTIL_LPM_STOPMODE)
                                     ? TIMER_IF_WUT_TICK_HZ
                                     : (TIMER_IF_WUT_TICK_HZ / 2U);

    /* ms -> WUT ticks, ceiling so the WUT fires AT or AFTER the requested
     * delay. Clamp to TIMER_IF_WUT_MAX_TICKS (~31.7 s); the generic
     * UTIL_TIMER engine re-arms for any remainder -- see file header
     * "chains through longer requests for free". */
    uint32_t wut_ticks = (delay_ms * wut_hz_effective + 999U) / 1000U;
    if (wut_ticks == 0U)               { wut_ticks = 1U; }
    if (wut_ticks > TIMER_IF_WUT_MAX_TICKS) { wut_ticks = TIMER_IF_WUT_MAX_TICKS; }

    RTC_EnableWakeUp(DISABLE);
    /* User Manual S20.4.5: WTEN=0 -> WTWF=1 takes up to 2 RTCCLK cycles
     * (~61 us at 32.768 kHz) before RTC_WKUPT may be safely rewritten --
     * a standard write-protection handshake for this class of wake-up
     * timer register. Reconfiguring without this wait is undefined per
     * the manual. */
    while (RTC_GetFlagStatus(RTC_FLAG_WTWF) == RESET)
    {
    }

    /* WUT-startup glitch guard: the N32WB031 RTC transiently asserts WUTF
     * the instant WUTEN goes high. With EXTI_LINE9 armed in IMASK this
     * propagates: WUTF -> EXTI PR -> NVIC pending, causing a spurious
     * immediate RTC_IRQn fire. This was masked at the old ~1-2s/tick
     * coarseness (an early spurious fire just looked like normal jitter)
     * but becomes a real hazard at this fast a tick rate. Mask EXTI_LINE9
     * before enabling the WUT so the glitch edge can't reach NVIC, clear
     * the resulting pending flags, then re-arm. */
    EXTI->IMASK &= ~EXTI_LINE9;

    RTC_SetWakeUpCounter(wut_ticks);
    RTC_ClrIntPendingBit(RTC_INT_WUT);
    EXTI_ClrITPendBit(EXTI_LINE9);

    RTC_ConfigInt(RTC_INT_WUT, ENABLE);
    RTC_EnableWakeUp(ENABLE);            /* glitch: WUTF set -> EXTI PR set,
                                           * but IMASK=0 -> NVIC never latches */

    RTC_ClrIntPendingBit(RTC_INT_WUT);   /* clear WUTF source flag */
    EXTI_ClrITPendBit(EXTI_LINE9);       /* clear EXTI PR */

    EXTI->IMASK |= EXTI_LINE9;           /* re-enable -- EXTI PR already clean */

    return UTIL_TIMER_OK;
}

static UTIL_TIMER_Status_t TimerIf_StopTimerEvt(void)
{
    RTC_ConfigInt(RTC_INT_WUT, DISABLE);
    RTC_EnableWakeUp(DISABLE);
    while (RTC_GetFlagStatus(RTC_FLAG_WTWF) == RESET)
    {
    }

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
    /* Cleared unconditionally, before the RTC_GetITStatus() check -- a
     * spurious EXTI_LINE9 edge (a clock-domain-crossing glitch on RTC's
     * APB sync logic during Standby-mode transitions) with RTC_INT_WUT
     * never actually set used to leave this pending bit set forever,
     * causing NVIC to re-dispatch this ISR instantly in an infinite
     * storm. */
    EXTI_ClrITPendBit(EXTI_LINE9);

    if (RTC_GetITStatus(RTC_INT_WUT) != RESET)
    {
        RTC_ClrIntPendingBit(RTC_INT_WUT);
        /* Called directly from ISR context -- safe here since this
         * project has no radio/SPI peripheral that gets deinitialized
         * around StopMode (unlike LORAWAN's timer_if.c, which defers this
         * to a sequencer task for exactly that reason). See file header. */
        UTIL_TIMER_IRQ_Handler();
    }
}
