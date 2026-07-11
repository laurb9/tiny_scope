#!/usr/bin/env bash
#
# simavr-run.sh - Run the tiny_scope firmware on a simulated ATmega328P via
# simavr and compare its "sim:" trace lines against a golden baseline.
#
# Usage:
#   test/simavr-run.sh [firmware.elf]      Run and compare against the golden file.
#   test/simavr-run.sh --update            Run and (re)write the golden file instead
#                                          of comparing (also: UPDATE=1 env var).
#
# The default firmware path is .pio/build/uno_sim/firmware.elf (built with
# `pio run -e uno_sim` - the SIMAVR_TEST build with serial trace enabled).
# Override behavior with these environment variables:
#   SIMAVR   simavr binary            (default: simavr)
#   MCU      target MCU               (default: atmega328p)
#   FREQ     clock frequency in Hz    (default: 16000000)
#   TIMEOUT  max seconds to wait      (default: 60)
#
# The firmware prints "sim: TESTS COMPLETE" after a few capture/render loops;
# we poll the captured output for that marker and kill the simulator as soon
# as it appears (simavr never exits on its own because loop() runs forever).
#
# Comparison normalizes both the capture and the golden by keeping only "sim:"
# trace lines and stripping digits, so ADC/timing values that drift with
# toolchain versions don't cause false failures while an OK<->FAIL or
# YES<->NO verdict flip still does. Line order is preserved and compared.

set -u

FIRMWARE="${1:-.pio/build/uno_sim/firmware.elf}"
SIMAVR="${SIMAVR:-simavr}"
MCU="${MCU:-atmega328p}"
FREQ="${FREQ:-16000000}"
TIMEOUT="${TIMEOUT:-60}"

UPDATE="${UPDATE:-0}"
if [ "${FIRMWARE}" = "--update" ]; then
    UPDATE=1
    FIRMWARE=".pio/build/uno_sim/firmware.elf"
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
GOLDEN="${SCRIPT_DIR}/uno-simavr.txt"
MARKER="TESTS COMPLETE"

if [ ! -f "${FIRMWARE}" ]; then
    echo "ERROR: firmware not found: ${FIRMWARE}" >&2
    echo "Build it first, e.g. with: pio run -e uno_sim" >&2
    exit 1
fi

TMP_RAW="$(mktemp)"
TMP_CLEAN="$(mktemp)"
cleanup() {
    [ -n "${SIM_PID:-}" ] && kill "${SIM_PID}" >/dev/null 2>&1
    rm -f "${TMP_RAW}" "${TMP_CLEAN}"
}
trap cleanup EXIT

# Run simavr in the background, capturing stdout+stderr to a temp file, then poll
# that file for the completion marker with a deadline.
"${SIMAVR}" -m "${MCU}" -f "${FREQ}" "${FIRMWARE}" >"${TMP_RAW}" 2>&1 &
SIM_PID=$!

found=0
deadline=$(( SECONDS + TIMEOUT ))
while [ "${SECONDS}" -lt "${deadline}" ]; do
    if ! kill -0 "${SIM_PID}" >/dev/null 2>&1; then
        # simavr exited on its own; check whether the marker was printed
        if grep -q "${MARKER}" "${TMP_RAW}"; then
            found=1
        fi
        break
    fi
    if grep -q "${MARKER}" "${TMP_RAW}"; then
        found=1
        kill "${SIM_PID}" >/dev/null 2>&1
        break
    fi
    sleep 1
done

# Ensure the simulator is stopped and reaped before we read the capture.
kill "${SIM_PID}" >/dev/null 2>&1
wait "${SIM_PID}" 2>/dev/null
SIM_PID=""

# Clean the capture: strip ANSI color codes and keep only our trace lines.
sed -e 's/\x1b\[[0-9;]*m//g' "${TMP_RAW}" \
    | grep '^sim: ' \
    > "${TMP_CLEAN}"

if [ "${found}" -ne 1 ]; then
    echo "ERROR: completion marker '${MARKER}' never appeared (crash, hang, or timeout after ${TIMEOUT}s)." >&2
    echo "----- captured output -----" >&2
    sed -e 's/\x1b\[[0-9;]*m//g' "${TMP_RAW}" >&2
    exit 1
fi

if [ "${UPDATE}" -eq 1 ]; then
    cp "${TMP_CLEAN}" "${GOLDEN}"
    echo "Updated golden file: ${GOLDEN}"
    cat "${GOLDEN}"
    exit 0
fi

if [ ! -f "${GOLDEN}" ]; then
    echo "ERROR: golden file not found: ${GOLDEN}" >&2
    echo "Run with --update first to generate it." >&2
    exit 1
fi

# Normalize: strip digits so ADC readings/timings that drift with toolchain
# versions don't break the comparison, while verdict flips still do.
normalize() {
    sed 's/[0-9][0-9]*//g' "$1"
}

NORM_CAPTURE="$(normalize "${TMP_CLEAN}")"
NORM_GOLDEN="$(normalize "${GOLDEN}")"

if [ "${NORM_CAPTURE}" = "${NORM_GOLDEN}" ]; then
    count="$(printf '%s\n' "${NORM_CAPTURE}" | wc -l | tr -d ' ')"
    echo "PASS: ${count} trace lines match the golden baseline."
    exit 0
fi

echo "FAIL: simavr output does not match the golden baseline (${GOLDEN})."
echo "----- diff (golden vs capture, digits stripped) -----"
diff <(printf '%s\n' "${NORM_GOLDEN}") <(printf '%s\n' "${NORM_CAPTURE}")
echo "----- captured output -----"
cat "${TMP_CLEAN}"
exit 1
