Tiny Scope for Arduino
======================

Requires: <a href="http://arduino.cc/en/Main/ArduinoBoardUno">Arduino UNO R3</a> or <a href="http://www.pjrc.com/teensy/index.html">Teensy USB</a> board
and <a href="http://www.adafruit.com/products/326">Monochrome 0.96" 128x64 OLED graphic display</a> (SSD1306 driver)

Features
========
- Configurable ADC speed for sampling rates from 10KHz to 200KHz (on UNO)
- Auto grid on time base from 2ms to 0.1ms (on UNO)
- 0V trigger
- Display minimum-maximum voltage of captured signal
- "Logic" mode shows square waveforms properly
- Auto voltmeter mode when input is steady

The screenshots below are on a two-color display (yellow/blue).

<img src="http://www.wotevah.com/tiny_scope/images/uno_splash_default.jpg" 
     width="162" alt="Arduino UNO R3, default ADC settings">
<img src="http://www.wotevah.com/tiny_scope/images/uno_default_2ms.jpg" 
     width="162" alt="Arduino UNO R3, default ADC settings, sine wave">

<img src="http://www.wotevah.com/tiny_scope/images/uno_splash_fastest.jpg" 
     width="162" alt="Arduino UNO R3, fastest ADC settings">
<img src="http://www.wotevah.com/tiny_scope/images/uno_fastest_01ms.jpg" 
     width="162" alt="Arduino UNO R3, fastest ADC settings, 1KHz signal">

Voltmeter mode is automatically switched on with a steady voltage signal (when it would be represented by a flat line).

<img src="http://wotevah.com/tiny_scope/images/voltmeter_3.jpg" 
     width="162" alt="Voltmeter mode">

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
- <a href="https://www.sparkfun.com/products/11021">Arduino UNO R3</a>, <a href="http://www.adafruit.com/category/17?&main_page=index&cPath=17">Other Arduino board</a> or <a href="http://www.pjrc.com/teensy/index.html"</a> (less expensive but much more powerful)
- <a href="http://www.amazon.com/Yellow-Serial-128X64-Display-Arduino/dp/B00O2LLT30">0.96" OLED display</a> with SSD1306 and I2C mode (that's what I tried. SPI mode is also supported by the Adafruit library but wiring is different). <a href="http://www.adafruit.com/products/938">Many stores</a> sell these and in different colors.
- <a href="http://arduino.cc/en/Main/Software">Arduino IDE</a>
- type B USB cable, few jumper wires

Libraries
=========
Either use Sketch -> Import Library or make a libraries folder and drop them in there.
- Adafruit_SSD1306 from https://github.com/adafruit/Adafruit_SSD1306
  - Must edit Adafruit_SSD1306.h to pick the set display type to SSD1306_128_64
- Adafruit_GFX from https://github.com/adafruit/Adafruit-GFX-Library
