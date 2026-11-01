# brikOS DRUMS

A standalone 7-track drum machine and synthesizer firmware for the
**[Axiometa Genesis One](https://axiometa.io)** (ESP32-S3). Every sound is DSP-synthesized
sample-by-sample — there is no sample playback. It boots playing and never stops; everything
auto-saves to flash and comes back in the same state on the next power-up.

![board: Axiometa Genesis One](https://img.shields.io/badge/board-Axiometa%20Genesis%20One-111) ![mcu: ESP32--S3](https://img.shields.io/badge/mcu-ESP32--S3-111) ![license: MIT](https://img.shields.io/badge/license-MIT-111)

## Features

- **7 tracks** — 5 drum voices (kick, snare, hat, clap, tom) + 2 synth-bass voices, 8 steps
- **180 synth voices** across 9 categories (analog, FM, west-coast, keys, strings, leads, …)
- **274 presets** and **204 drum kits**, each with genre-matched groove, swing and basslines
- **Per-track effects** (11: delay, reverb, phaser, flange, chorus, drive, crush, tremolo, ring-mod, stutter)
- **Global glitch FX** (16: gate, beat-repeat, reverse, tapestop, vinyl, freeze, warp, …)
- **Master bus** — DC blocker, glue compressor, tone shelf, and a send reverb with pre-delay
- Scene morph, pattern banks, chain and song arrangement, ratchets, per-step chance,
  swing, humanize, micro-timing, sidechain ducking, portamento, transpose
- 20 reactive LED-matrix animations and a live on-screen scope
- Band-limited oscillators (PolyBLEP) and a lookup-table DSP path, dual-core: audio on
  core 0, UI on core 1

## Hardware

Axiometa Genesis One (ESP32-S3-WROOM-1-N8R2), with modules on all 8 ports:

| Port | Module | Role |
|------|--------|------|
| P1 | Rotary encoder | Grid navigation + select |
| P2 | ST7735 160×80 LCD | UI |
| P3 | TTP223 touchpad | Live drum trigger |
| P4 | LED button | Live drum trigger + fill + copy |
| P5 | Rotary encoder | BPM + mode cycle |
| P6 | Potentiometer | Master volume + context parameter |
| P7 | Two-pad | Jam synth |
| P8 | MAX98357A | I²S audio out |

Pin assignments come from the Genesis One board variant — see [`axiometa.json`](axiometa.json)
for the port → module map.

## Build

Install the **Axiometa** board package (adds `esp32:esp32:axiometa_genesis_one`) and these
libraries: Adafruit GFX, Adafruit ST7735/ST7789, Adafruit NeoPixel, and
[RotaryEncoder](https://github.com/mathertel/RotaryEncoder).

With [`arduino-cli`](https://arduino.github.io/arduino-cli/):

```bash
arduino-cli compile --fqbn esp32:esp32:axiometa_genesis_one brikos
arduino-cli upload  --fqbn esp32:esp32:axiometa_genesis_one -p <PORT> brikos
```

In the Arduino IDE, open [`brikos.ino`](brikos.ino), select **Axiometa Genesis One** as the
board, and upload. (The companion `.h`/`.inc` files load automatically alongside the sketch.)

The device enumerates as native USB-CDC. If the serial port is busy, hold BOOT, tap reset, and
re-plug to enter ROM download mode.

## Controls

The full control reference is in [`REFERENCE.md`](REFERENCE.md) — every encoder turn, press,
hold and mode, plus a map of the display.

## License

[MIT](LICENSE). Built with [Adafruit GFX / ST7735 / NeoPixel](https://github.com/adafruit) and
[RotaryEncoder](https://github.com/mathertel/RotaryEncoder). Provided as-is — flashing hardware
is at your own risk.
