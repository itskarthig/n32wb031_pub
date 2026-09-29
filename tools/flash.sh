#!/usr/bin/env bash
# flash.sh — flash one N32WB031 app via OpenOCD + the Qflash RAM stub
# (openocd/flash_n32.tcl + tools/flash_write_ram.elf). Called by the
# top-level Makefile's flash-<app>/flash-<app>-full targets.
#
# Usage:
#   flash.sh plain <app> <root> <build_dir> <openocd> <openocd_cfg>
#   flash.sh full  <app> <root> <build_dir> <openocd> <openocd_cfg>
#
# plain: flashes just <app>'s own FLASH region (assumes app_installer is
#        already on-chip at its own address).
# full:  chip-erase, then flashes app_installer followed by <app> (for a
#        blank/first-time chip). <app> must be one of the APP1-slot apps
#        (app_installer itself has no "full" mode).
#
# FLASH address/size for each app are read from that app's own linker
# script (FLASH ... ORIGIN=/LENGTH=), same approach as tools/elfsize.sh.

set -eu

MODE="$1"
APP="$2"
ROOT="$3"
BUILD_DIR="$4"
OPENOCD="$5"
OPENOCD_CFG="$6"

# ---- Read FLASH ORIGIN/LENGTH from a linker script, print "<addr> <bytes>" ----
flash_region() {
    local ld="$1" addr len
    addr=$(sed -n 's/^[[:space:]]*FLASH[[:space:]]*(.*)[[:space:]]*:.*ORIGIN[[:space:]]*=[[:space:]]*\([0-9A-Za-zx]*\).*/\1/p' "$ld" | head -n1)
    len=$(sed -n 's/^[[:space:]]*FLASH[[:space:]]*(.*)[[:space:]]*:.*LENGTH[[:space:]]*=[[:space:]]*\([0-9A-Za-zx]*\).*/\1/p' "$ld" | head -n1)
    [[ -n "$addr" ]] || { echo "[ERROR] could not find FLASH ORIGIN in $ld" >&2; exit 1; }
    [[ -n "$len"  ]] || { echo "[ERROR] could not find FLASH LENGTH in $ld" >&2; exit 1; }
    case "$len" in
        *K|*k) len=$(( ${len%[Kk]} * 1024 )) ;;
        *M|*m) len=$(( ${len%[Mm]} * 1024 * 1024 )) ;;
        *) len=$(( len )) ;;
    esac
    echo "$addr $len"
}

require_bin() {
    local bin="$1" app="$2"
    [[ -f "$bin" ]] || { echo "[ERROR] $bin not found -- run 'make $app' first" >&2; exit 1; }
}

app_ld()  { echo "$ROOT/$1/linker/$1_gcc.ld"; }
app_bin() { echo "$BUILD_DIR/$1/$1.bin"; }

APP_LD=$(app_ld "$APP")
APP_BIN=$(app_bin "$APP")
[[ -f "$APP_LD" ]] || { echo "[ERROR] linker script not found: $APP_LD" >&2; exit 1; }
require_bin "$APP_BIN" "$APP"

read -r APP_ADDR APP_LEN < <(flash_region "$APP_LD")
APP_SIZE=$(wc -c < "$APP_BIN" | tr -d ' ')
if (( APP_SIZE > APP_LEN )); then
    echo "[ERROR] $APP_BIN ($APP_SIZE B) is larger than $APP's declared FLASH region ($APP_LEN B) -- refusing to flash" >&2
    exit 1
fi
APP_SECTORS=$(( APP_LEN / 4096 ))

cd "$ROOT"

if [[ "$MODE" == "full" ]]; then
    INSTALLER_LD=$(app_ld "app_installer")
    INSTALLER_BIN=$(app_bin "app_installer")
    [[ -f "$INSTALLER_LD" ]] || { echo "[ERROR] linker script not found: $INSTALLER_LD" >&2; exit 1; }
    require_bin "$INSTALLER_BIN" "app_installer"
    read -r INSTALLER_ADDR _ < <(flash_region "$INSTALLER_LD")

    echo "[FLASH] $APP (full): chip erase, then app_installer @ $INSTALLER_ADDR + $APP @ $APP_ADDR"
    "$OPENOCD" -f "$OPENOCD_CFG" -f "$ROOT/openocd/flash_n32.tcl" \
        -c "init" -c "n32_connect" -c "n32_chip_erase" \
        -c "n32_program {$INSTALLER_BIN} $INSTALLER_ADDR 0" \
        -c "n32_program {$APP_BIN} $APP_ADDR 0" \
        -c "n32_reset" -c "exit"
else
    echo "[FLASH] $APP: $APP_BIN ($APP_SIZE B) @ $APP_ADDR, erasing $APP_SECTORS sectors"
    "$OPENOCD" -f "$OPENOCD_CFG" -f "$ROOT/openocd/flash_n32.tcl" \
        -c "init" -c "n32_connect" -c "n32_program {$APP_BIN} $APP_ADDR $APP_SECTORS" \
        -c "n32_reset" -c "exit"
fi
