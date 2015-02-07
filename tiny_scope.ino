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
AVR_ADC adc = AVR_ADC(ADC_PIN);

#define DISPLAY_I2C_ADDRESS 0x3C
extern Display display;

Capture capture = Capture(adc.input, SCREEN_WIDTH, VOLTS_RANGE * 1000);

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
  display.printf(F("ADC CLOCK %lu KHz\n"), adc.getClock());
  display.printf(F("Sample Rate %lu KHz\n"), adc.getSampleRate());
  
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
  // set up PWM outputs for testing
  analogWrite(9, 64); // 500Hz 0.5ms pulse,  2ms period
  analogWrite(5, 64); // 1KHz  0.25ms pulse, 1ms period
}

void loop(){
    static Scope scope = Scope(display);

    if (!capture.samples){
        delay(10000);
        return;
    }
    
    capture.capture();
    capture.tomV();
  
    display.clearDisplay();
    // Enable voltmeter mode at low frequency
    if (adc.getSampleRate() < 10000 && scope.isFlatLine(capture)){
        scope.displayVoltMeter(capture);
    } else {
        scope.displayScope(capture);
    }
    display.display();

    delay(100);
}
