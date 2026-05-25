# SCOPEv2 Refactor Plan

This plan describes how to split `Firmware/SCOPEv2/SCOPEv2.ino` into smaller, logical C++/Arduino modules while preserving behavior.

Goal constraints:

- Keep implementation files under ~500 LOC where practical.
- Keep external behavior identical at each step.
- Make progress in small compile-verified steps suitable for an autonomous coding agent.
- Maintain Arduino IDE compatibility.

---

## 1) Current File Assessment

`SCOPEv2.ino` currently mixes:

- Hardware probing and board variant setup.
- OLED initialization and all rendering.
- Encoder/button UI state machine.
- ADC ISR and sampling utilities.
- DAC abstraction + generator synthesis.
- Per-mode runtime logic (LFO, WAVE, TUNER, GEN).
- EEPROM persistence and config menu.
- Utility formatting and note detection.

This coupling causes:

- Large global namespace, hard to reason about side-effects.
- Risky edits due to cross-cutting state.
- Difficult targeted testing/debugging.

---

## 2) Target Architecture

Use a classic embedded split: one central state object + feature modules.

### 2.1 Proposed File Layout

Under `Firmware/SCOPEv2/`:

- `SCOPEv2.ino`
  - Minimal entry point (`setup()`, `loop()`), boot splash call, top-level orchestration.
- `scope_config.h`
  - Compile-time constants, pin maps, EEPROM addresses, mode IDs, limits.
- `scope_types.h`
  - Shared structs/enums (e.g., `ModeSettings`, mode/state enums).
- `scope_state.h`
  - `struct ScopeState` containing all mutable runtime state.
- `scope_state.cpp`
  - Definition of global `ScopeState gState`.

- `hal_adc.h` / `hal_adc.cpp`
  - ADC ISR, start/stop sampling, fast analog read.
- `hal_dac.h` / `hal_dac.cpp`
  - DAC init/stop/write, MCP4725 scan, conversion helpers.
- `hal_display.h` / `hal_display.cpp`
  - Display init and low-level display helper hooks.
- `hal_hw.h` / `hal_hw.cpp`
  - Hardware detection, encoder instantiation, board variant detection.

- `storage_settings.h` / `storage_settings.cpp`
  - EEPROM defaults/load/save and mode settings persistence.

- `ui_parameter_bar.h` / `ui_parameter_bar.cpp`
  - `drawParameterBar()` and related text formatting for top bar.
- `ui_config_menu.h` / `ui_config_menu.cpp`
  - Long-press settings menu logic and rendering.
- `ui_controls.h` / `ui_controls.cpp`
  - Encoder/button interpretation, param navigation/edit transitions.

- `mode_lfo.h` / `mode_lfo.cpp`
- `mode_wave.h` / `mode_wave.cpp`
- `mode_tuner.h` / `mode_tuner.cpp`
- `mode_gen.h` / `mode_gen.cpp`
  - Per-mode runtime behavior only.

- `dsp_tuner.h` / `dsp_tuner.cpp`
  - `detectFrequencyZC()`, note mapping and note tables.
- `dsp_generator.h` / `dsp_generator.cpp`
  - Wave synthesis functions/tables (`sineWave`, `generateSample`, `calcPhaseInc`).

- `util_format.h` / `util_format.cpp`
  - `fmtInt`, `fmtDec1`, `fmtDec2`, `fmtFreq`.

- `app_mode_manager.h` / `app_mode_manager.cpp`
  - `setupMode()`, mode switch transitions.

This split yields files typically in the 80-350 LOC range.

### 2.2 Why This Split Works

- HAL files isolate board/MCU-specific details.
- Mode files isolate behavior by user-facing mode.
- UI files isolate menu/interaction complexity from signal processing.
- DSP files isolate math tables/algorithms from rendering and controls.
- `ScopeState` eliminates brittle implicit globals and makes dependencies explicit.

---

## 3) State Management Strategy

### 3.1 Central State Container

Introduce:

```cpp
struct ScopeState {
  // existing mutable globals grouped by concern
};
extern ScopeState gState;
```

Guidelines:

- Keep hardware pointers (`display`, `encoder`) in `gState` initially.
- Keep ISR-shared fields `volatile` and document ownership.
- Keep immutable lookup tables in module-local `static`/`PROGMEM` where possible.

### 3.2 Ownership Rules

- ISR-owned data: modified in `hal_adc.cpp`, read elsewhere.
- UI-owned state: `param`, `param_select`, button/encoder transitions in `ui_controls.cpp`.
- Mode-owned ephemeral statics: prefer moving `static` locals into dedicated mode-private structs if growth continues.

---

## 4) Arduino IDE Compatibility Notes

Because Arduino auto-generates prototypes only for `.ino`, moving code to `.cpp` requires explicit headers/prototypes.

Rules:

- Every `.cpp` has a matching `.h` with declarations.
- Include `Arduino.h` in headers needing Arduino types/macros.
- Keep ISR in a `.cpp` that includes `<avr/interrupt.h>`.
- Keep include graph acyclic: `scope_state.h` and `scope_config.h` should be low-level roots.

---

## 5) Incremental Execution Plan (Agent-Friendly)

Each phase must end with a successful compile in Arduino IDE before continuing.

### Phase 0: Baseline and Safety Net

Tasks:

- Record baseline compile status and binary size (if available).
- Add a short behavior checklist (manual smoke test list) at end of this file.

Acceptance:

- Current sketch compiles unchanged.

### Phase 1: Extract Constants and Types (No Behavior Change)

Tasks:

- Create `scope_config.h` and move `#define` constants (pins, EEPROM offsets, mode IDs, limits).
- Create `scope_types.h` for `ModeSettings` and mode enums/constants.
- Include these from `SCOPEv2.ino`.

Acceptance:

- Compile passes.
- No logic movement yet.

### Phase 2: Introduce `ScopeState`

Tasks:

- Create `scope_state.h/.cpp` and migrate mutable globals into `gState`.
- Update `SCOPEv2.ino` references to `gState.<field>`.
- Keep function bodies still in `.ino`.

Acceptance:

- Compile passes.
- Runtime behavior unchanged.

### Phase 3: Move Formatting Utilities

Tasks:

- Move `fmtInt`, `fmtDec1`, `fmtDec2`, `fmtFreq` to `util_format.cpp`.
- Add `util_format.h` declarations.

Acceptance:

- Compile passes.
- String outputs unchanged.

### Phase 4: Move DSP Helpers

Tasks:

- Move tuner math + note mapping (`frequencyToNote`, note tables, `detectFrequencyZC`) to `dsp_tuner.cpp`.
- Move generator math/tables (`sineWave`, `generateSample`, `calcPhaseInc`) to `dsp_generator.cpp`.

Acceptance:

- Compile passes.
- Tuner and GEN visual outputs match baseline.

### Phase 5: Move HAL Layers

Tasks:

- Move ADC ISR and helpers to `hal_adc.cpp`.
- Move DAC logic (LGT8F/MCP4725) to `hal_dac.cpp`.
- Move hardware detect + display init to `hal_hw.cpp` and `hal_display.cpp`.

Acceptance:

- Compile passes.
- ADC capture and GEN output still function.

### Phase 6: Move EEPROM/Settings

Tasks:

- Move `resetEEPROMDefaults`, `loadAllSettings`, `saveCurrentModeToRAM`, `saveAllSettings` to `storage_settings.cpp`.

Acceptance:

- Compile passes.
- Save/load and long-press save behavior unchanged.

### Phase 7: Move UI Layers

Tasks:

- Move `drawParameterBar` to `ui_parameter_bar.cpp`.
- Move `configMenu` to `ui_config_menu.cpp`.
- Extract encoder/button transition logic from `loop()` into `ui_controls.cpp` (pure state transition helpers).

Acceptance:

- Compile passes.
- Menu cursor underline behavior still works exactly as in 3.4.1.

### Phase 8: Move Mode Implementations

Tasks:

- Move `runLFOMode`, `runWaveMode`, `runTunerMode`, `runGeneratorMode` to per-mode `.cpp` files.
- Move `setupMode` to `app_mode_manager.cpp`.

Acceptance:

- Compile passes.
- Manual smoke tests pass for all 4 modes.

### Phase 9: Thin `SCOPEv2.ino`

Tasks:

- Keep only includes, global app init, `setup()`, `loop()`, and minimal orchestration.
- Ensure final `.ino` is mostly readable high-level flow.

Acceptance:

- Compile passes.
- `SCOPEv2.ino` ideally <250 LOC.

### Phase 10: Cleanup and Guardrails

Tasks:

- Remove dead declarations and duplicate comments.
- Add short file headers documenting module responsibility.
- Ensure no implementation file exceeds ~500 LOC; split again if needed.

Acceptance:

- Compile passes.
- All source files under target LOC when practical.

---

## 6) Recommended Seams (Exact Function Migration Map)

Move from `SCOPEv2.ino` as follows:

- `hal_adc.cpp`
  - `ISR(ADC_vect)`
  - `startADCSampling(...)`
  - `stopADCSampling()`
  - `fastAnalogRead()`

- `hal_dac.cpp`
  - `scanMCP4725()`
  - `dacInit()`
  - `dacStop()`
  - `dacWrite(...)`
  - `dcVoltageToDac(...)`

- `hal_hw.cpp`
  - `detectHardware()`

- `hal_display.cpp`
  - `initDisplay()`

- `storage_settings.cpp`
  - `resetEEPROMDefaults()`
  - `loadAllSettings()`
  - `saveCurrentModeToRAM()`
  - `saveAllSettings()`

- `app_mode_manager.cpp`
  - `setupMode(...)`

- `mode_lfo.cpp`
  - `runLFOMode(...)`

- `mode_wave.cpp`
  - `runWaveMode(...)`

- `mode_tuner.cpp`
  - `runTunerMode(...)`

- `mode_gen.cpp`
  - `runGeneratorMode(...)`

- `ui_parameter_bar.cpp`
  - `drawParameterBar(...)`

- `ui_config_menu.cpp`
  - `configMenu()`

- `dsp_tuner.cpp`
  - `frequencyToNote(...)`
  - `detectFrequencyZC()`
  - note tables

- `dsp_generator.cpp`
  - `sineWave(...)`
  - `generateSample(...)`
  - `calcPhaseInc(...)`
  - waveform tables

- `util_format.cpp`
  - `fmtInt(...)`
  - `fmtDec1(...)`
  - `fmtDec2(...)`
  - `fmtFreq(...)`

---

## 7) Compile & Verification Checklist Per Phase

At end of every phase:

- Build in Arduino IDE for target board profiles used by project.
- Confirm no new warnings related to missing prototypes or duplicate symbols.

Hardware/manual checks (quick):

- Boot splash appears with correct version string.
- Encoder rotate/press works.
- Top bar selection and underline cursor behavior works.
- LFO mode updates and trigger freeze works.
- WAVE mode captures and redraws correctly.
- TUNER shows note + cents when fed periodic signal.
- GEN output (on v2.5) still produces waveform/DC.
- Save settings and power-cycle persistence still works.

---

## 8) Risk Register and Mitigations

Risk: ISR/shared data race after moving files.

- Mitigation: keep volatile qualifiers; avoid multi-byte writes without atomic guards where needed.

Risk: Arduino include-order/prototype breakages.

- Mitigation: explicit headers and declarations; avoid reliance on `.ino` auto-prototypes.

Risk: Flash/RAM regression.

- Mitigation: compare binary size at baseline and final; keep `F()` and `PROGMEM` usage.

Risk: behavior drift in menu state machine.

- Mitigation: isolate UI transitions last and verify with checklist after each UI-related phase.

---

## 9) File Size Budget (Target)

Suggested rough caps:

- `SCOPEv2.ino`: 150-250 LOC
- each `mode_*.cpp`: 120-350 LOC
- `hal_adc.cpp`: 80-180 LOC
- `hal_dac.cpp`: 120-250 LOC
- `storage_settings.cpp`: 120-250 LOC
- UI modules each: 100-250 LOC

If any file exceeds ~500 LOC, split by sub-responsibility.

---

## 10) Agent Work Protocol

For each phase the implementation agent should:

1. Create/update files for that phase only.
2. Keep behavior-preserving changes only (no feature additions).
3. Compile and report success/failure.
4. If compile fails, fix within phase before proceeding.
5. Commit with phase label (optional, but recommended).

Recommended commit message style:

- `refactor(scopev2): phase N - <short description>`

---

## 11) Changelog and Versioning Policy During Refactor

For this repository workflow:

- Every firmware-affecting change should bump semantic patch version.
- Update `CHANGELOG.md` for each patch release.

Suggested sequence for refactor rollout:

- 3.4.2: phases 1-3
- 3.4.3: phases 4-6
- 3.4.4: phases 7-9
- 3.4.5: cleanup/stabilization

(Exact grouping can change, but each released step should compile and be flashable.)

---

## 12) Definition of Done

Refactor is done when:

- Build succeeds in Arduino IDE.
- Manual checklist passes on hardware.
- `SCOPEv2.ino` is thin and readable.
- Source files are logically modular and mostly below 500 LOC.
- Version and changelog updated for delivered patch.
