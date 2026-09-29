/**
 * @file timer_if.h
 * @brief RTC WakeUp-Timer-backed hardware interface for n32_timer (UTIL_TimerDriver).
 *
 * Bridges n32_timer.c's UTIL_TIMER_Driver_s contract to the RTC WakeUp
 * Timer (WUT) -- LSE-clocked, always-on, survives STOP mode. Tick = 1
 * millisecond (RTCCLK/16 = 2048 Hz WUT clock mode + RTC sub-second
 * register for millisecond-resolution GetTimerValue() -- raised from the
 * original whole-second CK_SPRE_16BITS mode; see timer_if.c's file header
 * for the StopMode-dependent WUT-rate finding this port carries over from
 * the LORAWAN project, and why it applies here too via this project's own
 * PB3 wake-window StopMode-blocked period).
 *
 * Precondition: APP_RTC_Init() (rtc.c -- clock source + prescaler) must
 * already have run before UTIL_TIMER_Init() is called.
 *
 * GetTimerValue() reads only the raw RTC calendar + sub-second register --
 * systime_if.c's SysTimeSet() never rewrites that calendar (its correction
 * delta lives in .noinit RAM instead, see nvm_ram.h), so this file's tick
 * base is structurally immune to a SysTimeSet() call. No compensation entry
 * point is needed here (there previously was one, TimerIf_AdjustContext() --
 * removed once systime_if.c stopped rewriting the calendar).
 */

#ifndef TIMER_IF_H
#define TIMER_IF_H

#include "n32_timer.h"

#endif /* TIMER_IF_H */
