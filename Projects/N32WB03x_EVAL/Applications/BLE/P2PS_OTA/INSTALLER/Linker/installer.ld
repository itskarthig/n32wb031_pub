/*
 * app_installer_gcc.ld — GNU LD linker script for app_installer
 *
 * Flash layout:
 *   0x01000000  8 KB   app_installer (this image)
 *   0x01002000  4 KB   InstallFlag_t (written by app_fuota, read by installer via Qflash_Read)
 *   0x01003000  4 KB   App data (NVM)
 *   0x01004000  240 KB APP1
 *   0x01040000  240 KB APP2 (FUOTA Download Slot)
 *
 * SRAM:
 *   0x20000000  16 KB  BLE ROM working area (DO NOT USE — owned by BLE ROM)
 *   0x20004000  32640 B  User SRAM
 *   0x2000BF80  128 B    NVM_RAM (app_test_l .noinit pulse counters — do not touch)
 */

MEMORY
{
    FLASH         (rx)  : ORIGIN = 0x01000000, LENGTH = 8K
    RAM           (xrw) : ORIGIN = 0x20004000, LENGTH = 0x7F80  /* 32640 B — leaves NVM_RAM (0x2000BF80, 128 B) untouched */
    NVM_RAM       (rw)  : ORIGIN = 0x2000BF80, LENGTH = 128
}

/* Entry point */
ENTRY(Reset_Handler)

/* Stack top = base of NVM_RAM — hard boundary prevents stack from touching app_p2ps_ota .noinit retention region */
_estack = ORIGIN(NVM_RAM);

_Min_Heap_Size  = 0x400;   /* 1 KB minimum heap  */
_Min_Stack_Size = 0x800;   /* 2 KB minimum stack */

SECTIONS
{
    /* ---- Interrupt vector table ----------------------------------------- */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH

    /* ---- Code + read-only data ------------------------------------------ */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        KEEP(*(.init))
        KEEP(*(.fini))
        . = ALIGN(4);
        _etext = .;
    } >FLASH

    /* ---- ARM exception unwinding (required by GCC runtime) -------------- */
    .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
    .ARM        : {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >FLASH

    /* ---- Initialized data (copied from flash to RAM at startup) ---------- */
    _sidata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT>FLASH

    /* ---- Zero-initialized data ------------------------------------------ */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } >RAM

    /* ---- Heap + stack (ARM C library) ------------------------------------ */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE(end = .);
        PROVIDE(_end = .);
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } >RAM

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
