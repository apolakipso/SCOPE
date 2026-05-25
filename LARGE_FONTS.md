# Large Parameter Fonts Plan (SCOPEv2)

## Short answer first

Yes, Adafruit_GFX can do larger text in multiple ways:

1. Built-in classic font scaling

- You can scale the built-in 5x7 font with `setTextSize(2)` (or higher).
- This is already used in this codebase (for example boot splash and tuner screens), so no new library is needed.
- This is the lowest-risk path for parameter edit UI.

2. GFX custom fonts (`setFont(...)`)

- Adafruit_GFX supports alternate bitmap fonts from `Fonts/*.h` and custom-converted fonts.
- This gives better typography, but costs flash and adds layout complexity.
- Custom fonts do not support background fill the same way as classic font; redraw/erase rectangles are needed.

3. Pre-rendered or code-drawn glyphs (no general font file)

- Since we only need short words and numeric values, we can draw a small, custom glyph set.
- This can be more memory-efficient than a full font and can be tuned for readability.

## Current implementation baseline

Relevant places:

- Parameter top bar is single-line classic font at y=0:
  - `Firmware/SCOPEv2/ui/ui_parameter_bar.cpp`
- The current design allocates about 8 px height for parameter labels/values.
- Full display height is 64 px, and mode renderers below the bar generally have enough room to absorb a taller header if coordinated.

Implication:

- We can increase edit UI prominence by reserving 16-24 px for parameter editing and shifting the waveform/main content down only while editing.

## Goals

1. Use more of the 64 px display for parameter edit state.
2. Improve readability at performance-safe redraw rates.
3. Keep AVR flash/RAM growth controlled.
4. Keep code maintainable across all modes (LFO/WAVE/TUNER/GEN).

## Candidate approaches

### Option A: Scale built-in font x2 (recommended first)

What it is:

- Keep classic font, switch to `setTextSize(2)` when drawing active parameter edit UI.

Pros:

- No new font assets.
- Minimal code churn.
- Predictable memory usage.
- Fast enough on SSD1306 for short strings.

Cons:

- Pixelated look.
- Width can become tight for long values.

Implementation notes:

- Add a dedicated large edit row renderer in `ui_parameter_bar.cpp`.
- Reserve a 16 px row (or 18 px with spacing) for active parameter name + value.
- Show non-active params in smaller text or as minimal markers.

### Option B: Use Adafruit GFX custom font(s)

What it is:

- Include one compact GFX font for labels/values (or one for labels + one for digits).

Pros:

- Better readability and style.
- Better character width control if chosen carefully.

Cons:

- Flash increase can be significant on 32 KB-class MCUs.
- Layout complexity (baseline handling, variable widths).
- Redraw requires bounds-aware erase to avoid artifacts.

Implementation notes:

- Introduce a compile-time switch to compare with Option A.
- Use `getTextBounds()` and clear text bounds before redraw.
- Keep fonts limited to one or two small files.

### Option C: Small custom glyph renderer for specific tokens (recommended second)

What it is:

- Hand-define bitmaps or tiny stroke glyphs for only needed strings/tokens:
  - Labels: `LFO`, `WAVE`, `TUNE`, `GEN`, `T`, `O`, `R`, `ZC`
  - Units/symbols: `V`, `%`, `+`, `-`, `Hz`
  - Digits and dot for values

Pros:

- Very tight control over flash budget.
- Crisp and consistent look at exactly desired size.
- No dependency on big font headers.

Cons:

- More custom code to maintain.
- Less flexible if text needs expand.

Implementation notes:

- Keep API tiny: `drawTokenLarge(x, y, tokenId)` and `drawNumberLarge(x, y, valueFmt)`.
- Store glyphs in `PROGMEM`.
- Use fixed-width grid for simple alignment.

### Option D: Numeric emphasis only (hybrid)

What it is:

- Keep labels small, render only active value large.

Pros:

- Biggest readability gain for least complexity.
- Minimal risk of layout overlap.

Cons:

- Parameter name remains small.

Implementation notes:

- Works especially well for `param2` in LFO/WAVE/GEN and tuner-related values.

## Recommended rollout

1. Phase 1 (quick win): Option A + D hybrid

- Large (`x2`) active parameter value.
- Keep row compact for non-active parameters.
- No new assets.

2. Phase 2 (optional refinement): evaluate Option C

- If Phase 1 readability is still insufficient or style needs improvement, add token-based custom glyphs for the most frequent labels/values.

3. Phase 3 (optional typography): selective GFX custom font pilot

- Only if needed after measuring flash usage and UX gain.

## Detailed execution plan

### Step 1: Add UI layout constants and mode-aware header height

Files:

- `Firmware/SCOPEv2/ui/ui_parameter_bar.cpp`
- `Firmware/SCOPEv2/ui/ui_parameter_bar.h`
- Potentially mode files to consume dynamic top offset:
  - `Firmware/SCOPEv2/mode/mode_lfo.cpp`
  - `Firmware/SCOPEv2/mode/mode_wave.cpp`
  - `Firmware/SCOPEv2/mode/mode_tuner.cpp`
  - `Firmware/SCOPEv2/mode/mode_gen.cpp`

Actions:

- Introduce constants like:
  - `kParamBarSmallH = 9`
  - `kParamBarLargeH = 18`
- Add helper:
  - `uint8_t getParamBarHeight(bool showParams)`
- Let mode renderers shift content down by returned height when params visible.

### Step 2: Implement large active-parameter renderer (built-in x2)

File:

- `Firmware/SCOPEv2/ui/ui_parameter_bar.cpp`

Actions:

- Keep existing compact bar behavior when not actively editing.
- When editing (`param_select != 0`), render:
  - Top: active parameter label in size 1 or 2 (depending fit)
  - Main: active value in size 2 centered/left-aligned
- Use explicit clear rect for the header area every frame.

### Step 3: Ensure per-mode geometry safety

Files:

- `Firmware/SCOPEv2/mode/mode_*.cpp`

Actions:

- Replace hardcoded top offsets that assume 0..8 occupied.
- Use computed header height to avoid overlapping plots/text.
- Keep bottom helper text (`TRIG`, tuning bars, etc.) visible.

### Step 4: Add compile-time feature flag

Files:

- `Firmware/SCOPEv2/core/scope_config.h`
- `Firmware/SCOPEv2/ui/ui_parameter_bar.cpp`

Actions:

- Add `#define UI_LARGE_PARAM_FONT 1`.
- Keep fallback to current compact behavior when disabled.

### Step 5: Measure and tune

Checks:

- Visual fit for all modes and all param ranges.
- Redraw stability (no ghosting/flicker).
- Build size before/after.
- Encoder responsiveness during rapid turns.

## Optional custom-glyph track (Option C)

If proceeding after Phase 1:

1. Create `Firmware/SCOPEv2/ui/ui_large_glyphs.h/.cpp`.
2. Add `PROGMEM` glyph tables for:

- `0-9`, `+`, `-`, `.`, `V`, `%`, `H`, `z`
- Token IDs for frequent labels.

3. Implement:

- `drawGlyphScaled(...)`
- `drawTokenLarge(...)`
- `drawValueLarge(...)`

4. Swap renderer behind same `UI_LARGE_PARAM_FONT` API surface.

## Risks and mitigations

1. Overlap with waveform/tuner graphics

- Mitigation: centralize top offset and consume it in each mode renderer.

2. Flash growth

- Mitigation: start with built-in scaling; add no external font first.

3. Flicker on text updates

- Mitigation: clear only bounded header area; keep one `display()` per frame.

4. Inconsistent width for dynamic values

- Mitigation: use fixed formatting helpers (`fmtDec1`, `fmtFreq`) and measured width rules.

## Acceptance criteria

1. Active parameter is clearly readable at arm-length distance.
2. At least ~16 px of vertical space is used for edit emphasis.
3. No clipping in any mode for max/min values.
4. No visible corruption artifacts when spinning encoder quickly.
5. Build remains within board flash/RAM limits.

## Suggested first implementation (concrete)

- Start with built-in font scaling only.
- Render a single large line with one-space left padding.
- Keep the current top row unchanged.
- Do not clear a black background behind the overlay; let the scope trace remain visible underneath.
- Show the overlay only briefly after a value change so it does not permanently cover the plot.
- Use a full black background and 3x text size for the MODE slot only.

This provides the biggest improvement with minimal risk and no new font file.

## Decision log

### Decision: implement Option A now

Date:

- 2026-05-25

Decision:

- Adopt Option A (built-in font scaling with `setTextSize(2)`) for parameter edit emphasis.
- Keep the existing top row exactly as-is.
- Render a temporary large overlay below row 1 as a single left-padded line containing the active parameter label and value.
- Render the MODE slot as a 3x banner with full black background and left padding, omitting the word "MODE" and showing only the mode value.

Behavior detail from UI sketch:

- The 8 text rows are treated as:
  - Row 1: unchanged compact top bar
  - Rows 2-8: reusable area for temporary large parameter/value overlay
- Overlay appears when a parameter value changes and auto-hides after about 1 second.
- The overlay is drawn without an explicit black background so the scope trace shows through underneath.
- The MODE slot is the exception: it uses a full black background and 3x value text.
- This intentionally allows temporary overlap with normal mode drawing under the top bar.

Reason:

- Matches requested UX while minimizing flash/RAM impact and code risk.
- Avoids adding external fonts or custom glyph assets in this phase.
