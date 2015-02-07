/*
 * (C)2015 Laurentiu Badea
 *
 * Configure ADC clock
 * NOTE: this will only work on AVR
 *
 * ADC prescaler info (CPU clock / 2^inverse_prescaler) (both 0 and 1 are 1:2 prescalers)
 * 7 (default): normal ADC, ~18Khz normal precision.
 * Faster modes reduce accuracy. Up to 1Mhz clock precision is maintained.
 * 1 ~250Khz, 2 ~200Khz, 3 ~143Khz, 4 ~100Khz, 5 ~59Khz, 6 ~33Khz
 * 4 is a 1:16 prescaler, at 16Mhz this reaches max ADC clock of 1Mhz so 1-3 may not be that useful
 *
 * more info on AVRs ADC:
 * http://www.microsmart.co.za/technical/2014/03/01/advanced-arduino-adc/
 * http://maxembedded.com/2011/06/the-adc-of-the-avr/
 * http://www.atmel.com/Images/doc2559.pdf
 * http://forum.arduino.cc/index.php/topic,6549.0.html
 */
#include "adc.h"

AVR_ADC::AVR_ADC(byte input)
     :cur_mode(0),
      input(input),
      resolution(ADC_BITS)
{
    pinMode(input, INPUT);
}
byte AVR_ADC::prescalers[6] = {7,6,5,4,3,2};

/*
 * Get the number of
 */
byte AVR_ADC::getModeCount(){
    return sizeof(AVR_ADC::prescalers);
}

/*
 * Set ADC prescaler.
 * This should accept a clock or time base instead so it's not AVR specific.
 */
void AVR_ADC::setPrescaler(byte mode){
    // set prescaler
    if (mode < AVR_ADC::getModeCount()){
        cur_mode = mode;
        byte prescaler = prescalers[mode];
        prescaler & 4 ? sbi(ADCSRA,ADPS2) : cbi(ADCSRA,ADPS2);
        prescaler & 2 ? sbi(ADCSRA,ADPS1) : cbi(ADCSRA,ADPS1);
        prescaler & 1 ? sbi(ADCSRA,ADPS0) : cbi(ADCSRA,ADPS0);
    }
    delay(10); // allow the ADC to settle
}

/*
 * Configure ADC for given mode.
 */
unsigned long AVR_ADC::setMode(byte mode){
    AVR_ADC::setPrescaler(mode);
    return AVR_ADC::getClock();
}


/*
 * Return ADC clock in KHz. This is only useful to estimate sampling rate.
 */
unsigned long AVR_ADC::getClock(){
    return F_CPU/(1<<prescalers[cur_mode])/1000;
}

/*
 * Return ADC sampling rate, if available, otherwise 0
 */
unsigned long AVR_ADC::getSampleRate(){
    return AVR_ADC::getClock() / 13;
}
