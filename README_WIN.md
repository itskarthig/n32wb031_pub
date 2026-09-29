# Windows Build & Flash Guide

This is a Windows-specific companion to [README.md](README.md). The build system
(`Makefile`, CMake, OpenOCD) is POSIX-oriented and was originally macOS/Linux-only;
this file documents everything needed to build and flash from plain `cmd.exe` /
PowerShell on Windows, plus the CMSIS-DAP driver issue you will almost certainly hit
the first time you try to flash.

---

## Table of Contents

1. [Prerequisites](#1-prerequisites)
2. [First-time Setup](#2-first-time-setup)
3. [Building](#3-building)
4. [Flashing](#4-flashing)
5. [CMSIS-DAP driver troubleshooting (Zadig)](#5-cmsis-dap-driver-troubleshooting-zadig)
6. [ST-Link as an alternative](#6-st-link-as-an-alternative)
7. [make.bat internals](#7-makebat-internals)
8. [Known-good tool versions on this machine](#8-known-good-tool-versions-on-this-machine)
9. [Vendor NSpyocd tool — not currently usable](#9-vendor-nspyocd-tool--not-currently-usable)

---

## 1. Prerequisites

| Tool | Notes |
|------|-------|
| **Git for Windows** | Provides `bash.exe`, `sed`, `grep`, `find` — the `Makefile` is POSIX and only runs correctly under this shell. `make.bat` (see below) drives it for you, so you don't need to open Git Bash by hand. Default install path assumed: `C:\Program Files\Git\bin\bash.exe`. |
| **GNU make** | Not bundled with Git for Windows. Install with `winget install ezwinports.make`. After installing, open a **new** terminal window — Windows doesn't refresh a running process's PATH, only new ones (see [make.bat internals](#7-makebat-internals) for why `make.bat` mostly works around this anyway). |
| **arm-none-eabi-gcc 10.3-2021.10** | [Arm GNU Toolchain](https://developer.arm.com/downloads/-/gnu-rm). A newer GCC (e.g. 15.x) may also work but is untested against this codebase — stick to 10.3 unless you have a reason not to. |
| **CMake 3.20+** | [cmake.org](https://cmake.org/download/) |
| **Ninja** | [github.com/ninja-build/ninja/releases](https://github.com/ninja-build/ninja/releases) — grab `ninja-win.zip`, extract anywhere. Not on PATH by default; point `NINJA` at it in settings.json (see below). |
| **xPack OpenOCD 0.12.0-7** | [xpack-dev-tools/openocd-xpack releases](https://github.com/xpack-dev-tools/openocd-xpack/releases) — grab the `win32-x64` archive. Needed for flashing (`make.bat flash ...`), not for building. |
| **A debug probe** | CMSIS-DAP (e.g. a DAPLink-based probe) or ST-Link V2. Both are supported by this repo's OpenOCD configs (`openocd/n32wb031_dap.cfg` / `openocd/n32wb031.cfg`). CMSIS-DAP needs a Zadig driver fix on Windows — see [§5](#5-cmsis-dap-driver-troubleshooting-zadig). |
| **Zadig** | [zadig.akeo.ie](https://zadig.akeo.ie) — only needed if OpenOCD can't open your CMSIS-DAP probe (see §5). Not needed for ST-Link, which uses ST's own driver. |

None of this needs WSL or MSYS2 — Git for Windows' bundled `bash.exe` is sufficient.

---

## 2. First-time Setup

Same `.vscode/settings.json` → `cmake.configureSettings` block described in the main
[README.md](README.md#2-first-time-setup), with Windows-specific values. Example, filled
in for a typical Windows dev machine:

```json
"cmake.configureSettings": {
    "GCC_PATH": "C:/tools/GNU Arm Embedded Toolchain/10 2021.10/bin",
    "NINJA": "C:/tools/ninja-win/ninja.exe",
    "OPENOCD_BIN": "C:/tools/xpack-openocd-0.12.0-7-win32-x64/xpack-openocd-0.12.0-7/bin/openocd.exe",
    "SDK_ROOT": "../N32WB03x_SDK/N32WB03x_SDK_V2.0.0",
    "BOARD": "N32WB03x_EVAL",
    "CATEGORY": "BLE",
    "PROJECT": "P2PS_OTA",
    "PROBE": "dap",
    "BUILD": "release"
}
```

Notes specific to Windows:

- **Use forward slashes** (`C:/tools/...`), not backslashes, in every path in this file —
  backslashes inside a JSON string are escape sequences and will silently corrupt the
  path (e.g. `\10` is not `\1` followed by `0`).
- **`NINJA`** exists specifically because Windows has no standard install location for
  Ninja the way `GCC_PATH`/`OPENOCD_BIN` do — it's rarely already on PATH after a plain
  zip extract. The `Makefile` reads it the same way as `GCC_PATH`/`OPENOCD_BIN`: settings.json
  first, falling back to a bare `ninja` PATH lookup if unset (which is normally fine on
  macOS/Linux, where Ninja usually is on PATH).
- **Double-check every value is comma-terminated except the last one** in the JSON object
  — a missing comma here is easy to introduce by hand-editing and doesn't always break the
  `Makefile`'s own `grep`/`sed`-based extraction (which doesn't actually parse JSON), but it
  *will* break VS Code's CMake Tools extension and any real JSON parser. If in doubt, open
  the file in an editor with JSON syntax highlighting and confirm there are no red squiggles.
- `BOARD`/`CATEGORY`/`PROJECT` select which project you're building — see
  [README.md §3](README.md#3-project-structure) for the directory layout, or run
  `make.bat list-projects` to enumerate every legal combination.

---

## 3. Building

From a plain `cmd.exe` or PowerShell prompt, in the repo root:

```powershell
make.bat build INSTALLER
make.bat build APP
make.bat build ALL              # INSTALLER then APP
make.bat build APP BUILD=debug
make.bat build APP CATEGORY=STANDALONE PROJECT=PERIPHERAL BOARD=N32WB03x_EVAL   # one-off override
make.bat list-projects
make.bat clean
make.bat size
make.bat help
```

This is a 1:1 passthrough to the real `Makefile`'s own `make build INSTALLER|APP|ALL`
etc. (see [README.md §4](README.md#4-building) and the `Makefile`'s own header comment
for the full command reference) — every flag/override documented there works identically
here, just prefixed with `.\` or typed as `make.bat` instead of `make`.

---

## 4. Flashing

```powershell
make.bat flash INSTALLER
make.bat flash APP
make.bat flash ALL              # chip erase + INSTALLER + APP
make.bat flash APP PROBE=stlink
make.bat flash APP PROBE=dap BUILD=release
```

Same passthrough as building — see [README.md §5](README.md#5-flashing) for the full
command reference. `PROBE` defaults to whatever `.vscode/settings.json` has (typically
`dap`), overridable per-invocation.

**If flashing fails with `Error: unable to find a matching CMSIS-DAP device`**, go to
[§5](#5-cmsis-dap-driver-troubleshooting-zadig) below — this is the single most common
Windows-specific flashing problem with this repo, caused by Windows USB driver binding,
not anything in this repo's config.

---

## 5. CMSIS-DAP driver troubleshooting (Zadig)

### Symptom

```
Error: unable to find a matching CMSIS-DAP device
```
or, with verbose logging (`openocd -d3 ...`):
```
could not open device 0x0d28:0x0204: Operation not supported or unimplemented on this platform
```

### Root cause

A CMSIS-DAP probe (e.g. DAPLink-based, VID:PID `0D28:0204`) exposes several USB
interfaces at once — mass storage (drag-and-drop firmware update), a CDC serial port,
a HID interface (CMSIS-DAP v1), and a WinUSB bulk interface (CMSIS-DAP v2). OpenOCD's
`cmsis-dap` driver on this xPack build talks to the probe over **libusb**, targeting the
**CMSIS-DAP v2 (WinUSB bulk) interface** specifically — it does not fall back to the HID
v1 interface (this particular OpenOCD build appears to lack `hidapi` support; the debug
log only ever shows `cmsis_dap_usb_bulk.c` attempts, never a HID backend).

Even when Windows Device Manager shows that interface already bound to `WINUSB`
(common — many DAPLink firmwares ship a Microsoft OS descriptor that gets Windows to
auto-bind WinUSB with zero driver install needed), **libusb may still fail to open it**
with `Operation not supported or unimplemented on this platform`. This is a libusb/Windows
driver-stack quirk, not a config problem in this repo.

### Fix

1. **Disconnect every other debug probe.** Two identical `0D28:0204` devices (or an
   ST-Link plugged in alongside) make enumeration ambiguous — OpenOCD may pick the wrong
   one, or fail outright. Keep exactly one probe connected while flashing.

2. Run [Zadig](https://zadig.akeo.ie) **as Administrator**.

3. **Options → List All Devices.**

4. Find the entry for the CMSIS-DAP v2 interface specifically — Device Manager shows this
   as `CMSIS-DAP v2` or `WebUSB: CMSIS-DAP`, always with an `MI_04` (or similar,
   check `pnputil`/Device Manager to confirm the exact interface number) suffix on its
   hardware ID. **Do not pick the bare/top-level composite device entry** (shown in Zadig
   or Device Manager without an `MI_xx` qualifier, or simply named after the probe with no
   interface annotation) — replacing the driver there collapses the whole device down to a
   single interface (usually the mass-storage one), which has no bulk endpoints and makes
   things *worse*, not better. This is a real mistake that's easy to make and was hit
   during this repo's own Windows bring-up — see the "undo" steps below if you did this.

5. Select **WinUSB** as the target driver, click **Replace Driver** (or **Install Driver**
   if none is bound yet).

6. Retry:
   ```powershell
   make.bat flash APP PROBE=dap
   ```

### Undoing a driver replaced on the wrong (whole-device) interface

If Device Manager now shows an entry like `DAPLink CMSIS-DAP` bound to `WinUSB` with
**no** `MI_xx` suffix in its hardware ID, that's the mistake described above:

1. Device Manager → find that entry → right-click → **Uninstall device** → check
   *"Attempt to remove the driver for this device"* → OK.
2. **Unplug and replug the probe** so Windows re-enumerates it as a proper composite
   device again (its `MI_00`–`MI_04` children reappear).
3. Retry flashing — the correct `MI_04` interface may already have been natively
   WinUSB-bound before you ever touched Zadig, in which case no further Zadig action is
   needed at all now that the bad whole-device binding is gone.

### Verifying driver state from PowerShell

Useful for confirming what's actually bound before/after a Zadig change, without opening
Device Manager's GUI:

```powershell
Get-PnpDevice | Where-Object { $_.InstanceId -match 'VID_0D28' } | ForEach-Object {
  $id = $_.InstanceId
  $svc = (Get-PnpDeviceProperty -InstanceId $id -KeyName DEVPKEY_Device_Service -ErrorAction SilentlyContinue).Data
  "{0,-30} svc={1,-15} {2}" -f $_.FriendlyName, $svc, $id
}
```

Look for an entry ending in `&MI_04` with `svc=WINUSB` (or `WinUSB`) — that's the one
that actually needs to work. `svc=usbccgp` on the bare (no `&MI_xx`) parent entry is
normal and expected (that's Windows' generic composite-device driver, holding the whole
thing together so the `MI_xx` children can exist).

### Confirming a working probe with openocd directly

Bypasses the `Makefile`/`make.bat` layer entirely, for isolating whether a flash failure
is a driver problem or something else:

```powershell
& "C:\tools\xpack-openocd-0.12.0-7-win32-x64\xpack-openocd-0.12.0-7\bin\openocd.exe" `
  -f openocd\n32wb031_dap.cfg -c init -c shutdown
```

A working probe prints something like:
```
Info : Using CMSIS-DAPv2 interface with VID:PID=0x0d28:0x0204, serial=...
Info : CMSIS-DAP: SWD supported
...
Info : [n32wb031.cpu] Examination succeed
```
Add `-d3` before `-f` for verbose USB-level debug logging if it's still failing.

---

## 6. ST-Link as an alternative

If you have an ST-Link V2 probe instead of (or in addition to) a CMSIS-DAP one, it uses
`openocd/n32wb031.cfg` and generally needs no Zadig intervention — ST's own driver
(installed via ST-Link Utility or the ST-Link driver package) is typically sufficient. Try:

```powershell
make.bat flash APP PROBE=stlink
```

If this also fails with `Error: open failed`, the same class of libusb/driver issue can
occur — the troubleshooting approach in [§5](#5-cmsis-dap-driver-troubleshooting-zadig)
applies equally (disconnect other probes first; Zadig can target the ST-Link's WinUSB
interface the same way, look for `STM32 STLink`/`ST-Link Debug` in the device list).

---

## 7. make.bat internals

`make.bat` (repo root) is a thin Windows entry point — it does not reimplement any build
logic, it just re-invokes the real, POSIX `Makefile` inside Git Bash:

```
bash.exe -lc "cd <repo root, converted to a POSIX path> && make <your args>"
```

A few things worth knowing if it misbehaves:

- **It self-heals a just-installed `make`.** If `winget install ezwinports.make` was just
  run in the *same* terminal window that's still open, Windows won't have refreshed that
  process's PATH yet (PATH changes only take effect in *new* processes). `make.bat` detects
  this (checking specifically for `make.exe`, not bare `make` — see note below) and, if
  `make` isn't yet resolvable, searches
  `%LOCALAPPDATA%\Microsoft\WinGet\Packages\ezwinports.make_*\bin` and prepends it to PATH
  before invoking bash. A brand-new terminal window is still the more reliable fix if this
  doesn't work for some reason.
- **Why it checks `make.exe` specifically, not `make`:** Windows' `where` command always
  searches the current directory first, regardless of PATH. Since this very script is named
  `make.bat`, a naive `where make` run from the repo root matches **itself** and reports
  success even when the real `make.exe` isn't installed anywhere. Checking `where make.exe`
  avoids this self-match.
- **Exit codes propagate correctly** (`exit /b %errorlevel%`) — safe to use from CI/tasks
  that check the return code.
- Extra arguments pass straight through untouched, so anything documented for the real
  `Makefile` works verbatim: `make.bat build APP CATEGORY=STANDALONE PROJECT=PERIPHERAL`.
- If Git Bash itself isn't found at either of the two standard install locations
  (`C:\Program Files\Git\bin\bash.exe` or the `(x86)` variant), `make.bat` fails fast with
  a clear message rather than a cryptic error further down.

---

## 8. Known-good tool versions on this machine

Recorded here as a reference point, not a hard requirement — later versions of most of
these will likely work fine, just untested against this specific codebase.

| Tool | Version | Install path used |
|------|---------|--------------------|
| arm-none-eabi-gcc | 10.3.1 (GNU Arm Embedded Toolchain 10.3-2021.10) | `C:\tools\GNU Arm Embedded Toolchain\10 2021.10\bin` |
| CMake | 4.3.2 | `C:\tools\CMake\bin` |
| Ninja | 1.13.2 | `C:\tools\ninja-win\ninja.exe` |
| xPack OpenOCD | 0.12.0+dev (win32-x64) | `C:\tools\xpack-openocd-0.12.0-7-win32-x64\xpack-openocd-0.12.0-7\bin\openocd.exe` |
| GNU make (ezwinports) | 4.4.1 | `%LOCALAPPDATA%\Microsoft\WinGet\Packages\ezwinports.make_Microsoft.Winget.Source_8wekyb3d8bbwe\bin` (via `winget install ezwinports.make`) |
| Git for Windows | (bundles bash/sed/grep/find) | `C:\Program Files\Git` |

A second, newer ARM GCC copy (15.2.1, from `arm.com`'s current downloads) was also seen at
`C:\tools\Arm\GNU Toolchain mingw-w64-x86_64-arm-none-eabi\bin` on this machine but is
**not** what `GCC_PATH` should point at — stick with 10.3 unless you have a specific
reason to test newer GCC against this codebase (untested for warnings/code-size changes).

---

## 9. Vendor NSpyocd tool — not currently usable

The SDK ships its own flashing utility at
`N32WB03x_SDK/N32WB03x_SDK_V2.0.0/utilities/dfu/NSpyocd/NSpyocd.exe`, driven by the
adjacent `NSLinkProgramming.bat`. This is **not** a drop-in alternative to this repo's
`make.bat flash` — worth knowing about, but don't expect it to just work:

- It's paired with NationsTech's own **NSLink** SWD probe specifically, not a generic
  CMSIS-DAP or ST-Link probe (though it may support other probes too — untested here).
- Its flashing flow is materially different from this repo's: it first merges a
  `masterboot` + `bootsetting` + app image into one combined `.hex` via `NSUtil.exe`
  (`pack mergebin`), then calls `NSpyocd.exe erase --chip` + `NSpyocd.exe load ... .hex`
  — it has no awareness of this repo's per-project `Linker`/`.bin` output layout, so
  it can't be pointed at a `Projects/.../APP/*.bin` directly without first replicating
  that merge step.
- **On this machine, it currently crashes immediately** on any invocation (even
  `NSpyocd.exe --help`) with:
  ```
  UnicodeEncodeError: 'charmap' codec can't encode characters in position 0-3:
  character maps to <undefined>
  ```
  This happens before any real argument parsing — it's a PyInstaller-bundled Python app
  failing to print something (likely a startup banner containing a non-ASCII character)
  because its stdout encoding resolves to `cp1252` instead of UTF-8. Neither `chcp 65001`
  nor `$env:PYTHONIOENCODING="utf-8"` fixed it in testing, which suggests the encoding is
  baked into how the executable was frozen rather than being something a caller can
  override from outside. It's possible this only works when run from a terminal window
  you open and type into directly (a real attached console), rather than through any
  scripted/piped invocation — untested. If you need this tool and hit the same crash, that's
  the next thing to try before assuming it's unusable outright.

Unless you specifically have an NSLink probe and a reason to use the vendor's own image
format, use `make.bat flash` (OpenOCD-based) instead — it's the actively-maintained,
tested path for this repo.
