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
#include "adc_avr.h"
#if defined(ADCSRA) && defined(ADCL)

// Actual voltage of Vbg (internal 1.1V ref) in mV. This is different from chip to chip.
// = 5000*1100/VREFmeasured (with accurate multimeter). 3300*1100/VREFmeasured for 3.3V systems.
#define INTERNAL_REF_MV 1090

const uint8_t ADCInput::prescalers[] = {7,6,5,4,3,2}; // 1:8MHz clock is out of ADC spec for 16MHz AVR

bool ADCInput::init(uint8_t newInput, uint8_t mode){
    curMode = 0;
    input = newInput;
    bits = ADC_BITS;
    pinMode(input, INPUT);
    if (input <= 5)
        sbi(DIDR0, 1 << input); // Digital input buffer disable (reduce power consumption)
    ADCInput::setMode(mode);
}

/*
 * Get the number of
 */
uint8_t ADCInput::getModeCount(){
    return sizeof(ADCInput::prescalers);
}

/*
 * Set ADC prescaler
 */
bool ADCInput::setPrescaler(uint8_t mode){
    if (mode < ADCInput::getModeCount()){
        uint8_t prescaler = prescalers[mode];
        prescaler & 4 ? sbi(ADCSRA,ADPS2) : cbi(ADCSRA,ADPS2);
        prescaler & 2 ? sbi(ADCSRA,ADPS1) : cbi(ADCSRA,ADPS1);
        prescaler & 1 ? sbi(ADCSRA,ADPS0) : cbi(ADCSRA,ADPS0);
        sbi(ADCSRA, ADEN);
        delay(10); // allow the ADC to settle
    } else {
        return false;
    }
    return true;
}

/*
 * Configure ADC for given mode.
 */
bool ADCInput::setMode(uint8_t mode){
    if (ADCInput::setPrescaler(mode)){
        curMode = mode;
    }
    return (curMode = mode);
}

/*
 * Perform a series of readings, saving into a data buffer
 */
volatile uint16_t *adc_buffer, *adc_buffer_end;
void ADCInput::readMulti(uint16_t *buffer, unsigned size){
    adc_buffer = buffer;
    adc_buffer_end = buffer + size;
    //cbi(ADCSRB, ADTS2); // Set trigger to free-running (this is the default)
    //cbi(ADCSRB, ADTS1);
    //cbi(ADCSRB, ADTS0);
    sbi(ADCSRA, ADATE); // Enable trigger
    cbi(ADCSRA, ADIE);  // Disable interrupt (default)
    sbi(ADCSRA, ADSC);  // ADSC=AD Start Conversion

    while (adc_buffer != adc_buffer_end){
        loop_until_bit_is_set(ADCSRA, ADIF); // Wait for conversion to finish
        *adc_buffer++ = ADCL | (ADCH << 8);
        sbi(ADCSRA, ADIF);
    }
    cbi(ADCSRA, ADATE);  // Disable free running
}

/*
 * Read internal reference voltage.
 */
uint16_t ADCInput::calibrateAREF(){
    // reset ADC to default mode for highest precision.
    setPrescaler(0);
    read(); // allow the port to be configured (discard result)

    ADMUX = (ADMUX & 0xf0) | 0x0e; // switch to reading the internal 1.1V reference

    // wait for the reference input to stabilize
    readFast();
    delay(200);
    uint16_t reference = readFast();

    // reference should represent 1100mV
    uint16_t rangemV = INTERNAL_REF_MV * ((1L<<bits)-1) / long(reference);

    // set ADC to previous mode
    setPrescaler(curMode);
    read();

    return rangemV;
}

/*
 * Return ADC clock in Hz. This is only useful to estimate sampling rate.
 */
uint32_t ADCInput::getClock(){
    return F_CPU/(1<<prescalers[curMode]);
}

/*
 * Return ADC sampling rate, if available, otherwise 0
 */
uint32_t ADCInput::getSampleRate(){
    return ADCInput::getClock() / ADC_CLOCK_TO_SAMPLING;
}
#endif /* defined(ADCSRA) && defined(ADCL) */
#endif /* defined(__AVR__) */
