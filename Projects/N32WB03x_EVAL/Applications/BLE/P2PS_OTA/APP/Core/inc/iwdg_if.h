/**
 * @file iwdg_if.h
 * @brief Independent hardware watchdog (IWDG), switchable from app_config.h (default off in this project).
 *
 * Why: the sleep path has spins under PRIMASK=1 (the two PWR_CR1.BLE_OSC_EN waits in lpm_if.c) and a
 * WFI that can in principle never be woken (LORAWAN_WMBUS_AMR_V20 first stall: mark=3, ~960 uA, SWD dead). A software
 * watchdog cannot see those because it only runs when the CPU does; the IWDG resets the chip.
 *
 * Switches (app_config.h): IWDG_ENABLE (0 compiles everything out, call sites need no #if),
 * IWDG_TIMEOUT_MS, IWDG_SELFTEST.
 *
 * IMPORTANT: app_env.iwdg_cycle (the vendor's own IWDG option in ns_ble.c) must stay 0. If it is
 * set, ns_iwdg_config() re-programs the IWDG to /32 (about 3.2 s max) and overrides this module.
 */
#ifndef IWDG_IF_H
#define IWDG_IF_H

#include "app_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (IWDG_ENABLE == 1)

/** Remember whether the previous reset came from the IWDG. Call first thing in APP_Init(): the
 *  vendor's ns_peripheral_config() clears the reset flags when BLE initialises. */
void IwdgIf_CaptureResetCause(void);

/** Start the watchdog (logs the previous reset cause and the timeout). Needs the trace up. */
void IwdgIf_Init(void);

/** Reload the counter. One register write; call once per BLE pump pass. */
void IwdgIf_Feed(void);

#if (IWDG_SELFTEST == 1)
/** One-off experiment: after ~30 s enter deep Sleep with PB3 as the only wake source. See iwdg_if.c. */
void IwdgIf_SelfTestPoll(void);
#else
static inline void IwdgIf_SelfTestPoll(void) {}
#endif

#else /* IWDG_ENABLE == 0 */

static inline void IwdgIf_CaptureResetCause(void) {}
static inline void IwdgIf_Init(void) {}
static inline void IwdgIf_Feed(void) {}
static inline void IwdgIf_SelfTestPoll(void) {}

#endif

#ifdef __cplusplus
}
#endif

#endif /* IWDG_IF_H */
