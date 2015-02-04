/*
 * setup high frequency DAC
 * (C)2015 Laurentiu Badea
 * http://forum.arduino.cc/index.php/topic,6549.0.html
 * NOTE: this will only work on AVR
 */
/*
 * ADC prescaler info (CPU clock / 2^inverse_prescaler) (both 0 and 1 are 1:2 prescalers)
 * 7 (default): normal ADC, ~18Khz normal precision.
 * Faster modes reduce accuracy.
 * 1 ~250Khz, 2 ~200Khz, 3 ~143Khz, 4 ~100Khz, 5 ~59Khz, 6 ~33Khz
 * 4 is a 1:16 prescaler, at 16Mhz this reaches max ADC clock of 1Mhz so 1-3 may not be that useful
 *
 * more info on AVRs ADC: http://maxembedded.com/2011/06/the-adc-of-the-avr/
 * http://www.atmel.com/Images/doc2559.pdf
 */
#ifndef ADC_H_
#define ADC_H_
#include <Arduino.h>

#define ADC_PRESCALER        7 // 2 - 7
#define ADC_BITS 10

void setupADC(byte prescaler);

#endif /* ADC_H_ */
