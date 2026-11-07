/**
 * @file rtc.h
 * @brief Basic RTC calendar (init + set/get date-time) for P2PS_OTA/APP.
 *
 * Thin wrapper around the N32WB03x_SDK RTC driver (n32wb03x_rtc.c/.h) --
 * clock source select + prescaler config, and BIN-format date/time
 * set/get. Register-level sequence (clock select order, prescaler values,
 * Time-before-Date read order to avoid the RTC shadow-register freeze
 * quirk) modeled on the SDK's own RTC/Calendar reference example.
 */

#ifndef APP_RTC_H
#define APP_RTC_H

#include "n32wb03x_rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * RTC clock source selection -- override in utilities_conf.h.
 *   0 (default) = LSE (32.768 kHz crystal -- matches every other project in
 *       this repo; accurate, low power; requires the crystal to be fitted).
 *   1 = LSI (internal RC oscillator -- no crystal needed, far less accurate).
 */
#ifndef RTC_USE_LSI
#define RTC_USE_LSI 0
#endif

/**
 * @brief Configure the RTC clock source (LSE/LSI per RTC_USE_LSI) and
 *        prescalers. Does not touch the calendar date/time -- call
 *        APP_RTC_SetDateTime() separately to set it.
 */
void APP_RTC_Init(void);

/**
 * @brief Set the RTC calendar date and time (BIN format).
 * @return SUCCESS if both the underlying RTC_ConfigTime()/RTC_SetDate()
 *         calls succeeded, ERROR otherwise (e.g. RTC_EnterInitMode()
 *         timed out) -- callers should check this instead of assuming the
 *         write landed.
 */
ErrorStatus APP_RTC_SetDateTime(const RTC_DateType *date, const RTC_TimeType *time);

/**
 * @brief Read the current RTC calendar date and time (BIN format).
 * @note  Reads Time before Date -- required order to avoid the RTC shadow
 *        register freeze (reading TIME freezes the shadow registers until
 *        DATE is also read).
 */
void APP_RTC_GetDateTime(RTC_DateType *date, RTC_TimeType *time);

/**
 * @brief Read date, time AND raw RTC_SUBS as one coherent snapshot.
 *
 * User Manual S20.3.11: "reading RTC_SUBS or RTC_TSH will lock the value in
 * the high-order calendar shadow register until RTC_DATE is read" -- so the
 * only order that yields a consistent triple is SUBS -> TSH -> DATE. The old
 * pattern (APP_RTC_GetDateTime() followed by RTC_GetSubSecond()) read
 * TSH -> DATE -> (new snapshot) SUBS, so a second rollover between the two
 * snapshots made the result jump back ~1000 ms (CLAUDE.md, timer audit
 * 2026-11-06, finding 2). RTC_GetSubSecond() itself is NOT used here: it reads
 * DATE right after SUBS ("to unfreeze"), which is the wrong place.
 *
 * The triple is read twice and compared (TSH/DATE equal and SUBS not
 * increased -- SUBS counts down within a second); up to 3 attempts. After
 * that the first read is returned (never blocks).
 *
 * @param subsec  raw RTC_SUBS value (counts DIVS..0 within a second).
 */
void APP_RTC_GetDateTimeSubSec(RTC_DateType *date, RTC_TimeType *time, uint32_t *subsec);

/**
 * @brief Re-synchronise the calendar shadow registers after a low-power wake.
 *
 * User Manual S20.3.11: "After waking up from low-power mode, RSYF must be
 * cleared by software. Software must then wait until RSYF is set again before
 * reading RTC_SUBS, RTC_TSH and RTC_DATE" (shadow registers are not refreshed
 * in Standby/Sleep, S20.3.3). Bounded (SDK SYNCHRO_TIMEOUT). Call once per
 * wake, before the first calendar read.
 */
void APP_RTC_ResyncAfterWake(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_RTC_H */
