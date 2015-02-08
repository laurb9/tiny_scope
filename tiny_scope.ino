/*
 * Tiny Scope for Arduino
 * (C)2015 Laurentiu Badea
 */
#include <Arduino.h>
#if 0
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#endif
#include "adc.h"
#include "scope.h"
#include "capture.h"

// Which analog input to read
#define ADC_PIN 1
AVR_ADC adc = AVR_ADC(ADC_PIN);

#define DISPLAY_I2C_ADDRESS 0x3C
extern Display display;

Capture capture = Capture(adc, SCREEN_WIDTH, VOLTS_RANGE * 1000);

/*
 * Display splash screen
 */
void displaySplash(){
    display.clearDisplay();
    display.setTextColor(WHITE);

    display.setTextSize(2);
    display.print(F("Tiny Scope"));

    display.setTextSize(1);
    display.printf(F("\nINPUT A%d\n\n"), adc.input);
    display.print(F("ADC CLOCK "));
    display.printLargeUnits(adc.getClock(), "Hz\n");
    display.print(F("Sample Rate "));
    display.printLargeUnits(adc.getSampleRate(), "Hz\n");

    display.display();
    delay(4000);
}

void setup(){
    display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS);
    adc.setMode(0);
    displaySplash();
    if (capture.init()){
        display.print(F("Reading A/D data..."));
    } else {
        display.print(F("Out of memory!"));
    }
    display.display();
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
}

void loop(){
    static Scope scope = Scope(display, 0, SCREEN_WIDTH, 0, 50);

    if (!capture.samples){
        delay(10000);
        return;
    }

    capture.capture();
    capture.tomV();

    display.clearDisplay();
    // Enable voltmeter mode at < 10KHz
    if (adc.getSampleRate() < 10000 && scope.isFlatLine(capture)){
        scope.displayVoltMeter(capture);
    } else {
        scope.displayScope(capture);
    }
    display.display();

    delay(100);
}
