Tiny Scope for Arduino
======================

Requires: <a href="http://arduino.cc/en/Main/ArduinoBoardUno">Arduino UNO R3</a>
and <a href="http://www.adafruit.com/products/326">Monochrome 0.96" 128x64 OLED graphic display</a> (SSD1306 driver)

The screenshots below are on a two-color display (top yellow, rest blue). For a 5V system, the dots change to yellow above 3.45V, which may be handy.

Default ADC settings on the UNO watching a 490Hz PWM (pin 9). The actual read speed is about 9KHz even though sampling rate is 10KHz.

<img src="http://www.wotevah.com/tiny_scope/images/uno_splash_default.jpg" 
     width="162" alt="Arduino UNO R3, default ADC settings">
<img src="http://www.wotevah.com/tiny_scope/images/uno_default_2ms.jpg" 
     width="162" alt="Arduino UNO R3, default ADC settings, 488Hz signal">

Fastest ADC settings on UNO (16MHz). I get a read speed of 147KHz, so about half the sampling rate. The signal displayed is a 980Hz PWM with 25% duty cycle (pin 5).

<img src="http://www.wotevah.com/tiny_scope/images/uno_splash_fastest.jpg" 
     width="162" alt="Arduino UNO R3, fastest ADC settings">
<img src="http://www.wotevah.com/tiny_scope/images/uno_fastest_01ms.jpg" 
     width="162" alt="Arduino UNO R3, fastest ADC settings, 1KHz signal">

Voltmeter mode is automatically switched on with a steady voltage signal (when it would be represented by a flat line). Last picture is random noise picked up by the test lead.

<img src="http://wotevah.com/tiny_scope/images/voltmeter_3.jpg" 
     width="162" alt="Voltmeter mode">
<img src="http://wotevah.com/tiny_scope/images/noise_2.jpg" 
     width="162" alt="Noise">

Connections
===========
- OLED VCC   - UNO 3.3V
- OLED GND   - UNO GND
- OLED SCL   - UNO A5 or SCL
- OLED SDA   - UNO A4 or SDA
- PROBE WIRE - UNO A1 (defined in tiny_scope.ino)

Testing
=======
- A1 to 5V should display 5.00 V
- A1 to GND should display 0.00 V
- A1 to D9 should display a square wave (PWM) with >2ms period and >0.5ms pulse
- A1 to D5 should display a square wave (PWM) with >1ms period and >0.2ms pulse

Entire project
==============

<img src="http://www.wotevah.com/tiny_scope/images/tiny_scope_on_arduino.jpg" alt="Tiny Scope running on UNO R3" width="200">

Parts
=====
- <a href="https://www.sparkfun.com/products/11021">Arduino UNO R3</a> or <a href="http://www.adafruit.com/category/17?&main_page=index&cPath=17">Other Arduino board</a>
- <a href="http://www.amazon.com/Yellow-Serial-128X64-Display-Arduino/dp/B00O2LLT30">0.96" OLED display</a> with SSD1306 and I2C mode (that's what I tried. SPI mode is also supported by the Adafruit library but wiring is different). <a href="http://www.adafruit.com/products/938">Many stores</a> sell these and in different colors.
- <a href="http://arduino.cc/en/Main/Software">Arduino IDE</a>
- type B USB cable, few jumper wires
