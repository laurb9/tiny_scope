Tiny Scope for Arduino
======================

Requires: Arduino UNO R3 and 0.95" I2C OLED screen (128x64)

The screenshots below are taken using a bicolor display (top yellow, rest blue). When set to 5V, the voltage changes color to yellow above 3.45V which I find useful.

Default ADC settings on the UNO, read speed is about 9KHz.

<img src="http://www.wotevah.com/tiny_scope/images/uno_splash_default.jpg" 
     width="162" alt="Arduino UNO R3, default ADC settings">
<img src="http://www.wotevah.com/tiny_scope/images/uno_default_2ms.jpg" 
     width="162" alt="Arduino UNO R3, default ADC settings, 488Hz signal">

Fastest ADC settings on UNO. At 16MHz I get a read speed of 147KHz so about half the sampling rate.

<img src="http://www.wotevah.com/tiny_scope/images/uno_splash_fastest.jpg" 
     width="162" alt="Arduino UNO R3, fastest ADC settings">
<img src="http://www.wotevah.com/tiny_scope/images/uno_fastest_01ms.jpg" 
     width="162" alt="Arduino UNO R3, fastest ADC settings, 1KHz signal">

Voltmeter mode and random noise.

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
