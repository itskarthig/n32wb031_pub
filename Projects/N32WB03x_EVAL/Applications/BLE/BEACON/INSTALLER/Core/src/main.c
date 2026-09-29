/**
 * @file main.c
 * @brief N32WB031 app_installer — STM32WBA55-style copy-on-boot installer.
 *
 * Runs at 0x01000000 on every boot. Checks for a pending OTA install flag,
 * copies APP2 → APP1 if flagged, resets, then on next boot jumps to APP1.
 *
 * Flash layout:
 *   0x01000000  8 KB   app_installer  (this image)
 *   0x01002000  4 KB   InstallFlag_t  (erased = no pending install)
 *   0x01003000  4 KB   App NVM data
 *   0x01004000  240 KB APP1           (always-active execution bank)
 *   0x01040000  240 KB APP2           (FUOTA download slot)
 */

#include "n32wb03x.h"
#include "n32wb03x_qflash.h"
#include "dfu_crc.h"
#include "install_flag.h"

#define INSTALLER_APP1_ADDR  0x01004000U
#define INSTALLER_APP2_ADDR  0x01040000U

void SystemInit(void) {}   /* clocks already configured by BLE ROM */

/* ---- Flash helpers --------------------------------------------------------- */

#define COPY_SECTOR_SIZE  0x1000U
#define COPY_CHUNK_SIZE   256U

/* NVM_RAM (0x2000BF80, 128 B) is owned by APP's nvm_ram.c (.noinit
 * persistent state, e.g. the PB1 pulse counter). app_installer must NOT
 * write to this region. */

static uint8_t copy_buf[COPY_CHUNK_SIZE] __attribute__((aligned(4)));

static void flash_copy(uint32_t dst, uint32_t src, uint32_t size)
{
    uint32_t nsectors = (size + COPY_SECTOR_SIZE - 1) / COPY_SECTOR_SIZE;
    uint32_t i, pos = 0;

    for (i = 0; i < nsectors; i++)
        Qflash_Erase_Sector(dst + i * COPY_SECTOR_SIZE);

    while (pos < size)
    {
        uint32_t chunk = size - pos;
        if (chunk > COPY_CHUNK_SIZE) chunk = COPY_CHUNK_SIZE;
        Qflash_Read(src + pos, copy_buf, chunk);
        Qflash_Write(dst + pos, copy_buf, chunk);
        pos += chunk;
    }
}

static uint32_t flash_crc32(uint32_t addr, uint32_t size)
{
    uint32_t remaining = size;
    RCC->AHBPCLKEN |= RCC_AHB_PERIPH_CRC;
    CRC->CRC32CTRL = CRC32_CTRL_RESET;
    while (remaining > 0)
    {
        uint32_t chunk = (remaining >= COPY_CHUNK_SIZE) ? COPY_CHUNK_SIZE : remaining;
        Qflash_Read(addr, copy_buf, chunk);
        for (uint32_t ci = 0; ci + 4u <= chunk; ci += 4)
        {
            CRC->CRC32DAT =
                ((uint32_t)copy_buf[ci]     << 24) |
                ((uint32_t)copy_buf[ci + 1] << 16) |
                ((uint32_t)copy_buf[ci + 2] <<  8) |
                 (uint32_t)copy_buf[ci + 3];
        }
        addr      += chunk;
        remaining -= chunk;
    }
    return CRC->CRC32DAT;
}

/* ---- Boot logic ------------------------------------------------------------ */

static void app_install(void)
{
    Qflash_Init();

    InstallFlag_t flag;
    Qflash_Read(INSTALL_FLAG_ADDR, (uint8_t *)&flag, sizeof(InstallFlag_t));

    if (flag.magic == INSTALL_MAGIC)
    {
        /* Validate source image CRC before overwriting APP1 */
        uint32_t src_crc = flash_crc32(flag.src_addr, flag.size);
        if (src_crc != flag.crc)
            goto do_jump;   /* CRC mismatch — fallback to existing APP1 */

        flash_copy(INSTALLER_APP1_ADDR, flag.src_addr, flag.size);

        /* Verify copy integrity */
        if (flash_crc32(INSTALLER_APP1_ADDR, flag.size) != flag.crc)
            while (1) {}    /* copy CRC FAILED — hang, preserve APP2 for retry */

        /* Clear flag then reset — QSPI reinitialises, XIP cache flushes */
        Qflash_Erase_Sector(INSTALL_FLAG_ADDR);
        NVIC_SystemReset();
        /* Does not return */
    }

do_jump:
    {
        uint32_t sp = *(volatile uint32_t *)INSTALLER_APP1_ADDR;
        uint32_t pc = *(volatile uint32_t *)(INSTALLER_APP1_ADDR + 4U);
        __set_MSP(sp);
        ((void (*)(void))(pc | 1U))();
    }
}

int main(void)
{
    app_install();
    while (1) {}
}
