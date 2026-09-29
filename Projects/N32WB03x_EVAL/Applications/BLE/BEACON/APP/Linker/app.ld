/**
 *
 * BLE-enabled build: boots via app_installer (0x01000000).
 * Flash: APP1 slot at 0x01004000, 240 KB.
 * RAM: 0x20004000 (BLE ROM area 0x20000000–0x20003FFF reserved for BLE ROM).
 * Stack top = NVM_RAM base = 0x2000BF80.
 *
 * Memory map:
 *   FLASH   0x01004000  240 KB   .isr_vector, .text, .rodata
 *   RAM     0x20004000  ~32 KB   .data, .bss, heap, stack
 *   NVM_RAM 0x2000BF80  128 B    .noinit 
 */

/* Entry point */
ENTRY(Reset_Handler)

/* Stack top = base of NVM_RAM (NVM_RAM occupies the top 128 bytes of user SRAM) */
_estack = 0x2000BF80;

_Min_Heap_Size  = 0x0;     /* no heap — malloc() not used anywhere in codebase */
_Min_Stack_Size = 0x1000;  /* 4 KB minimum stack — LoRa+BLE call chains + ISR nesting */

/* GCC linker symbol: start of application flash */
PROVIDE(__app_flash_start = ORIGIN(FLASH));

MEMORY
{
    FLASH   (rx)  : ORIGIN = 0x01004000, LENGTH = 240K
    RAM     (xrw) : ORIGIN = 0x20004000, LENGTH = 0x7F80  /* 32640 B — BLE ROM area reserved below */
    NVM_RAM (rw)  : ORIGIN = 0x2000BF80, LENGTH = 128     /* .noinit — survives NRST while VDD held */
    USER_CFG (r)  : ORIGIN = 0x0107E000, LENGTH = 8K      /* userConfig_t — accessed via Qflash API */
}

SECTIONS
{
    /* ---- Flash sections ---- */

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

    .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >FLASH

    /* ---- Initialized data load address --------------------------------- */
    _sidata = LOADADDR(.data);

    /* ---- RAM sections ---- */

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT> FLASH

    /* ---- OTA magic keyword — MUST be the last bytes in the .bin ----------- */
    .tag_ota_end :
    {
        . = ALIGN(4);
        KEEP(*(.tag_ota_end))
        . = ALIGN(4);
    } >FLASH

    /* NOLOAD -- without this, .bss has no explicit LMA and ld continues it
     * from wherever the previous section's LMA left off (flash, right
     * after .tag_ota_end), so objcopy -O binary emits .bss's zero content
     * as real trailing bytes in the .bin -- breaking the "magic keyword is
     * the last 4 bytes" invariant app_fuota.c depends on. Matches .noinit's
     * already-correct (NOLOAD) below. See n32wb031_gcc's CLAUDE.md
     * LORAWAN_WMBUS_AMR section. */
    .bss (NOLOAD) :
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

    /* ---- No-init section — physically in NVM_RAM, NEVER zero-filled by startup.
     * Survives NRST (hardware reset) while VDD is held.
     * Mark C variables with:  __attribute__((section(".noinit")))
     */
    .noinit (NOLOAD) :
    {
        . = ALIGN(4);
        _snoinit = .;
        *(.noinit)
        *(.noinit*)
        . = ALIGN(4);
        _enoinit = .;
    } >NVM_RAM
}
