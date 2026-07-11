# AGENTS.md - AI Coding Assistant Guidelines

This file provides guidelines for AI coding assistants working with the tiny_scope repository.

## Project Overview

**tiny_scope** is an Arduino application implementing a small oscilloscope: it samples an
analog input via the ADC and renders the waveform on a 128x64 SSD1306 OLED display (I2C).
Features include selectable sampling rates, auto time-base grid, 0V auto trigger, AREF
calibration, and an automatic voltmeter mode for steady signals.

- **Language**: C++ (Arduino)
- **License**: MIT
- **Type**: Application sketch (not a library)
- **Target boards**: Arduino UNO/AVR, Teensy 3.x/LC, Adafruit Feather M0 (SAMD), ESP32 (build-only)
- **Author/Maintainer**: Laurentiu Badea

## Repository Structure

```
tiny_scope/
├── tiny_scope/             # The application sketch
│   ├── tiny_scope.ino      # Entry point: setup/loop, pin config, splash screen
│   ├── adc.h               # ADCBase portable interface + per-platform dispatch
│   ├── adc_avr.h/cpp       # AVR ADC driver (prescaler-based sampling modes)
│   ├── adc_teensy3.h/cpp   # Teensy 3.x/LC ADC driver (pedvide ADC library)
│   ├── capture.h/cpp       # Capture: samples buffer, mV conversion, min/max
│   ├── display.h/cpp       # Display: Adafruit_SSD1306 + printf/units helpers
│   └── scope.h/cpp         # Scope: waveform/grid/voltmeter rendering
├── .devcontainer/          # Dev container: arduino-cli, PlatformIO, simavr
├── .claude/skills/         # Project skills for agent work (building, verify)
├── .github/workflows/      # CI: arduino.yml + platformio.yml build matrices
├── Makefile                # arduino-cli build automation
├── arduino-cli.yaml        # arduino-cli config (board indexes, .arduino dirs)
├── platformio.ini          # PlatformIO envs: uno, teensy31, teensylc,
│                           #   adafruit_feather_m0, esp32dev
└── README.md               # User-facing docs, wiring, parts list
```

## Class Map

```
ADCBase                  # adc.h - portable analogRead()-based interface
└── ADCInput             # exactly one is selected by #if in adc.h:
    ├── adc_avr.h        #   AVR (ADCSRA prescaler modes)
    ├── adc_teensy3.h    #   Teensy 3.x/LC (pedvide ADC, continuous mode)
    └── (ADCBase as-is)  #   fallback for SAMD/ESP32/others

Adafruit_SSD1306
└── Display              # display.h - adds printf(), unit formatting (mV/us)

Capture                  # capture.h - owns ADCInput + sample buffer, converts to mV
Scope                    # scope.h - renders Capture on Display (graph or voltmeter)
```

## Branches

- `master` is the only branch that matters going forward; it builds all targets from one
  tree using per-platform `#if` guards.
- `teensy` and `feather_m0` are **historical** target branches from before arduino-cli
  could resolve per-board libraries; their deltas are being folded into master. Do not
  base new work on them. Other old branches (`fft`, `low_power`, `freeram`, `edison`)
  are experiments, also inactive.

## Building and Testing

### arduino-cli via Makefile

```bash
make setup                # one-time: downloads arduino-cli, indexes, libraries
make all                  # build for the default TARGET (arduino:avr:uno)
make all TARGET=teensy:avr:teensy31
make all TARGET=teensy:avr:teensyLC
make all TARGET=adafruit:samd:adafruit_feather_m0
make clean
```

Artifacts land in `build/`. The core for `TARGET` is installed automatically.
Support files (cores, libraries) live in the git-ignored `.arduino/` directory.

### Dev container

`.devcontainer/` provides Ubuntu with make, arduino-cli (via `make setup`), PlatformIO
and simavr preinstalled. Open the repo in VS Code and "Reopen in Container", or build
and run `.devcontainer/Dockerfile` manually. Always prefer building/testing inside the
container for reproducibility.

### Verification expectations

Before declaring a change done, build for **at least** `arduino:avr:uno` and one ARM
target — see `.claude/skills/verify`. `--warnings all` is on; do not introduce new
warnings. Display/ADC behavior can only be fully confirmed on hardware; call out
runtime-affecting changes (display init, ADC timing, pin changes) in the PR description
so the maintainer can flash-test.

## Coding Conventions

- 4-space indentation
- `PascalCase` classes, `camelCase` methods, `UPPER_SNAKE_CASE` macros/constants
- Include guards `#ifndef NAME_H_` in headers; MIT copyright notice at top of each file
- Per-platform code is selected with preprocessor guards (see `adc.h`); prefer extending
  the `ADCInput` dispatch there over sprinkling `#ifdef` through shared code
- User-tunable settings (pins, I2C address, AREF) stay at the top of `tiny_scope.ino`
  in the "Configurable parameters" block

## Performance Constraints

> **Critical**: The primary target is an ATmega328P: 2KB RAM, 32KB flash, 16 MHz, no FPU.

- The capture buffer is `SCREEN_WIDTH` (128) uint16_t samples — RAM is nearly full on
  AVR; avoid adding buffers or `String`, prefer stack-free `F()` flash strings
- `ADCInput::read()/readFast()/readMulti()` are the sampling hot path: keep them
  inline, integer-only, and branch-light; sampling jitter shows up directly as
  waveform distortion
- Use `PROGMEM`/`F()` for constant data on AVR
- No dynamic allocation outside the single `capture.init()` buffer

## Commit & Pull Request Guidelines

- Clear, descriptive commit titles; reference issues where applicable
- **Agent commits**: identify yourself via env vars without changing the user's git config:
  ```bash
  GIT_AUTHOR_NAME="model-name (tool)" GIT_COMMITTER_NAME="model-name (tool)" git commit ...
  ```
- Add an `Agent: model-name (tool)` footer to PR descriptions
- PRs must pass CI before merge and state which targets were build-verified

## Documentation Maintenance

Keep this file in sync with reality: update the Repository Structure and Class Map
sections when files move, and the Building section when build tooling changes. The
README is user-facing (wiring, parts, usage); AGENTS.md is contributor/agent-facing.
