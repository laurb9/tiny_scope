/*
 * Configure ADC clock
 *
 * (C)2015 Laurentiu Badea
 * http://forum.arduino.cc/index.php/topic,6549.0.html
 * NOTE: this will only work on AVR
 */
#include <Arduino.h>
#include "adc.h"

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

AVR_ADC::AVR_ADC(byte input)
     :input(input)
{}

/*
 * Set ADC prescaler.
 * This should accept a clock or time base instead so it's not AVR specific.
 */
void AVR_ADC::setPrescaler(byte newPrescaler){
    // set prescaler
    prescaler = newPrescaler;
    if (prescaler){
        prescaler & 4 ? sbi(ADCSRA,ADPS2) : cbi(ADCSRA,ADPS2);
        prescaler & 2 ? sbi(ADCSRA,ADPS1) : cbi(ADCSRA,ADPS1);
        prescaler & 1 ? sbi(ADCSRA,ADPS0) : cbi(ADCSRA,ADPS0);
    }
    delay(10); // allow the ADC to settle
}

/*
 * Return ADC clock. This is only useful to estimate sampling rate.
 */
unsigned long AVR_ADC::getClock(){
    return F_CPU/(1<<prescaler)/1000;
}
