[![Build Status](https://travis-ci.com/laurb9/tiny_scope.svg?branch=master)](https://travis-ci.com/laurb9/tiny_scope)

Tiny Scope for Arduino
======================

Features
========
- Configurable ADC speed for sampling rates from 10KHz to 300KHz (on UNO)
- Auto grid on time base from 2ms to 50us (on UNO)
- Vmax calibration and 3.3-5V detection using internal reference
- 0V auto trigger
- Display minimum-maximum voltage of captured signal
- Square waveforms are displayed properly
- Auto voltmeter mode when input is steady

The screenshots below are on a two-color display (yellow/blue).

<img src="http://www.wotevah.com/tiny_scope/images/arduino_default_splash.jpg" 
     width="162" alt="Arduino UNO R3, default ADC settings">
<img src="http://www.wotevah.com/tiny_scope/images/arduino_default_scope.jpg" 
     width="162" alt="Arduino UNO R3, default ADC settings, sine wave">

<img src="http://www.wotevah.com/tiny_scope/images/arduino_fast_splash.jpg" 
     width="162" alt="Arduino UNO R3, fastest ADC settings">
<img src="http://www.wotevah.com/tiny_scope/images/arduino_fast_scope.jpg" 
     width="162" alt="Arduino UNO R3, fastest ADC settings, 255us PWM pulse">

Voltmeter mode is automatically switched on with a steady voltage signal (when it would be represented by a flat line).

<img src="http://wotevah.com/tiny_scope/images/voltmeter_3.jpg" 
     width="162" alt="Voltmeter mode">

Requires: <a href="http://arduino.cc/en/Main/ArduinoBoardUno">Arduino UNO R3</a> or <a href="http://www.pjrc.com/teensy/index.html">Teensy USB</a> board
and <a href="http://www.adafruit.com/products/326">Monochrome 0.96" 128x64 OLED graphic display</a> (SSD1306 driver)

Connections
===========
- OLED VCC   - 3.3V
- OLED GND   - GND
- OLED SCL   - A5 or SCL (Teensy SCL0 pin 19)
- OLED SDA   - A4 or SDA (Teensy SDA0 pin 18)
- PROBE WIRE - A1 (defined in tiny_scope.ino) (Teensy pin 15)
- BUTTON     - D7 button to GND

Testing
=======
- A1 to 5V should display 5.00 V
- A1 to GND should display 0.00 V
- A1 to D9 (Teensy pin 9) should display a square wave (PWM) with >2ms period and >0.5ms pulse
- A1 to D5 should display a square wave (PWM) with >1ms period and >0.2ms pulse
- Momentary (<0.5s) push on D7 switch cycles through available sampling speeds
- Long >0.5s push on D7 freezes display.

Entire project
==============

UNO R3 and Teensy 3.1 (with upside-down display mode).

<img src="http://www.wotevah.com/tiny_scope/images/tiny_scope_on_arduino.jpg" alt="Tiny Scope running on UNO R3" width="200">
<img src="http://www.wotevah.com/tiny_scope/images/tiny_scope_on_teensy31.jpg" alt="Tiny Scope running on Teensy 3.1" width="200">

Parts
=====
- Arduino board:
  - <a href="https://www.sparkfun.com/products/11021">Arduino UNO R3</a>, 
    <a href="https://www.sparkfun.com/products/11113">Arduino Pro Mini</a>,
    <a href="https://www.adafruit.com/products/2000">Arduino Pro Trinket</a> or
    <a href="http://www.adafruit.com/category/17?&main_page=index&cPath=17">Other Arduino Atmega328 board</a>,
    or 
  - <a href="http://www.pjrc.com/teensy/index.html">Teensy 3</a> (less expensive and more powerful)
- <a href="http://www.amazon.com/Yellow-Serial-128X64-Display-Arduino/dp/B00O2LLT30">0.96" OLED display</a> with SSD1306 and I2C mode (that's what I tried. SPI mode is also supported by the Adafruit library but wiring is different). <a href="http://www.adafruit.com/products/938">Many stores</a> sell these and in different colors.
- <a href="http://arduino.cc/en/Main/Software">Arduino IDE</a>
- type B USB cable, few jumper wires

Libraries
=========
Either use Sketch -> Import Library or make a libraries folder and drop them in there.
- Adafruit_SSD1306 from https://github.com/adafruit/Adafruit_SSD1306
  - Must edit Adafruit_SSD1306.h to pick the set display type to SSD1306_128_64
- Adafruit_GFX from https://github.com/adafruit/Adafruit-GFX-Library
