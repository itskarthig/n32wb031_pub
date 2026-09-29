# ============================================================
# N32WB031 GCC Developer Build (macOS/Linux) — CMake + Ninja + GCC
#
# Usage:
#   make config                      cmake configure only
#   make clean                       remove the whole build/ directory
#   make size                        print Flash / RAM usage (no build)
#   make help                        show this help
#
# The SDK version to build against, and which project to build/flash, are
# single values configured once in .vscode/settings.json's
# cmake.configureSettings -- not command-line choices. Switch either by
# editing that one file and rebuilding:
#   GCC_PATH, OPENOCD_BIN, SDK_ROOT, BOARD, CATEGORY, PROJECT, PROBE, BUILD
#
# The old flat app_* project layout (Phase 1) has been removed -- the
# Projects/ OTA tree below is the only active build target set. The
# `clean`/`rebuild` two-word verb form and the per-app APPS/APPS_APP1
# machinery further down still exist and work correctly with an empty app
# list (everything degrades to a harmless no-op) -- kept in place in case
# a flat app_* target is ever added back.
#
# ---- Projects/ OTA tree (separate from everything above -- own CMake
#      target-name convention, but shares the same build/ output dir) ----
#   make build INSTALLER|APP|ALL
#   make flash INSTALLER|APP|ALL
#   make list-projects               list every <CATEGORY> <Project> <TARGET>
#                                     (use this to find legal BOARD/CATEGORY/
#                                     PROJECT values for settings.json)
#   make clean-projects              remove the whole build/ directory (alias for 'make clean')
# BOARD/CATEGORY/PROJECT (from settings.json) select the literal
# Projects/<BOARD>/Applications/<CATEGORY>/<PROJECT> path -- no auto-detect,
# no CLI argument; see PROJECTS_ROOT's own comment below. BUILD defaults
# release, PROBE defaults dap (differs from flash-<app> above, which
# defaults PROBE=stlink); both settings.json-sourced with the same
# hardcoded fallback as before if unset there. `make build ALL`/`make flash
# ALL` both build/flash INSTALLER then APP; `flash ALL` additionally does a
# chip erase first. Every value (including BOARD/CATEGORY/PROJECT) can
# still be overridden per-invocation on the command line, e.g.
# `make build APP PROJECT=BEACON`. Examples:
#   make build INSTALLER
#   make build APP BUILD=debug
#   make build ALL
#   make flash INSTALLER
#   make flash INSTALLER PROBE=dap BUILD=release
#   make flash ALL
#   make build APP CATEGORY=STANDALONE PROJECT=PERIPHERAL              (one-off override)
# ============================================================

ROOT       := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
BUILD_DIR  := $(ROOT)/build
TOOLCHAIN  := cmake/toolchain-gcc.cmake
SETTINGS_JSON := $(ROOT)/.vscode/settings.json

# ---- Optional machine-local tool overrides (mirrors env.bat) ----
-include env.mk

CMAKE ?= cmake

# NINJA: .vscode/settings.json's cmake.configureSettings.NINJA is checked
# first (same pattern as GCC_PATH/OPENOCD_BIN below); falls back to the
# existing hardcoded "ninja" PATH-lookup default if settings.json doesn't
# have it (e.g. macOS/Linux, where ninja is normally already on PATH).
# Command-line NINJA=/path still overrides either source.
NINJA_FROM_SETTINGS := $(shell grep -v '^[[:space:]]*//' "$(SETTINGS_JSON)" 2>/dev/null | sed -n 's/.*"NINJA"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' | head -n1)
ifneq ($(NINJA_FROM_SETTINGS),)
NINJA ?= $(NINJA_FROM_SETTINGS)
endif
NINJA ?= ninja

# GCC_BIN: .vscode/settings.json's cmake.configureSettings.GCC_PATH is the
# only source now -- no PATH auto-detect fallback (matches OPENOCD_BIN
# below and cmake/toolchain-gcc.cmake's own GCC_PATH: explicit-only, error
# if unset). GCC_BIN=/path on the command line still overrides (ifndef only
# fires when the variable isn't already set). grep -v skips commented-out
# (//) lines first, same reasoning as OPENOCD_BIN's extraction below.
GCC_PATH_FROM_SETTINGS := $(shell grep -v '^[[:space:]]*//' "$(SETTINGS_JSON)" 2>/dev/null | sed -n 's/.*"GCC_PATH"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' | head -n1)
ifndef GCC_BIN
ifneq ($(GCC_PATH_FROM_SETTINGS),)
GCC_BIN := $(GCC_PATH_FROM_SETTINGS)
else
$(error GCC_BIN is not set. Set it in .vscode/settings.json's cmake.configureSettings.GCC_PATH, or pass GCC_BIN=/path/to/bin on the make command line)
endif
endif

# SDK_ROOT: .vscode/settings.json's cmake.configureSettings.SDK_ROOT is the
# only source -- a single, relative path (resolved against .vscode/'s own
# location) to whichever N32WB03x SDK tree the whole repo currently builds
# against. No V133/V200 shorthand or command-line override -- switch SDK
# versions by editing that one settings.json value. If unset, SDK_ROOT stays
# empty and CMakeLists.txt's own FATAL_ERROR catches it at configure time
# (single source of truth for the error, same reasoning as GCC_BIN above).
SDK_ROOT_FROM_SETTINGS := $(shell grep -v '^[[:space:]]*//' "$(SETTINGS_JSON)" 2>/dev/null | sed -n 's/.*"SDK_ROOT"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' | head -n1)
ifneq ($(SDK_ROOT_FROM_SETTINGS),)
SDK_ROOT := $(abspath $(ROOT)/.vscode/$(SDK_ROOT_FROM_SETTINGS))
endif

# Phase 1 (old flat app_* layout) removed -- both lists intentionally empty.
# Every APPS/APPS_APP1-driven construct below (APP_RULES, FLASH_RULES,
# FLASH_FULL_RULES, the two-word alias validation, the all/size loops)
# degrades to a no-op with nothing in these lists.
APPS :=
APPS_APP1 :=

ELFSIZE  := $(ROOT)/tools/elfsize.sh
FLASH_SH := $(ROOT)/tools/flash.sh

# ---- Two-word verb alias (make <verb> <app>) --------------------------------
# `make clean|rebuild <app>` is accepted as an alias for
# clean-<app>/rebuild-<app>. Only recognized when exactly two words are
# given, so e.g. `make all BUILD=debug` (a variable assignment, not a second
# goal) is unaffected. `build`/`flash` are deliberately EXCLUDED from this
# filter -- those two verbs now own the 2-word slot for the Projects/ tree's
# own `make build|flash INSTALLER|APP|ALL` syntax below; keeping them here
# would collide (e.g. `make build INSTALLER` would otherwise be misread as
# "build the app named INSTALLER").
ifeq ($(words $(MAKECMDGOALS)),2)
ALIAS_VERB := $(firstword $(MAKECMDGOALS))
ALIAS_ARG  := $(lastword $(MAKECMDGOALS))
ifneq ($(filter $(ALIAS_VERB),clean rebuild),)
ifeq ($(filter $(ALIAS_ARG),$(APPS)),)
$(error Unknown app '$(ALIAS_ARG)' for '$(ALIAS_VERB)' -- run 'make help' for the list of apps)
endif
endif
endif

# ---- Projects/ OTA tree (make build|flash INSTALLER|APP|ALL) ----------------
# make build INSTALLER|APP|ALL
# make flash INSTALLER|APP|ALL
# BOARD/CATEGORY/PROJECT select which project -- .vscode/settings.json's
# cmake.configureSettings.BOARD/CATEGORY/PROJECT is the only source (no
# hardcoded default; command-line BOARD=/CATEGORY=/PROJECT= still overrides).
#
# Fully separate from the old-structure APPS/build/ tree above: separate
# build-projects/ output directory (never touched by `make clean`), separate
# CMake target-name convention (<Project>_<INSTALLER|APP>), only recognized
# when exactly 2 words are given so it can't collide with anything else.
# No board-family auto-detection anymore -- BOARD is now an explicit
# settings.json value, so PROJ_REL_DIR is constructed directly
# (Projects/$(BOARD)/Applications/$(CATEGORY)/$(PROJECT)/<TARGET>) instead of
# globbing across every Projects/*/Applications/ (the old approach, back when
# no board-family argument existed anywhere and <CATEGORY>/<Project> had to
# stay unique across board families for the glob to resolve unambiguously --
# no longer a concern now that BOARD is explicit).
PROJECTS_ROOT := $(ROOT)/Projects/*/Applications
BOARD_FROM_SETTINGS    := $(shell grep -v '^[[:space:]]*//' "$(SETTINGS_JSON)" 2>/dev/null | sed -n 's/.*"BOARD"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' | head -n1)
CATEGORY_FROM_SETTINGS := $(shell grep -v '^[[:space:]]*//' "$(SETTINGS_JSON)" 2>/dev/null | sed -n 's/.*"CATEGORY"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' | head -n1)
PROJECT_FROM_SETTINGS  := $(shell grep -v '^[[:space:]]*//' "$(SETTINGS_JSON)" 2>/dev/null | sed -n 's/.*"PROJECT"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' | head -n1)
ifneq ($(BOARD_FROM_SETTINGS),)
BOARD ?= $(BOARD_FROM_SETTINGS)
endif
ifneq ($(CATEGORY_FROM_SETTINGS),)
CATEGORY ?= $(CATEGORY_FROM_SETTINGS)
endif
ifneq ($(PROJECT_FROM_SETTINGS),)
PROJECT ?= $(PROJECT_FROM_SETTINGS)
endif
ifeq ($(words $(MAKECMDGOALS)),2)
PROJ_VERB   := $(word 1,$(MAKECMDGOALS))
PROJ_TARGET := $(word 2,$(MAKECMDGOALS))
ifneq ($(filter $(PROJ_VERB),build flash),)
ifeq ($(filter $(PROJ_TARGET),INSTALLER APP ALL),)
$(error Unknown target '$(PROJ_TARGET)' (use INSTALLER, APP, or ALL))
endif
ifeq ($(BOARD)$(CATEGORY)$(PROJECT),)
$(error BOARD/CATEGORY/PROJECT not set. Set them in .vscode/settings.json's cmake.configureSettings (BOARD, CATEGORY, PROJECT) -- run 'make list-projects' for legal values -- or pass e.g. CATEGORY=BLE PROJECT=LORAWAN on the make command line)
endif
PROJ_REL_DIR_BASE := Projects/$(BOARD)/Applications/$(CATEGORY)/$(PROJECT)
ifeq ($(wildcard $(ROOT)/$(PROJ_REL_DIR_BASE)),)
$(error No project found at Projects/$(BOARD)/Applications/$(CATEGORY)/$(PROJECT) -- check BOARD/CATEGORY/PROJECT in .vscode/settings.json (currently BOARD=$(BOARD) CATEGORY=$(CATEGORY) PROJECT=$(PROJECT)); run 'make list-projects' for legal values)
endif
# Read the real CMake target name out of the project's own CMakeLists.txt
# (its add_executable() call) instead of assuming a fixed
# <Project>_<TARGET> naming convention. Needed because a <Project> name
# (e.g. "LORAWAN") can exist under more than one <CATEGORY>, and CMake
# target names must be globally unique across the whole build (every
# project's subdirectories are add_subdirectory()'d into one top-level
# CMakeLists.txt) -- so a same-named sibling project's own CMakeLists.txt
# is forced to pick a distinct target name (e.g. LORAWAN_STANDALONE_APP),
# which the old $(PROJ_NAME)_$(PROJ_TARGET) guess could not have derived.
# The sed script is stashed in its own plain variable (not inlined) because
# its unescaped literal '(' / ')' characters would otherwise unbalance
# Make's own paren-depth scan of the $(shell ...) call below.
CMAKE_TARGET_SED := s/^[[:space:]]*add_executable([[:space:]]*\([A-Za-z0-9_]*\).*/\1/p
# ALL has no CMakeLists.txt of its own -- build:/_flash_project's ALL
# branches each resolve both INSTALLER's and APP's target names themselves.
ifneq ($(PROJ_TARGET),ALL)
PROJ_REL_DIR := $(PROJ_REL_DIR_BASE)/$(PROJ_TARGET)
ifeq ($(wildcard $(ROOT)/$(PROJ_REL_DIR)),)
$(error No project found matching $(PROJ_REL_DIR))
endif
PROJ_CMAKE_TARGET := $(shell sed -n '$(CMAKE_TARGET_SED)' "$(ROOT)/$(PROJ_REL_DIR)/CMakeLists.txt" 2>/dev/null | head -n1)
ifeq ($(PROJ_CMAKE_TARGET),)
$(error Could not find add_executable() target name in $(PROJ_REL_DIR)/CMakeLists.txt)
endif
endif
# Swallow the extra positional word so Make doesn't try to build it as a goal.
$(PROJ_TARGET): ; @:
endif
endif

# BUILD: .vscode/settings.json's cmake.configureSettings.BUILD is checked
# first (same as SDK_ROOT/BOARD/CATEGORY/PROJECT above); falls back to the
# existing hardcoded "release" default if settings.json doesn't have it.
# Command-line BUILD=debug still overrides either source.
BUILD_FROM_SETTINGS := $(shell grep -v '^[[:space:]]*//' "$(SETTINGS_JSON)" 2>/dev/null | sed -n 's/.*"BUILD"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' | head -n1)
ifneq ($(BUILD_FROM_SETTINGS),)
BUILD ?= $(BUILD_FROM_SETTINGS)
endif
BUILD ?= release
ifeq ($(filter $(BUILD),release debug),)
$(error Unknown BUILD: $(BUILD) (use release or debug))
endif
ifeq ($(BUILD),debug)
CMAKE_BUILD_TYPE := Debug
else
CMAKE_BUILD_TYPE := Release
endif
# Same directory as BUILD_DIR (build/) -- Phase 1's app_* layout is gone, so
# there's no longer a reason to keep two build trees. No per-BUILD (release/
# debug) subfolder either: the CMake configure below always runs with
# --fresh regardless of BUILD, so nothing was gained by caching release and
# debug in separate directories. This also matches the path
# .vscode/c_cpp_properties.json's compileCommands already expects
# (build/compile_commands.json), so IntelliSense picks up the Projects/
# tree correctly instead of pointing at a stale/empty location.
BUILD_PROJECTS_DIR := $(ROOT)/build
FLASH_PROJECTS_SH  := $(ROOT)/tools/flash_projects.sh

# ---- OpenOCD (for `make flash-<app>`) ----
# .vscode/settings.json's cmake.configureSettings.OPENOCD_BIN is the only
# source now -- no xPack-glob/bare-openocd auto-detect (removed, matching
# cmake/toolchain-gcc.cmake's GCC_PATH: explicit-only, error if unset).
# OPENOCD=/path on the command line still overrides (?= only assigns when
# the variable isn't already set). The "error if unset" check is
# deliberately NOT unconditional at the top level here (unlike GCC_PATH in
# the toolchain file) -- OpenOCD is only needed for flashing, never for
# building, so an unconditional check would break every `make` command
# (build, help, list-projects, ...) whenever it's unset. Instead
# check-openocd (below) is a prerequisite only the actual flash targets
# depend on. (SETTINGS_JSON itself is defined near ROOT/BUILD_DIR above,
# shared with GCC_BIN's own settings.json read.)
# grep -v skips commented-out (//) lines first -- otherwise a commented-out
# placeholder like //"OPENOCD_BIN": "/path/to/openocd" would still match the
# sed pattern below and get treated as a real value.
OPENOCD_FROM_SETTINGS := $(shell grep -v '^[[:space:]]*//' "$(SETTINGS_JSON)" 2>/dev/null | sed -n 's/.*"OPENOCD_BIN"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' | head -n1)
ifneq ($(OPENOCD_FROM_SETTINGS),)
OPENOCD ?= $(OPENOCD_FROM_SETTINGS)
endif

.PHONY: check-openocd
check-openocd:
ifndef OPENOCD
	$(error OPENOCD is not set. Set it in .vscode/settings.json's cmake.configureSettings.OPENOCD_BIN, or pass OPENOCD=/path/to/openocd on the make command line)
endif

# PROBE: .vscode/settings.json's cmake.configureSettings.PROBE is checked
# first (same pattern as BUILD above); falls back to the existing hardcoded
# defaults if settings.json doesn't have it -- old hyphenated flash-<app>
# commands default to stlink (matches flash.bat's proven-working behavior),
# the Projects/ tree's `make flash INSTALLER|APP|ALL` verb defaults to dap
# instead. Command-line PROBE=... still overrides either source.
PROBE_FROM_SETTINGS := $(shell grep -v '^[[:space:]]*//' "$(SETTINGS_JSON)" 2>/dev/null | sed -n 's/.*"PROBE"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' | head -n1)
ifneq ($(PROBE_FROM_SETTINGS),)
PROBE ?= $(PROBE_FROM_SETTINGS)
else ifneq ($(PROJ_VERB),)
PROBE ?= dap
else
PROBE ?= stlink
endif
ifeq ($(PROBE),dap)
OPENOCD_CFG := $(ROOT)/openocd/n32wb031_dap.cfg
else ifeq ($(PROBE),stlink)
OPENOCD_CFG := $(ROOT)/openocd/n32wb031.cfg
else
$(error Unknown PROBE: $(PROBE) (use dap or stlink))
endif

.PHONY: all help config size clean build rebuild flash \
        $(APPS) \
        $(addprefix clean-,$(APPS)) \
        $(addprefix rebuild-,$(APPS)) \
        $(addprefix flash-,$(APPS)) \
        $(foreach app,$(APPS_APP1),flash-$(app)-full)

help:
	@echo ""
	@echo " N32WB031 GCC Build (macOS/Linux)"
	@echo " --------------------------------"
	@echo " make config                        CMake configure only"
	@echo " make clean                         Remove the whole build/ directory"
	@echo " make size                          Show Flash / RAM usage (no build)"
	@echo " make help                          Show this help"
	@echo ""
	@echo " Targets: (none -- Phase 1 app_* layout removed, see Projects/ tree below)"
	@echo ""
	@echo " SDK path: .vscode/settings.json's cmake.configureSettings.SDK_ROOT --"
	@echo " single value for the whole repo, no command-line selector. Switch SDK"
	@echo " versions by editing that one value and rebuilding."
	@echo ""
	@echo " Examples:"
	@echo "   make clean"
	@echo "   make size"
	@echo ""
	@echo " Projects/ OTA tree (own CMake target-name convention <Project>_<INSTALLER|APP>,"
	@echo " output dir build/ -- same directory 'make clean' already removes):"
	@echo "   make build INSTALLER|APP|ALL"
	@echo "   make flash INSTALLER|APP|ALL"
	@echo "   make list-projects              List every <CATEGORY> <Project> <TARGET>"
	@echo "                                    (legal values for settings.json below)"
	@echo "   make clean-projects             Remove the whole build/ directory (alias for 'make clean')"
	@echo "   BOARD/CATEGORY/PROJECT (.vscode/settings.json's cmake.configureSettings)"
	@echo "   select the literal Projects/<BOARD>/Applications/<CATEGORY>/<PROJECT> path"
	@echo "   -- no auto-detect, no CLI argument. BUILD defaults release, PROBE defaults"
	@echo "   dap (also settings.json-sourced; differs from the flash-<app> commands"
	@echo "   above, which default PROBE=stlink). ALL builds/flashes INSTALLER then APP"
	@echo "   ('flash ALL' additionally does a chip erase first). Every value can still"
	@echo "   be overridden per-invocation on the command line."
	@echo ""
	@echo " Examples (Projects/ tree):"
	@echo "   make build INSTALLER                                        (settings.json project)"
	@echo "   make build APP BUILD=debug"
	@echo "   make build ALL"
	@echo "   make flash INSTALLER                                        (dap/release default)"
	@echo "   make flash INSTALLER PROBE=dap BUILD=release"
	@echo "   make flash INSTALLER PROBE=stlink BUILD=debug"
	@echo "   make flash ALL                                              (chip erase + both)"
	@echo "   make build APP CATEGORY=STANDALONE PROJECT=PERIPHERAL       (one-off override)"
	@echo "   make list-projects"
	@echo ""
	@echo " Tool paths:"
	@echo "   cmake  : $(CMAKE)"
	@echo "   ninja  : $(NINJA)"
	@echo "   gcc    : $(GCC_BIN)"
	@echo "   openocd: $(OPENOCD)  (probe=$(PROBE), cfg=$(OPENOCD_CFG))"
	@echo ""

build:
ifneq ($(PROJ_VERB),)
	@echo "[SELECT] BOARD=$(BOARD) CATEGORY=$(CATEGORY) PROJECT=$(PROJECT)"
ifeq ($(PROJ_TARGET),ALL)
	@INST_TARGET=$$(sed -n 's/^[[:space:]]*add_executable([[:space:]]*\([A-Za-z0-9_]*\).*/\1/p' "$(ROOT)/$(PROJ_REL_DIR_BASE)/INSTALLER/CMakeLists.txt" | head -n1); \
	APP_TARGET=$$(sed -n 's/^[[:space:]]*add_executable([[:space:]]*\([A-Za-z0-9_]*\).*/\1/p' "$(ROOT)/$(PROJ_REL_DIR_BASE)/APP/CMakeLists.txt" | head -n1); \
	$(MAKE) --no-print-directory _build_project PROJ_REL_DIR="$(PROJ_REL_DIR_BASE)/INSTALLER" PROJ_TARGET=INSTALLER PROJ_CMAKE_TARGET="$$INST_TARGET" SDK_ROOT="$(SDK_ROOT)" BUILD="$(BUILD)" && \
	$(MAKE) --no-print-directory _build_project PROJ_REL_DIR="$(PROJ_REL_DIR_BASE)/APP" PROJ_TARGET=APP PROJ_CMAKE_TARGET="$$APP_TARGET" SDK_ROOT="$(SDK_ROOT)" BUILD="$(BUILD)"
else
	@$(MAKE) --no-print-directory _build_project \
		PROJ_REL_DIR="$(PROJ_REL_DIR)" SDK_ROOT="$(SDK_ROOT)" \
		PROJ_CMAKE_TARGET="$(PROJ_CMAKE_TARGET)" BUILD="$(BUILD)"
endif
else ifeq ($(ALIAS_ARG),)
	$(error Usage: make build <app>  (or 'make <app>' directly, or 'make all')  |  make build INSTALLER|APP|ALL)
else
	@: # goal 2 (the app name) already builds it for real via its own recipe
endif

rebuild:
ifeq ($(ALIAS_ARG),)
	$(error Usage: make rebuild <app>  (or 'make rebuild-<app>' directly))
endif
	@$(MAKE) rebuild-$(ALIAS_ARG)

flash:
ifneq ($(PROJ_VERB),)
	@echo "[SELECT] BOARD=$(BOARD) CATEGORY=$(CATEGORY) PROJECT=$(PROJECT)"
	@$(MAKE) --no-print-directory _flash_project \
		PROJ_TARGET="$(PROJ_TARGET)" \
		PROJ_REL_DIR="$(PROJ_REL_DIR)" PROJ_REL_DIR_BASE="$(PROJ_REL_DIR_BASE)" \
		PROJ_CMAKE_TARGET="$(PROJ_CMAKE_TARGET)" BUILD="$(BUILD)" PROBE="$(PROBE)"
else ifeq ($(ALIAS_ARG),)
	$(error Usage: make flash <app>  (or 'make flash-<app>' directly)  |  make flash INSTALLER|APP|ALL)
else
	@$(MAKE) flash-$(ALIAS_ARG)
endif

config:
	@echo ""
	@echo "[CONFIG] CMake configure (SDK_ROOT=$(SDK_ROOT))..."
	"$(CMAKE)" -S "$(ROOT)" -B "$(BUILD_DIR)" \
		-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN) \
		-DCMAKE_MAKE_PROGRAM="$(NINJA)" \
		-DGCC_PATH="$(GCC_BIN)" \
		-DN32WB03X_SDK_ROOT="$(SDK_ROOT)" \
		-G Ninja -Wno-dev --fresh

# ---- Per-app linker script mapping (for size reporting) --------------------
linker_script = $(ROOT)/$(1)/linker/$(1)_gcc.ld

define PRINT_SIZE
	@echo ""
	@echo " ============================================================"
	@echo "  Memory Usage"
	@echo " ============================================================"
	@"$(ELFSIZE)" "$(BUILD_DIR)/$(1)/$(1).elf" "$(call linker_script,$(1))" $(1)
	@echo " ============================================================"
endef

# ---- Per-app build/clean/rebuild rules --------------------------------------
# $(1)'s recipe is a no-op when this invocation is really just the swallowed
# second goal of `make clean|rebuild|flash <app>` (the two-word alias above)
# -- otherwise `make clean app_x` would also rebuild app_x right after
# cleaning it. `config` is called explicitly here (not a hard prerequisite)
# so that no-op case also skips the cmake reconfigure.
define APP_RULES
.PHONY: $(1)
$(1):
ifeq ($$(ALIAS_VERB)/$$(ALIAS_ARG),clean/$(1))
else ifeq ($$(ALIAS_VERB)/$$(ALIAS_ARG),rebuild/$(1))
else ifeq ($$(ALIAS_VERB)/$$(ALIAS_ARG),flash/$(1))
else
	@$(MAKE) --no-print-directory config
	@echo ""
	@echo "[BUILD] $(1) ..."
	"$(CMAKE)" --build "$(BUILD_DIR)" --target $(1)
	$$(call PRINT_SIZE,$(1))
endif

.PHONY: clean-$(1)
clean-$(1):
	@echo "[CLEAN] Removing $(BUILD_DIR)/$(1)"
	@if [ -d "$(BUILD_DIR)/$(1)" ]; then rm -rf "$(BUILD_DIR)/$(1)"; echo "[CLEAN] Done."; else echo "[CLEAN] Nothing to clean."; fi

.PHONY: rebuild-$(1)
rebuild-$(1): clean-$(1) $(1)
endef

$(foreach app,$(APPS),$(eval $(call APP_RULES,$(app))))

# ---- Per-app flash rules ----------------------------------------------------
# flash-<app>: flash just <app>'s own FLASH region (assumes app_installer is
#              already on-chip). flash-<app>-full (APPS_APP1 only): chip
#              erase + app_installer + <app>, for a blank/first-time chip.
# Address/size come from <app>'s own linker script -- see tools/flash.sh.
define FLASH_RULES
.PHONY: flash-$(1)
flash-$(1): check-openocd
	@"$(FLASH_SH)" plain $(1) "$(ROOT)" "$(BUILD_DIR)" "$(OPENOCD)" "$(OPENOCD_CFG)"
endef

$(foreach app,$(APPS),$(eval $(call FLASH_RULES,$(app))))

define FLASH_FULL_RULES
.PHONY: flash-$(1)-full
flash-$(1)-full: check-openocd
	@"$(FLASH_SH)" full $(1) "$(ROOT)" "$(BUILD_DIR)" "$(OPENOCD)" "$(OPENOCD_CFG)"
endef

$(foreach app,$(APPS_APP1),$(eval $(call FLASH_FULL_RULES,$(app))))

all: $(APPS)
	@echo ""
	@echo " ============================================================"
	@echo "  Memory Usage"
	@echo " ============================================================"
	@$(foreach app,$(APPS),"$(ELFSIZE)" "$(BUILD_DIR)/$(app)/$(app).elf" "$(call linker_script,$(app))" $(app);)
	@echo " ============================================================"

clean:
ifneq ($(ALIAS_ARG),)
	@$(MAKE) --no-print-directory clean-$(ALIAS_ARG)
else
	@echo "[CLEAN] Removing $(BUILD_DIR)"
	@if [ -d "$(BUILD_DIR)" ]; then rm -rf "$(BUILD_DIR)"; echo "[CLEAN] Done."; else echo "[CLEAN] Nothing to clean."; fi
endif

size:
	@echo ""
	@echo " ============================================================"
	@echo "  Memory Usage (BOARD=$(BOARD) CATEGORY=$(CATEGORY) PROJECT=$(PROJECT) SDK_ROOT=$(SDK_ROOT))"
	@echo " ============================================================"
	@$(foreach app,$(APPS),"$(ELFSIZE)" "$(BUILD_DIR)/$(app)/$(app).elf" "$(call linker_script,$(app))" $(app);)
	@echo " ============================================================"

# ---- Projects/ OTA tree: build/flash/list implementation --------------------
.PHONY: _build_project _flash_project list-projects clean-projects

_build_project:
	@test -f "$(ROOT)/$(PROJ_REL_DIR)/CMakeLists.txt" || { echo "[ERROR] $(PROJ_REL_DIR)/CMakeLists.txt not found -- $(PROJ_TARGET) not implemented yet"; exit 1; }
	@echo ""
	@echo "[CONFIG] CMake configure (SDK_ROOT=$(SDK_ROOT), BUILD=$(BUILD))..."
	"$(CMAKE)" -S "$(ROOT)" -B "$(BUILD_PROJECTS_DIR)" \
		-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN) \
		-DCMAKE_MAKE_PROGRAM="$(NINJA)" \
		-DGCC_PATH="$(GCC_BIN)" \
		-DN32WB03X_SDK_ROOT="$(SDK_ROOT)" \
		-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
		-G Ninja -Wno-dev --fresh
	@echo ""
	@echo "[BUILD] $(PROJ_CMAKE_TARGET) ($(BUILD)) ..."
	"$(CMAKE)" --build "$(BUILD_PROJECTS_DIR)" --target $(PROJ_CMAKE_TARGET)
	@LD=$$(find "$(ROOT)/$(PROJ_REL_DIR)/Linker" -maxdepth 1 -name '*.ld' 2>/dev/null | head -n1); \
	ELF="$(BUILD_PROJECTS_DIR)/$(PROJ_REL_DIR)/$(PROJ_CMAKE_TARGET).elf"; \
	if [ -n "$$LD" ] && [ -f "$$ELF" ]; then \
		echo ""; \
		echo " ============================================================"; \
		echo "  Memory Usage"; \
		echo " ============================================================"; \
		"$(ELFSIZE)" "$$ELF" "$$LD" $(PROJ_CMAKE_TARGET); \
		echo " ============================================================"; \
	fi

_flash_project: check-openocd
ifeq ($(PROJ_TARGET),ALL)
	@INST_TARGET=$$(sed -n 's/^[[:space:]]*add_executable([[:space:]]*\([A-Za-z0-9_]*\).*/\1/p' "$(ROOT)/$(PROJ_REL_DIR_BASE)/INSTALLER/CMakeLists.txt" | head -n1); \
	APP_TARGET=$$(sed -n 's/^[[:space:]]*add_executable([[:space:]]*\([A-Za-z0-9_]*\).*/\1/p' "$(ROOT)/$(PROJ_REL_DIR_BASE)/APP/CMakeLists.txt" | head -n1); \
	INST_BIN="$(BUILD_PROJECTS_DIR)/$(PROJ_REL_DIR_BASE)/INSTALLER/$${INST_TARGET}.bin"; \
	APP_BIN="$(BUILD_PROJECTS_DIR)/$(PROJ_REL_DIR_BASE)/APP/$${APP_TARGET}.bin"; \
	INST_LD=$$(find "$(ROOT)/$(PROJ_REL_DIR_BASE)/INSTALLER/Linker" -maxdepth 1 -name '*.ld' 2>/dev/null | head -n1); \
	APP_LD=$$(find "$(ROOT)/$(PROJ_REL_DIR_BASE)/APP/Linker" -maxdepth 1 -name '*.ld' 2>/dev/null | head -n1); \
	"$(FLASH_PROJECTS_SH)" all "$$INST_BIN" "$$INST_LD" "$$APP_BIN" "$$APP_LD" "$(OPENOCD)" "$(OPENOCD_CFG)" "$(ROOT)"
else
	@BIN="$(BUILD_PROJECTS_DIR)/$(PROJ_REL_DIR)/$(PROJ_CMAKE_TARGET).bin"; \
	LD=$$(find "$(ROOT)/$(PROJ_REL_DIR)/Linker" -maxdepth 1 -name '*.ld' 2>/dev/null | head -n1); \
	"$(FLASH_PROJECTS_SH)" plain "$$BIN" "$$LD" "$(OPENOCD)" "$(OPENOCD_CFG)" "$(ROOT)"
endif

# Enumerate every <CATEGORY> <Project> <TARGET> triple with a CMakeLists.txt
# under Projects/<BoardFamily>/Applications/ -- e.g. "BLE BEACON INSTALLER".
# Board family is appended for information only -- the CLI itself never
# takes it as an argument (see PROJECTS_ROOT comment above).
list-projects:
	@for f in $(PROJECTS_ROOT)/*/*/*/CMakeLists.txt; do \
		[ -f "$$f" ] || continue; \
		t=$$(basename $$(dirname $$f)); \
		p=$$(basename $$(dirname $$(dirname $$f))); \
		c=$$(basename $$(dirname $$(dirname $$(dirname $$f)))); \
		bf=$$(basename $$(dirname $$(dirname $$(dirname $$(dirname $$(dirname $$f)))))); \
		echo "$$c $$p $$t   [$$bf]"; \
	done

# Alias for `make clean` -- build-projects/ was merged into build/, so
# there's no longer a separate tree for this to remove on its own.
clean-projects: clean
