cmake_minimum_required(VERSION 3.20)

# ---- SDK helpers -------------------------------------------------------------
if(NOT DEFINED N32WB03X_SDK_ROOT)
    set(N32WB03X_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../N32WB03x_SDK/N32WB03x_SDK_V2.0.0")
endif()

set(SDK      "${N32WB03X_SDK_ROOT}")

# ---- Project-local source tree -- Core/UserApp/BLE split (each with its own
# src/inc), not a flat src/+inc/. ----------------------------------------------
set(APP_CORE_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/Core/src")
set(APP_CORE_INC    "${CMAKE_CURRENT_SOURCE_DIR}/Core/inc")
set(APP_USERAPP_SRC "${CMAKE_CURRENT_SOURCE_DIR}/UserApp/src")
set(APP_USERAPP_INC "${CMAKE_CURRENT_SOURCE_DIR}/UserApp/inc")
set(APP_BLE_SRC     "${CMAKE_CURRENT_SOURCE_DIR}/BLE/src")
set(APP_BLE_INC     "${CMAKE_CURRENT_SOURCE_DIR}/BLE/inc")

set(LD_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/Linker/app.ld")

# ---- BLE stack (ns_library, wrapping the N32WB031's RivieraWaves/RWIP BLE
# core) -- paths/requirements independently verified directly against
# N32WB03x_SDK_V2.0.0 (symbol_g15.txt, host_lib, ns_library/ns_ble_stack
# source layout), not derived from app_beacon or any app_ble_* project. See
# CLAUDE.md's BEACON/APP BLE section for the full design rationale.
set(BLE_STK  "${SDK}/middlewares/Nationstech/ble_library")

# ---- Custom profile framework (P2PS/FUOTA) -- shared, generic middleware at
# repo root, not vendor SDK code, not project-specific (confirmed by reading
# both profiles/p2ps/p2pss and profiles/fuota/fuotas in full before porting
# the app_p2ps.c/app_fuota.c layer on top of them).
set(PRF_ROOT "${CMAKE_SOURCE_DIR}/profiles")

# ---- Source files -----------------------------------------------------------
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

    # Peripheral driver needed by ns_sec.c (bond-DB storage, even though
    # bonding is disabled by default -- BLE_BOND_STORE_ENABLE=0 in
    # app_user_config.h -- ns_sec.c references Qflash_* unconditionally)
    "${SDK}/firmware/n32wb03x_std_periph_driver/src/n32wb03x_qflash.c"

    # BLE stack glue (ROM-resident rwip_schedule()/rwip_sleep()/etc. are
    # resolved via symbol_g15.txt below -- these are the non-ROM support files)
    "${BLE_STK}/ns_ble_stack/stack_common/rwip.c"
    "${BLE_STK}/ns_ble_stack/stack_common/rwip_driver.c"
    "${BLE_STK}/ns_ble_stack/stack_common/rwip_host.c"

    # BLE profile framework (required even with zero profiles registered --
    # pulled in by ns_sec.c/ns_ble.c)
    "${BLE_STK}/ns_ble_profile/prf.c"
    "${BLE_STK}/ns_ble_profile/prf_utils.c"
    "${BLE_STK}/ns_ble_profile/prf_utils_128.c"

    # Device Information Service (server) profile
    "${BLE_STK}/ns_ble_profile/dis/diss/src/diss.c"
    "${BLE_STK}/ns_ble_profile/dis/diss/src/diss_task.c"

    # Peer-to-Peer Server (P2PS) and FUOTA (BLE OTA) custom profiles
    "${PRF_ROOT}/p2ps/p2pss/src/p2pss.c"
    "${PRF_ROOT}/p2ps/p2pss/src/p2pss_task.c"
    "${PRF_ROOT}/fuota/fuotas/src/fuotas.c"
    "${PRF_ROOT}/fuota/fuotas/src/fuotas_task.c"

    # ns_library -- N32's own wrapper over the BLE host/controller (GAP/GATT
    # device+advertising config, security, sleep integration)
    "${BLE_STK}/ns_library/error/ns_error.c"
    "${BLE_STK}/ns_library/sleep/ns_sleep.c"
    "${BLE_STK}/ns_library/sec/ns_sec.c"
    "${BLE_STK}/ns_library/scheduler/ns_scheduler.c"
    "${BLE_STK}/ns_library/ble/ns_ble.c"
    "${BLE_STK}/ns_library/ble/ns_ble_task.c"

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

    # Deferred NS_LOG_x staging queue + drain task -- see Core/inc/ns_log_if.h
    "${APP_CORE_SRC}/ns_log_if.c"

    # Generic .noinit RAM persistence (single struct, single magic word --
    # see Core/inc/nvm_ram.h)
    "${APP_CORE_SRC}/nvm_ram.c"

    # User application logic (periodic task/timer registration, moved out of app_entry.c)
    "${APP_USERAPP_SRC}/application.c"
    "${APP_USERAPP_SRC}/user_config.c"

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

    # BLE bring-up (device/GAP config, security baseline, fast/slow ADV)
    "${APP_BLE_SRC}/app_ble.c"

    # Device Information Service application layer
    "${APP_BLE_SRC}/app_dis.c"

    # Peer-to-Peer Server and FUOTA application layer
    "${APP_BLE_SRC}/app_p2ps.c"
    "${APP_BLE_SRC}/app_fuota.c"
)

# ---- Include directories ----------------------------------------------------
set(APP_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/../Common"   # P2PS_OTA/Common/install_flag.h (project-local, unique magic)
    # APP_CORE_INC first -- this project's own utilities_conf.h (peripheral/pin
    # selection for Utilities/trace/adv_trace) must shadow common/utilities_conf.h.
    "${APP_CORE_INC}"
    "${APP_USERAPP_INC}"
    "${APP_BLE_INC}"
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

    # ---- BLE stack include paths (translated directly from the vendor
    # ble/beacon Keil project's own IncludePath list, MDK-ARM/beacon.uvprojx
    # -- genuine SDK directory layout, independently confirmed to exist) ----
    "${BLE_STK}/ns_ble_stack/ip/ahi/api"
    "${BLE_STK}/ns_ble_stack/ip/ble/hl/api"
    "${BLE_STK}/ns_ble_stack/ip/ble/hl/inc"
    "${BLE_STK}/ns_ble_stack/ip/ble/hl/src/gap"
    "${BLE_STK}/ns_ble_stack/ip/ble/hl/src/gatt"
    "${BLE_STK}/ns_ble_stack/ip/ble/hl/src/l2c"
    "${BLE_STK}/ns_ble_stack/ip/ble/ll/api"
    "${BLE_STK}/ns_ble_stack/ip/ble/ll/src"
    "${BLE_STK}/ns_ble_stack/ip/ble/ll/src/llc"
    "${BLE_STK}/ns_ble_stack/ip/ble/ll/src/lld"
    "${BLE_STK}/ns_ble_stack/ip/ble/ll/src/llm"
    "${BLE_STK}/ns_ble_stack/ip/em/api"
    "${BLE_STK}/ns_ble_stack/ip/hci/api"
    "${BLE_STK}/ns_ble_stack/ip/sch/api"
    "${BLE_STK}/ns_ble_stack/modules/aes/api"
    "${BLE_STK}/ns_ble_stack/modules/common/api"
    "${BLE_STK}/ns_ble_stack/modules/common/src"
    "${BLE_STK}/ns_ble_stack/modules/dbg/api"
    "${BLE_STK}/ns_ble_stack/modules/ecc_p256/api"
    "${BLE_STK}/ns_ble_stack/modules/h4tl/api"
    "${BLE_STK}/ns_ble_stack/modules/ke/api"
    "${BLE_STK}/ns_ble_stack/modules/rwip/api"
    "${BLE_STK}/ns_ble_stack/rfinit/api"
    "${BLE_STK}/ns_ble_stack/arch"
    "${BLE_STK}/ns_ble_stack/stack_common"
    "${BLE_STK}/ns_ble_profile"
    "${BLE_STK}/ns_ble_profile/dis/diss/api"
    "${PRF_ROOT}/p2ps/p2pss/api"
    "${PRF_ROOT}/fuota/fuotas/api"
    "${BLE_STK}/ns_library/adv"
    "${BLE_STK}/ns_library/timer"
    "${BLE_STK}/ns_library/log"
    "${BLE_STK}/ns_library/sleep"
    "${BLE_STK}/ns_library/delay"
    "${BLE_STK}/ns_library/sec"
    "${BLE_STK}/ns_library/ble"
    "${BLE_STK}/ns_library/error"
    "${BLE_STK}/ns_library/scheduler"
)

# ---- Compile definitions ----------------------------------------------------
set(APP_DEFINES
    N32WB03x
    USE_STDPERIPH_DRIVER
    APP_BANK=1
)

# ---- Target -----------------------------------------------------------------
add_executable(P2PS_OTA_APP ${APP_SOURCES})

target_include_directories(P2PS_OTA_APP PRIVATE ${APP_INCLUDES})
target_compile_definitions(P2PS_OTA_APP PRIVATE ${APP_DEFINES})

target_compile_options(P2PS_OTA_APP PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -Os
        # SHELL: prevents CMake's compile-options de-duplication from
        # silently dropping the second "-include" flag below (it treats
        # repeated bare "-include" tokens across the option list as
        # duplicates of each other and removes all but one, keeping only
        # the differing file-path arguments -- a real, confirmed failure
        # mode, not theoretical: produced "cannot specify -o with -c/-S/-E
        # with multiple files" because gcc_compat.h's own -include vanished
        # and ns_log_if.h's path was left as a bare positional argument).
        "SHELL:-include ${CMAKE_SOURCE_DIR}/Projects/Common/V200/gcc_compat.h"
        -Wno-unused-parameter
        # BLE stack register-access headers (e.g. rfinit/api/reg_ipcore.h)
        # declare functions __INLINE, which CMSIS's core_cm0.h (GCC branch)
        # defines as plain C99 "inline" (not "static inline") -- under
        # strict C99 rules that can require an external definition GCC
        # never emits, producing "undefined reference" link errors for
        # functions like ip_deepslcntl_deep_sleep_corr_en_setf(). Restores
        # GNU89 inline semantics (always locally emittable, no external
        # definition required) -- the standard fix for this class of
        # older-CMSIS-header-ported-to-modern-GCC issue, no SDK source
        # changes needed.
        -fgnu89-inline
        # Redirect ns_library's NS_LOG_ERROR/WARNING/INFO/DEBUG (used
        # throughout ns_ble.c/ns_sec.c/etc.) to this project's own APP_LOG
        # (trace_if.c/USART1) instead of the SDK's unusable-here USART/RTT
        # log backends. See ns_log_if.h for how the override survives each
        # BLE stack file's own later include of ns_log.h.
        "SHELL:-include ${APP_CORE_INC}/ns_log_if.h"
    >
)

if(COMMAND n32wb03x_apply_build_type)
    n32wb03x_apply_build_type(P2PS_OTA_APP)
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

set_target_properties(P2PS_OTA_APP PROPERTIES
    SUFFIX ".elf"
    OUTPUT_NAME "P2PS_OTA_APP"
)

# BLE ROM symbol table (rwip_schedule, rwip_sleep, etc.) -- plain GNU-ld
# symbol-assignment file, passed as a second -T script merged with app.ld's
# own section layout. Genuinely present in the SDK (confirmed directly),
# not app_beacon-specific.
set(BLE_SYMDEF "${BLE_STK}/ns_ble_stack/symdef/symbol_g15.txt")

# BLE host library (GCC ELF archive, confirmed present in the SDK's own
# host_lib/ directory alongside the Keil-format .lib).
set(BLE_HOST_LIB "${BLE_STK}/ns_ble_stack/host_lib/libcentral_relay.a")

target_link_options(P2PS_OTA_APP PRIVATE
    -mcpu=cortex-m0
    -mthumb
    -mfloat-abi=soft
    -Wl,--gc-sections
    -Wl,--no-wchar-size-warning
    # Companion to -fgnu89-inline above: under GNU89 inline semantics, every
    # translation unit that includes a BLE stack register-access header
    # (declared plain __INLINE, no __STATIC) emits its own external-linkage
    # copy of that trivial accessor function -- functionally identical
    # duplicates across TUs, harmless to fold into one arbitrary copy.
    -Wl,--allow-multiple-definition
    -T "${LD_SCRIPT}"
    -T "${BLE_SYMDEF}"
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/P2PS_OTA_APP.map
    --specs=nano.specs
    --specs=nosys.specs
    -lm
)

target_link_libraries(P2PS_OTA_APP PRIVATE "${BLE_HOST_LIB}")

# Post-build: generate .bin
if(DEFINED OBJCOPY_EXECUTABLE)
    add_custom_command(TARGET P2PS_OTA_APP POST_BUILD
        COMMAND "${OBJCOPY_EXECUTABLE}"
            -O binary
            "$<TARGET_FILE:P2PS_OTA_APP>"
            "${CMAKE_CURRENT_BINARY_DIR}/P2PS_OTA_APP.bin"
        COMMENT "Generating P2PS_OTA_APP.bin"
    )
    if(DEFINED SIZE_EXECUTABLE)
        add_custom_command(TARGET P2PS_OTA_APP POST_BUILD
            COMMAND "${SIZE_EXECUTABLE}" "$<TARGET_FILE:P2PS_OTA_APP>"
            COMMENT "Binary size:"
        )
    endif()
endif()
