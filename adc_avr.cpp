/*
 * adc_avr.cpp - Interface to configure ADC speed and capture data for Atmel AVR chips
 * Tiny Scope for Arduino project
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 *
 * NOTE: this will only work on AVR and was only tested on Atmega328.
 *
 * ADC prescaler info (CPU clock / 2^prescaler) (both 0 and 1 are 1:2 prescalers)
 * 7 is default mode, 9KHz sampling rate
 * Faster modes reduce accuracy. Up to 1Mhz clock accuracy is reasonably maintained.
 * ADC Clock@16MHz AVR: 1 8MHz,    2 4MHz,    3 2MHz,    4 1MHz,   5 512KHz, 6 256KHz, 7 128Khz
 * Sampling @16MHz AVR: 1 ~615KHz, 2 ~307KHz, 3 ~153KHz, 4 ~76Khz, 5 ~38KHz, 6 ~19KHz, 7 ~9KHz
 * 4 is a 1:16 prescaler, at 16Mhz this reaches max ADC clock of 1Mhz so 1-3 may not be that useful
 *
 * more info on AVRs ADC:
 * http://www.microsmart.co.za/technical/2014/03/01/advanced-arduino-adc/
 * http://maxembedded.com/2011/06/the-adc-of-the-avr/
 * http://www.atmel.com/Images/doc2559.pdf
 * http://forum.arduino.cc/index.php/topic,6549.0.html
 */
#ifdef __AVR__
#include "adc.h"

const uint8_t ADCInput::prescalers[] = {7,6,5,4,3,2}; // 1:8MHz clock is out of ADC spec for 16MHz AVR

bool ADCInput::init(uint8_t newInput, uint8_t mode){
    cur_mode = 0;
    input = newInput;
    bits = ADC_BITS;
    pinMode(input, INPUT);
}

/*
 * Get the number of
 */
uint8_t ADCInput::getModeCount(){
    return sizeof(ADCInput::prescalers);
}

/*
 * Set ADC prescaler.
 * This should accept a clock or time base instead so it's not AVR specific.
 */
void ADCInput::setPrescaler(uint8_t mode){
    // set prescaler
    if (mode < ADCInput::getModeCount()){
        cur_mode = mode;
        uint8_t prescaler = prescalers[mode];
        prescaler & 4 ? sbi(ADCSRA,ADPS2) : cbi(ADCSRA,ADPS2);
        prescaler & 2 ? sbi(ADCSRA,ADPS1) : cbi(ADCSRA,ADPS1);
        prescaler & 1 ? sbi(ADCSRA,ADPS0) : cbi(ADCSRA,ADPS0);
    }
    delay(10); // allow the ADC to settle
}

/*
 * Configure ADC for given mode.
 */
bool ADCInput::setMode(uint8_t mode){
    ADCInput::setPrescaler(mode);
    return 1;
}


/*
 * Return ADC clock in Hz. This is only useful to estimate sampling rate.
 */
uint32_t ADCInput::getClock(){
    return F_CPU/(1<<prescalers[cur_mode]);
}

/*
 * Return ADC sampling rate, if available, otherwise 0
 */
uint32_t ADCInput::getSampleRate(){
    return ADCInput::getClock() / ADC_CLOCK_TO_SAMPLING;
}

#endif /* __AVR__ */
