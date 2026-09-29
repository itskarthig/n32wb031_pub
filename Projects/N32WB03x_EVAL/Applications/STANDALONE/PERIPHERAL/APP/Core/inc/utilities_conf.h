/**
 * @file utilities_conf.h
 * @brief Project-local configuration for PERIPHERAL/APP.
 *
 * Supplies the macro contract required by n32_adv_trace.h/.c (ring-buffer
 * sizing, PRIMASK critical sections, verbose-level gating), by
 * Utilities/sequencer/n32_seq.h/.c (task/priority counts, PRIMASK critical
 * sections), and by n32_timer/n32_lpm (PRIMASK critical sections). Each
 * project gets its own copy of this file so sizing can differ per project
 * without touching shared code. Trace UART peripheral/pin selection
 * (USE_LPUART1/USE_USART1/USE_USART2) lives in trace_if.h instead -- it's
 * consumed only by trace_if.c, not by this file's own macro contract.
 */

#ifndef UTILITIES_CONF_H
#define UTILITIES_CONF_H

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include "cmsis_compiler.h"   /* __get_PRIMASK, __disable_irq, __set_PRIMASK */

/* -------------------------------------------------------------------------
 * n32_adv_trace ring-buffer engine configuration
 * ------------------------------------------------------------------------- */

/** Circular FIFO size for trace output. Must be a power of 2. */
#ifndef UTIL_ADV_TRACE_FIFO_SIZE
#define UTIL_ADV_TRACE_FIFO_SIZE             512U
#endif

/** Scratch buffer used by UTIL_ADV_TRACE_FSend()/COND_FSend() for vsnprintf. */
#define UTIL_ADV_TRACE_TMP_BUF_SIZE          256U

/** memset wrapper. */
#define UTIL_ADV_TRACE_MEMSET8(d, v, s)      memset((d), (v), (s))

/** vsnprintf wrapper. */
#define UTIL_ADV_TRACE_VSNPRINTF             vsnprintf

/** One-time resource allocation (no-op -- no OS primitives needed). */
#define UTIL_ADV_TRACE_INIT_CRITICAL_SECTION()

/**
 * Enter/exit critical section: save/restore PRIMASK (Cortex-M0 has no
 * BASEPRI). ENTER declares a local variable; ENTER and EXIT must appear
 * in the same C scope -- satisfied by n32_adv_trace.c.
 */
#define UTIL_ADV_TRACE_ENTER_CRITICAL_SECTION() \
    uint32_t _trace_pm = __get_PRIMASK(); __disable_irq()

#define UTIL_ADV_TRACE_EXIT_CRITICAL_SECTION()  \
    __set_PRIMASK(_trace_pm)

/* -------------------------------------------------------------------------
 * UTIL_ADV_TRACE_CONDITIONNAL -- verbose-level + timestamp + region gating
 * ------------------------------------------------------------------------- */

/** Enable conditional (verbose-level gated) trace API. */
#define UTIL_ADV_TRACE_CONDITIONNAL

/** Scratch space for the timestamp prefix in COND_FSend. */
#define UTIL_ADV_TRACE_TMP_MAX_TIMESTMAP_SIZE  20U

/* Verbose level values -- 0 = silent, 3 = most verbose */
#define VLEVEL_OFF     0
#define VLEVEL_ALWAYS  0
#define VLEVEL_L       1
#define VLEVEL_W       1
#define VLEVEL_M       2
#define VLEVEL_H       3

/* Timestamp selector passed as first arg to APP_LOG / COND_FSend */
#define TS_OFF  0
#define TS_ON   1

/* Region mask: 0 means "no region filtering" -- always passes the region check */
#define T_REG_OFF  0

/** Default verbose level (override by defining VERBOSE_LEVEL before this header). */
#ifndef VERBOSE_LEVEL
#define VERBOSE_LEVEL  VLEVEL_H
#endif

/** Controls whether APP_LOG() generates code (1) or is compiled away (0). */
#ifndef APP_LOG_ENABLED
#define APP_LOG_ENABLED  1
#endif

/* -------------------------------------------------------------------------
 * n32_seq cooperative task sequencer configuration
 * ------------------------------------------------------------------------- */

/** Number of priority levels supported. */
#define UTIL_SEQ_CONF_PRIO_NBR   2U

/** Task bitmap width (32-bit -> 32 tasks maximum). */
#define UTIL_SEQ_CONF_TASK_NBR   32U

/** memset wrapper (fallback used by n32_seq.c when UTIL_SEQ_MEMSET8 isn't set). */
#define UTILS_MEMSET8(dest, value, size)  memset((dest), (value), (size))

/** One-time resource allocation (no-op -- no OS primitives needed). */
#define UTIL_SEQ_INIT_CRITICAL_SECTION()

/**
 * Enter/exit critical section: save/restore PRIMASK (Cortex-M0 has no
 * BASEPRI). ENTER declares a local variable; ENTER and EXIT must appear
 * in the same C scope -- satisfied by n32_seq.c.
 */
#define UTIL_SEQ_ENTER_CRITICAL_SECTION() \
    uint32_t _seq_pm = __get_PRIMASK(); __disable_irq()

#define UTIL_SEQ_EXIT_CRITICAL_SECTION()  \
    __set_PRIMASK(_seq_pm)

/* -------------------------------------------------------------------------
 * n32_timer software timer server configuration
 * ------------------------------------------------------------------------- */

/** One-time resource allocation (no-op -- no OS primitives needed). */
#define UTIL_TIMER_INIT_CRITICAL_SECTION()

/**
 * Enter/exit critical section: save/restore PRIMASK (Cortex-M0 has no
 * BASEPRI). ENTER declares a local variable; ENTER and EXIT must appear
 * in the same C scope -- satisfied by n32_timer.c.
 */
#define UTIL_TIMER_ENTER_CRITICAL_SECTION() \
    uint32_t _timer_pm = __get_PRIMASK(); __disable_irq()

#define UTIL_TIMER_EXIT_CRITICAL_SECTION()  \
    __set_PRIMASK(_timer_pm)

/* -------------------------------------------------------------------------
 * n32_lpm low power manager configuration
 * ------------------------------------------------------------------------- */

/** One-time resource allocation (no-op -- no OS primitives needed). */
#define UTIL_LPM_INIT_CRITICAL_SECTION()

/**
 * Enter/exit critical section: save/restore PRIMASK (Cortex-M0 has no
 * BASEPRI). ENTER declares a local variable; ENTER and EXIT must appear
 * in the same C scope -- satisfied by n32_lpm.c.
 */
#define UTIL_LPM_ENTER_CRITICAL_SECTION() \
    uint32_t _lpm_pm = __get_PRIMASK(); __disable_irq()

#define UTIL_LPM_EXIT_CRITICAL_SECTION()  \
    __set_PRIMASK(_lpm_pm)

#endif /* UTILITIES_CONF_H */
