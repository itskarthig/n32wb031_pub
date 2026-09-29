/**
 * @file rtc.c
 * @brief Basic RTC calendar (init + set/get date-time) for BEACON/APP.
 */

#include "rtc.h"
#include "n32wb03x.h"
#include "app_log.h"

/**
 * @brief Enable and select the RTC clock source (LSE or LSI per RTC_USE_LSI),
 *        matching the SDK RTC/Calendar example's proven sequence.
 */
static void RTC_ClockSourceConfig(void)
{
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

    /* Disable RTC clock before switching source */
    RCC_EnableRtcClk(DISABLE);

#if (RTC_USE_LSI == 1)
    RCC_EnableLsi(ENABLE);
    while (RCC_GetFlagStatus(RCC_LSCTRL_FLAG_LSIRD) == RESET)
    {
    }
    RCC_ConfigLSXSEL(RCC_RTCCLK_SRC_LSI);
#else
    RCC_ConfigLse(RCC_LSE_ENABLE);
    while (RCC_GetFlagStatus(RCC_LSCTRL_FLAG_LSERD) == RESET)
    {
    }
    RCC_ConfigLSXSEL(RCC_RTCCLK_SRC_LSE);
    RCC_EnableLsi(DISABLE);
#endif

    RCC_EnableRtcClk(ENABLE);
    RTC_WaitForSynchro();
}

void APP_RTC_Init(void)
{
    RTC_InitType RTC_InitStructure;

    RTC_ClockSourceConfig();

    /* Explicitly disable Bypass Shadow BEFORE touching the prescaler --
     * order matters. RTC_EnableBypassShadow()'s own doc comment: "When
     * Bypass Shadow is enabled the calendar values are taken directly
     * from the Calendar counter" -- this bypasses the exact shadow-
     * register double-buffering RTC_WaitForSynchro()'s RSYF flag is
     * designed to signal, so a stuck-on BYPS (from an earlier firmware
     * flash -- RTC_CTRL lives in the same always-on domain as the
     * calendar counter, so a bit set once persists across reflashes/
     * resets until something writes the opposite value; hardware-
     * confirmed stuck-on in this exact codebase, see CLAUDE.md BEACON/APP
     * Bug #19) makes any RSYF-based wait done afterward trivially
     * succeed without providing real protection. Must run before
     * RTC_Init()'s prescaler reconfiguration and the RTC_WaitForSynchro()
     * call below, not after -- confirmed on hardware: doing this after
     * (the original order) did not fix the garbled-first-read symptom the
     * prescaler change exposed. */
    RTC_EnableBypassShadow(DISABLE);

    /* 32.768 kHz source (LSE or LSI) -- same prescaler split for both.
     * AsynchPrediv=0x1F (/32) x SynchPrediv=0x3FF (/1024) = 32768, keeping
     * the calendar's 1 Hz tick (ck_spre) exact -- only the RTC_SUBS
     * sub-second resolution changes with this split (User Manual S20.3.12/
     * S20.4.10: resolution = 1/(DIVS+1) s). Raised from the original SDK
     * RTC/Calendar example's SynchPrediv=0xFF/AsynchPrediv=0x7F (256
     * steps/s, ~3.9 ms) to 1024 steps/s (~0.98 ms) -- matches
     * STM32CubeWL's own real-world battery-powered reference split
     * (RTC_PREDIV_A=31/RTC_PREDIV_S=1023), not the manual's own
     * power-maximizing extreme (DIVS=0x7FFF/DIVA=0, full-rate 32.768 kHz
     * toggling). Raising DIVS increases RTC dynamic power draw (manual's
     * own explicit warning) -- this split is a deliberate middle ground,
     * not a mistake if a future review finds it non-maximal.
     * systime_if.c's SYSTIME_RTC_SYNC_PREDIV must track RTC_SynchPrediv
     * exactly -- keep both in sync if this ever changes again. */
    RTC_InitStructure.RTC_HourFormat   = RTC_24HOUR_FORMAT;
    RTC_InitStructure.RTC_AsynchPrediv = 0x1FU;
    RTC_InitStructure.RTC_SynchPrediv  = 0x3FFU;
    (void)RTC_Init(&RTC_InitStructure);

    /* RTC_Init() (n32wb03x_rtc.c) writes RTC->PRE directly and returns --
     * it never waits for the shadow registers to resynchronize. Per the
     * User Manual (S20.4: "after initialization, software must wait until
     * RSYF is set before reading RTC_SUBS/RTC_TSH/RTC_DATE again"), any
     * read of those registers immediately after a genuine prescaler value
     * change (not a no-op reprogram of the same value) can return stale/
     * transitional content -- hardware-confirmed: first boot after
     * changing this file's prescaler split showed a garbled epoch
     * (nonsensical seconds + out-of-range sub-second ms) on the very
     * first read, self-correcting one periodic-task cycle later. Harmless
     * while the prescaler value never actually changed between boots;
     * became observable the moment it did. */
    RTC_WaitForSynchro();
}

ErrorStatus APP_RTC_SetDateTime(const RTC_DateType *date, const RTC_TimeType *time)
{
    /* Time before Date -- matches app_ble_cellular_wmbus's proven-working
     * stm32_systime_if.c order exactly. This function had no real caller
     * until systime_if.c's live calendar-rewrite fix wired it in, so its
     * original Date-then-Time order was never hardware-exercised; don't
     * deviate from the validated reference's order without a hardware-
     * confirmed reason to. */
    RTC_DateType date_copy = *date;
    RTC_TimeType time_copy = *time;
    ErrorStatus time_status;
    ErrorStatus date_status;

    /* Return status instead of discarding it -- RTC_ConfigTime()/
     * RTC_SetDate() silently skip the actual register write and return
     * ERROR if RTC_EnterInitMode() times out; callers need to know that
     * happened instead of assuming the write landed. */
    APP_LOG(TS_OFF, VLEVEL_M, "[RTC] before: CTRL=%08lX INITSTS=%08lX TSH=%08lX DATE=%08lX\r\n",
            (unsigned long)RTC->CTRL, (unsigned long)RTC->INITSTS, (unsigned long)RTC->TSH, (unsigned long)RTC->DATE);

    time_status = RTC_ConfigTime(RTC_FORMAT_BIN, &time_copy);
    APP_LOG(TS_OFF, VLEVEL_M, "[RTC] after ConfigTime(status=%d): CTRL=%08lX INITSTS=%08lX TSH=%08lX DATE=%08lX\r\n",
            (int)time_status, (unsigned long)RTC->CTRL, (unsigned long)RTC->INITSTS, (unsigned long)RTC->TSH, (unsigned long)RTC->DATE);

    date_status = RTC_SetDate(RTC_FORMAT_BIN, &date_copy);
    APP_LOG(TS_OFF, VLEVEL_M, "[RTC] after SetDate(status=%d): CTRL=%08lX INITSTS=%08lX TSH=%08lX DATE=%08lX\r\n",
            (int)date_status, (unsigned long)RTC->CTRL, (unsigned long)RTC->INITSTS, (unsigned long)RTC->TSH, (unsigned long)RTC->DATE);

    return ((time_status == SUCCESS) && (date_status == SUCCESS)) ? SUCCESS : ERROR;
}

void APP_RTC_GetDateTime(RTC_DateType *date, RTC_TimeType *time)
{
    /* Time must be read before Date -- reading TIME freezes the shadow
     * registers until DATE is also read (RTC/Calendar example's
     * "Unfreeze the RTC DAT Register" comment). */
    RTC_GetTime(RTC_FORMAT_BIN, time);
    RTC_GetDate(RTC_FORMAT_BIN, date);
}
