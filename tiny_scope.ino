/*
 * Tiny Scope for Arduino
 * (C)2015 Laurentiu Badea
 */
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
// Auto Voltmeter Mode 
#define AUTO_VOLTMETER 1

#define DISPLAY_I2C_ADDRESS 0x3C

extern Display display;

/*
 * Display splash screen
 */
void displaySplash(byte adc_pin){
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  display.setTextSize(2);
  display.print(F("Tiny Scope"));

  display.setTextSize(1);
  display.printf(F("\nINPUT A%d\n\n"), adc_pin);
  display.printf(F("ADC CLOCK %lu KHz\n"), F_CPU/(1<<ADC_PRESCALER)/1000);
  display.printf(F("PRESCALER 1:%d\n"), 1<<ADC_PRESCALER);
  
  display.display();
  delay(4000);
}

void setup(){
  setupADC(ADC_PRESCALER);
  display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_I2C_ADDRESS);
  displaySplash(ADC_PIN);
  display.print(F("Reading A/D data..."));
  display.display();
  // set up PWM outputs for testing
  analogWrite(9, 64); // 500Hz 0.5ms pulse 
  analogWrite(5, 64); // 1KHz  0.25ms pulse
}

Capture capture = Capture(ADC_PIN, SCREEN_WIDTH, VOLTS_RANGE * 1000);
void loop(){
    static Scope scope = Scope(display);

    if (!capture.data){
        display.clearDisplay();
        display.print(F("Out of memory!"));
        display.display();
        delay(1000);
        return;
    }
    capture.capture();
    capture.tomV();
  
    display.clearDisplay();
    if (ADC_PRESCALER >= 6 && scope.isFlatLine(capture)){
        scope.displayVoltMeter(capture);
    } else {
        scope.displayScope(capture);
    }
    display.display();

    delay(200);
}