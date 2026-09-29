# flash_n32.tcl — OpenOCD Tcl orchestration for N32WB031 flash programming
#
# Uses a RAM-resident stub (tools/flash_write_ram.elf) that drives the
# proprietary Qflash controller via the SDK's binary blob.
#
# RAM layout (must match flash_write_ram.c / flash_stub.ld):
#   0x20000000  firmware data   (loaded here by load_image, up to 47 KB per pass)
#   0x2000B800  parameter block (16 bytes)
#   0x2000B810  stub ELF
#
# Binaries larger than 47 KB are automatically split across multiple stub runs
# within the same OpenOCD session (no intermediate resets — safe for Qflash).
#
# Usage (called by flash.bat via -c flags):
#   n32_program <filename> <flash_addr> <erase_sectors>
#   n32_reset

set N32_STUB_ELF  "tools/flash_write_ram.elf"
set N32_DATA_BASE  0x20000000
set N32_PARAM_BASE 0x2000B800

# Read e_entry from a 32-bit little-endian ELF header (offset 24, 4 bytes).
# Uses only Jim Tcl primitives (no 'binary' command — not available in OpenOCD).
# Returns the entry address with Thumb bit set (bit 0 = 1).
proc n32_elf_entry {filename} {
    set f [open $filename rb]
    seek $f 24
    set data [read $f 4]
    close $f
    set entry 0
    for {set i 0} {$i < 4} {incr i} {
        scan [string index $data $i] %c byte
        set entry [expr {$entry | ($byte << ($i * 8))}]
    }
    return [expr {$entry | 1}]
}

# ── n32_connect ──────────────────────────────────────────────────────────────
# Perform reset+halt to gain SWD access before a flash session.
# Call this ONCE before a sequence of n32_program calls.
proc n32_connect {} {
    reset halt
    echo "  Connected (halted)"
}

# ── n32_program_one ───────────────────────────────────────────────────────────
# Internal: run one stub pass for a single chunk (must be <= 47 KB).
#   filename      — path to chunk .bin (or "" for erase-only)
#   flash_addr    — absolute flash address for this chunk
#   erase_sectors — sectors to erase (0 = no erase)
#   do_reset      — 1 = reset+halt first (only for very first op in session)
proc n32_program_one {filename flash_addr erase_sectors {do_reset 0}} {
    global N32_STUB_ELF N32_DATA_BASE N32_PARAM_BASE

    set data_length 0
    if {$filename ne "" && [file exists $filename]} {
        set data_length [file size $filename]
    } elseif {$filename ne ""} {
        error "Binary not found: $filename"
    }

    # Reset only when explicitly requested (first op in a session).
    if {$do_reset} {
        reset halt
    }

    # Load stub into RAM at 0x2000B810
    load_image $N32_STUB_ELF

    # Load firmware data into RAM at 0x20000000
    if {$data_length > 0} {
        load_image $filename $N32_DATA_BASE bin
    }

    # Write parameter block
    mww $N32_PARAM_BASE                    $flash_addr
    mww [expr {$N32_PARAM_BASE + 4}]       $data_length
    mww [expr {$N32_PARAM_BASE + 8}]       $erase_sectors
    mww [expr {$N32_PARAM_BASE + 12}]      0xDEADBEEF

    # Run stub — it erases, writes, then executes bkpt #0 to halt
    set entry [n32_elf_entry $N32_STUB_ELF]
    resume $entry

    # Wait up to 120 s (erase of 60 sectors + write of 47 KB)
    wait_halt 120000

    # Check result
    mem2array status_arr 32 [expr {$N32_PARAM_BASE + 12}] 1
    set status $status_arr(0)
    if {$status != 0} {
        error [format "Flash FAILED: status=0x%08X (0x1xxxxxxx=erase error sector#, 0x2xxxxxxx=write error page#)" $status]
    }

    echo "  OK"
}

# ── n32_program ──────────────────────────────────────────────────────────────
# Program (erase + write) a binary file into flash.
#   filename      — path to .bin file (forward slashes), or "" for erase-only
#   flash_addr    — absolute flash start address (e.g. 0x01004000)
#   erase_sectors — number of 4 KB sectors to erase before writing
#                   (pass 0 to skip erase; pass >0 and filename="" to erase only)
#   do_reset      — (optional, default 0) set to 1 to reset+halt before this op.
#                   Pass 1 only for the FIRST operation in a session; subsequent
#                   operations reuse the already-halted CPU so the Qflash
#                   controller state is never disturbed between writes.
#
# Large binaries (> 47 KB) are split into 47 KB chunks automatically.
# All chunks are written in the same OpenOCD session without intermediate resets.
proc n32_program {filename flash_addr erase_sectors {do_reset 0}} {
    global N32_STUB_ELF N32_DATA_BASE N32_PARAM_BASE

    if {![file exists $N32_STUB_ELF]} {
        error "Stub ELF not found: $N32_STUB_ELF — run flash.bat to build it first"
    }

    # Max bytes the stub RAM buffer can hold (0x2000B800 - 0x20000000)
    set MAX_CHUNK [expr {$N32_PARAM_BASE - $N32_DATA_BASE}]

    # Determine data length (0 for erase-only)
    set data_length 0
    if {$filename ne "" && [file exists $filename]} {
        set data_length [file size $filename]
    } elseif {$filename ne ""} {
        error "Binary not found: $filename"
    }

    echo [format "  erase %d sectors @ 0x%08X" $erase_sectors $flash_addr]
    if {$data_length > 0} {
        echo [format "  write %d bytes   @ 0x%08X" $data_length $flash_addr]
    }

    if {$data_length <= $MAX_CHUNK} {
        # Single pass — common case
        n32_program_one $filename $flash_addr $erase_sectors $do_reset
        return
    }

    # ── Multi-pass for large binaries ─────────────────────────────────────────
    # Split the binary into MAX_CHUNK-sized pieces using Jim Tcl binary file I/O.
    # Each chunk is written to a temp file, flashed, then deleted.
    # Erase is done only in the first pass (all sectors were already calculated
    # to cover the full binary by the caller).
    set tmpfile [file join [file dirname $filename] _n32_chunk_tmp.bin]
    set f [open $filename rb]

    set chunk_addr $flash_addr
    set remaining  $data_length
    set first_pass 1

    while {$remaining > 0} {
        set chunk_size $remaining
        if {$chunk_size > $MAX_CHUNK} { set chunk_size $MAX_CHUNK }

        # Read chunk from source binary
        set chunk_data [read $f $chunk_size]

        # Write chunk to temp file
        set tf [open $tmpfile wb]
        puts -nonewline $tf $chunk_data
        close $tf

        # Flash chunk (erase only on first pass)
        if {$first_pass} {
            n32_program_one $tmpfile $chunk_addr $erase_sectors $do_reset
            set first_pass 0
        } else {
            n32_program_one $tmpfile $chunk_addr 0 0
        }

        catch {file delete $tmpfile}

        set chunk_addr [expr {$chunk_addr + $chunk_size}]
        set remaining  [expr {$remaining  - $chunk_size}]
    }

    close $f
}

# ── n32_chip_erase ───────────────────────────────────────────────────────────
# Erase all 128 sectors of the 512 KB Qflash (0x01000000–0x0107FFFF).
# Call this after n32_connect and before n32_program calls.
proc n32_chip_erase {} {
    echo "  Chip erase: 128 sectors @ 0x01000000 (512 KB)..."
    n32_program_one "" 0x01000000 128 0
    echo "  Chip erase done"
}

# ── n32_reset ────────────────────────────────────────────────────────────────
# Clear the data buffer region (0x20000000–0x2000AFEF) that was used to stage
# firmware images, so stale flash-stub data doesn't corrupt BLE ROM tables
# that live in the same SRAM area after reset.
proc n32_reset {} {
    global N32_DATA_BASE N32_PARAM_BASE
    halt
    # Zero the 47 KB data buffer (0x20000000..0x2000B7FF) that was used to
    # stage firmware images, so stale Qflash stub data doesn't sit in SRAM
    # where the BLE ROM expects its working memory after the final reset.
    set words [expr {($N32_PARAM_BASE - $N32_DATA_BASE) / 4}]
    mww $N32_DATA_BASE 0 $words
    # cortex_m reset_config sysresetreq is only registered under DAP-capable
    # drivers (e.g. CMSIS-DAP) -- the legacy "hla" driver (ST-Link) handles
    # reset internally and doesn't expose this command at all, so guard it
    # rather than letting it abort the whole reset (and the flash it follows).
    catch {cortex_m reset_config sysresetreq}
    reset run
}
