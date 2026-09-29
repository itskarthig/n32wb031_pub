/**
 * @file user_config.c
 * @brief Flash load/save for user_config.h's user_config_t.
 *
 * Pattern ported from app_ble_cellular_wmbus's user_config.c (Qflash-based
 * load/save, single erase + page write since the whole struct fits in one
 * 256-byte page) -- the struct itself is this project's own minimal
 * template shape (see user_config.h), not the old project's 234-byte
 * cellular/WMBus-heavy one.
 */

#include "user_config.h"
#include "n32wb03x_qflash.h"
#include <string.h>

#define CFG_PAGE_SIZE 0x100U /* 256-byte write granularity */

const user_config_t UserConfig_Default = {
    .frame_type       = USER_CFG_FRAME_TYPE,
    .device_type      = DEVICE_TYPE_BCD,
    .magic            = USER_CFG_MAGIC,
    .version          = USER_CFG_VERSION,
    .firmware_version = FIRMWARE_VERSION_BCD,
    .hardware_version = HARDWARE_VERSION_BCD,
    .epoch_systime    = 0U,
    .vcc_mv           = 0U,
    .die_temp_c       = 0,
};

int User_Config_Load(user_config_t *dst)
{
    Qflash_Init();
    Qflash_Read(USER_CFG_FLASH_ADDR, (uint8_t *)dst, sizeof(*dst));
    dst->firmware_version = FIRMWARE_VERSION_BCD; /* always use current firmware version */
    dst->hardware_version = HARDWARE_VERSION_BCD; /* always use current hardware version */
    if (dst->magic != USER_CFG_MAGIC || dst->version != USER_CFG_VERSION)
    {
        *dst = UserConfig_Default;
        return -1;
    }
    return 0;
}

int User_Config_Save(const user_config_t *src)
{
    uint8_t page_buf[CFG_PAGE_SIZE];

    Qflash_Init();
    Qflash_Erase_Sector(USER_CFG_FLASH_ADDR); /* user_config_t (~22 B) fits in one 256-byte page */

    memset(page_buf, 0xFF, CFG_PAGE_SIZE);
    memcpy(page_buf, src, sizeof(*src));
    Qflash_Write(USER_CFG_FLASH_ADDR, page_buf, CFG_PAGE_SIZE);
    return 0;
}
