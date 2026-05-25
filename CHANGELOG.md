# Changelog

All notable firmware changes are documented in this file.

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
