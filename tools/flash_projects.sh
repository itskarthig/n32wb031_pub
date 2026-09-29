#!/usr/bin/env bash
# flash_projects.sh — flash one Projects/ OTA-tree target (or an
# INSTALLER+APP pair) via OpenOCD + the Qflash RAM stub. Called by the
# Makefile's `make flash <SDK> <CATEGORY> <Project> INSTALLER|APP|ALL`.
#
# Unlike tools/flash.sh (old flat <app>/<app>.bin convention), the
# Projects/ tree doesn't follow a fixed name-derived path pattern (linker
# script filenames vary, e.g. installer.ld), so bin/ld paths are passed in
# explicitly rather than derived from a naming convention.
#
# Usage:
#   flash_projects.sh plain <bin> <ld> <openocd> <openocd_cfg> <root>
#   flash_projects.sh all <installer_bin> <installer_ld> <app_bin> <app_ld> <openocd> <openocd_cfg> <root>
#
# plain: flashes just that one target's own FLASH region.
# all:   chip-erase, then flashes the installer followed by the app (for a
#        blank/first-time chip).

set -eu

MODE="$1"

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

require_file() {
    local f="$1" label="$2"
    [[ -f "$f" ]] || { echo "[ERROR] $f not found -- run 'make build ... $label' first" >&2; exit 1; }
}

region_for() {
    local bin="$1" ld="$2" label="$3"
    require_file "$bin" "$label"
    [[ -f "$ld" ]] || { echo "[ERROR] linker script not found: $ld" >&2; exit 1; }
    local addr len size
    read -r addr len < <(flash_region "$ld")
    size=$(wc -c < "$bin" | tr -d ' ')
    if (( size > len )); then
        echo "[ERROR] $bin ($size B) is larger than $label's declared FLASH region ($len B) -- refusing to flash" >&2
        exit 1
    fi
    echo "$addr $len $size"
}

if [[ "$MODE" == "all" ]]; then
    INSTALLER_BIN="$2"; INSTALLER_LD="$3"; APP_BIN="$4"; APP_LD="$5"
    OPENOCD="$6"; OPENOCD_CFG="$7"; ROOT="$8"

    read -r INSTALLER_ADDR _ _ < <(region_for "$INSTALLER_BIN" "$INSTALLER_LD" "INSTALLER")
    read -r APP_ADDR _ _ < <(region_for "$APP_BIN" "$APP_LD" "APP")

    cd "$ROOT"
    echo "[FLASH] chip erase, then INSTALLER @ $INSTALLER_ADDR + APP @ $APP_ADDR"
    "$OPENOCD" -f "$OPENOCD_CFG" -f "$ROOT/openocd/flash_n32.tcl" \
        -c "init" -c "n32_connect" -c "n32_chip_erase" \
        -c "n32_program {$INSTALLER_BIN} $INSTALLER_ADDR 0" \
        -c "n32_program {$APP_BIN} $APP_ADDR 0" \
        -c "n32_reset" -c "exit"
else
    BIN="$2"; LD="$3"; OPENOCD="$4"; OPENOCD_CFG="$5"; ROOT="$6"

    read -r ADDR LEN SIZE < <(region_for "$BIN" "$LD" "target")
    SECTORS=$(( LEN / 4096 ))

    cd "$ROOT"
    echo "[FLASH] $BIN ($SIZE B) @ $ADDR, erasing $SECTORS sectors"
    "$OPENOCD" -f "$OPENOCD_CFG" -f "$ROOT/openocd/flash_n32.tcl" \
        -c "init" -c "n32_connect" -c "n32_program {$BIN} $ADDR $SECTORS" \
        -c "n32_reset" -c "exit"
fi
