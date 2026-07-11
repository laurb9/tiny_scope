/*
 * sim_display.c - Run tiny_scope firmware on a simulated ATmega328P with a
 * virtual SSD1306 OLED attached over TWI (I2C), then dump the display's VRAM
 * as ASCII art for a pixel-exact golden comparison.
 *
 * Usage:
 *   sim_display <firmware.elf> <output.txt> [--timeout <sim-seconds>]
 *
 * The firmware (built as the PlatformIO `uno_sim` env) prints
 * "sim: TESTS COMPLETE" on UART0 right after its 5th display.display(), so we
 * step the CPU until that marker appears and snapshot the framebuffer then -
 * this makes the rendered output deterministic. A crash or timeout dumps the
 * captured UART text to stderr and exits nonzero.
 *
 * The virtual display part (ssd1306_virt.c/.h) is vendored from simavr's
 * examples and is GPLv3 - see README.md. This file is test tooling only and is
 * never linked into the MIT-licensed firmware.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sim_avr.h"
#include "sim_elf.h"
#include "sim_core.h"
#include "avr_uart.h"

#include "ssd1306_virt.h"

#define DEFAULT_MCU        "atmega328p"
#define DEFAULT_FREQ       16000000
#define DEFAULT_TIMEOUT_S  30
#define MARKER             "sim: TESTS COMPLETE"

/* OLED reset pin as wired by the firmware (scope.cpp: OLED_RESET 4 -> PD4). */
#define OLED_RESET_PORT    'D'
#define OLED_RESET_PIN     4

/* Ring-free growable capture of UART0 output so we can scan for the marker. */
static char   uart_text[8192];
static size_t uart_len = 0;
static int    marker_seen = 0;

static void
uart_out_hook (struct avr_irq_t *irq, uint32_t value, void *param)
{
	(void) irq;
	(void) param;
	if (uart_len < sizeof(uart_text) - 1) {
		uart_text[uart_len++] = (char) value;
		uart_text[uart_len] = '\0';
		if (!marker_seen && strstr(uart_text, MARKER))
			marker_seen = 1;
	}
}

/*
 * Dump the display framebuffer to fp: 64 rows of 128 chars, '#' lit, '.' dark.
 * VRAM is pages x columns, one bit per pixel vertically. The firmware applies
 * setRotation(2) in software before writing VRAM, so we dump it verbatim.
 */
static void
dump_vram (FILE *fp, ssd1306_t *oled)
{
	for (int y = 0; y < 64; y++) {
		for (int x = 0; x < 128; x++) {
			int bit = (oled->vram[y / 8][x] >> (y % 8)) & 1;
			fputc(bit ? '#' : '.', fp);
		}
		fputc('\n', fp);
	}
}

int
main (int argc, char *argv[])
{
	const char *firmware_path = NULL;
	const char *output_path = NULL;
	long timeout_s = DEFAULT_TIMEOUT_S;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--timeout") == 0 && i + 1 < argc) {
			timeout_s = strtol(argv[++i], NULL, 10);
		} else if (!firmware_path) {
			firmware_path = argv[i];
		} else if (!output_path) {
			output_path = argv[i];
		}
	}

	if (!firmware_path || !output_path) {
		fprintf(stderr,
		        "usage: %s <firmware.elf> <output.txt> [--timeout <sim-seconds>]\n",
		        argv[0]);
		return 2;
	}

	elf_firmware_t fw;
	memset(&fw, 0, sizeof(fw));
	if (elf_read_firmware(firmware_path, &fw) < 0) {
		fprintf(stderr, "ERROR: could not read firmware: %s\n", firmware_path);
		return 2;
	}

	const char *mcu = fw.mmcu[0] ? fw.mmcu : DEFAULT_MCU;
	uint32_t freq = fw.frequency ? fw.frequency : DEFAULT_FREQ;

	avr_t *avr = avr_make_mcu_by_name(mcu);
	if (!avr) {
		fprintf(stderr, "ERROR: unknown MCU: %s\n", mcu);
		return 2;
	}
	avr_init(avr);
	avr->frequency = freq;
	avr_load_firmware(avr, &fw);

	/* Capture UART0 output and stop simavr echoing it to our stdout. */
	avr_irq_t *uart_out =
	        avr_io_getirq(avr, AVR_IOCTL_UART_GETIRQ('0'), UART_IRQ_OUTPUT);
	if (uart_out)
		avr_irq_register_notify(uart_out, uart_out_hook, NULL);

	uint32_t uart_flags = 0;
	avr_ioctl(avr, AVR_IOCTL_UART_GET_FLAGS('0'), &uart_flags);
	uart_flags &= ~AVR_UART_FLAG_STDIO;
	avr_ioctl(avr, AVR_IOCTL_UART_SET_FLAGS('0'), &uart_flags);

	/* Attach the virtual SSD1306 over TWI (connect_twi wires the TWI IRQs and
	 * the reset pin itself, given part->avr set by ssd1306_init). */
	ssd1306_t oled;
	ssd1306_init(avr, &oled, 128, 64);
	ssd1306_wiring_t wiring;
	memset(&wiring, 0, sizeof(wiring));
	wiring.reset.port = OLED_RESET_PORT;
	wiring.reset.pin = OLED_RESET_PIN;
	ssd1306_connect_twi(&oled, &wiring);

	/* Deadline in CPU cycles. */
	uint64_t deadline = (uint64_t) freq * (uint64_t) timeout_s;

	int state = cpu_Running;
	while (!marker_seen && avr->cycle < deadline) {
		state = avr_run(avr);
		if (state == cpu_Done || state == cpu_Crashed)
			break;
	}

	if (!marker_seen) {
		fprintf(stderr,
		        "ERROR: marker '%s' never appeared (state=%d, cycle=%llu, timeout=%lds).\n",
		        MARKER, state, (unsigned long long) avr->cycle, timeout_s);
		fprintf(stderr, "----- captured UART output -----\n%s\n", uart_text);
		return 1;
	}

	FILE *fp = fopen(output_path, "w");
	if (!fp) {
		fprintf(stderr, "ERROR: cannot open output file: %s\n", output_path);
		return 2;
	}
	dump_vram(fp, &oled);
	fclose(fp);

	return 0;
}
