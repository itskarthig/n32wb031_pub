/**
 * @file n32_lpm.h
 * @brief Generic Low Power Manager -- voter-bitmask engine, hardware-agnostic.
 *
 * Pairs with a project-local lpm_if.c/.h that implements the 6
 * UTIL_LPM_Driver_s functions using real N32 PWR register sequences.
 */

#ifndef N32_LPM_H
#define N32_LPM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** Bit mask of an LPM mode voter identifier. */
typedef uint32_t UTIL_LPM_bm_t;

/** Whether a given voter allows (ENABLE) or blocks (DISABLE) a mode. */
typedef enum
{
    UTIL_LPM_ENABLE = 0,
    UTIL_LPM_DISABLE,
} UTIL_LPM_State_t;

/** Low power mode tiers, lightest to deepest. */
typedef enum
{
    UTIL_LPM_SLEEPMODE,
    UTIL_LPM_STOPMODE,
    UTIL_LPM_OFFMODE,
} UTIL_LPM_Mode_t;

/** Hardware driver contract -- implemented by the project-local lpm_if.c. */
struct UTIL_LPM_Driver_s
{
    void (*EnterSleepMode)(void);
    void (*ExitSleepMode)(void);
    void (*EnterStopMode)(void);
    void (*ExitStopMode)(void);
    void (*EnterOffMode)(void);
    void (*ExitOffMode)(void);
};

/** Defined and initialized in the project-local lpm_if.c. */
extern const struct UTIL_LPM_Driver_s UTIL_PowerDriver;

/** Initializes the LPM resources (clears all voter bitmasks). */
void UTIL_LPM_Init(void);

/** Un-initializes the LPM resources. */
void UTIL_LPM_DeInit(void);

/** Returns the low power mode that would be entered right now. */
UTIL_LPM_Mode_t UTIL_LPM_GetMode(void);

/**
 * @brief Raw bitmask of voters currently blocking StopMode (1 bit per
 * voter, see the project's own LpmVoterId_t) -- lets a caller diagnose
 * *which* voter is stuck, beyond what UTIL_LPM_GetMode()'s picked tier
 * alone can show.
 */
UTIL_LPM_bm_t UTIL_LPM_GetStopModeDisableMask(void);

/**
 * @brief Notifies the LPM whether the given voter(s) allow StopMode.
 * @param lpm_id_bm identifier bit(s) of the voter (1 bit per user)
 * @param state UTIL_LPM_ENABLE (allow) or UTIL_LPM_DISABLE (block)
 * @note Default for every voter is StopMode allowed.
 */
void UTIL_LPM_SetStopMode(UTIL_LPM_bm_t lpm_id_bm, UTIL_LPM_State_t state);

/**
 * @brief Notifies the LPM whether the given voter(s) allow OffMode.
 * @param lpm_id_bm identifier bit(s) of the voter (1 bit per user)
 * @param state UTIL_LPM_ENABLE (allow) or UTIL_LPM_DISABLE (block)
 * @note Default for every voter is OffMode allowed -- callers that never
 *       want the MCU to reset on wake must explicitly DISABLE it once.
 */
void UTIL_LPM_SetOffMode(UTIL_LPM_bm_t lpm_id_bm, UTIL_LPM_State_t state);

/**
 * @brief Enters the deepest low power mode currently allowed by all voters.
 * @note Intended to be called from the sequencer's idle hook
 *       (UTIL_SEQ_Idle()), which already runs with interrupts masked.
 */
void UTIL_LPM_EnterLowPower(void);

#ifdef __cplusplus
}
#endif

#endif /* N32_LPM_H */
