/**
 * @file n32_mem.h
 * @brief Standard memory utility functions — N32WB031 analog of stm32_mem.h.
 *
 * Provides UTIL_MEM_cpy_8, UTIL_MEM_cpyr_8, UTIL_MEM_set_8.
 * Used by LoRaWAN crypto and radio drivers (LoRaMacCrypto, SX126x driver).
 *
 * UTIL_MEM_PLACE_IN_SECTION / UTIL_MEM_ALIGN map to the GCC equivalents
 * via utilities_conf.h so LoRaWAN code compiled against stm32_mem.h
 * links without modification.
 */
#ifndef N32_MEM_H
#define N32_MEM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "utilities_conf.h"

/* ---- Memory placement macros (stm32_mem.h compatible) ------------------- */
#define UTIL_MEM_PLACE_IN_SECTION( __x__ )  UTIL_PLACE_IN_SECTION( __x__ )
#define UTIL_MEM_ALIGN                       ALIGN

/* ---- API ----------------------------------------------------------------- */

/**
 * @brief Copy size bytes from src to dst (byte-by-byte, forward).
 */
void UTIL_MEM_cpy_8( void *dst, const void *src, uint16_t size );

/**
 * @brief Copy size bytes from src to dst in reverse order.
 *        dst[0] = src[size-1], dst[1] = src[size-2], ...
 *        Used by radio drivers to swap byte order of EUI/keys.
 */
void UTIL_MEM_cpyr_8( void *dst, const void *src, uint16_t size );

/**
 * @brief Fill size bytes at dst with value.
 */
void UTIL_MEM_set_8( void *dst, uint8_t value, uint16_t size );

#ifdef __cplusplus
}
#endif

#endif /* N32_MEM_H */
