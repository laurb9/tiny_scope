# sim_display - emulated SSD1306 OLED render test

This is a headless display test for tiny_scope. It runs the firmware on an
emulated ATmega328P (via [simavr](https://github.com/buserror/simavr)) with a
**virtual SSD1306 OLED** attached over TWI (I2C), waits for the firmware's
`sim: TESTS COMPLETE` marker on the serial port, then dumps the display's video
RAM as ASCII art (`#` = lit pixel, `.` = dark, 64 rows x 128 columns). That
render is compared pixel-for-pixel against the golden baseline
`test/uno-display.txt`, so it catches display regressions that the serial-only
smoke test (`test/simavr-run.sh`) cannot.

## Files

- `sim_display.c` - the runner (loads the ELF, wires up UART capture and the
  virtual OLED, steps the CPU to the marker, writes the ASCII-art dump). This is
  our own code.
- `ssd1306_virt.c` / `ssd1306_virt.h` - the virtual SSD1306 part, **vendored
  verbatim from simavr v1.6** (`examples/parts/ssd1306_virt.{c,h}`).

## Licensing

> `ssd1306_virt.c` / `ssd1306_virt.h` are **GPLv3** (Copyright Michel Pollet and
> Doug Szumski), unlike the rest of tiny_scope which is MIT. They are **test
> tooling only** and are compiled solely into the `sim_display` host binary -
> they are **never** linked into the firmware. Keep their GPL headers intact.

## Running

Requires `gcc`, `libsimavr-dev`, and `libelf-dev` (all present in the
devcontainer). From the repo root:

```bash
make sim-display-test          # build uno_sim + runner, render and compare
make sim-display-test-update   # regenerate test/uno-display.txt after an
                               # intentional display change
```

The Makefile builds the `sim_display` binary
(`gcc ... -I/usr/include/simavr -lsimavr -lelf`) and the `uno_sim` PlatformIO
firmware, then `test/sim-display-run.sh` runs the render and diff. Override the
simulated-seconds timeout with `TIMEOUT=<n>`.
