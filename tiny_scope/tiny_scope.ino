/*
 * tiny_scope.ino - main application
 * Tiny Scope for Arduino project
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>
#if defined(ARDUINO_ARCH_SAMD)
// SAMD has no EEPROM: last used ADC mode is not persisted across resets
class {
public:
    int read(int){ return 0; };
    void write(int, byte){ };
} EEPROM;
#else
#include <EEPROM.h>
#endif
#include "adc.h"
#include "scope.h"
#include "capture.h"

/****************************************************************************
 * Configurable parameters
 */

// Which analog input to read the data from.
#define ADC_PIN 1

// Which digital input the button is on - use to cycle through all the ADC modes
#if defined(ARDUINO_ARCH_SAMD)
#define MODE_BUTTON_PIN 6
#else
#define MODE_BUTTON_PIN 7
#endif

// ADC reference voltage (mV). Default 5000 for AVR 5V, 3300 for 3.3V boards
// Change this if AREF is connected to a different voltage reference
#define AREF_MV ADC_AREF_MV

// Address of I2C OLED display (128x64 SSD1306, see display.h)
#define DISPLAY_I2C_ADDRESS 0x3C

// Enable PWM outputs on pins 9 and 5 for testing (see setup()). Comment out to disable.
#define ENABLE_PWM

// How long to show the splash screen (ms). Overridden for emulator runs.
#ifndef SPLASH_DELAY_MS
#define SPLASH_DELAY_MS 4000
#endif

// Samples per capture: default one per pixel column. The simavr test build
// overrides this - its serial trace buffers use the RAM headroom this needs.
#ifndef CAPTURE_SAMPLES
#define CAPTURE_SAMPLES SCREEN_WIDTH
#endif

/*
 * End Configurable parameters
 ****************************************************************************/

// Address to start last ADC mode used in EEPROM.
#define ADC_MODE_ADDR 0

extern Display display;
static Capture capture;

/*
 * Display splash screen
 */
void displaySplash(){
    display.clearDisplay();
    display.setTextColor(WHITE);

    display.setTextSize(2);
    display.print(F("Tiny Scope\n"));

    display.setTextSize(1);
    display.printf(F("\nINPUT %d VREF "), capture.adc.input);
    display.printSmallUnits(1000L*capture.rangemV, "V\n"); // printSmallUnits expects micro[V]
    if (capture.adc.getClock()){
        display.print(F("ADC CLOCK "));
        display.printLargeUnits(capture.adc.getClock(), "Hz\n");
        display.print(F("Sample Rate "));
        display.printLargeUnits(capture.adc.getSampleRate(), "Hz\n");
    }
    display.display();
}

void setup(){
#ifdef SIMAVR_TEST
    Serial.begin(115200);
#endif
    delay(100);  // give time for display to init; if display blank increase delay
    // begin() allocates the 1KB framebuffer: do it first, while the heap is
    // empty, and stop here if it fails - drawing without a buffer corrupts memory
    if (!display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS)){
#ifdef SIMAVR_TEST
        Serial.println(F("sim: display.begin FAIL"));
#endif
        for (;;);
    }
    display.setRotation(2);
    ADCInput adc = ADCInput();
    adc.init(ADC_PIN, EEPROM.read(ADC_MODE_ADDR));
    uint16_t aref = adc.calibrateAREF();
    int success = capture.init(adc, CAPTURE_SAMPLES, (aref) ? aref : AREF_MV);
    // Configure capture one sample per pixel (SCREEN_WIDTH samples)
    displaySplash();
    delay(SPLASH_DELAY_MS);
    if (success){
        display.print(F("Reading A/D data..."));
    } else {
        display.print(F("Out of memory!"));
    }
    display.display();

#ifdef ENABLE_PWM
    /* 
     * set up PWM outputs for testing: 
     * Read http://playground.arduino.cc/Code/PwmFrequency on influence on millis(), delay() etc
     * short: we should really only change 9 or 10 base freq
     * pins 3, 9, 10, and 11: base 31250Hz / 64 = 488Hz
     * pins 5, 6: base 62500 Hz / 64 = 976Hz
     */
    pinMode(9, OUTPUT);
    pinMode(5, OUTPUT);
    analogWrite(9, 64); // 488Hz 0.512ms pulse, 2.05ms period
    analogWrite(5, 64); // 976Hz 0.255ms pulse, 1.02ms period
#endif

    /*
     * Enable ADC mode button
     */
    pinMode(MODE_BUTTON_PIN, INPUT);
    digitalWrite(MODE_BUTTON_PIN, HIGH);

#ifdef SIMAVR_TEST
    // Trace state over serial so test/simavr-run.sh can verify boot (no-op in normal builds)
    Serial.println(F("sim: display OK"));
    Serial.print(F("sim: init "));
    Serial.println(success ? F("OK") : F("FAIL"));
    Serial.print(F("sim: vref mV="));
    Serial.println(capture.rangemV);
    Serial.print(F("sim: adc modes="));
    Serial.println(capture.adc.getModeCount());
    Serial.print(F("sim: sample rate Hz="));
    Serial.println(capture.adc.getSampleRate());
#endif
}

/*
 * Cycle through the ADC modes with each button push.
 */
void setADCMode(){
    int adcMode = EEPROM.read(ADC_MODE_ADDR);
    unsigned count = 0;
    
    if (digitalRead(MODE_BUTTON_PIN) == LOW){
        // Freeze display while button stays pressed
        while (digitalRead(MODE_BUTTON_PIN) == LOW){
            delay(50);
            count++;
        }
        // Momentary push (0.5s) means adc mode switch; long push freeze only
        if (count < 10){
            adcMode = (adcMode+1) % capture.adc.getModeCount();
            capture.adc.setMode(adcMode);
            EEPROM.write(ADC_MODE_ADDR, byte(adcMode));
        }
    }
}

void loop(){
    static Scope scope = Scope(display, 0, SCREEN_WIDTH, 0, 47);

    if (!capture.samples){
        // leave "Out of memory" printed by setup() onscreen.
        delay(10000);
        return;
    }

    capture.capture();
    capture.tomV();

    display.clearDisplay();
    // Enable voltmeter mode if line is flat
    if (scope.isFlatLine(capture)){
        scope.displayVoltMeter(capture);
    } else {
        scope.displayScope(capture);
    }
    display.display();

#ifdef SIMAVR_TEST
    static uint8_t traceLoops = 0;
    if (traceLoops < 5 && ++traceLoops == 5){
        Serial.print(F("sim: capture samples="));
        Serial.println(capture.samples);
        Serial.print(F("sim: flatline "));
        Serial.println(scope.isFlatLine(capture) ? F("YES") : F("NO"));
        Serial.println(F("sim: TESTS COMPLETE"));
    }
#endif

    setADCMode();
    // displaying at max 20fps
    delay(50);
}
