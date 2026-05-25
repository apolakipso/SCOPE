# Remove GEN Refactor Plan (Build-Time Optional)

## Goal

Remove Function Generator (GEN) functionality from firmware builds while keeping the codebase maintainable and allowing build-time selection:

- `ENABLE_GEN_MODE=1`: current behavior (GEN included)
- `ENABLE_GEN_MODE=0`: GEN fully excluded from build (code and UI paths)

## Short answer

Yes, this can be done cleanly with config flags.

The key is to use one top-level compile-time macro and propagate it through:

1. mode selection constants
2. module includes (`SCOPEv2_modules.cpp`)
3. runtime logic branches (`MODE_GEN` cases)
4. state storage fields that only exist for GEN

## Design

## 1) Add a single compile-time feature flag

File:

- `Firmware/SCOPEv2/core/scope_config.h`

Add:

```cpp
#ifndef ENABLE_GEN_MODE
#define ENABLE_GEN_MODE 1
#endif
```

Also define mode count from that flag:

```cpp
#define MODE_LFO 1
#define MODE_WAVE 2
#define MODE_TUNER 3
#define MODE_GEN 4

#if ENABLE_GEN_MODE
#define NUM_MODES 4
#else
#define NUM_MODES 3
#endif
```

Notes:

- Keep `MODE_GEN` numeric value stable for compatibility with saved EEPROM data.
- `NUM_MODES` controls knob wrap and loops.

## 2) Build-time control points

Prefer `#if ENABLE_GEN_MODE` around includes and GEN-only code blocks.

Primary files:

- `Firmware/SCOPEv2/SCOPEv2.ino`
- `Firmware/SCOPEv2/SCOPEv2_modules.cpp`
- `Firmware/SCOPEv2/app/app_mode_manager.cpp`
- `Firmware/SCOPEv2/ui/ui_controls.cpp`
- `Firmware/SCOPEv2/ui/ui_parameter_bar.cpp`
- `Firmware/SCOPEv2/app/storage_settings.cpp`
- `Firmware/SCOPEv2/hal/hal_hw.cpp`
- `Firmware/SCOPEv2/core/scope_state.h`
- `Firmware/SCOPEv2/core/scope_state.cpp`

## Refactor phases

## Phase 1: Functional disable via flag (safe intermediate)

Objective:

- GEN absent from menu and runtime when `ENABLE_GEN_MODE=0`.

Changes:

1. `SCOPEv2.ino`

- Guard GEN include:
  - `#if ENABLE_GEN_MODE`
  - `#include "mode/mode_gen.h"`
  - `#endif`
- Guard boot splash DAC suffix (`I2C`/`DAC`) under `ENABLE_GEN_MODE`.
- In setup mode restore logic:
  - if saved mode is `MODE_GEN` and GEN disabled, force `MODE_LFO`.
- Guard `case MODE_GEN:` in `loop()`.

2. `ui_controls.cpp`

- Keep current mode wrap logic based on `NUM_MODES`.
- Guard GEN-specific branches in param range logic.
- When GEN disabled, remove/skip `MODE_GEN` branches entirely.

3. `ui_parameter_bar.cpp`

- Guard `dsp_generator.h` include with `#if ENABLE_GEN_MODE`.
- Guard all `MODE_GEN` label/value rendering.
- Ensure slot rendering remains valid for 3 modes.

4. `app_mode_manager.cpp`

- Guard `dsp_generator.h` include and `MODE_GEN` case.
- Ensure non-GEN modes work identically.

5. `storage_settings.cpp`

- Guard `MODE_GEN` sanitization logic.
- Keep mode settings array bounds based on `NUM_MODES`.

Acceptance for Phase 1:

- With `ENABLE_GEN_MODE=0`, no GEN menu entry and no GEN runtime path.
- Builds and behaves correctly for LFO/WAVE/TUNER.

## Phase 2: Binary stripping (remove GEN object code)

Objective:

- Actually reduce flash by excluding GEN modules and DAC-gen paths from compilation.

Changes:

1. `SCOPEv2_modules.cpp`

- Guard module includes:

```cpp
#if ENABLE_GEN_MODE
#include "hal/hal_dac.cpp"
#include "dsp/dsp_generator.cpp"
#include "mode/mode_gen.cpp"
#endif
```

- If `hal_dac.cpp` provides non-GEN-used symbols, split file first:
  - GEN-only DAC code in a dedicated unit
  - keep any shared DAC-independent helpers elsewhere

2. `hal_hw.cpp`

- Guard generator hardware detection path (`scanMCP4725`, `dacIsI2C`, `genAvailable`).
- When GEN disabled:
  - set `genAvailable = false`
  - skip I2C DAC probing for MCP4725

3. `scope_state.h/.cpp`

- Wrap GEN-only fields and references:
  - `genPhase`, `genPhaseInc`, `genAvailable`, `dacIsI2C`, `dacI2CAddr`, `calOffset`, `calGain`, `genSampleRate`, `genSamplePeriodUs`
- Keep any calibration fields that are still used outside GEN (if any) unguarded.

Acceptance for Phase 2:

- `ENABLE_GEN_MODE=0` build compiles without linking `mode_gen.cpp`/`dsp_generator.cpp`.
- Program size measurably decreases.

## Phase 3: Cleanup and simplification

Objective:

- Minimize conditional complexity and retain readability.

Actions:

- Consolidate repeated guards into helper macros where useful.
- Remove stale comments mentioning GEN in no-GEN flow.
- Add brief docs for feature flag usage.

## EEPROM and compatibility

1. Saved mode fallback

- Preserve current fallback behavior:
  - if EEPROM mode is GEN but GEN disabled, load LFO.

2. Per-mode settings layout

- If mode settings array size changes with `NUM_MODES`, verify existing EEPROM parsing remains safe.
- Prefer robust bounds checking over assuming array size from old firmware.

## Build configuration strategy

## Default

- Keep `ENABLE_GEN_MODE=1` by default to preserve current behavior.

## Disable for slim builds

Option A (source-controlled):

- Set in `scope_config.h`:
  - `#define ENABLE_GEN_MODE 0`

Option B (toolchain define, preferred long-term):

- Use compiler flag (`-DENABLE_GEN_MODE=0`) and keep header default to `1`.
- Header should use `#ifndef ENABLE_GEN_MODE` so command-line define overrides it.

## Test matrix

1. `ENABLE_GEN_MODE=1`

- Existing behavior unchanged.
- GEN selectable when `genAvailable` true.

2. `ENABLE_GEN_MODE=0`

- No GEN in menu.
- No GEN case in mode loop.
- No MCP4725 probe for GEN path.
- LFO/WAVE/TUNER parameter controls all work.

3. Upgrade scenario

- Flash from GEN-enabled build to GEN-disabled build with EEPROM containing `MODE_GEN`.
- Confirm startup falls back to LFO without instability.

## Risk list

1. Hidden compile dependencies

- Risk: a GEN symbol still referenced when flag off.
- Mitigation: phase-by-phase compile checkpoints and guard includes first.

2. EEPROM assumptions

- Risk: array/enum mismatch after `NUM_MODES` change.
- Mitigation: strict bounds checks during load.

3. UI branch regressions

- Risk: parameter slot rendering assumes GEN-specific layout.
- Mitigation: verify slot width/render logic for 3-mode builds.

## Suggested implementation order

1. Add `ENABLE_GEN_MODE` in `scope_config.h`.
2. Guard includes and `MODE_GEN` switch cases in app/UI/main.
3. Build-check both flag values.
4. Guard `SCOPEv2_modules.cpp` includes for GEN files.
5. Guard `hal_hw` probing and GEN state fields.
6. Run flash-size comparison and behavioral smoke tests.

## Definition of done

- Two successful build variants from same source tree:
  - GEN-enabled
  - GEN-disabled
- GEN-disabled binary contains no `mode_gen` path and no GEN menu behavior.
- No regressions in LFO/WAVE/TUNER UX.
- README/changelog note documents flag and expected behavior.
