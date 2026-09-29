/**
 * @file trace_if.h
 * @brief Unified interrupt-driven UART/LPUART hardware driver for n32_adv_trace.
 *
 * Peripheral and pin selection is compile-time, driven by the macros defined
 * directly below:
 *
 *   USE_LPUART1  -- LPUART1, PB1(TX)/PB2(RX) AF4 by default, or
 *                    PB12(TX)/PB11(RX) AF2 if LPUART1_PINS_PB11_PB12 is set.
 *   USE_USART1   -- USART1, PB6(TX)/PB7(RX) AF4 (fixed).
 *   USE_USART2   -- USART2, PB4(TX)/PB5(RX) AF3 by default, or
 *                    PA6(TX)/PB10(RX) AF2 if USART2_PINS_PA6_PB10 is set.
 *
 * Exactly one of USE_LPUART1 / USE_USART1 / USE_USART2 must be defined.
 * TX is fully interrupt-driven (no polling/blocking) -- trace_if.c defines
 * UTIL_TraceDriver (see n32_adv_trace.h) backed by Init/DeInit/StartRx/Send.
 *
 * ONLY_TX (default 1) -- 1 = TX-only, RX pin left unconfigured and StartRx()
 * is a stub. 0 = also configure the RX pin and mode, and StartRx() arms a
 * real single-byte interrupt-driven receive (re-armed continuously, one
 * callback per byte). Override by defining ONLY_TX 0 in utilities_conf.h.
 */

#ifndef TRACE_IF_H
#define TRACE_IF_H

/* -------------------------------------------------------------------------
 * trace_if.c peripheral/pin selection
 *
 * Exactly one of USE_LPUART1 / USE_USART1 / USE_USART2 must be defined.
 *   USE_LPUART1: default PB1(TX)/PB2(RX) AF4; define LPUART1_PINS_PB11_PB12
 *                to select PB12(TX)/PB11(RX) AF2 instead.
 *   USE_USART1:  fixed PB6(TX)/PB7(RX) AF4.
 *   USE_USART2:  default PB4(TX)/PB5(RX) AF3; define USART2_PINS_PA6_PB10
 *                to select PA6(TX)/PB10(RX) AF2 instead.
 *
 * USART1 (PB6/PB7) is this board's active trace UART -- confirmed connected
 * and working (see CLAUDE.md's BEACON/APP module map).
 * ------------------------------------------------------------------------- */
// #define USE_LPUART1
// /* #define LPUART1_PINS_PB11_PB12 */

#define USE_USART1

#include "n32_adv_trace.h"

#ifndef ONLY_TX
#define ONLY_TX 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Re-init the trace UART hardware after a low-power (Sleep-mode)
 * wake, and force-complete any transmission that was genuinely in flight
 * across the transition. Called from lpm_if.c's LpmIf_ExitStopMode() --
 * see trace_if.c for the full rationale.
 */
void TraceIf_WakeReinit(void);

/**
 * @brief Deinit the trace UART hardware before entering Sleep mode --
 * disables the peripheral and sets TX/RX to ANALOG to save power, mirroring
 * the vendor's own non-BLE peripheral_alone reference. Called from
 * lpm_if.c's LpmIf_EnterStopMode() right before PWR_EnterSLEEPMode().
 */
void TraceIf_PreSleepDeinit(void);

/**
 * @brief Spin-wait (bounded, ~120 ms budget) until the trace UART has
 * genuinely finished transmitting everything queued -- both this driver's
 * own in-flight chunk (s_tx_idx < s_tx_len) and anything still queued in
 * the ring buffer above it (UTIL_ADV_TRACE_IsBufferEmpty()). Must be
 * called with PRIMASK=0 so the TX completion ISR can actually run and
 * drain the FIFO while this spins -- mirrors the proven old-structure
 * app_ble_cellular_wmbus/app_ble_lorawan_wmbus uart2_trace_if.c's
 * uart2_trace_flush(), called from ble_pump_task() immediately before
 * rwip_sleep(), as a second, independent guarantee on top of the
 * CFG_LPM_UART_TX_Id voter (PreSendHook/PostSendHook) that StopMode is
 * never entered mid-transmission -- ported from LORAWAN/APP's identical
 * fix, see CLAUDE.md LORAWAN section, "garbled trace UART output"
 * investigation, for why the voter alone was not sufficient there.
 * Peripheral-agnostic -- works identically regardless of which of
 * USE_LPUART1/USE_USART1/USE_USART2 is selected above, since it only
 * reads this driver's own shared s_tx_idx/s_tx_len state.
 */
void TraceIf_Flush(void);

#ifdef __cplusplus
}
#endif

#endif /* TRACE_IF_H */
