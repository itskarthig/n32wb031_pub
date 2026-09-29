/**
 * @file app_ble.h
 * @brief BLE bring-up for P2PS_OTA/APP -- device/GAP config, security
 * baseline, and fast(60s)->slow advertising, built entirely on ns_library
 * (middlewares/Nationstech/ble_library/ns_library), the N32WB031 SDK's own
 * wrapper over its RivieraWaves/RWIP BLE core. See CLAUDE.md's BEACON/APP
 * BLE section for the full design rationale (why ns_sleep()/entry_sleep()
 * replaces lpm_if.c's StopMode tier once BLE is linked, and why fast/slow
 * ADV needs no app-level timer -- both confirmed directly against SDK
 * source, not any app_* project).
 */

#ifndef APP_BLE_H
#define APP_BLE_H

#include "ns_ble.h"
#include "ns_ble_task.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bring up the BLE stack and start advertising. Call once from
 * APP_Init(), after the non-BLE peripherals (trace/RTC/timer/sequencer/LPM)
 * are already initialized -- BLE bring-up itself has no ordering dependency
 * on them, but TraceIf_PreSleepDeinit()/WakeReinit() (wired into
 * app_sleep_prepare_proc()/app_sleep_resume_proc(), see lpm_if.c) do.
 */
void APP_BLE_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_BLE_H */
