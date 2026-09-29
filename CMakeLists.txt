cmake_minimum_required(VERSION 3.20)

# Usage:
#   cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-gcc.cmake -G Ninja
#   cmake --build build --target beacon

project(N32WB031_Base_GCC LANGUAGES C ASM)

# ---- SDK root -----------------------------------------------------------
# N32WB03X_SDK_ROOT is a plain external path, resolved and passed by the
# Makefile from .vscode/settings.json's cmake.configureSettings.SDK_ROOT --
# a single, relative path to whichever N32WB03x SDK tree the whole repo
# currently builds against. No V133/V200 shorthand, no per-target selection
# -- switch SDK versions by editing that one settings.json value. No
# shorthand-to-path derivation left in CMake itself, matching GCC_PATH's
# "explicit only" treatment.
if(NOT DEFINED N32WB03X_SDK_ROOT OR N32WB03X_SDK_ROOT STREQUAL "")
    message(FATAL_ERROR
        "N32WB03X_SDK_ROOT is not set.\n"
        "The Makefile resolves and passes this automatically on every invocation "
        "from .vscode/settings.json's cmake.configureSettings.SDK_ROOT -- this "
        "error only fires when cmake is invoked directly without it, e.g. "
        "manually with -DN32WB03X_SDK_ROOT unset.")
endif()
set(N32WB03X_SDK_ROOT "${N32WB03X_SDK_ROOT}" CACHE PATH "Path to N32WB03x GCC SDK root" FORCE)

# SDK_COMMON_DIR: override files (GCC-compatible replacements). Used only for
# dfu_crc.c/.h (every INSTALLER's CMakeLists.txt) -- byte-identical between
# V133 and V200, so this is a single flat directory, not one per SDK version.
# Lives under Utilities/ (moved from a top-level common/ folder) -- variable
# name kept as-is since it's already referenced by every INSTALLER
# CMakeLists.txt in the Projects/ tree; only what it points to changed.
set(SDK_COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Utilities/common"
    CACHE INTERNAL "SDK override files")

# OPENOCD_BIN: optional. Settable from .vscode/settings.json's
# cmake.configureSettings.OPENOCD_BIN (VS Code's CMake Tools extension passes
# it through as -DOPENOCD_BIN=... when configuring), same pattern as
# GCC_PATH above/in cmake/toolchain-gcc.cmake -- no auto-detect (find_program
# removed; matches GCC_PATH's "explicit only" treatment). Deliberately NOT a
# FATAL_ERROR when unset, unlike GCC_PATH: this variable is never consumed
# by the CMake build itself (flashing is Makefile/shell-driven, tools/
# flash.sh + tools/flash_projects.sh, which resolve OpenOCD independently
# and do their own "required, error if unset" check) -- cmake configure runs
# unconditionally on every build, so erroring here would break every build
# for a variable that was never load-bearing for compilation.
set(OPENOCD_BIN "" CACHE FILEPATH "Path to the openocd executable")

# n32wb03x_apply_build_type() -- opt-in per-target Release/Debug flags for the
# Projects/ OTA tree. No effect on any target below that doesn't call it.
include(cmake/build_type.cmake)

# ---- Projects/ OTA tree (INSTALLER + APP pairs) -------------------------------
add_subdirectory(Projects/N32WB03x_EVAL/Applications/BLE/BEACON/INSTALLER)
add_subdirectory(Projects/N32WB03x_EVAL/Applications/BLE/BEACON/APP)
add_subdirectory(Projects/N32WB03x_EVAL/Applications/STANDALONE/PERIPHERAL/INSTALLER)
add_subdirectory(Projects/N32WB03x_EVAL/Applications/STANDALONE/PERIPHERAL/APP)
add_subdirectory(Projects/N32WB03x_EVAL/Applications/BLE/P2PS_OTA/INSTALLER)
add_subdirectory(Projects/N32WB03x_EVAL/Applications/BLE/P2PS_OTA/APP)
add_subdirectory(Projects/N32WB03x_CUSTOM_BOARD/Applications/BLE/P2PS_OTA/INSTALLER)
add_subdirectory(Projects/N32WB03x_CUSTOM_BOARD/Applications/BLE/P2PS_OTA/APP)
