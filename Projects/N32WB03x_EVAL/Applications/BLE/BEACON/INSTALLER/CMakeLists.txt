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
    "${CMAKE_CURRENT_SOURCE_DIR}/../Common"   # BEACON/Common/install_flag.h (project-local, unique magic) -- must come before common/
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
add_executable(BEACON_INSTALLER ${INSTALLER_SOURCES})

target_include_directories(BEACON_INSTALLER PRIVATE ${INSTALLER_INCLUDES})
target_compile_definitions(BEACON_INSTALLER PRIVATE ${INSTALLER_DEFINES})

target_compile_options(BEACON_INSTALLER PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -Os
        -include "${CMAKE_SOURCE_DIR}/Projects/Common/V200/gcc_compat.h"
        -Wno-unused-parameter
    >
)

if(COMMAND n32wb03x_apply_build_type)
    n32wb03x_apply_build_type(BEACON_INSTALLER)
endif()

set_target_properties(BEACON_INSTALLER PROPERTIES
    SUFFIX ".elf"
    OUTPUT_NAME "BEACON_INSTALLER"
)

target_link_options(BEACON_INSTALLER PRIVATE
    -mcpu=cortex-m0
    -mthumb
    -mfloat-abi=soft
    -Wl,--gc-sections
    -Wl,--no-wchar-size-warning
    -T "${LD_SCRIPT}"
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/BEACON_INSTALLER.map
    --specs=nano.specs
    --specs=nosys.specs
    -lm
)

# Post-build: generate .bin
if(DEFINED OBJCOPY_EXECUTABLE)
    add_custom_command(TARGET BEACON_INSTALLER POST_BUILD
        COMMAND "${OBJCOPY_EXECUTABLE}"
            -O binary
            "$<TARGET_FILE:BEACON_INSTALLER>"
            "${CMAKE_CURRENT_BINARY_DIR}/BEACON_INSTALLER.bin"
        COMMENT "Generating BEACON_INSTALLER.bin"
    )
    if(DEFINED SIZE_EXECUTABLE)
        add_custom_command(TARGET BEACON_INSTALLER POST_BUILD
            COMMAND "${SIZE_EXECUTABLE}" "$<TARGET_FILE:BEACON_INSTALLER>"
            COMMENT "Binary size:"
        )
    endif()
endif()
