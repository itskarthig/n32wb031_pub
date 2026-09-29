/**
 * @file app_log.h
 * @brief APP_LOG / APP_PRINTF / APP_TPRINTF convenience macros for n32_adv_trace.
 *
 * Modeled on STM32CubeWL's sys_app.h logging macro block (Init/battery/temp/
 * unique-ID prototypes intentionally not carried over -- out of scope here).
 * Requires UTIL_ADV_TRACE_Init() to have been called once before use.
 */

#ifndef APP_LOG_H
#define APP_LOG_H

#include "n32_adv_trace.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(APP_LOG_ENABLED) && (APP_LOG_ENABLED == 1)
/**
 * @brief Conditionally log a formatted string.
 * @param TS  TS_ON to prepend a timestamp, TS_OFF for none.
 * @param VL  Required verbose level (VLEVEL_L / _M / _H). Skipped if VL is
 *            above the level set by UTIL_ADV_TRACE_SetVerboseLevel().
 */
#define APP_LOG(TS, VL, ...) \
    do { UTIL_ADV_TRACE_COND_FSend((VL), T_REG_OFF, (TS), __VA_ARGS__); } while (0)
#elif defined(APP_LOG_ENABLED) && (APP_LOG_ENABLED == 0)
#define APP_LOG(TS, VL, ...)  do {} while (0)
#else
#error "APP_LOG_ENABLED not defined (check utilities_conf.h)"
#endif

/** @brief Unconditional log, no timestamp. */
#define APP_PRINTF(...) \
    do { UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF, __VA_ARGS__); } while (0)

/** @brief Unconditional log, with timestamp. */
#define APP_TPRINTF(...) \
    do { UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_ON, __VA_ARGS__); } while (0)

#ifdef __cplusplus
}
#endif

#endif /* APP_LOG_H */
