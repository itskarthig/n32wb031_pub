/**
 * @file n32_lpm.c
 * @brief Generic Low Power Manager -- voter-bitmask engine, hardware-agnostic.
 *
 * Two independent voter bitmasks (StopModeDisable/OffModeDisable) select
 * the deepest mode every voter still allows; dispatch goes through the
 * project-local UTIL_PowerDriver (see lpm_if.c).
 *
 * PRIMASK save/restore uses local hand-rolled lpm_prim_save()/restore()
 * helpers, not utilities_conf.h's UTIL_LPM_ENTER/EXIT_CRITICAL_SECTION()
 * macros -- keeps this module self-contained with no dependency on that
 * header's critical-section contract.
 */

#include "n32_lpm.h"

#define UTIL_LPM_NO_BIT_SET (0UL)

/* PRIMASK helpers -- no CMSIS dependency. "memory" clobber on all three
 * matches genuine ARM CMSIS (cmsis_gcc.h's __get_PRIMASK/__disable_irq/
 * __set_PRIMASK) and this project's own common/cmsis_compiler.h shim exactly,
 * so the compiler can't reorder memory accesses across these barriers. */
static inline uint32_t lpm_prim_save(void)
{
    uint32_t m;
    __asm volatile ("MRS %0, PRIMASK" : "=r"(m) :: "memory");
    __asm volatile ("CPSID I" ::: "memory");
    return m;
}
static inline void lpm_prim_restore(uint32_t m)
{
    __asm volatile ("MSR PRIMASK, %0" : : "r"(m) : "memory");
}

/** Voters that currently block StopMode (nonzero -> SleepMode forced). */
static UTIL_LPM_bm_t StopModeDisable = UTIL_LPM_NO_BIT_SET;

/** Voters that currently block OffMode (nonzero -> StopMode forced). */
static UTIL_LPM_bm_t OffModeDisable = UTIL_LPM_NO_BIT_SET;

void UTIL_LPM_Init(void)
{
    StopModeDisable = UTIL_LPM_NO_BIT_SET;
    OffModeDisable  = UTIL_LPM_NO_BIT_SET;
}

void UTIL_LPM_DeInit(void)
{
}

void UTIL_LPM_SetStopMode(UTIL_LPM_bm_t lpm_id_bm, UTIL_LPM_State_t state)
{
    uint32_t m = lpm_prim_save();

    switch (state)
    {
        case UTIL_LPM_DISABLE:
            StopModeDisable |= lpm_id_bm;
            break;
        case UTIL_LPM_ENABLE:
            StopModeDisable &= (~lpm_id_bm);
            break;
        default:
            break;
    }

    lpm_prim_restore(m);
}

void UTIL_LPM_SetOffMode(UTIL_LPM_bm_t lpm_id_bm, UTIL_LPM_State_t state)
{
    uint32_t m = lpm_prim_save();

    switch (state)
    {
        case UTIL_LPM_DISABLE:
            OffModeDisable |= lpm_id_bm;
            break;
        case UTIL_LPM_ENABLE:
            OffModeDisable &= (~lpm_id_bm);
            break;
        default:
            break;
    }

    lpm_prim_restore(m);
}

UTIL_LPM_Mode_t UTIL_LPM_GetMode(void)
{
    UTIL_LPM_Mode_t mode_selected;

    uint32_t m = lpm_prim_save();

    if (StopModeDisable != UTIL_LPM_NO_BIT_SET)
    {
        /* At least one user disallows StopMode. */
        mode_selected = UTIL_LPM_SLEEPMODE;
    }
    else if (OffModeDisable != UTIL_LPM_NO_BIT_SET)
    {
        /* At least one user disallows OffMode. */
        mode_selected = UTIL_LPM_STOPMODE;
    }
    else
    {
        mode_selected = UTIL_LPM_OFFMODE;
    }

    lpm_prim_restore(m);

    return mode_selected;
}

UTIL_LPM_bm_t UTIL_LPM_GetStopModeDisableMask(void)
{
    UTIL_LPM_bm_t mask;

    uint32_t m = lpm_prim_save();
    mask = StopModeDisable;
    lpm_prim_restore(m);

    return mask;
}

void UTIL_LPM_EnterLowPower(void)
{
    uint32_t m = lpm_prim_save();

    if (StopModeDisable != UTIL_LPM_NO_BIT_SET)
    {
        /* SleepMode required. */
        UTIL_PowerDriver.EnterSleepMode();
        UTIL_PowerDriver.ExitSleepMode();
    }
    else if (OffModeDisable != UTIL_LPM_NO_BIT_SET)
    {
        /* StopMode required. */
        UTIL_PowerDriver.EnterStopMode();
        UTIL_PowerDriver.ExitStopMode();
    }
    else
    {
        /* OffMode required. */
        UTIL_PowerDriver.EnterOffMode();
        UTIL_PowerDriver.ExitOffMode();
    }

    lpm_prim_restore(m);
}
