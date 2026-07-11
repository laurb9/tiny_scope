#!/usr/bin/env bash
#
# sim-display-run.sh - Run the tiny_scope firmware on a simulated ATmega328P
# with a virtual SSD1306 OLED attached over TWI, render its framebuffer to
# ASCII art, and compare it pixel-for-pixel against a golden baseline.
#
# Usage:
#   test/sim-display-run.sh [firmware.elf]   Run and compare against the golden file.
#   test/sim-display-run.sh --update         Run and (re)write the golden file instead
#                                            of comparing (also: UPDATE=1 env var).
#
# The default firmware path is .pio/build/uno_sim/firmware.elf (built with
# `pio run -e uno_sim`). The runner binary (test/sim_display/sim_display) is
# built by the Makefile - run `make sim-display-test` rather than this script
# directly, or build the runner first.
#
# Unlike the serial smoke test (simavr-run.sh) this is a pixel test: the ASCII
# art is compared verbatim, with no digit stripping.
#
# Environment variables:
#   RUNNER   sim_display binary       (default: test/sim_display/sim_display)
#   TIMEOUT  max simulated seconds     (default: 30)

set -u

FIRMWARE="${1:-.pio/build/uno_sim/firmware.elf}"
TIMEOUT="${TIMEOUT:-30}"

UPDATE="${UPDATE:-0}"
if [ "${FIRMWARE}" = "--update" ]; then
    UPDATE=1
    FIRMWARE=".pio/build/uno_sim/firmware.elf"
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RUNNER="${RUNNER:-${SCRIPT_DIR}/sim_display/sim_display}"
GOLDEN="${SCRIPT_DIR}/uno-display.txt"

if [ ! -x "${RUNNER}" ]; then
    echo "ERROR: runner not built: ${RUNNER}" >&2
    echo "Build it first, e.g. with: make sim-display-test" >&2
    exit 1
fi

if [ ! -f "${FIRMWARE}" ]; then
    echo "ERROR: firmware not found: ${FIRMWARE}" >&2
    echo "Build it first, e.g. with: pio run -e uno_sim" >&2
    exit 1
fi

TMP_OUT="$(mktemp)"
cleanup() {
    rm -f "${TMP_OUT}"
}
trap cleanup EXIT

if ! "${RUNNER}" "${FIRMWARE}" "${TMP_OUT}" --timeout "${TIMEOUT}"; then
    echo "ERROR: sim_display run failed (see output above)." >&2
    exit 1
fi

if [ "${UPDATE}" -eq 1 ]; then
    cp "${TMP_OUT}" "${GOLDEN}"
    echo "Updated golden file: ${GOLDEN}"
    cat "${GOLDEN}"
    exit 0
fi

if [ ! -f "${GOLDEN}" ]; then
    echo "ERROR: golden file not found: ${GOLDEN}" >&2
    echo "Run with --update first to generate it." >&2
    exit 1
fi

if diff -q "${GOLDEN}" "${TMP_OUT}" >/dev/null; then
    echo "PASS: rendered display matches the golden baseline."
    exit 0
fi

echo "FAIL: rendered display does not match the golden baseline (${GOLDEN})."
echo "----- diff (golden vs capture) -----"
diff "${GOLDEN}" "${TMP_OUT}"
echo "----- captured display -----"
cat "${TMP_OUT}"
exit 1
