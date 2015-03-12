/*
 * tiny_scope.ino - main application
 * Tiny Scope for Arduino project
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#if 0 /* add all necessary libs here or they will not be found on the path */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ADC.h>
#endif
#include <Arduino.h>
#include <EEPROM.h>
#include "adc.h"
#include "scope.h"
#include "capture.h"

/****************************************************************************
 * Configurable parameters
 */

// Which analog input to read the data from.
#define ADC_PIN 1

// Which digital input the button is on - use to cycle through all the ADC modes
#define MODE_BUTTON_PIN 7

// ADC reference voltage (mV). Default 5000 for AVR 5V, 3300 for Teensy3
// Change this if AREF is connected to a different voltage reference
#define AREF_MV ADC_AREF_MV

// Address of I2C OLED display. If screen looks scaled edit Adafruit_SSD1306.h
// and pick SSD1306_128_64 or SSD1306_128_32 that matches display type.
#define DISPLAY_I2C_ADDRESS 0x3C

// Enable PWM outputs on pins 9 and 5 for testing (see setup()). Comment out to disable.
#define ENABLE_PWM

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
    display.print(F("Tiny Scope"));

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
    ADCInput adc = ADCInput();
    adc.init(ADC_PIN, EEPROM.read(ADC_MODE_ADDR));
    uint16_t aref = adc.calibrateAREF();
    int success = capture.init(adc, SCREEN_WIDTH, (aref) ? aref : AREF_MV);
    delay(100);  // give time for display to init; if display blank increase delay
    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS);
    display.setRotation(2);
    // Configure capture one sample per pixel (SCREEN_WIDTH samples)
    displaySplash();
    delay(4000);
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

    setADCMode();
    // displaying at max 20fps
    delay(50);
}
