/**
 * @file install_flag.h
 * @brief Shared install-pending flag — written by app_fuota, read by app_installer.
 *
 * Lives at 0x01002000 (a dedicated 4 KB flash sector, same address previously
 * used by NS_Bootsetting_t).  The sector is left erased (0xFF) at production
 * flash time — no boot-settings generation tool required.
 *
 * STM32WBA55 BLE_ApplicationInstaller-compatible pattern:
 *   - Production flash: just app_installer.bin + app_p2ps_ota.bin (two files).
 *   - Normal boot: magic != INSTALL_MAGIC → installer jumps to APP1 directly.
 *   - OTA install: app writes flag → reboots → installer copies APP2→APP1 → clears flag.
 */

#ifndef INSTALL_FLAG_H
#define INSTALL_FLAG_H

#include <stdint.h>

/* Magic word stored in the flag sector to indicate a pending install.
 * Any value other than this (including 0xFFFFFFFF = erased) means no install.
 *
 * Unique per *_OTA project (not the shared common/install_flag.h value) so a
 * stray/leftover flag written by a DIFFERENT project's app (before this
 * project's firmware was ever flashed onto the chip) can't be misread as a
 * valid pending install here. INSTALL_FLAG_ADDR stays the same across all
 * projects -- it's a flash-layout fact, not project-specific, since only one
 * project's firmware runs on a given chip at a time. */
#define INSTALL_MAGIC       0x50325F32U   /* "P2_2" -- P2PS_OTA (N32WB03x_CUSTOM_BOARD copy) */

/* Flash address of the 4 KB flag sector (0x01002000). */
#define INSTALL_FLAG_ADDR   0x01002000U

/**
 * @brief Pending-install flag written to flash at INSTALL_FLAG_ADDR.
 *
 * Written by app_fuota on OTA download complete.
 * Read by app_installer on every boot.
 * Cleared (sector erased) by app_installer after a successful install.
 */
typedef struct
{
    uint32_t magic;     /**< INSTALL_MAGIC when install pending; 0xFFFFFFFF = none */
    uint32_t src_addr;  /**< Source flash address (download slot = 0x01040000)     */
    uint32_t size;      /**< Image size in bytes                                   */
    uint32_t crc;       /**< CRC32 of image (computed via Qflash_Read)             */
} InstallFlag_t;

#endif /* INSTALL_FLAG_H */
