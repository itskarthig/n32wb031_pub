/**
 * @file lpm_if.h
 * @brief Project-local Low Power Manager hardware driver for BEACON/APP.
 *
 * Implements the UTIL_LPM_Driver_s contract (n32_lpm.h) against the N32WB031
 * power modes documented in N32WB03x_Docs/N32WB03x_User_Manual.md S3.3.
 * Tier mapping:
 *
 *   UTIL_LPM_SLEEPMODE -> PWR_EnterIDLEMode()   (S3.3.2 Idle -- WFI, clocks
 *                         stay on, SDK convenience wrapper used as-is)
 *   UTIL_LPM_STOPMODE  -> PWR_EnterSLEEPMode()  (S3.3.4 "Sleep mode" --
 *                         despite the standby-sounding name, the deep tier:
 *                         BB/RF and CPU clocks off, SRAM retained, wakes on
 *                         any armed EXTI/RTC IRQ. Works standalone with no
 *                         BLE stack, confirmed against the vendor's own
 *                         non-BLE peripheral_alone reference -- requires the
 *                         active trace UART to be cleanly deinitialized
 *                         first, see TraceIf_PreSleepDeinit() in trace_if.c
 *                         and CLAUDE.md's BEACON/APP bugs for the full
 *                         history, including why Standby mode (S3.3.3) was
 *                         tried and rejected -- it structurally keeps
 *                         BLE BB/RF powered "available" per the manual,
 *                         capping achievable current well above this tier)
 *   UTIL_LPM_OFFMODE   -> PWR_EnterPDMode()     (S3.3.5 PD -- deepest,
 *                         resets on wake, SDK convenience wrapper used
 *                         as-is)
 *
 * BLE (app_ble.c) is linked into this project, but this LPM engine is
 * unchanged by that -- PWR_EnterSLEEPMode() performs the identical
 * BLE-deep-sleep register handshake ns_library's own entry_sleep()
 * (ns_sleep.c) does, confirmed by reading both side by side. See
 * lpm_if.c's file header for the full reasoning and the one accepted
 * trade-off (StopMode has no visibility into BLE's own radio schedule,
 * unlike ns_sleep()'s rwip_sleep() check).
 */

#ifndef APP_LPM_IF_H
#define APP_LPM_IF_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* APP_LPM_IF_H */
