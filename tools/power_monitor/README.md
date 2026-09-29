# Power / Log Anomaly Monitor

Correlates AIR9000 (`iot_recoder-win-x64.exe`) current captures with the
DUT's debug-UART log to find periods where current stays abnormally elevated
with no matching wake-cause in the log — the class of bug documented in
CLAUDE.md as LPM voters stuck DISABLE / STOP not re-entered (e.g. Bug 8,
Bug 42, Bug 51).

## One-time setup

```powershell
pip install pyserial
```

## Running a capture

Find the AIR9000 USB device name first:

```powershell
& "C:\tools\iot_recoder\iot_recoder-win\iot_recoder-win-x64.exe" -p list
```

Then run a joint capture + analysis (example: 1 hour on COM8):

```powershell
./run_capture.ps1 -Duration 3600 -PowerDevice USBCC-1A5FFB100 -SerialPort COM8
```

For a long/overnight/multi-day run, just increase `-Duration` (seconds) --
the capture is automatically split into `-ChunkSeconds` chunks (default
3600 = 1 hour) instead of one long-lived recorder process. This matters:
a single multi-hour `iot_recoder-win-x64.exe` invocation can have the USB
power probe silently stop streaming partway through (no error, no crash --
it just stops writing rows) and you only find out at the very end with an
empty CSV and a lost run. Chunking means:
- each chunk's power CSV is checked for actual data rows right after it
  finishes; a 0-row chunk is retried once automatically
- the recorder's own stdout/stderr is captured per chunk
  (`chunk_000N/recorder_stdout.log` / `recorder_stderr.log`) for diagnosis
- a bad chunk only costs that chunk -- the run continues and everything
  that did capture is still usable

Output lands in `runs/<timestamp>/`:
- `chunk_0001/`, `chunk_0002/`, ... — per-chunk `power_capture.csv` /
  `serial_log.csv` / recorder logs
- `power_capture_combined.csv`, `serial_log_combined.csv` — all chunks
  concatenated (header written once)
- `report.md` — anomaly report over the combined data (generated
  automatically)

If a chunk repeatedly gets 0 power samples, check: the AIR9000 USB cable/hub,
and Windows USB selective suspend on that port (Device Manager > the USB
hub/root port > Power Management tab > uncheck "Allow the computer to turn
off this device to save power") -- selective suspend is a common cause of a
USB device going quiet on a multi-hour idle-looking capture without the host
noticing.

## Re-running analysis only

If you already have both CSVs (e.g. from a manual capture, or the
`*_combined.csv` files from a chunked run) and just want to re-run the
correlation with different thresholds:

```powershell
python analyze.py --power runs\20260815_120000\power_capture_combined.csv `
                   --log runs\20260815_120000\serial_log_combined.csv `
                   --report runs\20260815_120000\report.md `
                   --baseline-ua 55 --tolerance-ua 20 --active-threshold-ua 200
```

## Tuning thresholds

Defaults are based on the measured baseline with the USB-TTL debug adapter
attached (50-60 uA sleep current):

| Flag | Default | Meaning |
|------|---------|---------|
| `--baseline-ua` | 55 | Expected STOP-mode current |
| `--tolerance-ua` | 20 | +/- band still counted as "baseline" |
| `--active-threshold-ua` | 200 | Above this = MCU considered active/awake |
| `--settle-seconds` | 5 | Grace period after a session-end log line before "stuck active" fires |
| `--check-span-seconds` | 10 | Window checked after settle time for stuck-active |
| `--event-hold-scale` | 1.0 | Multiplier on each subsystem's hold time (see `MARKER_GROUPS` in `analyze.py`: WMBus/BLE/FUOTA=3s, cellular=5s, slave=65s to bridge normal AT-command gaps within a session) |
| `--bucket-seconds` | 1 | Averaging window for current samples |

Run a short smoke test first (e.g. `-Duration 60`) to confirm both captures
line up and the report generates cleanly before doing a multi-hour run.

## Anomaly types in the report

- **unexplained_wake** — current elevated above threshold with no active
  log-driven session (Slave/Cellular/BLE/WMBus) overlapping.
- **stuck_active** — current stays elevated for longer than the settle window
  after a session clearly ended in the log (`cmd_reset`, `CIPCLOSE`,
  disconnect, `ArmSlaveWakeup`, etc.) — matches the historical
  "LPM voter stuck DISABLE" bug class.
- **missing_return_to_baseline** — current never comes back down to baseline
  for the remainder of the run after the last known session ended.

Each anomaly entry includes the surrounding log lines so you can correlate
it back to `app_config.h`'s `LpmVoterId_t` bits and the relevant driver file.

## Note on `tools/iotpower_cc/`

That directory contains earlier raw `pyusb` experiments talking to the
AIR9000 directly. They're superseded by `iot_recoder-win-x64.exe`, which
already exposes clean CSV/TCP output for this device — this monitor tool
uses the exe, not those scripts.
