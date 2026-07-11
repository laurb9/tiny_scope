---
name: verify
description: What to run before declaring a tiny_scope change done - the build matrix, warnings policy, and simavr checks. Use before committing or opening a PR in this repo.
---

# Verifying a tiny_scope change

Run these before calling a change complete. All of it works inside the devcontainer.

## 1. Build matrix

Minimum for any code change — the primary AVR target plus one ARM target:

```bash
make all TARGET=arduino:avr:uno
make all TARGET=teensy:avr:teensy31
```

Full matrix when touching shared code (`adc.h`, `capture.*`, `display.*`, `scope.*`,
`tiny_scope.ino`) or anything build-related:

```bash
make all TARGET=arduino:avr:uno
make all TARGET=teensy:avr:teensy31
make all TARGET=teensy:avr:teensyLC
make all TARGET=adafruit:samd:adafruit_feather_m0
pio run                        # if platformio.ini exists, all default envs
```

## 2. Policies

- Builds use `--warnings all`: a change that adds warnings is not done.
- Watch the flash/RAM summary on the Uno build — it runs close to the 32KB/2KB limits.
  Call out significant size increases in the PR.

## 3. simavr

```bash
make sim-test                   # run the firmware on an emulated ATmega328P, compare serial trace to baseline
make sim-test-update            # regenerate test/uno-simavr.txt after intentional trace changes
make sim-display-test           # render the emulated SSD1306 framebuffer, compare pixels to baseline
make sim-display-test-update    # regenerate test/uno-display.txt after intentional display changes
```

`sim-test` catches boot crashes and memory exhaustion that compilation cannot (it
already caught a display-buffer OOM and an out-of-bounds ADC mode). `sim-display-test`
attaches a virtual SSD1306 over TWI and compares the rendered VRAM (ASCII art)
pixel-for-pixel against `test/uno-display.txt`, catching rendering regressions the serial
trace misses. Both require PlatformIO and simavr; `sim-display-test` also needs
`libsimavr-dev`, `libelf-dev`, and `gcc` (all in the devcontainer).

## 4. Hardware caveat

Emulation and compilation can't confirm display rendering or real ADC timing. If the
change affects display init, ADC configuration, pins, or timing, say so explicitly in
the PR description so the maintainer can flash a real board.

## 5. Docs sync

If files were added/moved or build commands changed, update AGENTS.md (structure/class
map/build sections) and this skill or the building skill as needed.
