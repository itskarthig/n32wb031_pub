@echo off
REM ============================================================
REM N32WB031 Windows build wrapper -- thin passthrough to the
REM repo's POSIX Makefile via Git Bash.
REM
REM The Makefile itself uses grep/sed/find and only runs correctly
REM under a POSIX shell. This batch file lets you drive it from a
REM plain cmd.exe or PowerShell prompt without opening Git Bash
REM by hand.
REM
REM Usage (mirrors Makefile's own usage exactly -- see Makefile
REM header comment for the full list):
REM   make.bat build INSTALLER^|APP^|ALL
REM   make.bat flash INSTALLER^|APP^|ALL
REM   make.bat list-projects
REM   make.bat config
REM   make.bat size
REM   make.bat clean
REM   make.bat help
REM
REM Extra args pass straight through, e.g.:
REM   make.bat build APP BUILD=debug
REM   make.bat build APP CATEGORY=STANDALONE PROJECT=PERIPHERAL
REM
REM GNU make itself must be installed and on PATH *inside* Git
REM Bash (this batch file does not install it). If you just ran
REM `winget install ezwinports.make`, this script tries to find
REM it under %LOCALAPPDATA%\Microsoft\WinGet\Packages even in a
REM terminal window that hasn't been restarted yet (Windows only
REM refreshes a running process's PATH on next launch, not live) --
REM but a fresh terminal window is the more reliable fix.
REM ============================================================

setlocal

set "BASH_EXE=C:\Program Files\Git\bin\bash.exe"
if not exist "%BASH_EXE%" set "BASH_EXE=C:\Program Files (x86)\Git\bin\bash.exe"
if not exist "%BASH_EXE%" (
    echo [ERROR] Git Bash not found at "C:\Program Files\Git\bin\bash.exe" ^(or the x86 path^).
    echo         Install Git for Windows from https://git-scm.com/download/win, or edit
    echo         make.bat if your Git install lives somewhere else.
    exit /b 1
)

REM Fallback: if this cmd/PowerShell session's own PATH doesn't see
REM `make.exe` yet (e.g. `winget install ezwinports.make` just ran
REM in a window that hasn't been restarted), look for the known
REM ezwinports install location under WinGet's per-package packages
REM dir and append it -- so make.bat is self-healing for that one
REM common case without requiring a new terminal window. Checked
REM specifically as "make.exe" (not bare "make") because `where`
REM always searches the current directory first, and this very
REM script is named make.bat -- a bare `where make` would match
REM itself and report a false positive every time it's run from
REM the repo root.
where make.exe >nul 2>nul
if errorlevel 1 (
    for /d %%D in ("%LOCALAPPDATA%\Microsoft\WinGet\Packages\ezwinports.make_*") do (
        if exist "%%D\bin\make.exe" set "PATH=%%D\bin;%PATH%"
    )
)

REM %* holds every argument exactly as typed. Quote it as a single
REM string handed to `bash -lc` so `make` sees the same word-split
REM args a native `make build ALL` invocation would see.
"%BASH_EXE%" -lc "cd \"$(cygpath -u '%~dp0')\" && make %*"
set "MAKE_RC=%ERRORLEVEL%"

if not "%MAKE_RC%"=="0" (
    "%BASH_EXE%" -lc "command -v make >/dev/null 2>&1"
    if errorlevel 1 (
        echo.
        echo [HINT] 'make' was not found inside Git Bash. Install GNU make there, e.g.:
        echo          winget install ezwinports.make
        echo        then open a new Git Bash / cmd / PowerShell window and retry.
    )
)

endlocal & exit /b %MAKE_RC%
