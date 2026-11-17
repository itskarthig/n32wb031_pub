/**
 * @file lpm_if.h
 * @brief Project-local Low Power Manager hardware driver.
 *
 * Implements the UTIL_LPM_Driver_s contract (n32_lpm.h):
 *   UTIL_LPM_SLEEPMODE -> PWR_EnterIDLEMode()  (plain WFI)
 *   UTIL_LPM_STOPMODE  -> N32WB031 "Sleep mode" (User Manual S3.3.4), register-for-register the vendor
 *                         ns_sleep.c entry_sleep() sequence plus this board's peripheral deinit/reinit
 *   UTIL_LPM_OFFMODE   -> PWR_EnterPDMode()    (never used, permanently blocked)
 * Ported from LORAWAN_WMBUS_AMR_V20 on 2026-11-17; see lpm_if.c.
 */

#ifndef APP_LPM_IF_H
#define APP_LPM_IF_H

#include <stdint.h>

/** Vendor entry_idle(): re-arm PA11 wake, clear PWR mode bits, WFI. Call with PRIMASK=1. */
void LpmIf_EntryIdle(void);

/** Number of real StopMode entries since boot (timer_if.c diagnostic). */
uint32_t LpmIf_GetStopEnterCount(void);

#endif /* APP_LPM_IF_H */
