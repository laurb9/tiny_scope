---
name: building
description: How to build tiny_scope for each supported board with arduino-cli (Makefile) or PlatformIO. Use when compiling, adding a build target, or debugging build/toolchain issues in this repo.
---

# Building tiny_scope

One sketch (`tiny_scope/tiny_scope.ino`) builds for all boards; per-platform code is
selected by preprocessor guards in `tiny_scope/adc.h`.

## arduino-cli (Makefile)

```bash
make setup        # one-time: downloads arduino-cli into .arduino/, indexes, libraries
make all TARGET=arduino:avr:uno                    # default target
make all TARGET=teensy:avr:teensy31
make all TARGET=teensy:avr:teensyLC
make all TARGET=adafruit:samd:adafruit_feather_m0
```

- Artifacts land in `build/`; the core for `TARGET` auto-installs on first build.
- Board indexes and directory layout: `arduino-cli.yaml`. All downloaded state lives in
  git-ignored `.arduino/` — safe to delete to get a clean slate.
- Libraries: Adafruit SSD1306 + GFX (installed by `make setup`). The Teensy ADC library
  ships with the Teensy core, not as a separate install.
- Builds run with `--warnings all`; treat new warnings as failures.

## PlatformIO

If `platformio.ini` exists at the repo root:

```bash
pio run                # all default envs
pio run -e uno         # single env; see platformio.ini for the env list
```

## Where to build

Prefer the devcontainer (`.devcontainer/`) — it has make, PlatformIO and simavr
preinstalled; `make setup` runs on container create. Host builds work on macOS/Linux
too (the Makefile picks the right arduino-cli binary).

## Gotchas

- AVR is the constrained target: 2KB RAM / 32KB flash. Check the flash/RAM usage lines
  printed at the end of a build; the Uno build sits near the limits.
- If arduino-cli behaves oddly after an upgrade, delete `.arduino/` and re-run
  `make setup`.
