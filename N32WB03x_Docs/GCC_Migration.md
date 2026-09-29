# N32WB031 — ARMCLANG → GCC Migration Notes

## Overview

`Base_GCC/` is a parallel workspace that builds the same N32WB031 firmware using
**arm-none-eabi-gcc 10.3 (2021.10)** instead of ARMCLANG (AC6). It uses a different
SDK version (V1.3.3) which ships a GCC-compatible startup file and BLE host library.

| Item | Base (ARMCLANG) | Base_GCC (GCC) |
|------|----------------|----------------|
| Compiler | `armclang.exe` AC6 | `arm-none-eabi-gcc` 10.3 |
| Assembler | `armasm.exe` (legacy syntax) | `arm-none-eabi-gcc -x assembler-with-cpp` |
| Linker | `armlink.exe` + `.sct` scatter | `arm-none-eabi-gcc` ld + `.ld` GNU linker script |
| Post-build | `fromelf.exe --bin` | `arm-none-eabi-objcopy -O binary` |
| SDK | V2.0.0 | V1.3.3 (GCC ELF archive, GCC startup) |
| Compat shim | `common/armcc_compat.h` | `common/gcc_compat.h` |

---

## 1. Toolchain File — `cmake/toolchain-gcc.cmake`

```cmake
set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m0)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)  # skip linker check at configure
cmake_policy(SET CMP0123 NEW)                       # required for ARMClang/GCC processor detection

set(CPU_FLAGS "-mcpu=cortex-m0 -mthumb -mfloat-abi=soft")

set(CMAKE_C_FLAGS_INIT
    "${CPU_FLAGS} -std=c99 -O2 -g -fshort-wchar -fshort-enums -ffunction-sections -fdata-sections")

set(CMAKE_ASM_FLAGS_INIT
    "${CPU_FLAGS} -x assembler-with-cpp")
```

### Key flags and rationale

| Flag | Reason |
|------|--------|
| `-mcpu=cortex-m0 -mthumb -mfloat-abi=soft` | Cortex-M0 is Thumb-only, no FPU |
| `-std=c99` | SDK headers use C99 features |
| **`-O2`** | Default optimisation — see [Section 4](#4-optimisation-levels) |
| `-g` | DWARF debug info (does not inflate .bin, stripped at objcopy) |
| **`-fshort-wchar`** | Must match ABI of `n32wb03x_host_lib.lib` (precompiled BLE stack) |
| **`-fshort-enums`** | Same — enum size must match host lib |
| `-ffunction-sections -fdata-sections` | Enables `--gc-sections` dead-code removal at link |

---

## 2. SDK Version Difference (V1.3.3 vs V2.0.0)

### Why V1.3.3 for GCC

V2.0.0 (used by `Base/`) ships only an ARMCC5/AC6 startup file
(`startup_n32wb03x.s` using `AREA`/`PROC`/`ENDP` legacy syntax) and an AC6 BLE
host library. V1.3.3 additionally provides:

- `firmware/CMSIS/device/startup/startup_n32wb03x_gcc.s` — GAS unified-syntax startup
- `firmware/CMSIS/device/system_delay.s` — `system_delay_cycles` symbol in GAS syntax
- `n32wb03x_host_lib.lib` built as a GCC ELF archive (compatible with `arm-none-eabi-ld`)

### Startup file differences

| Feature | `startup_n32wb03x.s` (ARMASM, V2.0.0) | `startup_n32wb03x_gcc.s` (GAS, V1.3.3) |
|---------|---------------------------------------|----------------------------------------|
| Syntax | `AREA`, `PROC`, `ENDP` | `.syntax unified`, `.thumb`, `.section` |
| Assembler | `armasm.exe --cpu=cortex-m0 --16` | `arm-none-eabi-gcc -x assembler-with-cpp` |
| `.noinit` handling | Does not zero .noinit (not defined) | Does not zero .noinit (zeroing stops at `_ebss`) |
| BSS zeroing | `LDR`/`STR` loop to `_ebss` | Same — `LoopFillZerobss` stops at `_ebss` |

### `system_delay.s` (GAS)

V2.0.0 implements `system_delay_cycles` inside `system_n32wb03x.c` using an
`__ASM void` block — ARMCC syntax not accepted by GCC. V1.3.3 provides it as a
separate `.s` file in unified GAS syntax:

```asm
system_delay_cycles:
    subs r0, #1
    bne  system_delay_cycles
```

Both `startup_n32wb03x_gcc.s` and `system_delay.s` are added to `BEACON_SOURCES` in
`app_beacon/CMakeLists.txt`.

---

## 3. Compatibility Shim — `common/gcc_compat.h`

SDK headers use two ARMCC built-in keywords that GCC does not recognise.
`gcc_compat.h` maps them to GCC `__attribute__` equivalents and is
`-include`'d into every C translation unit via `target_compile_options`.

```c
// __align(n)  →  __attribute__((aligned(n)))
// Fixes: n32wb03x_qflash.c, ns_ble.c
#define __align(n)  __attribute__((aligned(n)))

// __weak  →  __attribute__((weak))
// Fixes: ns_sleep.c (app_sleep_prepare_proc / app_sleep_resume_proc)
#define __weak  __attribute__((weak))
```

Applied in `CMakeLists.txt`:
```cmake
target_compile_options(app_beacon PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -include "${CMAKE_SOURCE_DIR}/common/gcc_compat.h"
        -Wno-unused-parameter
    >
)
```

Equivalent to `Base/common/armcc_compat.h` which served the same purpose for AC6
(`__align` and `__weak` are also not built-ins in ARMCLANG).

---

## 4. Optimisation Levels

### Global default: `-O2`

Set in `CMAKE_C_FLAGS_INIT` in the toolchain file. Applied to all C sources.

- Dead-code removal: `--gc-sections` (linker) removes unused functions/data whose
  sections were isolated by `-ffunction-sections` / `-fdata-sections`.
- At 64 MHz Cortex-M0, `-O2` gives the best size/speed trade-off without the
  aliasing risks of `-O3`.

### Per-file exception: `system_n32wb03x.c` compiled at `-O0`

```cmake
set_source_files_properties(
    "${SDK}/firmware/CMSIS/device/system_n32wb03x.c"
    PROPERTIES COMPILE_FLAGS "-O0"
)
```

**Reason**: `SystemTrimValueRead()` inside this file copies a 4-byte machine-code
sequence to a stack buffer and calls it via a function pointer. GCC `-O2` eliminates
the `memcpy` as "dead code" because it cannot determine that the destination buffer
is executed rather than read. Result: trim read returns a garbage value → HSI
calibration fails → clock runs at wrong frequency → LPUART baud rate wrong.

`-O0` disables this optimisation and preserves the `memcpy` call exactly as written.

### Summary table

| Source | Optimisation | Reason |
|--------|-------------|--------|
| All C sources (default) | `-O2` | Best trade-off; dead code removed by gc-sections |
| `system_n32wb03x.c` | `-O0` | GCC eliminates `SystemTrimValueRead` memcpy at O2+ |

---

## 5. Linker Script — `app_beacon/linker/app_beacon_gcc.ld`

GNU ld format (`.ld`) replaces the ARM scatter file (`.sct`) used by `armlink`.

### Memory map

```
FLASH    (rx)  : ORIGIN = 0x01000000, LENGTH = 448K   /* code + rodata */
APP_DATA (r)   : ORIGIN = 0x01070000, LENGTH = 4K     /* persistent flash data */
RAM      (xrw) : ORIGIN = 0x20004000, LENGTH = 0x7F80 /* bss / heap / stack (32640 B) */
NVM_RAM  (rw)  : ORIGIN = 0x2000BF80, LENGTH = 128    /* .noinit — never zero-filled */
```

Key decisions:

| Choice | Details |
|--------|---------|
| RAM starts at `0x20004000` | `0x20000000–0x20003FFF` is the BLE ROM working area. GCC's newlib `malloc` would corrupt it if RAM started at `0x20000000`. |
| `NVM_RAM` physically separate from `RAM` | newlib's `malloc` arena initialises metadata starting at `end` = `_ebss` aligned. If `.noinit` variables were placed immediately after `_ebss` in the same region, the malloc arena would overwrite them on first `malloc` call. A separate region avoids this completely. |
| `_estack = 0x2000BF80` | Stack grows downward from the base of `NVM_RAM`. Stack and `.noinit` do not overlap. |
| `APP_DATA` at `0x01070000` | Must be in Flash Bank B (`0x01040000–0x0107FFFF`). The app executes from Bank A. Erasing a Bank A sector during XIP would corrupt running code; the Qflash library exits XIP mode for Bank B operations. |

### ROM symbol file as second `-T` script

```cmake
target_link_options(app_beacon PRIVATE
    -T "${LD_SCRIPT}"            # app_beacon_gcc.ld  (layout)
    -T "${BLE_SYMDEF_TXT}"       # symbol_g15.txt (ROM addresses)
    --specs=nosys.specs
    -Wl,--gc-sections
    -lm
)
```

GNU ld accepts multiple `-T` scripts. `symbol_g15.txt` contains only `symbol = address;`
assignments (addresses of ROM functions like `ke_init`, `rwip_schedule`, etc.) and is
merged with `app_beacon_gcc.ld`. Same approach as `Base/` but using `-T` instead of the
ARMLINK `--via` / `--entry` mechanism.

---

## 6. `.noinit` / RAM Retention

### Problem encountered

Initially, `.noinit` variables were declared in `RAM` directly after `.bss`. On every
`NRST` reset, `ram_magic` read as `0x00000000` despite the startup code stopping BSS
zeroing at `_ebss`. Root cause: newlib `malloc` writes chunk metadata starting at
`end` (= `_ebss` rounded up to 8-byte alignment), which overlapped the `.noinit`
variables on the first `malloc`/`printf` call after reset.

### Fix

Add a physically separate `NVM_RAM` region at the top of user SRAM:

```ld
RAM      (xrw) : ORIGIN = 0x20004000, LENGTH = 0x7F80  /* 32640 B */
NVM_RAM  (rw)  : ORIGIN = 0x2000BF80, LENGTH = 128     /* .noinit */

_estack = 0x2000BF80;   /* stack top = NVM_RAM base */

.noinit (NOLOAD) :
{
    . = ALIGN(4); _snoinit = .;
    *(.noinit) *(.noinit*)
    . = ALIGN(4); _enoinit = .;
} >NVM_RAM
```

C usage:
```c
#define RAM_MAGIC  0xCAFEBABEUL
static volatile uint32_t ram_magic   __attribute__((section(".noinit")));
static volatile uint32_t ram_counter __attribute__((section(".noinit")));
```

**Verified on hardware**: `RETAINED resets=1`, `resets=2` … after NRST button press
while VDD held. Full details in `SRAM_Retention.md`.

### Verified symbol layout (from `nm`)

| Symbol | Address | Notes |
|--------|---------|-------|
| `_ebss` | `0x20005214` | BSS end / heap base |
| `_estack` | `0x2000BF80` | Stack top |
| `_snoinit` / `ram_magic` | `0x2000BF80` | NVM_RAM start |
| `ram_counter` | `0x2000BF84` | |
| `_enoinit` | `0x2000BF88` | 8 B used, 120 B free |

---

## 7. LPUART Pin Override — `common/ns_log_lpuart.c`

Same pattern as `Base/common/ns_log_lpuart.c`. The SDK default (`_LPUART1_COM_ = 0`)
uses PB1/PB2 with AF4. The board hardware uses PB12/PB11 with AF2:

```c
#define _LPUART1_COM_    (1)   /* PB12 TX, PB11 RX, AF2 */
```

Referenced by all `CMakeLists.txt` targets instead of the SDK copy so the pin
selection survives SDK upgrades.

---

## 8. Build Commands

```bat
:: Configure + build
make build app_beacon
make build app_installer
make build app_p2ps_ota
make build all

:: Clean / rebuild
make clean app_beacon
make rebuild app_beacon

:: Flash (OpenOCD / ST-Link)
flash app_beacon
flash app_p2ps_full

:: Flash (NSLink / NSpyocd)
flash_nslink app_beacon
flash_nslink app_p2ps_full
```

Direct CMake invocation:
```bat
set cmake="C:\Program Files\CMake\bin\cmake.exe"
%cmake% -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-gcc.cmake -G Ninja
%cmake% --build build --target app_beacon
```

---

## 9. Project Structure Differences vs Base

| Aspect | Base (ARMCLANG) | Base_GCC (GCC) |
|--------|----------------|----------------|
| Startup | `startup_n32wb03x.s` (ARMASM, AREA/PROC) | `startup_n32wb03x_gcc.s` (GAS, .syntax unified) |
| Delay cycles | Inline `__ASM void` in `system_n32wb03x.c` | `system_delay.s` separate GAS file |
| Compat header | `common/armcc_compat.h` | `common/gcc_compat.h` |
| Scatter / linker | `.sct` (ARM format) | `.ld` (GNU ld format) |
| Linker driver | `armlink.exe` | `arm-none-eabi-gcc` wrapping `ld` |
| ROM symbols | `symbol_g15.obj` (ARM COFF) | `symbol_g15.txt` (GNU ld assignments, `-T`) |
| `.bin` generation | `fromelf.exe --bin` | `arm-none-eabi-objcopy -O binary` |
| BLE host lib | `n32wb03x_host_lib.lib` (AC6 ELF) | `n32wb03x_host_lib.lib` (GCC ELF) |
| SDK version | V2.0.0 | V1.3.3 |
| `system_n32wb03x.c` | Project-local copy (naked fix) | SDK copy compiled at `-O0` |

---

## 10. Known Linker Warnings

The following warnings appear at link time and are **benign** — they pre-exist in the
SDK and do not indicate a bug:

```
warning: ... uses 2-byte wchar_t yet the output is to use 4-byte wchar_t
```

**Cause**: Some SDK peripheral driver `.c` files are compiled without `-fshort-wchar`
(they do not include any header that triggers the pragma). The BLE host library was
built with `-fshort-wchar`. The mismatch warning is emitted by `ld`, but wchar_t is
not used across the boundary, so no actual ABI corruption occurs.

**Suppression** (if desired): add `-fshort-wchar` to the individual driver compile
flags, or pass `-Wno-wchar-mismatch` to the linker via `-Wl,--no-wchar-size-warning`.
Not currently applied — warnings are left visible as a reminder.
