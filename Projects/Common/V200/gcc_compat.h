/**
 * @file gcc_compat.h
 * @brief ARMCC5/AC6 → GCC compatibility shims.
 *
 * Automatically prepended to every C translation unit via:
 *   target_compile_options(... -include "${CMAKE_SOURCE_DIR}/common/gcc_compat.h")
 *
 * Replaces keywords that existed in ARMCC but are not built-in to GCC.
 */

#ifndef _GCC_COMPAT_H_
#define _GCC_COMPAT_H_

/* __align(n) — storage-class specifier in ARMCC.
 * GCC equivalent: __attribute__((aligned(n)))
 * Fixes: n32wb03x_qflash.c, ns_ble.c */
#ifndef __align
#define __align(n)  __attribute__((aligned(n)))
#endif

/* __weak — linkage specifier in ARMCC.
 * GCC equivalent: __attribute__((weak))
 * Fixes: ns_sleep.c (app_sleep_prepare_proc / app_sleep_resume_proc) */
#ifndef __weak
#define __weak  __attribute__((weak))
#endif

#endif /* _GCC_COMPAT_H_ */
