# GCC ARM Toolchain for N32WB031 (Cortex-M0)
# Toolchain: arm-none-eabi-gcc 10.3 2021.10
# Path: C:/tools/GNU Arm Embedded Toolchain/10 2021.10/bin/

cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_policy(SET CMP0123 NEW)

# CMake's own compiler-ABI detection (CMakeDetermineCompilerABI.cmake, run
# from the project() call below) does a try_compile() into a separate,
# nested scratch sub-project that re-includes this toolchain file but does
# NOT automatically inherit -D command-line variables from the outer
# configure -- only CMAKE_TOOLCHAIN_FILE itself propagates by default.
# Without this, GCC_PATH reads as undefined inside that nested try_compile
# even though it was passed correctly to the outer configure, and the
# FATAL_ERROR below fires spuriously. This is the documented mechanism for
# exactly this case -- must be set before that nested try_compile runs,
# i.e. unconditionally, early in this file.
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES GCC_PATH)

# ---- Toolchain path ----------------------------------------------------------
# GCC_EXE_SUFFIX describes the HOST running the compiler (Windows needs .exe),
# not the Cortex-M0 target, so it's keyed off CMAKE_HOST_WIN32.
if(CMAKE_HOST_WIN32)
    set(GCC_EXE_SUFFIX ".exe")
else()
    set(GCC_EXE_SUFFIX "")
endif()

if(NOT DEFINED GCC_PATH)
    message(FATAL_ERROR
        "GCC_PATH is not set.\n"
        "Set it in .vscode/settings.json's cmake.configureSettings.GCC_PATH "
        "(VS Code's CMake Tools extension passes it through as -DGCC_PATH=...), "
        "or pass -DGCC_PATH=<path> directly to cmake.\n"
        "The terminal `make` workflow (Makefile) already resolves and passes "
        "this automatically on every invocation -- this error only fires when "
        "cmake is invoked without it, e.g. directly via VS Code with GCC_PATH "
        "left unset in .vscode/settings.json.")
endif()

file(TO_CMAKE_PATH "${GCC_PATH}" GCC_PATH)

set(CROSS_PREFIX "arm-none-eabi-")
set(GCC_EXE     "${GCC_PATH}/${CROSS_PREFIX}gcc${GCC_EXE_SUFFIX}")
set(GXX_EXE     "${GCC_PATH}/${CROSS_PREFIX}g++${GCC_EXE_SUFFIX}")
set(AR_EXE      "${GCC_PATH}/${CROSS_PREFIX}ar${GCC_EXE_SUFFIX}")
set(OBJCOPY_EXE "${GCC_PATH}/${CROSS_PREFIX}objcopy${GCC_EXE_SUFFIX}")
set(SIZE_EXE    "${GCC_PATH}/${CROSS_PREFIX}size${GCC_EXE_SUFFIX}")

if(NOT EXISTS "${GCC_EXE}")
    message(FATAL_ERROR
        "arm-none-eabi-gcc not found at: ${GCC_EXE}\n"
        "Pass -DGCC_PATH=<path> to specify the tools bin directory.")
endif()

# Export objcopy path for post-build use
set(OBJCOPY_EXECUTABLE "${OBJCOPY_EXE}" CACHE FILEPATH "Path to arm-none-eabi-objcopy")
set(SIZE_EXECUTABLE    "${SIZE_EXE}"    CACHE FILEPATH "Path to arm-none-eabi-size")

# ---- Compiler assignments ----------------------------------------------------
set(CMAKE_C_COMPILER   "${GCC_EXE}")
set(CMAKE_CXX_COMPILER "${GXX_EXE}")
set(CMAKE_ASM_COMPILER "${GCC_EXE}")
set(CMAKE_AR           "${AR_EXE}")

# Prevent CMake from testing the compiler (cross-compiler, no OS)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ---- CPU / ABI flags ---------------------------------------------------------
set(CPU_FLAGS "-mcpu=cortex-m0 -mthumb -mfloat-abi=soft")

# -fshort-wchar and -fshort-enums must match the precompiled BLE host library
# -ffunction-sections / -fdata-sections enable --gc-sections dead code removal
set(CMAKE_C_FLAGS_INIT
    "${CPU_FLAGS} -std=c99 -O2 -g -fshort-wchar -fshort-enums -ffunction-sections -fdata-sections")

# ASM files use the GCC driver with assembler-with-cpp so C-style // comments work
set(CMAKE_ASM_FLAGS_INIT
    "${CPU_FLAGS} -x assembler-with-cpp")

# ---- Sysroot isolation -------------------------------------------------------
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
