/**
 * @file rtc.h
 * @brief Basic RTC calendar (init + set/get date-time) for PERIPHERAL/APP.
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

#ifdef __cplusplus
}
#endif

#endif /* APP_RTC_H */
