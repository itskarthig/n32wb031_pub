cmake_minimum_required(VERSION 3.20)

# ---- SDK helpers -------------------------------------------------------------
if(NOT DEFINED N32WB03X_SDK_ROOT)
    set(N32WB03X_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../N32WB03x_SDK/N32WB03x_SDK_V2.0.0")
endif()

set(SDK      "${N32WB03X_SDK_ROOT}")

# ---- Project-local source tree -- Core/UserApp split (each with its own
# src/inc), not a flat src/+inc/. No BLE/ split -- this project has no BLE
# code. ----------------------------------------------------------------------
set(APP_CORE_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/Core/src")
set(APP_CORE_INC    "${CMAKE_CURRENT_SOURCE_DIR}/Core/inc")
set(APP_USERAPP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/UserApp/src")
set(APP_USERAPP_INC "${CMAKE_CURRENT_SOURCE_DIR}/UserApp/inc")

set(LD_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/Linker/app.ld")

# ---- Source files -----------------------------------------------------------
# Minimal set matching the CURRENT source tree (main.c only -- no BLE stack,
# FUOTA, or P2PS code exists here yet). app.ld's memory layout (APP1 slot,
# .tag_ota_end OTA tag) anticipates growing this into a full BLE+FUOTA app
# like app_beacon/CMakeLists.txt -- expand APP_SOURCES/APP_INCLUDES the same
# way (BLE host lib, ns_library, DIS/FUOTA/P2PS profile sources) once real
# application source files are added under src/.
set(APP_SOURCES
    # GCC startup (GAS / unified syntax)
    "${SDK}/firmware/CMSIS/device/startup/startup_n32wb03x_gcc.s"

    # Delay cycles assembly -- provides system_delay_cycles() symbol
    # (system_n32wb03x.c has its __ASM version commented out for GCC)
    "${SDK}/firmware/CMSIS/device/system_delay.s"

    # System init (GCC SDK version already has __ASM block commented out)
    "${SDK}/firmware/CMSIS/device/system_n32wb03x.c"

    # HAL peripheral drivers (SystemInit()'s clock/trim setup needs these)
    "${SDK}/firmware/n32wb03x_std_periph_driver/src/misc.c"
    "${SDK}/firmware/n32wb03x_std_periph_driver/src/n32wb03x_rcc.c"

    # Peripheral drivers needed by Utilities/trace/adv_trace/trace_if.c
    "${SDK}/firmware/n32wb03x_std_periph_driver/src/n32wb03x_gpio.c"
    "${SDK}/firmware/n32wb03x_std_periph_driver/src/n32wb03x_usart.c"
    "${SDK}/firmware/n32wb03x_std_periph_driver/src/n32wb03x_lpuart.c"

    # Peripheral driver needed by rtc.c
    "${SDK}/firmware/n32wb03x_std_periph_driver/src/n32wb03x_rtc.c"

    # Peripheral driver needed by timer_if.c (RTC WUT wakeup routes via EXTI_LINE9)
    "${SDK}/firmware/n32wb03x_std_periph_driver/src/n32wb03x_exti.c"

    # Peripheral driver needed by lpm_if.c (PWR_EnterIDLE/SLEEP/PDMode)
    "${SDK}/firmware/n32wb03x_std_periph_driver/src/n32wb03x_pwr.c"

    # Peripheral driver needed by adc.c
    "${SDK}/firmware/n32wb03x_std_periph_driver/src/n32wb03x_adc.c"

    # Utilities -- generic trace/logging engine (shared, hardware-agnostic)
    "${CMAKE_SOURCE_DIR}/Utilities/trace/adv_trace/n32_adv_trace.c"

    # Utilities -- generic cooperative task sequencer (shared, hardware-agnostic)
    "${CMAKE_SOURCE_DIR}/Utilities/sequencer/n32_seq.c"

    # Utilities -- generic system time / calendar-math engine (shared, hardware-agnostic)
    "${CMAKE_SOURCE_DIR}/Utilities/misc/n32_systime.c"

    # Utilities -- generic software timer server (shared, hardware-agnostic)
    "${CMAKE_SOURCE_DIR}/Utilities/timer/n32_timer.c"

    # Utilities -- generic low power manager (shared, hardware-agnostic)
    "${CMAKE_SOURCE_DIR}/Utilities/lpm/tiny_lpm/n32_lpm.c"

    # Application entry
    "${APP_CORE_SRC}/main.c"
    "${APP_CORE_SRC}/app_entry.c"
    "${APP_CORE_SRC}/rtc.c"

    # Generic .noinit RAM persistence (single struct, single magic word --
    # see Core/inc/nvm_ram.h)
    "${APP_CORE_SRC}/nvm_ram.c"

    # User application logic (periodic task/timer registration, moved out of app_entry.c)
    "${APP_USERAPP_SRC}/application.c"

    # Project-local hardware trace driver (peripheral/pin selection differs per project)
    "${APP_CORE_SRC}/trace_if.c"

    # Project-local systime hardware interface (bridges n32_systime to rtc.c)
    "${APP_CORE_SRC}/systime_if.c"

    # Project-local timer hardware interface (bridges n32_timer to the RTC WakeUp Timer)
    "${APP_CORE_SRC}/timer_if.c"

    # Project-local LPM hardware interface (bridges n32_lpm to PWR_Enter*Mode)
    "${APP_CORE_SRC}/lpm_if.c"

    # Project-local ADC driver (internal VREF/VCC voltage + die temperature)
    "${APP_CORE_SRC}/adc.c"

    # Board GPIO init (PB1 button) + centralized ISR for peripherals that
    # don't own their own driver file (PB1 EXTI handler)
    "${APP_CORE_SRC}/gpio.c"
    "${APP_CORE_SRC}/n32wb03x_it.c"
)

# ---- Include directories ----------------------------------------------------
set(APP_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/../Common"   # PERIPHERAL/Common/install_flag.h (project-local, unique magic)
    # APP_CORE_INC first -- this project's own utilities_conf.h (peripheral/pin
    # selection for Utilities/trace/adv_trace) must shadow common/utilities_conf.h.
    "${APP_CORE_INC}"
    "${APP_USERAPP_INC}"
    "${CMAKE_SOURCE_DIR}/Projects/Common"
    "${CMAKE_SOURCE_DIR}/Projects/Common/V200"
    "${SDK}/firmware/CMSIS/core"
    "${SDK}/firmware/CMSIS/device"
    "${SDK}/firmware/n32wb03x_std_periph_driver/inc"
    "${CMAKE_SOURCE_DIR}/Utilities/trace/adv_trace"
    "${CMAKE_SOURCE_DIR}/Utilities/sequencer"
    "${CMAKE_SOURCE_DIR}/Utilities/misc"
    "${CMAKE_SOURCE_DIR}/Utilities/timer"
    "${CMAKE_SOURCE_DIR}/Utilities/lpm/tiny_lpm"
)

# ---- Compile definitions ----------------------------------------------------
set(APP_DEFINES
    N32WB03x
    USE_STDPERIPH_DRIVER
    APP_BANK=1
)

# ---- Target -----------------------------------------------------------------
add_executable(PERIPHERAL_APP ${APP_SOURCES})

target_include_directories(PERIPHERAL_APP PRIVATE ${APP_INCLUDES})
target_compile_definitions(PERIPHERAL_APP PRIVATE ${APP_DEFINES})

target_compile_options(PERIPHERAL_APP PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -Os
        -include "${CMAKE_SOURCE_DIR}/Projects/Common/V200/gcc_compat.h"
        -Wno-unused-parameter
    >
)

if(COMMAND n32wb03x_apply_build_type)
    n32wb03x_apply_build_type(PERIPHERAL_APP)
endif()

# system_n32wb03x.c uses a trick of copying machine code to a stack buffer and
# calling it via a function pointer (SystemTrimValueRead). GCC -O2 eliminates the
# memcpy as "dead code" because it can't see that the copied bytes are executed.
# Compiling with -O0 prevents this optimisation, preserving the memcpy call.
# (Same workaround as app_beacon/CMakeLists.txt.)
set_source_files_properties(
    "${SDK}/firmware/CMSIS/device/system_n32wb03x.c"
    PROPERTIES COMPILE_FLAGS "-O0"
)

set_target_properties(PERIPHERAL_APP PROPERTIES
    SUFFIX ".elf"
    OUTPUT_NAME "PERIPHERAL_APP"
)

target_link_options(PERIPHERAL_APP PRIVATE
    -mcpu=cortex-m0
    -mthumb
    -mfloat-abi=soft
    -Wl,--gc-sections
    -Wl,--no-wchar-size-warning
    -T "${LD_SCRIPT}"
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/PERIPHERAL_APP.map
    --specs=nano.specs
    --specs=nosys.specs
    -lm
)

# Post-build: generate .bin
if(DEFINED OBJCOPY_EXECUTABLE)
    add_custom_command(TARGET PERIPHERAL_APP POST_BUILD
        COMMAND "${OBJCOPY_EXECUTABLE}"
            -O binary
            "$<TARGET_FILE:PERIPHERAL_APP>"
            "${CMAKE_CURRENT_BINARY_DIR}/PERIPHERAL_APP.bin"
        COMMENT "Generating PERIPHERAL_APP.bin"
    )
    if(DEFINED SIZE_EXECUTABLE)
        add_custom_command(TARGET PERIPHERAL_APP POST_BUILD
            COMMAND "${SIZE_EXECUTABLE}" "$<TARGET_FILE:PERIPHERAL_APP>"
            COMMENT "Binary size:"
        )
    endif()
endif()
