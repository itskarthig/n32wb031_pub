/**
 * @file ns_log_if.h
 * @brief Redirects ns_library's own NS_LOG_ERROR/WARNING/INFO/DEBUG macros
 * to a small deferred staging queue (ns_log_if.c), drained one message per
 * scheduler pump cycle into APP_LOG (trace_if.c/USART1) -- instead of the
 * SDK's own LPUART/USART/RTT backends (ns_log_lpuart.c/ns_log_usart.c/
 * ns_log_rtt.c -- none of which are linked; NS_LOG_USART_OUTPUT in
 * particular routes through libc printf(), which this project can't use --
 * --specs=nosys.specs has no real _write() syscall).
 *
 * Force-included (-include, see CMakeLists.txt) into every translation
 * unit in this target, ahead of any BLE stack .c file's own
 * "#include \"ns_log.h\"". ns_log.h is header-guard-protected
 * (__NS_LOG_H__) -- since we #include it here first and it defines
 * NS_LOG_ERROR/WARNING/INFO/DEBUG unconditionally (no #ifndef), we #undef
 * and redefine them immediately after; the later #include "ns_log.h" seen
 * inside ns_ble.c/ns_sec.c/etc. then no-ops on the guard and our
 * redefinitions survive as the only ones in that translation unit.
 *
 * Deferred, not direct, because the SDK's own NS_LOG_DEBUG calls fire in
 * dense bursts (e.g. the BLE ADV FSM's app_create_advertising/
 * app_set_adv_data/app_set_scan_rsp_data/app_start_advertising sequence,
 * all logging within one scheduler tick) -- faster than the UART can drain
 * the shared trace FIFO (Utilities/trace/adv_trace/n32_adv_trace.c), which
 * silently drops a whole message with zero error propagation if it lacks
 * room. A first attempt at fixing this simply enlarged the shared FIFO
 * (utilities_conf.h, UTIL_ADV_TRACE_FIFO_SIZE 512->1024) -- closed most of
 * the gap but not all of it (the single densest burst, at cold boot, could
 * still overflow even the larger FIFO). This staging queue supersedes that
 * approach: spreading the SDK's own bursts across multiple pump cycles
 * (one message drained per cycle, see ns_log_if.c) means the shared FIFO
 * itself never needs to absorb more than one message at a time, so it was
 * reverted back to its original 512 B size (utilities_conf.h) once this
 * queue was in place everywhere -- see that file's own comment. Scope is
 * deliberately NS_LOG_x only -- this project's own direct APP_LOG() calls
 * are not bursty and stay on the existing synchronous path.
 */

#ifndef NS_LOG_IF_H
#define NS_LOG_IF_H

#include <stdint.h>

#include "ns_log.h"
#include "app_log.h"

#undef NS_LOG_ERROR
#undef NS_LOG_WARNING
#undef NS_LOG_INFO
#undef NS_LOG_DEBUG

#define NS_LOG_ERROR(...)    NsLogIf_Enqueue(__VA_ARGS__)
#define NS_LOG_WARNING(...)  NsLogIf_Enqueue(__VA_ARGS__)
#define NS_LOG_INFO(...)     NsLogIf_Enqueue(__VA_ARGS__)
#define NS_LOG_DEBUG(...)    NsLogIf_Enqueue(__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Formats and pushes one message onto the deferred staging queue;
 * safe to call from any context, including BLE-kernel dispatch. Silently
 * drops (see NsLogIf_GetDropCount()) if the queue is full. */
void NsLogIf_Enqueue(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

/** @brief Sequencer task (TASK_NS_LOG_DRAIN_ID) -- pops and forwards one
 * queued message to APP_LOG() per call; self-re-arms at CFG_SEQ_Prio_1 if
 * more remain. Registered in app_entry.c's APP_Init(). */
void NsLogIf_DrainTask(void);

/** @brief Count of messages dropped because the staging queue was full.
 * Diagnostic only -- not checked by any call site today. */
uint32_t NsLogIf_GetDropCount(void);

#ifdef __cplusplus
}
#endif

#endif /* NS_LOG_IF_H */
