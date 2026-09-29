#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#include <stdint.h>

#define USER_CFG_MAGIC 0xA5C3E1F7UL
#define USER_CFG_VERSION 13U
#define USER_CFG_FLASH_ADDR 0x0107E000UL /* top 8 KB of flash */
#define USER_CFG_FLASH_SIZE 0x2000U      /* 8 KB = 2 × 4 KB sectors */
#define FIRMWARE_VERSION_BCD 0x0100U      /* BCD: 0x0100 = V1.00 */
#define HARDWARE_VERSION_BCD 0x0103U      /* BCD: 0x0103 = V1.3 */
#define DEVICE_TYPE_BCD 0x7700U          /* BCD: 0x7700 = 77.00 P2PS Device*/

#define USER_CFG_FRAME_TYPE 0x0001U          /* Frame header for Configuration */
#define USER_PWD_FRAME_TYPE 0x0000U          /* Frame header for Password */
#define USER_TIMESTAMP_EPOCH_FRAME_TYPE 0x0002U /* Frame header for Timestamp Epoch */

#define BLE_SESSION_TIMEOUT_SEC 300U /* BLE session timeout in seconds */

/* Placeholder value, shared verbatim across every project in this repo that
 * has one -- change this per deployment before shipping. app_p2ps.c's
 * session-password gate accepts this exact 16-byte value from any client. */
#define USER_PWD {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}
#define USER_PWD_FRAME_LEN  18U  /* 2-byte frame_type + 16-byte password */

typedef struct __attribute__((__packed__))
{
    uint16_t frame_type;          /* 0x0001 - Frame header for Configuration *//*Read-only*/
    uint16_t device_type;         /* offset 0 — BCD: BCD: 0x7700 = 77.00 P2PS Device*//*Read-only*/
    uint32_t magic;               /* offset 2 — USER_CFG_MAGIC *//*Read-only*/
    uint32_t version;             /* offset 6 *//*Read-only*/
    uint16_t firmware_version;    /* BCD: 0x0100 = V1.0 *//*Read-only*/
    uint16_t hardware_version;    /* BCD: 0x0100 = V1.0 *//*Read-only*/
    uint32_t epoch_systime;          /* seconds since 1970-01-01 00:00:00 UTC — set by mobile app */

    uint8_t vcc_mv; /**< Supply voltage encoded (from internal ADC CH6) */ /*Read-only*/
    int8_t die_temp_c;      /**< Die temperature in °C (from internal ADC CH7) */ /*Read-only*/
} user_config_t;


extern const user_config_t UserConfig_Default;


/* Load config from flash into dst.
 * Returns 0 on success.
 * Returns -1 if flash is blank or magic is wrong — dst is filled with defaults. */
int User_Config_Load(user_config_t *dst);

/* Erase config region and write src to flash.
 * Returns 0 on success. */
int User_Config_Save(const user_config_t *src);

#endif /* USER_CONFIG_H */