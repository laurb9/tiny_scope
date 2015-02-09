Tiny Scope for Arduino
======================

Requires: Arduino UNO R3 and 0.95" I2C OLED screen

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
