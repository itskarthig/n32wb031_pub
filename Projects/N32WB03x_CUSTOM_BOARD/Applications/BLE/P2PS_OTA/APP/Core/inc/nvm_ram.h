/**
 * @file nvm_ram.h
 * @brief Generic .noinit RAM persistence -- single struct, single magic word,
 * survives NRST (warm reset) while VDD is held (does NOT survive a power
 * loss/cold boot). Backed by the NVM_RAM linker region (Linker/app.ld,
 * 0x2000BF80, 128 B) reserved just above the stack.
 */

#ifndef NVM_RAM_H
#define NVM_RAM_H

#include <stdint.h>

/* Add new persistent fields here as needed -- bump NVM_RAM_MAGIC in
 * nvm_ram.c whenever this layout changes, so a stale struct from an older
 * firmware's layout is never misread as valid. */
typedef struct
{
    uint32_t magic;
    uint32_t pulse_count;  /* PB1 button presses -- UserApp_PulseCounter() in application.c */
    uint32_t systime_delta_sec;       /* SysTimeSet() delta, seconds -- wraparound-signed,
                                        * matches SysTime_t.Seconds. See systime_if.c: this
                                        * substitutes for the RTC backup registers genuine
                                        * STM32 parts have and N32WB031 lacks -- the RTC
                                        * calendar itself is never rewritten, so timer_if.c's
                                        * scheduling clock stays permanently unaffected by a
                                        * SysTimeSet() call. */
    int16_t  systime_delta_subsec_ms; /* SysTimeSet() delta, sub-second ms part -- matches
                                        * SysTime_t.SubSeconds. */
} NvmRam_t;

/**
 * @brief Validates the .noinit-resident NvmRam_t; zero-fills it (and sets
 * magic) only on cold power-on / first boot / a layout mismatch, leaving it
 * untouched on a warm reset. Call once, early in APP_Init() -- before
 * anything could read or write NvmRam_Get().
 */
void NvmRam_Init(void);

/**
 * @brief Pointer to the single live .noinit-resident instance. Callers read
 * and write fields directly -- one global instance, no locking needed since
 * every writer runs in UTIL_SEQ task/main-loop context, never ISR context
 * (matches this codebase's cooperative-scheduler convention -- e.g. PB1's
 * own EXTI handler already defers to a task before touching state).
 */
NvmRam_t *NvmRam_Get(void);

#endif /* NVM_RAM_H */
