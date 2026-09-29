cmake_minimum_required(VERSION 3.20)

# ---- SDK helpers -------------------------------------------------------------
if(NOT DEFINED N32WB03X_SDK_ROOT)
    set(N32WB03X_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../N32WB03x_SDK/N32WB03x_SDK_V1.3.3")
endif()

set(SDK      "${N32WB03X_SDK_ROOT}")

# ---- Project-local source tree -- Core/src+Core/inc, matching APP's split ----
set(APP_CORE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/Core/src")
set(APP_CORE_INC "${CMAKE_CURRENT_SOURCE_DIR}/Core/inc")

set(LD_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/Linker/installer.ld")

# ---- Source files -----------------------------------------------------------
set(INSTALLER_SOURCES
    # GCC startup
    "${SDK}/firmware/CMSIS/device/startup/startup_n32wb03x_gcc.s"

    # HAL peripheral drivers
    "${SDK}/firmware/n32wb03x_std_periph_driver/src/misc.c"
    "${SDK}/firmware/n32wb03x_std_periph_driver/src/n32wb03x_rcc.c"
    "${SDK}/firmware/n32wb03x_std_periph_driver/src/n32wb03x_crc.c"
    "${SDK}/firmware/n32wb03x_std_periph_driver/src/n32wb03x_qflash.c"

    # DFU CRC utility
    "${SDK_COMMON_DIR}/dfu_crc.c"

    # Application entry
    "${APP_CORE_SRC}/main.c"
)

# ---- Include directories ----------------------------------------------------
set(INSTALLER_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/../Common"   # P2PS_OTA/Common/install_flag.h (project-local, unique magic) -- must come before common/
    "${APP_CORE_INC}"
    "${CMAKE_SOURCE_DIR}/Projects/Common"
    "${SDK_COMMON_DIR}"   # dfu_crc.h -- unrelated to the Projects/Common migration, stays here
    "${SDK}/firmware/CMSIS/core"
    "${SDK}/firmware/CMSIS/device"
    "${SDK}/firmware/n32wb03x_std_periph_driver/inc"
)

# ---- Compile definitions ----------------------------------------------------
set(INSTALLER_DEFINES
    N32WB03x
    USE_STDPERIPH_DRIVER
    APP_BANK=1
)

# ---- Target -----------------------------------------------------------------
add_executable(P2PS_OTA_CB_INSTALLER ${INSTALLER_SOURCES})

target_include_directories(P2PS_OTA_CB_INSTALLER PRIVATE ${INSTALLER_INCLUDES})
target_compile_definitions(P2PS_OTA_CB_INSTALLER PRIVATE ${INSTALLER_DEFINES})

target_compile_options(P2PS_OTA_CB_INSTALLER PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -Os
        -include "${CMAKE_SOURCE_DIR}/Projects/Common/V200/gcc_compat.h"
        -Wno-unused-parameter
    >
)

if(COMMAND n32wb03x_apply_build_type)
    n32wb03x_apply_build_type(P2PS_OTA_CB_INSTALLER)
endif()

set_target_properties(P2PS_OTA_CB_INSTALLER PROPERTIES
    SUFFIX ".elf"
    OUTPUT_NAME "P2PS_OTA_CB_INSTALLER"
)

target_link_options(P2PS_OTA_CB_INSTALLER PRIVATE
    -mcpu=cortex-m0
    -mthumb
    -mfloat-abi=soft
    -Wl,--gc-sections
    -Wl,--no-wchar-size-warning
    -T "${LD_SCRIPT}"
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/P2PS_OTA_CB_INSTALLER.map
    --specs=nano.specs
    --specs=nosys.specs
    -lm
)

# Post-build: generate .bin
if(DEFINED OBJCOPY_EXECUTABLE)
    add_custom_command(TARGET P2PS_OTA_CB_INSTALLER POST_BUILD
        COMMAND "${OBJCOPY_EXECUTABLE}"
            -O binary
            "$<TARGET_FILE:P2PS_OTA_CB_INSTALLER>"
            "${CMAKE_CURRENT_BINARY_DIR}/P2PS_OTA_CB_INSTALLER.bin"
        COMMENT "Generating P2PS_OTA_CB_INSTALLER.bin"
    )
    if(DEFINED SIZE_EXECUTABLE)
        add_custom_command(TARGET P2PS_OTA_CB_INSTALLER POST_BUILD
            COMMAND "${SIZE_EXECUTABLE}" "$<TARGET_FILE:P2PS_OTA_CB_INSTALLER>"
            COMMENT "Binary size:"
        )
    endif()
endif()
