/*
 * flash_stub.ld — Linker script for N32WB031 flash stub
 *
 * Places stub entirely in SRAM at 0x2000B000.
 * Initial SP is set by the stub's own naked entry point.
 *
 * RAM layout (coordinated with flash_write_ram.c and flash_n32.tcl):
 *   0x20000000  firmware data  (loaded by OpenOCD, up to 47 KB)
 *   0x2000B800  parameter block (16 bytes, accessed via PARAM_BASE)
 *   0x2000B810  stub code + blob (.text + .rodata)
 *   0x2000BFFC  initial SP
 */

ENTRY(stub_entry)

SECTIONS
{
    . = 0x2000B810;

    .text : {
        *(.text.stub_entry)   /* entry point first */
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    /DISCARD/ : {
        *(.ARM.*)
        *(.comment)
        *(.note*)
    }
}
