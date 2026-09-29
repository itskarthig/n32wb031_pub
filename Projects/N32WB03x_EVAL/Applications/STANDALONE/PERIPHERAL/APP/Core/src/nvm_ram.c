/**
 * @file nvm_ram.c
 * @brief Generic .noinit RAM persistence -- see nvm_ram.h.
 */

#include "nvm_ram.h"
#include <string.h>

/* "NRM2" -- bump whenever NvmRam_t's layout changes.
 * NRM1 -> NRM2: added systime_delta_sec/systime_delta_subsec_ms (SysTimeSet()
 * delta storage -- see systime_if.c). */
#define NVM_RAM_MAGIC 0x4E524D32U

static NvmRam_t s_nvm_ram __attribute__((section(".noinit")));

void NvmRam_Init(void)
{
    if (s_nvm_ram.magic != NVM_RAM_MAGIC)
    {
        memset(&s_nvm_ram, 0, sizeof(s_nvm_ram));
        s_nvm_ram.magic = NVM_RAM_MAGIC;
    }
}

NvmRam_t *NvmRam_Get(void)
{
    return &s_nvm_ram;
}
