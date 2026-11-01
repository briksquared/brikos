# brikOS — control reference

## Hardware layout (Axiometa Genesis One)

| Port | Control | Function |
|------|---------|----------|
| P1 | Encoder 1 | Grid navigation + select / clear |
| P2 | ST7735 display | 160×80 UI |
| P3 | Touchpad (TTP223) | Live drum trigger |
| P4 | LED button | Live drum trigger with LED feedback |
| P5 | Encoder 2 | BPM + mode cycle + mute toggle + ratchet |
| P6 | Pot | Master volume + context parameter |
| P7 | Two-pad | Jam synth (two buttons, melodic) |
| P8 | MAX98357A amp | I²S audio out |
| GPIO45 | Onboard button | Quick next preset |

## Encoder 1 (P1) — grid encoder

- **Rotate** — moves the grid cursor; in parameter modes, adjusts that parameter (sound, FX,
  kit, preset, LED style, chance, glide, transpose, swing, chain slot, song slot, nudge/voice track).
- **Short press** — SELECT, the context action for the active mode:
  - EDIT — step cycle off / hit / accent
  - MUTE — toggle highlighted track mute
  - PRESET — load the browsed preset
  - KIT — load the browsed drum kit
  - BANK — save live into current, load selected
  - SOUND / FX — advance to next track row
  - NOTE — advance to next synth step
  - TEMPO — tap tempo (tap 2–4 times in rhythm)
  - CHOKE / METRO / VOICE — toggle / pitch-vs-decay
  - CHAIN / SONG — start / stop that arrangement
- **Short press ×2** (within 400 ms, EDIT only) — CLEAR ROW, wipes the selected track row.
- **Long press** (≥700 ms, FX mode only) — toggle REVERSE on the selected track.
- **Long press** (≥1500 ms) — cycle to the next mode.
- **Hold + rotate ENC2** (in PRESET) — SCENE MORPH between current and next preset (release to
  commit if >90%, snap back if <10%, hold if in between).
- **Hold + rotate ENC1** (in EDIT) — per-step micro-timing nudge.
- **Hold + ENC2 short press** (in EDIT) — add ratchet (step repeats 2 / 3 / 4).

## Encoder 2 (P5) — tempo encoder

- **Rotate** — sets BPM (40–240); or, by mode: CHAIN slot preset number · SONG slot preset or
  repeat count · NUDGE timing offset (−8..+8) · VOICE decay or pitch · HUMAN velocity randomness
  (0–9) · PRESET+ENC1-held scene morph.
- **Short press** — EDIT: toggle mute on the selected track. Other modes: cycle to next mode.
- **Long press** (≥1500 ms, SONG) — flip ENC2 between editing preset number and repeat count.
- **Long press** (≥1500 ms, any other mode) — jump straight back to EDIT.
- **Hold** (≥1000 ms, REC) — clear the selected row.

## Pot (P6) — master volume + context parameter

| Mode | Function |
|------|----------|
| most | Master volume 0–10 (0 = mute) |
| MIX | Per-track gain for the selected row (0.0–1.0) |
| DUCK | Sidechain duck depth (0–100%) |
| FX | Master reverb send amount |
| GLITCH | Global glitch effect depth (0–100%) |
| COMP | Master compressor threshold |
| DENSITY | Global pattern density (thins all chance values together) |

## P3 touchpad — live drum / record

- **Default** — triggers the currently-selected drum (K / S / H / C / T).
- **In REC** — records a live hit into the selected track at the nearest step.
- **With ENC2 held** — records a kick on the current step.

## P4 LED button — live drum / fill / record / copy

- **Tap** — triggers the complementary drum (so P3 and P4 are always different drums).
- **In BANK** — copy the live pattern to the selected bank.
- **In REC** — records a live hit on the selected track.
- **Hold ≥500 ms** — queues a one-bar 16th-note FILL on the selected drum.
- **Double-tap in EDIT** (two taps <300 ms, same row) — copy that track row; the next
  double-tap on another row pastes it.
- **With ENC2 held** — records a snare on the current step.

## Two-pad (P7) — jam synth

- **Button 1 (left)** — plays synth B1 and steps UP the current scale.
- **Button 2 (right)** — plays synth B2 a fifth higher and steps DOWN the scale.
- **Hold either** — auto-retriggers at 16th-note rate.
- **Both pressed** — toggles BREAKDOWN (mutes all drum tracks).
- **Both held >1 s** — cycles scale: MIN PENT / MAJ PENT / DORIAN / MAJOR / BLUES / PHRYGIAN /
  OCTAVES / SPREAD.

## Modes

Cycled by ENC2 short-press, or ENC1 long-press. Long-press ENC1 ≥3 s jumps to the secondary tier.

**Primary tier** (uppercase icon): EDIT · MUTE · MIX · PRESET (browse and load the 274 presets) ·
SOUND (5 drum variants or 180 synth voices) · KIT (over all 5 drum rows) · NOTE · FX · GLITCH ·
LED · BANK (A/B/C/D) · CHAIN (up to 8 presets, auto-loop) · SONG (16-slot arrangement with
per-slot repeats) · TEMPO · SWING · REC.

**Secondary tier** (lowercase icon): NUDGE · DUCK · VOICE · GLIDE · TRANSPOSE · COMP · CHOKE ·
HUMAN · METRO · DENSITY · BRIGHT · BANK_SONG.

## Display (ST7735, 160 × 80)

**Top bar** — step-progress phase bar (flashes white on kick), mode icon (UV accent marks the
tier), BPM, context mode tag, CPU load bar (green <70% / yellow <90% / red at ceiling), and a
green dot that flashes on each kick.

**Grid** — 7 rows × 8 steps (K S H C T B b): row labels dim when muted, a live VU bar per track,
pair separators between steps, downbeat triangles above steps 1 and 5, a bright/dim UV comet for
the current/previous step, ratchet digits in the cell corner, accent markers (triangle points up
when REVERSE is on), a red strike across muted rows, and the cursor outline (white cell, UV row,
or blue for MUTE).

**SONG overlay** — PLAY/STOP state, PRESET/REPEATS indicator, a 16-slot block strip (filled =
non-empty, white = playing), and the `S## P### x<repeats>` line.

**Footer** — a scrolling preset-name marquee and a live scope strip.

## Persistence

Everything editable auto-saves to NVS flash after 2.5 s of idle — pattern, chance, ratchet,
swing, BPM, volume, mutes, sounds, FX, nudge, voice decay/pitch, reverse flags, micro-timing,
duck depth, gain, global FX, drum kit, banks and bank scenes, LED style, brightness, glide,
transpose, last preset, jam scale, compressor threshold, and chain/song arrangements. It powers
back on in the same state.
