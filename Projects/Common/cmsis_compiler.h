/**
 * @file cmsis_compiler.h
 * @brief CMSIS v5 compiler abstraction shim for N32WB031 GCC build.
 *
 * The N32 SDK ships CMSIS v4 which lacks cmsis_compiler.h.
 * n32_timer.h includes it for compiler portability defines.
 *
 * If n32wb03x.h was already included (pulls in core_cmFunc.h which
 * defines the PRIMASK intrinsics), we skip them to avoid redefinition.
 * If n32wb03x.h was NOT included first, we define them here via inline ASM.
 */

#ifndef CMSIS_COMPILER_H
#define CMSIS_COMPILER_H

#include <stdint.h>

/* ---- Compiler attribute helpers ------------------------------------------ */

#ifndef __WEAK
#define __WEAK   __attribute__((weak))
#endif

#ifndef __STATIC_INLINE
#define __STATIC_INLINE  static inline
#endif

#ifndef __STATIC_FORCEINLINE
#define __STATIC_FORCEINLINE  static inline __attribute__((always_inline))
#endif

/* ---- Cortex-M0 PRIMASK intrinsics ----------------------------------------
 * core_cmFunc.h (included transitively via n32wb03x.h) also defines these.
 * We define them here first and then set the core header include guards, so
 * that when the SDK headers are later included they see the guard and skip
 * their own duplicate definitions.
 * -------------------------------------------------------------------------*/
#ifndef __CORE_CMFUNC_H
#define __CORE_CMFUNC_H   /* block sdk core_cmFunc.h from redefining these */

__STATIC_FORCEINLINE uint32_t __get_PRIMASK(void)
{
    uint32_t result;
    __asm volatile ("MRS %0, primask" : "=r" (result) :: "memory");
    return result;
}

__STATIC_FORCEINLINE void __set_PRIMASK(uint32_t priMask)
{
    __asm volatile ("MSR primask, %0" :: "r" (priMask) : "memory");
}

__STATIC_FORCEINLINE void __disable_irq(void)
{
    __asm volatile ("cpsid i" ::: "memory");
}

__STATIC_FORCEINLINE void __enable_irq(void)
{
    __asm volatile ("cpsie i" ::: "memory");
}

__STATIC_FORCEINLINE uint32_t __get_MSP(void)
{
    uint32_t result;
    __asm volatile ("MRS %0, msp" : "=r" (result) :: "memory");
    return result;
}

__STATIC_FORCEINLINE void __set_MSP(uint32_t topOfMainStack)
{
    __asm volatile ("MSR msp, %0" :: "r" (topOfMainStack) : "memory");
}

#endif /* !__CORE_CMFUNC_H */

#ifndef __CORE_CMINSTR_H
#define __CORE_CMINSTR_H  /* block sdk core_cmInstr.h from redefining these */

__STATIC_FORCEINLINE void __WFI(void)
{
    __asm volatile ("wfi" ::: "memory");
}

__STATIC_FORCEINLINE void __WFE(void)
{
    __asm volatile ("wfe" ::: "memory");
}

__STATIC_FORCEINLINE void __SEV(void)
{
    __asm volatile ("sev" ::: "memory");
}

__STATIC_FORCEINLINE void __DMB(void)
{
    __asm volatile ("dmb 0xF" ::: "memory");
}

__STATIC_FORCEINLINE void __DSB(void)
{
    __asm volatile ("dsb 0xF" ::: "memory");
}

__STATIC_FORCEINLINE void __ISB(void)
{
    __asm volatile ("isb 0xF" ::: "memory");
}

#endif /* !__CORE_CMINSTR_H */

#endif /* CMSIS_COMPILER_H */
