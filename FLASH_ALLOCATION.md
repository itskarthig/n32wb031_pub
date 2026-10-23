# FLASH_ALLOCATION.md — N32WB031 flash memory map

Canonical, repo-root reference for how this chip's flash is partitioned — kept here once instead of duplicated across every project's `user_config.h`/`app.ld`/`installer.ld`/`main.c` doc comments, which only ever carry fragments of the full picture. Update this file alongside those comments whenever an address changes.

This repo (`n32wb031_pub`) carries only `P2PS_OTA` (both board families), `BEACON`, and `STANDALONE/PERIPHERAL` — no LoRaWAN projects exist here, so the `LORAWAN_NVM_BASE_ADDRESS` region documented in the main `n32wb031_gcc` repo's copy of this file doesn't apply to anything in this repo.

## Chip overview

Target part: **N32WB031KEQ6-2** (512 KB flash variant — the 256 KB variant, N32WB031KCQ6-1, has no Bank B at all and cannot run this repo's FUOTA design). Source: `N32WB03x_Docs/N32WB031_Complete_Reference.md`.

Flash spans `0x01000000`–`0x0107FFFF` (512 KB total), split into two 256 KB banks:

| Bank | Address range | Mode |
|---|---|---|
| **Bank A** | `0x01000000`–`0x0103FFFF` | XIP — code executes directly from here |
| **Bank B** | `0x01040000`–`0x0107FFFF` | non-XIP — any access (even a read) requires exiting XIP mode first |

## Bank A — 256 KB, fully allocated

| Region | Address | Size | Define |
|---|---|---|---|
| INSTALLER | `0x01000000` | 8 KB | — |
| Install flag | `0x01002000` | 4 KB | `INSTALL_FLAG_ADDR` |
| `userConfig_t` *(`P2PS_OTA` only — see "Per-project applicability")* | `0x01003000` | 4 KB | `USER_CFG_FLASH_ADDR` |
| APP (running application) | `0x01004000` | 240 KB | `FUOTA_APP1_ADDR` |

8 + 4 + 4 + 240 = 256 KB — no headroom left (in `P2PS_OTA`; `BEACON`/`STANDALONE/PERIPHERAL` have no `userConfig_t`, so their `0x01003000` 4 KB sits unused — see below).

## Bank B — 256 KB

| Region | Address | Size | Define |
|---|---|---|---|
| FUOTA download slot *(`P2PS_OTA` only, confirmed — `BEACON`/`STANDALONE/PERIPHERAL` have no `app_fuota.h` in this repo's current snapshot)* | `0x01040000` | 240 KB | `FUOTA_APP2_ADDR` / `FUOTA_DL_ADDR` |
| *(unused)* | `0x0107C000` | 16 KB | — |

No project in this repo uses `0x0107E000` for anything (no LoRaWAN NVM here) — the 16 KB tail of Bank B is free across the board.

## Per-project applicability

| Project | `USER_CFG_FLASH_ADDR` | FUOTA slot? |
|---|---|---|
| `BLE/P2PS_OTA` (`N32WB03x_CUSTOM_BOARD`) | `0x01003000` (Bank A) | yes |
| `BLE/P2PS_OTA` (`N32WB03x_EVAL`) | `0x01003000` (Bank A) | yes |
| `BLE/BEACON` (`N32WB03x_EVAL`) | no `user_config.h` | no (no `app_fuota.h` in this repo) |
| `STANDALONE/PERIPHERAL` (`N32WB03x_EVAL`) | no `user_config.h` | no |

**Flagged inconsistency, found while writing this doc, not fixed**: `BEACON` and `STANDALONE/PERIPHERAL` both still carry a `USER_CFG (r) : ORIGIN = 0x0107E000, LENGTH = 8K` region in their `app.ld` `MEMORY` block, despite having no `user_config.h` anywhere to actually use it — vestigial boilerplate, almost certainly left over from being forked from the same original template as `P2PS_OTA` before the "no config struct" divergence was made. Harmless (purely declarative, no `SECTIONS` block references it in any project — confirmed across every linker script checked), but worth cleaning up if these files are touched again.

## Swap history

**2026-10-23 — `USER_CFG_FLASH_ADDR` relocated from `0x0107E000` (Bank B) to `0x01003000` (Bank A) in both `P2PS_OTA` copies.**

- Mirrors the same move applied the same day in the main `n32wb031_gcc` repo's `P2PS_OTA`, for consistency between the two repos' only shared project.
- Not a swap — nothing exists at `0x01003000` to displace, and `0x0107E000` simply becomes free afterward. `USER_CFG_FLASH_SIZE` corrected `0x2000U` (8 KB, vestigial — `User_Config_Load()`/`Save()` always erase exactly one 4 KB sector regardless) → `0x1000U` (4 KB) at the same time.
- **Consequence for already-deployed devices**: a device already flashed with the old address that receives this update will have `userConfig_t` reset to defaults on its first boot after (the existing magic/version check correctly rejects the stale content now found at the new address).
- Not locally build-verified in this repo — `SDK_ROOT` (`.vscode/settings.json`, `../N32WB03x_SDK/N32WB03x_SDK_V2.0.0`) isn't present on this machine relative to this repo's location. The identical edit was build-confirmed clean in the main `n32wb031_gcc` repo first.

## If this needs to change again

| File | What's there |
|---|---|
| `APP/UserApp/inc/user_config.h` | `USER_CFG_FLASH_ADDR` + `USER_CFG_FLASH_SIZE` (the latter is vestigial — never referenced outside its own declaration, but keep in sync for documentation accuracy) |
| `APP/Linker/app.ld` | `MEMORY` block's `USER_CFG` region `ORIGIN`/`LENGTH` — purely declarative, zero effect on the actual build |
| `INSTALLER/Linker/installer.ld` | "Flash layout" header comment block — doc-only |
| `INSTALLER/Core/src/main.c` | Same "Flash layout" header comment block — doc-only |

Both `P2PS_OTA` copies need the same edit (`N32WB03x_CUSTOM_BOARD` and `N32WB03x_EVAL`) — this repo keeps them as two independent trees, not a shared source.
