# N32WB031 SRAM Retention — `.noinit` / NVM_RAM Pattern

## Overview

The N32WB031 Cortex-M0 clears its internal SRAM on a cold power-on but **retains SRAM
content across a soft reset (NRST pin / `NVIC_SystemReset()`) while VDD is held**.

This document describes the `NVM_RAM` / `.noinit` linker pattern used to exploit that
retention — without using flash erase cycles.

---

## Hardware Facts

| Item | Detail |
|------|--------|
| Total SRAM | 48 KB (`0x20000000–0x2000BFFF`) |
| BLE ROM working area | `0x20000000–0x20003FFF` (16 KB) — **never touch** |
| User SRAM | `0x20004000–0x2000BFFF` (32 KB) |
| Retention on NRST | Yes — contents survive while VDD ≥ VPOR |
| Retention on power-off | No — SRAM is volatile; contents lost when VDD drops |
| Retention through debugger halt+resume | **No** — SWD connect erases RAM in some tools |

---

## The Problem with `.bss` and `.noinit`

The GCC startup code (`Reset_Handler` in `startup_n32wb03x_gcc.s`) zero-fills the
entire `.bss` section on every boot:

```asm
FillZerobss:
    movs  r3, #0
    str   r3, [r2]
    adds  r2, r2, #4
LoopFillZerobss:
    ldr   r3, =_ebss
    cmp   r2, r3
    bcc   FillZerobss
```

Any variable in `.bss` is zeroed on every reset — retained or not.

**Additionally**, the newlib `malloc` arena (`__malloc_av_`, `impure_data`, etc.) occupies
the `.data` section and is re-initialised from flash on every boot by `Reset_Handler`'s
data-copy loop. Heap metadata written at runtime overwrites addresses starting at `end`
(`_ebss` aligned up). If `.noinit` variables are placed immediately after `_ebss` in the
same RAM region, a `malloc` / `ke_mem_init` call from the BLE stack can corrupt them.

**Solution**: place `.noinit` in a physically separate memory region (`NVM_RAM`) that
starts beyond the highest address reachable by `_ebss` + heap growth.

---

## Linker Script Pattern

### Memory regions

```ld
MEMORY
{
    RAM      (xrw) : ORIGIN = 0x20004000, LENGTH = 0x7F80  /* 32640 B — bss/heap/stack */
    NVM_RAM  (rw)  : ORIGIN = 0x2000BF80, LENGTH = 128     /* 128 B  — .noinit only    */
}
```

- `NVM_RAM` is placed at the **top** of user SRAM, just below `0x2000C000`.
- `RAM` ends at `0x2000BF80` — the stack top (`_estack`) is set to the same address.
- The 128-byte reservation leaves 120 bytes free for future retention variables (8 bytes
  currently used by `ram_magic` + `ram_counter`).

### Stack top

```ld
_estack = 0x2000BF80;   /* top of RAM, stack grows downward from here */
```

### `.noinit` section

```ld
.noinit (NOLOAD) :
{
    . = ALIGN(4);
    _snoinit = .;
    *(.noinit)
    *(.noinit*)
    . = ALIGN(4);
    _enoinit = .;
} >NVM_RAM
```

The `(NOLOAD)` attribute tells the linker this section has no LMA — no data is stored in
flash and nothing is copied to RAM at startup. Combined with the separate `NVM_RAM`
region, the startup zero-fill loop never reaches these addresses.

---

## C Usage

### Declaring a retention variable

```c
/* Magic word — chosen to be distinct from any flash magic values */
#define RAM_MAGIC  0xCAFEBABEUL

static volatile uint32_t ram_magic   __attribute__((section(".noinit")));
static volatile uint32_t ram_counter __attribute__((section(".noinit")));
```

Any plain C type works: `uint8_t`, `uint32_t`, structs, arrays — as long as the total
does not exceed the NVM_RAM size (128 bytes in this project).

### Retention check at boot entry

```c
int main(void)
{
    /* Check BEFORE any init that could touch these addresses */
    uint8_t ram_retained;
    if (ram_magic == RAM_MAGIC) {
        ram_counter++;
        ram_retained = 1;
    } else {
        /* Cold boot or RAM was cleared — initialise sentinels */
        ram_magic   = RAM_MAGIC;
        ram_counter = 0;
        ram_retained = 0;
    }

    /* ... rest of init ... */
    NS_LOG_INFO("  RAM : %s  resets=%lu\r\n",
                ram_retained ? "RETAINED" : "COLD BOOT (cleared)",
                (unsigned long)ram_counter);
}
```

**Rule**: the check must run before `app_ble_init()` / `ns_ble_stack_init()`. The BLE
stack `ke_mem_init` overwrites its heap pools on every boot. Those pools are in the BLE
ROM area (`0x20000000`), not in `NVM_RAM`, but as a general discipline always perform
the retention check at the very top of `main()`.

---

## Verified Address Map (app_beacon build)

```
20004aa8  _sbss          — start of zero-filled BSS
20005214  _ebss          — end of BSS / heap base
               ↕  ~26 KB free heap + stack
2000bf80  _estack        — stack top
2000bf80  _snoinit       — NVM_RAM start
2000bf80  ram_magic      — retention sentinel
2000bf84  ram_counter    — soft-reset count
2000bf88  _enoinit       — 8 bytes used, 120 bytes free
2000c000  (end of SRAM)
```

---

## Behaviour by Reset Type

| Reset cause | VDD held? | SRAM retained? | `ram_magic` at entry | Result |
|-------------|-----------|----------------|----------------------|--------|
| NRST button | Yes | **Yes** | `0xCAFEBABE` | `RETAINED` |
| `NVIC_SystemReset()` | Yes | **Yes** | `0xCAFEBABE` | `RETAINED` |
| Power cycle / battery swap | No | No | random / 0x00000000 | `COLD BOOT` |
| NSLink `erase --chip` + reflash | No* | No | `0x00000000` | `COLD BOOT` |
| SWD debugger halt (no reset) | Yes | **No** | `0x00000000` | `COLD BOOT` |

\* NSpyocd `erase --chip -M pre-reset` performs a chip reset which briefly drops
internal regulators below VPOR, clearing SRAM.

---

## Pitfalls

### 1. `.noinit` placed inside the same region as `.bss`

If `NVM_RAM` is not a separate memory region and `.noinit` is placed immediately after
`_ebss` inside `RAM`, the newlib `malloc` arena initialisation writes its metadata
(`__malloc_av_`, `impure_data`, etc.) starting at `end` and can overwrite `.noinit`
variables before `main()` even runs.

**Fix**: always use a physically separate `NVM_RAM` region as shown above.

### 2. `volatile` is required

Without `volatile`, the compiler may cache the magic value in a register across the
comparison and assignment, breaking the cold-boot detection. Always declare `.noinit`
variables as `volatile`.

### 3. No initialiser allowed

```c
/* WRONG — linker will put this in .data, not .noinit */
static uint32_t x __attribute__((section(".noinit"))) = 0xABCD;

/* CORRECT — no initialiser */
static volatile uint32_t x __attribute__((section(".noinit")));
```

### 4. Structs must also be `volatile`

```c
typedef struct {
    uint32_t magic;
    uint32_t count;
    uint8_t  data[8];
} NvmState_t;

static volatile NvmState_t nvm __attribute__((section(".noinit")));
```

Access individual fields: `nvm.magic`, `nvm.count`, etc.

---

## Adding New Retention Variables

1. Declare with `__attribute__((section(".noinit")))` in any `.c` file linked into the
   target.
2. Check total size stays ≤ 128 bytes:  
   `arm-none-eabi-nm beacon.elf | grep -E "_snoinit|_enoinit"` — difference is used bytes.
3. Increase `NVM_RAM LENGTH` and move `ORIGIN` down (adjusting `_estack` to match) if
   you need more than 128 bytes. Keep `NVM_RAM` contiguous with the top of `RAM`.

---

## Porting to Other Targets (app_p2ps_ota, app_simple, etc.)

Add the same two-region pattern to each target's linker script:

```ld
_estack = 0x2000BF80;

MEMORY {
    RAM     (xrw) : ORIGIN = 0x20004000, LENGTH = 0x7F80
    NVM_RAM (rw)  : ORIGIN = 0x2000BF80, LENGTH = 128
}
```

And the `.noinit` section mapping:

```ld
.noinit (NOLOAD) : { *(.noinit) *(.noinit*) } >NVM_RAM
```

Shared `.noinit` variables can live in a common header included by all targets.
