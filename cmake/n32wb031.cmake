# N32WB031 512KB Flash Memory Map
# Include this file from any sub-project CMakeLists.txt

# Flash base
set(FLASH_BASE          0x01000000)
set(FLASH_TOTAL_SIZE    0x80000)     # 512 KB

# Bootloader region
set(BOOT_FLASH_ORIGIN   0x01000000)
set(BOOT_FLASH_SIZE     0x2000)      # 8 KB

# Boot settings (NS_Bootsetting_t) -- flashed separately
set(BOOTSETTING_ORIGIN  0x01002000)
set(BOOTSETTING_SIZE    0x1000)      # 4 KB

# App persistent data / NVM
set(APP_DATA_ORIGIN     0x01003000)
set(APP_DATA_SIZE       0x1000)      # 4 KB

# Application banks (dual-bank OTA)
set(APP1_FLASH_ORIGIN   0x01004000)
set(APP_FLASH_SIZE      0x3C000)     # 240 KB each bank
set(APP2_FLASH_ORIGIN   0x01040000)

# SRAM
set(SRAM_ORIGIN         0x20000000)
set(SRAM_SIZE           0xC000)      # 48 KB

# Convenience: end addresses
math(EXPR BOOT_FLASH_END  "${BOOT_FLASH_ORIGIN}  + ${BOOT_FLASH_SIZE}"  OUTPUT_FORMAT HEXADECIMAL)
math(EXPR APP1_FLASH_END  "${APP1_FLASH_ORIGIN}  + ${APP_FLASH_SIZE}"   OUTPUT_FORMAT HEXADECIMAL)
math(EXPR APP2_FLASH_END  "${APP2_FLASH_ORIGIN}  + ${APP_FLASH_SIZE}"   OUTPUT_FORMAT HEXADECIMAL)

message(STATUS "N32WB031 memory map:")
message(STATUS "  Bootloader : ${BOOT_FLASH_ORIGIN} - ${BOOT_FLASH_END}  (${BOOT_FLASH_SIZE} bytes)")
message(STATUS "  APP1       : ${APP1_FLASH_ORIGIN} - ${APP1_FLASH_END}  (${APP_FLASH_SIZE} bytes)")
message(STATUS "  APP2       : ${APP2_FLASH_ORIGIN} - ${APP2_FLASH_END}  (${APP_FLASH_SIZE} bytes)")
message(STATUS "  SRAM       : ${SRAM_ORIGIN}, size ${SRAM_SIZE}")
