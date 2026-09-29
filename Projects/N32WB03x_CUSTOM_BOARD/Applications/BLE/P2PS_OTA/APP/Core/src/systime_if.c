/**
 * @file systime_if.c
 * @brief RTC-backed hardware interface for n32_systime (UTIL_SYSTIMDriver).
 *
 * The RTC calendar is NEVER rewritten by SysTimeSet(): the calendar/
 * free-running counter used for timer scheduling is always kept completely
 * independent of the wall-clock correction delta, so a SysTimeSet() call
 * can never desynchronize a pending UTIL_TIMER. Chips with an RTC
 * backup-register block would normally store that delta there
 * (HAL_RTCEx_BKUPWrite-style); N32WB031 has no such backup-register block
 * (confirmed directly from the vendor's own RTC_Module register struct --
 * ends at offset 0x44/ALRMASS, no BKPxR array). This driver substitutes
 * .noinit RAM (nvm_ram.c) for that missing hardware feature -- NvmRam_t's
 * systime_delta_sec/subsec_ms fields hold the delta, surviving
 * NVIC_SystemReset (while VDD is held) exactly like a real backup register
 * would.
 *
 * n32_systime.c's SysTimeSet()/SysTimeGet() (the shared, hardware-agnostic
 * engine) compute/apply a delta against whatever GetCalendarTime() returns.
 * This driver only needed to stop conflating "store the delta" with
 * "rewrite the calendar" in BKUPWrite_Seconds(). No TimerIf_AdjustContext()-
 * style compensation is needed anywhere now -- timer_if.c's scheduling
 * clock is structurally immune to SysTimeSet() by construction, not by an
 * explicit synchronized fixup call (see timer_if.c's own history of that
 * former approach).
 *
 * Superseded design note: an earlier version of this file rewrote the RTC
 * calendar directly (the only RTC state that survives NVIC_SystemReset on
 * this chip, absent real backup registers) and relied on timer_if.c's
 * TimerIf_AdjustContext() to keep its tick base continuous across the jump.
 * That worked, but required every timer-context consumer to be correctly
 * and atomically compensated -- a coupling this .noinit-RAM design avoids
 * needing at all.
 */

#include "systime_if.h"
#include "rtc.h"
#include "nvm_ram.h"
#include "n32wb03x.h"
#include "app_log.h"
#include <string.h>

/* Must track rtc.c's RTC_SynchPrediv exactly -- currently 0x3FF (1024
 * steps/s, ~0.98 ms resolution; User Manual S20.4.10's formula). */
#define SYSTIME_RTC_SYNC_PREDIV 0x3FFU

static uint32_t SystimeIf_GetCalendarTime(uint16_t *subSeconds)
{
    RTC_DateType date;
    RTC_TimeType time;
    struct tm calendar_tm;
    uint32_t raw_subsec;

    APP_RTC_GetDateTime(&date, &time);
    raw_subsec = RTC_GetSubSecond();

    memset(&calendar_tm, 0, sizeof(calendar_tm));
    calendar_tm.tm_year = (int)date.Year + 100;
    calendar_tm.tm_mon  = (int)date.Month - 1;
    calendar_tm.tm_mday = (int)date.Date;
    calendar_tm.tm_hour = (int)time.Hours;
    calendar_tm.tm_min  = (int)time.Minutes;
    calendar_tm.tm_sec  = (int)time.Seconds;

    if (subSeconds != NULL)
    {
        /* User Manual S20.4.10 (RTC_SUBS): "SS can be greater than DIVS
         * only after performing [a shift] operation... the correct
         * time/date is one second behind" -- i.e. raw_subsec > DIVS is a
         * real, documented condition, not just a theoretical one. Without
         * this clamp, (SYSTIME_RTC_SYNC_PREDIV - raw_subsec) underflows
         * (both uint32_t), and the subsequent *1000U overflows
         * unpredictably -- hardware-confirmed: produced a garbled,
         * differently-wrong-each-time sub-second value (e.g. 9237, 1713 --
         * both well outside the valid 0-999 range) on the very first
         * calendar read after a genuine RTC prescaler reconfiguration,
         * before the live down-counter had settled into its new range.
         * Clamping makes the subtraction safely evaluate to 0 instead. */
        if (raw_subsec > SYSTIME_RTC_SYNC_PREDIV)
        {
            raw_subsec = SYSTIME_RTC_SYNC_PREDIV;
        }
        *subSeconds = (uint16_t)(((SYSTIME_RTC_SYNC_PREDIV - raw_subsec) * 1000U) / (SYSTIME_RTC_SYNC_PREDIV + 1U));
    }

    return SysTimeMkTime(&calendar_tm);
}

static void SystimeIf_BkUpWriteSeconds(uint32_t Seconds)
{
    /* Seconds is the DELTA computed by SysTimeSet() (target - current
     * calendar), not an absolute epoch. Store it verbatim -- the calendar
     * itself is never touched. Wraparound-signed uint32_t, matching
     * SysTime_t.Seconds' own representation (SysTimeAdd/SysTimeSub rely on
     * the same two's-complement wraparound). */
    NvmRam_Get()->systime_delta_sec = Seconds;

    APP_LOG(TS_OFF, VLEVEL_M, "[SYSTIME] delta stored -> %ld s (calendar untouched, epoch now %lu)\r\n",
            (long)(int32_t)Seconds, (unsigned long)(SystimeIf_GetCalendarTime(NULL) + Seconds));
}

static uint32_t SystimeIf_BkUpReadSeconds(void)
{
    return NvmRam_Get()->systime_delta_sec;
}

static void SystimeIf_BkUpWriteSubSeconds(uint32_t SubSeconds)
{
    NvmRam_Get()->systime_delta_subsec_ms = (int16_t)SubSeconds;
}

static uint32_t SystimeIf_BkUpReadSubSeconds(void)
{
    /* Sign-extend back through int32_t before the final uint32_t cast --
     * matches SysTime_t.SubSeconds' int16_t representation. */
    return (uint32_t)(int32_t)NvmRam_Get()->systime_delta_subsec_ms;
}

const UTIL_SYSTIM_Driver_s UTIL_SYSTIMDriver =
{
    SystimeIf_BkUpWriteSeconds,
    SystimeIf_BkUpReadSeconds,
    SystimeIf_BkUpWriteSubSeconds,
    SystimeIf_BkUpReadSubSeconds,
    SystimeIf_GetCalendarTime,
};
