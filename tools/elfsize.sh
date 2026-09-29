#!/usr/bin/env bash
# elfsize.sh — Parse GCC ELF and print Flash/RAM usage with progress bars
# Portable (bash/awk) port of elfsize.ps1 for macOS/Linux.
#
# Usage: tools/elfsize.sh <elf_file> <ld_script> [label]
#
# Flash = text + data  (data stored in flash as load-region)
# RAM   = data + bss   (data copied to RAM on startup, bss zero-filled)
#
# Flash and RAM limits are read dynamically from the MEMORY block of <ld_script>:
#   FLASH  ... LENGTH = 512K / 240K / 8K    -> flash budget
#   RAM    ... LENGTH = 0x7F80 / 32K        -> RAM budget

set -u

ELF_FILE="${1:-}"
LD_SCRIPT="${2:-}"
LABEL="${3:-}"

# ---- Parse a GNU LD size expression (512K / 0x7F80 / 8192) to bytes ---------
to_bytes() {
    local s
    s=$(echo "$1" | sed -E 's/[[:space:]].*$//' | tr '[:lower:]' '[:upper:]')
    if [[ "$s" =~ ^0X([0-9A-F]+)$ ]]; then
        echo $((16#${BASH_REMATCH[1]}))
    elif [[ "$s" =~ ^([0-9]+)K$ ]]; then
        echo $((${BASH_REMATCH[1]} * 1024))
    elif [[ "$s" =~ ^([0-9]+)M$ ]]; then
        echo $((${BASH_REMATCH[1]} * 1024 * 1024))
    else
        echo "$s"
    fi
}

# ---- Read FLASH and RAM limits from linker script ---------------------------
FLASH_MAX=0
RAM_MAX=0

if [[ -f "$LD_SCRIPT" ]]; then
    flash_expr=$(grep -m1 -E '^[[:space:]]*FLASH\b[^:]*:.*LENGTH[[:space:]]*=' "$LD_SCRIPT" \
        | sed -E 's/.*LENGTH[[:space:]]*=[[:space:]]*([0-9A-Fa-fxX]+[KkMm]?).*/\1/')
    ram_expr=$(grep -m1 -E '^[[:space:]]*RAM\b[^:]*:.*LENGTH[[:space:]]*=' "$LD_SCRIPT" \
        | sed -E 's/.*LENGTH[[:space:]]*=[[:space:]]*([0-9A-Fa-fxX]+[KkMm]?).*/\1/')
    [[ -n "$flash_expr" ]] && FLASH_MAX=$(to_bytes "$flash_expr")
    [[ -n "$ram_expr"   ]] && RAM_MAX=$(to_bytes "$ram_expr")
else
    echo "  [WARN] LD script not found: $LD_SCRIPT"
fi

if [[ ! -f "$ELF_FILE" ]]; then
    name="${LABEL:-$(basename "$ELF_FILE")}"
    echo "  $name : not built"
    exit 0
fi

# Locate arm-none-eabi-size
SIZE_EXE="${SIZE_EXE:-}"
if [[ -z "$SIZE_EXE" ]] || [[ ! -x "$SIZE_EXE" ]]; then
    SIZE_EXE=$(command -v arm-none-eabi-size || true)
    if [[ -z "$SIZE_EXE" ]]; then
        echo "  [ERROR] arm-none-eabi-size not found"
        exit 1
    fi
fi

# Run arm-none-eabi-size (Berkeley format: text  data  bss  dec  hex  filename)
data_line=$("$SIZE_EXE" "$ELF_FILE" | sed -n '2p')
read -r text_size data_size bss_size _dec _hex _fname <<< "$data_line"

flash=$((text_size + data_size))
ram=$((data_size + bss_size))

show_bar() {
    local used=$1 total=$2 width=30
    local pct=0 filled=0
    if (( total > 0 )); then
        pct=$(( used * 100 / total ))
        filled=$(( used * width / total ))
    fi
    (( filled > width )) && filled=$width
    local bar
    bar=$(printf '#%.0s' $(seq 1 "$filled") 2>/dev/null)
    bar+=$(printf -- '-%.0s' $(seq 1 $((width - filled))) 2>/dev/null)
    printf "[%s] %3d%%" "$bar" "$pct"
}

if [[ -n "$LABEL" ]]; then
    hdr="  $LABEL"
else
    hdr="  $(basename "$ELF_FILE" .elf)"
fi
dec_size=$((text_size + data_size + bss_size))
hex_size=$(printf '%x' "$dec_size")

echo "$hdr"
printf "    text=%6d  data=%6d  bss=%6d  dec=%6d  hex=%s\n" \
    "$text_size" "$data_size" "$bss_size" "$dec_size" "$hex_size"
printf "    Flash: %6d / %6d B  %s\n" "$flash" "$FLASH_MAX" "$(show_bar "$flash" "$FLASH_MAX")"
printf "    RAM:   %6d / %6d B  %s\n" "$ram" "$RAM_MAX" "$(show_bar "$ram" "$RAM_MAX")"
