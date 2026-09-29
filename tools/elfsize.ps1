# elfsize.ps1 — Parse GCC ELF and print Flash/RAM usage with progress bars
#
# Usage: powershell -File tools/elfsize.ps1 <elf_file> <ld_script> [label]
#
# Flash = text + data  (data stored in flash as load-region)
# RAM   = data + bss   (data copied to RAM on startup, bss zero-filled)
#
# Flash and RAM limits are read dynamically from the MEMORY block of <ld_script>:
#   FLASH  ... LENGTH = 512K / 240K / 8K    -> flash budget
#   RAM    ... LENGTH = 0x7F80 / 32K        -> RAM budget
param(
    [string]$ElfFile,
    [string]$LdScript,
    [string]$Label = ""
)

# ---- Parse a GNU LD size expression (512K / 0x7F80 / 8192) to bytes ---------
function ConvertTo-Bytes([string]$expr) {
    # Strip anything after whitespace (e.g. inline comments "/* 32640 B */")
    $s = ($expr.Trim() -replace '\s.*', '').ToUpper()
    if ($s -match '^0X([0-9A-F]+)$') { return [Convert]::ToInt64($Matches[1], 16) }
    if ($s -match '^(\d+)K$')         { return [int64]$Matches[1] * 1024 }
    if ($s -match '^(\d+)M$')         { return [int64]$Matches[1] * 1024 * 1024 }
    return [int64]$s
}

# ---- Read FLASH and RAM limits from linker script ---------------------------
$FlashMax = 0
$RamMax   = 0

if (Test-Path $LdScript) {
    $ld = Get-Content $LdScript -Raw
    # FLASH region: line whose first identifier is FLASH
    if ($ld -match '(?m)^\s*FLASH\b[^:]*:.*?LENGTH\s*=\s*([0-9A-Fa-fxX]+[KkMm]?)') {
        $FlashMax = ConvertTo-Bytes $Matches[1]
    }
    # RAM region: first identifier is exactly RAM (not NVM_RAM or BOOT_SETTINGS)
    if ($ld -match '(?m)^\s*RAM\b[^:]*:.*?LENGTH\s*=\s*([0-9A-Fa-fxX]+[KkMm]?)') {
        $RamMax = ConvertTo-Bytes $Matches[1]
    }
} else {
    Write-Host ("  [WARN] LD script not found: {0}" -f $LdScript)
}

if (-not (Test-Path $ElfFile)) {
    $name = if ($Label) { $Label } else { [System.IO.Path]::GetFileName($ElfFile) }
    Write-Host "  $name : not built"
    exit 0
}

# Locate arm-none-eabi-size
$SizeExe = "C:\tools\GNU Arm Embedded Toolchain\10 2021.10\bin\arm-none-eabi-size.exe"
if (-not (Test-Path $SizeExe)) {
    $found = Get-Command "arm-none-eabi-size" -ErrorAction SilentlyContinue
    if ($found) { $SizeExe = $found.Source } else {
        Write-Host "  [ERROR] arm-none-eabi-size not found"
        exit 1
    }
}

# Run arm-none-eabi-size (Berkeley format: text  data  bss  dec  hex  filename)
$output = & $SizeExe $ElfFile 2>&1
# Second line contains the numbers
$dataLine = ($output | Select-Object -Skip 1 -First 1) -replace '\s+', ' '
$parts = $dataLine.Trim().Split(' ')

$textSize = [int]$parts[0]
$dataSize = [int]$parts[1]
$bssSize  = [int]$parts[2]

$flash = $textSize + $dataSize
$ram   = $dataSize + $bssSize

function Show-Bar([int64]$used, [int64]$total, [int]$width = 30) {
    $pct    = if ($total -gt 0) { [int](($used / $total) * 100) } else { 0 }
    $filled = if ($total -gt 0) { [int](($used / $total) * $width) } else { 0 }
    $bar    = ('#' * $filled) + ('-' * ($width - $filled))
    return "[{0}] {1,3}%" -f $bar, $pct
}

$flashBar = Show-Bar $flash $FlashMax
$ramBar   = Show-Bar $ram   $RamMax

$hdr = if ($Label) { "  $Label" } else { "  " + [System.IO.Path]::GetFileNameWithoutExtension($ElfFile) }
$decSize = $textSize + $dataSize + $bssSize
$hexSize = "{0:x}" -f $decSize
Write-Host $hdr
Write-Host ("    text={0,6}  data={1,6}  bss={2,6}  dec={3,6}  hex={4}" -f $textSize, $dataSize, $bssSize, $decSize, $hexSize)
Write-Host ("    Flash: {0,6} / {1,6} B  {2}" -f $flash, $FlashMax, $flashBar)
Write-Host ("    RAM:   {0,6} / {1,6} B  {2}" -f $ram,   $RamMax,   $ramBar)
