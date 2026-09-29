<#
.SYNOPSIS
    Run power + serial-log capture together, then analyze for anomalies.

    Captures in chunks (default 1 hour each) instead of one long-lived
    recorder process. This was added after a 30-hour unattended run produced
    an empty power_capture.csv -- the USB power probe silently stopped
    streaming partway through (no error from the recorder), and the whole
    run was lost since it was one single -t 108000 invocation. Chunking
    means a single bad chunk (flagged automatically -- see below) only
    costs that chunk, and the recorder's own stdout/stderr is now captured
    to a log file per chunk for diagnosis instead of being silently lost.

.EXAMPLE
    ./run_capture.ps1 -Duration 3600 -PowerDevice USBCC-1A5FFB100 -SerialPort COM8
.EXAMPLE
    # 30-hour run, verified/recorded in 1-hour chunks
    ./run_capture.ps1 -Duration 108000 -PowerDevice USBCC-1A5FFB100 -SerialPort COM8
#>
param(
    [Parameter(Mandatory=$true)][int]$Duration,
    [Parameter(Mandatory=$true)][string]$PowerDevice,
    [Parameter(Mandatory=$true)][string]$SerialPort,
    [int]$Baud = 115200,
    [int]$ChunkSeconds = 3600,
    [string]$RecorderExe = "C:\tools\iot_recoder\iot_recoder-win\iot_recoder-win-x64.exe",
    [string]$RunsDir = ""
)

$ErrorActionPreference = "Stop"

# $PSScriptRoot can be empty depending on how the script was invoked
# (observed empty under `-File`) -- fall back to $PSCommandPath.
$scriptDir = $PSScriptRoot
if ([string]::IsNullOrEmpty($scriptDir)) {
    $scriptDir = Split-Path -Parent $PSCommandPath
}
if ([string]::IsNullOrEmpty($RunsDir)) {
    $RunsDir = Join-Path $scriptDir "runs"
}

$stamp = Get-Date -Format "yyyyMMdd_HHmmss"
$runDir = Join-Path $RunsDir $stamp
New-Item -ItemType Directory -Force -Path $runDir | Out-Null
Write-Host "Run dir: $runDir"

function Get-DataRowCount([string]$csvPath) {
    if (-not (Test-Path $csvPath)) { return 0 }
    $lines = (Get-Content $csvPath | Measure-Object -Line).Lines
    return [Math]::Max(0, $lines - 1)  # minus header
}

function Run-Chunk([string]$chunkDir, [int]$chunkSeconds, [int]$attempt) {
    New-Item -ItemType Directory -Force -Path $chunkDir | Out-Null
    $powerOut = Join-Path $chunkDir "power_capture"
    $powerCsv = "$powerOut.csv"
    $serialCsv = Join-Path $chunkDir "serial_log.csv"
    $recStdout = Join-Path $chunkDir "recorder_stdout.log"
    $recStderr = Join-Path $chunkDir "recorder_stderr.log"

    Write-Host "  [attempt $attempt] power capture (${chunkSeconds}s)..."
    $powerProc = Start-Process -FilePath $RecorderExe `
        -ArgumentList @("-p", $PowerDevice, "-t", $chunkSeconds, "-c", "-o", $powerOut) `
        -PassThru -NoNewWindow `
        -RedirectStandardOutput $recStdout -RedirectStandardError $recStderr

    Write-Host "  [attempt $attempt] serial capture (${chunkSeconds}s)..."
    $serialProc = Start-Process -FilePath "python" `
        -ArgumentList @("$scriptDir\capture_serial.py", "--port", $SerialPort, `
                        "--baud", $Baud, "--duration", $chunkSeconds, "--out", $serialCsv) `
        -PassThru -NoNewWindow

    $powerProc.WaitForExit()
    $serialProc.WaitForExit()

    $rows = Get-DataRowCount $powerCsv
    if ($rows -eq 0) {
        Write-Warning "  Chunk at $chunkDir got 0 power samples (recorder exit code $($powerProc.ExitCode)). See $recStdout / $recStderr."
        return $false
    }
    Write-Host "  Chunk OK: $rows power samples."
    return $true
}

$remaining = $Duration
$chunkIndex = 1
$chunkDirs = @()
while ($remaining -gt 0) {
    $thisChunk = [Math]::Min($ChunkSeconds, $remaining)
    $chunkDir = Join-Path $runDir ("chunk_{0:D4}" -f $chunkIndex)
    Write-Host "Chunk $chunkIndex (${thisChunk}s), $remaining s remaining..."

    $ok = Run-Chunk -chunkDir $chunkDir -chunkSeconds $thisChunk -attempt 1
    if (-not $ok) {
        Write-Warning "  Retrying chunk $chunkIndex once..."
        $ok = Run-Chunk -chunkDir $chunkDir -chunkSeconds $thisChunk -attempt 2
        if (-not $ok) {
            Write-Warning "  Chunk $chunkIndex still has 0 power samples after retry -- check the USB power probe connection (cable/hub/USB selective suspend). Continuing with remaining chunks."
        }
    }

    $chunkDirs += $chunkDir
    $remaining -= $thisChunk
    $chunkIndex++
}

Write-Host "Merging chunks..."
$powerCombined = Join-Path $runDir "power_capture_combined.csv"
$serialCombined = Join-Path $runDir "serial_log_combined.csv"

$powerHeaderWritten = $false
$serialHeaderWritten = $false
foreach ($cd in $chunkDirs) {
    $pCsv = Join-Path $cd "power_capture.csv"
    $sCsv = Join-Path $cd "serial_log.csv"

    if (Test-Path $pCsv) {
        $lines = Get-Content $pCsv
        if ($lines.Count -gt 0) {
            if (-not $powerHeaderWritten) {
                $lines | Set-Content $powerCombined
                $powerHeaderWritten = $true
            } else {
                $lines | Select-Object -Skip 1 | Add-Content $powerCombined
            }
        }
    }

    if (Test-Path $sCsv) {
        $lines = Get-Content $sCsv
        if ($lines.Count -gt 0) {
            if (-not $serialHeaderWritten) {
                $lines | Set-Content $serialCombined
                $serialHeaderWritten = $true
            } else {
                $lines | Select-Object -Skip 1 | Add-Content $serialCombined
            }
        }
    }
}

if (-not $powerHeaderWritten) {
    Write-Warning "No power data captured in any chunk -- skipping analysis."
    exit 1
}

$reportMd = Join-Path $runDir "report.md"
Write-Host "Running analysis..."
python "$scriptDir\analyze.py" --power $powerCombined --log $serialCombined --report $reportMd

Write-Host "Done. Report: $reportMd"
