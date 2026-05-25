# Changelog

All notable firmware changes are documented in this file.

## [3.4.16] - 2026-05-26

- Implemented build-time GEN feature gating with `ENABLE_GEN_MODE` in `scope_config.h`.
- Added compile-time guards across main loop, mode setup, UI controls/rendering, settings validation, and hardware detection so GEN can be fully compiled out.
- Updated `SCOPEv2_modules.cpp` so `mode_gen`, `dsp_generator`, and `hal_dac` modules are excluded from the build when GEN is disabled.
- Set default to slim build (`ENABLE_GEN_MODE=0`). Set `ENABLE_GEN_MODE=1` to include GEN again.
- Updated SCOPEv2 version strings to `3.4.16` for patch-level flash verification.

## [3.4.15] - 2026-05-26

- Added the built-in smiley glyph (`0x02`) to the SCOPEv2 MODE/LFO 3x banner label in the temporary large overlay.
- Updated SCOPEv2 version strings to `3.4.15` for patch-level flash verification.

## [3.4.14] - 2026-05-26

- Updated SCOPEv2 2x temporary parameter overlay labels to fixed space-padded strings for alignment:
  - `" OFFSET " + value`
  - `"  RANGE " + value`
  - `"   TIME " + value`
- Kept existing MODE 3x black banner behavior and 1-second overlay timing.
- Updated SCOPEv2 version strings to `3.4.14` for patch-level flash verification.

## [3.4.13] - 2026-05-26

- Changed the MODE overlay in SCOPEv2 to a 3x banner with a full black background, left padding, and no "MODE" prefix text.
- Kept the other temporary parameter overlays as the existing 1-second transparent text line.
- Updated SCOPEv2 version strings to `3.4.13` for patch-level flash verification.

## [3.4.12] - 2026-05-26

- Reduced the temporary large parameter overlay lifetime from about 3 seconds to about 1 second.
- Changed the overlay to a single left-padded line and removed the explicit black background fill so the scope trace can show through underneath while the text is visible.
- Updated SCOPEv2 version strings to `3.4.12` for patch-level flash verification.

## [3.4.11] - 2026-05-25

- Reduced SCOPEv2 UI flash usage in the temporary large-parameter overlay path by removing buffer/`snprintf`-based formatting and rendering label/value directly.
- Preserved behavior: compact top row unchanged, left-aligned large overlay below it, auto-hide after about 3 seconds after value change.
- Updated SCOPEv2 version strings to `3.4.11` for patch-level flash verification.

## [3.4.10] - 2026-05-25

- Changed the temporary large parameter overlay text to left alignment (label and value) in SCOPEv2.
- Removed centering helper/alignment math from the overlay renderer to trim code size while preserving the 3-second temporary display behavior.
- Updated SCOPEv2 version strings to `3.4.10` for patch-level flash verification.

## [3.4.9] - 2026-05-25

- Implemented Option A large parameter overlay using classic Adafruit_GFX text scaling (`setTextSize(2)`) in SCOPEv2 parameter UI.
- Kept the existing compact top parameter row unchanged.
- Added temporary large active parameter label/value rendering below the top row that appears after a parameter value change and auto-hides after about 3 seconds.
- Updated SCOPEv2 version strings to `3.4.9` for patch-level flash verification.

## [3.4.8] - 2026-05-25

- Fixed the remaining module-relative include paths after moving SCOPEv2 sources into foldered modules.
- Ensured subfolder sources reference shared headers through the correct `../` paths so the Arduino preprocessor can resolve them when compiling the root module aggregator.
- Updated SCOPEv2 version strings to `3.4.8` for patch-level flash verification.

## [3.4.7] - 2026-05-25

- Added `Firmware/SCOPEv2/SCOPEv2_modules.cpp` as a root-level compilation unit that includes the foldered module `.cpp` files, so Arduino IDE links the refactored code correctly.
- Preserved the foldered module organization while restoring build completeness in Arduino's non-recursive sketch compilation model.
- Updated SCOPEv2 version strings to `3.4.7` for patch-level flash verification.

## [3.4.6] - 2026-05-25

- Fixed module-folder include resolution so Arduino can locate headers when files live under `core/`, `hal/`, `dsp/`, `mode/`, `ui/`, and `app/`.
- Converted module-internal includes to sibling-relative paths while keeping the root sketch’s folder-prefixed imports intact.
- Updated SCOPEv2 version strings to `3.4.6` for patch-level flash verification.

## [3.4.5] - 2026-05-25

- Reorganized `Firmware/SCOPEv2` source files into module folders for better Arduino project structure:
  - `core/` for shared config/state/types
  - `hal/` for hardware abstraction
  - `dsp/` for signal processing and formatting helpers
  - `mode/` for mode implementations
  - `ui/` for menu/control rendering and interaction
  - `app/` for app-level orchestration/settings modules
- Updated all local include paths to use folder-based module includes.
- Updated SCOPEv2 version strings to `3.4.5` for patch-level flash verification.

## [3.4.4] - 2026-05-25

- Fixed Arduino compile/verify failures introduced by state aliasing macros in the refactor.
- Replaced macro-based aliases in `scope_state.h` with typed `extern` references defined in `scope_state.cpp` to avoid token collisions (e.g. `modeSettings[idx].param1` and `display->display()`).
- Updated SCOPEv2 version strings to `3.4.4` for patch-level flash verification.

## [3.4.3] - 2026-05-25

- Implemented the SCOPEv2 multi-file refactor from the plan:
  - split hardware layers into `hal_adc`, `hal_dac`, `hal_display`, and `hal_hw` modules
  - split DSP/helpers into `dsp_tuner`, `dsp_generator`, and `util_format`
  - split app/UI/mode logic into `storage_settings`, `ui_controls`, `ui_config_menu`, `ui_parameter_bar`, `app_mode_manager`, and per-mode files
  - introduced shared runtime state container in `scope_state.h/.cpp`
- Replaced `Firmware/SCOPEv2/SCOPEv2.ino` with a thin orchestration entrypoint (`setup`/`loop`) that dispatches into modules.
- Updated SCOPEv2 version strings to `3.4.3` for patch-level flash verification.

## [3.4.2] - 2026-05-25

- Added [REFACTOR_PLAN.md](REFACTOR_PLAN.md), a detailed, agent-executable plan to split `Firmware/SCOPEv2/SCOPEv2.ino` into modular files with compile checkpoints.
- Defined module seams, file structure, migration phases, risk mitigations, and per-phase verification steps.

## [3.4.1] - 2026-05-25

- Added a visible underline cursor in the top parameter bar during menu navigation mode (`param_select == 0`).
- The currently navigated slot is now underlined when it is not actively highlighted/in edit mode.
- Updated SCOPEv2 firmware version strings from `3.4` / `v3.4x` to `3.4.1` / `v3.4.1` for patch-level verification after flashing.
