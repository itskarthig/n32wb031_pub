/**
 * @file systime_if.h
 * @brief RTC-backed hardware interface for n32_systime (UTIL_SYSTIMDriver).
 *
 * Bridges n32_systime.c's UTIL_SYSTIM_Driver_s contract to this project's
 * own rtc.c (calendar) and the N32 SDK's RTC_GetSubSecond(). N32WB031's RTC
 * has no backup-register block, so the SysTimeSet() delta is stored in
 * .noinit RAM instead (nvm_ram.h) -- the RTC calendar itself is never
 * touched, keeping the calendar and the correction delta always
 * independent. See systime_if.c's file header for the full rationale.
 */

#ifndef SYSTIME_IF_H
#define SYSTIME_IF_H

#include "n32_systime.h"

#endif /* SYSTIME_IF_H */
