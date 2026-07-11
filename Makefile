# Default build architecture and board
TARGET ?= arduino:avr:uno
CORE = $(shell echo $(TARGET) | cut -d: -f1,2)

# Libraries needed for this application
LIBS ?= "Adafruit SSD1306" "Adafruit GFX Library"

# Where to save the Arduino support files, this should match what is in arduino-cli.yaml
ARDUINO_DIR ?= .arduino

default:
	#################################################################################################
	# Initial setup: make setup
	#
	# Build the sketch: make all TARGET=arduino:avr:uno
	# Other targets: teensy:avr:teensy31, teensy:avr:teensyLC, adafruit:samd:adafruit_feather_m0
	#
	# The core for TARGET is installed automatically on build.
	# (edit arduino-cli.yaml and add its board index repository if needed)
	#################################################################################################

# See https://arduino.github.io/arduino-cli/installation/
# Detect OS and architecture and use the appropriate arduino-cli build
OS := $(shell uname -s)
ARCH := $(shell uname -m)
ifeq ($(OS),Darwin)
	ifeq ($(ARCH),arm64)
		ARDUINO_CLI_URL = https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_macOS_ARM64.tar.gz
	else
		ARDUINO_CLI_URL = https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_macOS_64bit.tar.gz
	endif
else ifeq ($(ARCH),aarch64)
	ARDUINO_CLI_URL = https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_ARM64.tar.gz
else
	ARDUINO_CLI_URL = https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz
endif
ARDUINO_CLI ?= $(ARDUINO_DIR)/arduino-cli --config-file arduino-cli.yaml

all: # Build all sketches
all: tiny_scope.hex
	ls -l build

%.hex: # Generic rule for compiling sketch to uploadable hex file
%.hex: tiny_scope/%.ino core
	$(ARDUINO_CLI) compile --warnings all --fqbn $(TARGET) --output-dir build tiny_scope

# Remove built objects
clean:
	rm -rfv build

core: $(ARDUINO_DIR)/arduino-cli
	$(ARDUINO_CLI) core install $(CORE)

$(ARDUINO_DIR)/arduino-cli:  # Download and install arduino-cli
$(ARDUINO_DIR)/arduino-cli:
	mkdir -p $(ARDUINO_DIR)
	curl -L -s $(ARDUINO_CLI_URL) \
	| tar xfz - -C $(ARDUINO_DIR) arduino-cli
	chmod 755 $@
	$(ARDUINO_CLI) version

setup: # Configure cores and libraries for arduino-cli (which it will download if missing)
setup: $(ARDUINO_DIR)/arduino-cli
	$(ARDUINO_CLI) config dump
	$(ARDUINO_CLI) core update-index
	$(ARDUINO_CLI) core list
	for lib in $(LIBS); do \
		$(ARDUINO_CLI) lib install "$${lib}" || exit 1 ; \
	done

setup-pio: # Install PlatformIO and dependencies
	pip3 install -U platformio intelhex

sim-test: # Run the firmware under simavr and compare to the golden baseline
	pio run -e uno_sim
	test/simavr-run.sh

sim-test-update: # Run under simavr and regenerate the golden baseline
	pio run -e uno_sim
	test/simavr-run.sh --update

# simavr display test: renders the emulated SSD1306 framebuffer to ASCII art
# and compares it pixel-for-pixel to the golden baseline. Needs libsimavr-dev
# and libelf-dev (in the devcontainer). ssd1306_virt.c is GPLv3 test tooling
# vendored from simavr - see test/sim_display/README.md.
SIM_DISPLAY_INC ?= /usr/include/simavr
SIM_DISPLAY_DIR = test/sim_display

# Our runner is held to -Wall -Wextra; the vendored simavr part needs
# -Wno-unused-parameter (its IRQ hooks ignore the irq/param args), so compile
# the two as separate objects.
$(SIM_DISPLAY_DIR)/sim_display: $(SIM_DISPLAY_DIR)/sim_display.c $(SIM_DISPLAY_DIR)/ssd1306_virt.c $(SIM_DISPLAY_DIR)/ssd1306_virt.h
	gcc -O2 -Wall -Wextra -I$(SIM_DISPLAY_INC) -c -o $(SIM_DISPLAY_DIR)/sim_display.o $(SIM_DISPLAY_DIR)/sim_display.c
	gcc -O2 -Wall -Wextra -Wno-unused-parameter -I$(SIM_DISPLAY_INC) -c -o $(SIM_DISPLAY_DIR)/ssd1306_virt.o $(SIM_DISPLAY_DIR)/ssd1306_virt.c
	gcc -o $@ $(SIM_DISPLAY_DIR)/sim_display.o $(SIM_DISPLAY_DIR)/ssd1306_virt.o -lsimavr -lelf

sim-display-test: # Render the emulated OLED framebuffer and compare to the golden baseline
sim-display-test: test/sim_display/sim_display
	pio run -e uno_sim
	test/sim-display-run.sh

sim-display-test-update: # Render the emulated OLED framebuffer and regenerate the golden baseline
sim-display-test-update: test/sim_display/sim_display
	pio run -e uno_sim
	test/sim-display-run.sh --update

.PHONY: clean %.hex all core setup setup-pio sim-test sim-test-update \
	sim-display-test sim-display-test-update
