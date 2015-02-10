/*
 * adc.h - Interface to configure ADC speed and capture data.
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 *
 * http://forum.arduino.cc/index.php/topic,6549.0.html
 */
#ifndef ADC_H_
#define ADC_H_
#include <Arduino.h>

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


class ADCBase {
protected:
    byte cur_mode;
public:
    byte input;                       // analog input port connected to this ADC
    byte bits;                        // ADC resolution in bits
    virtual byte getModeCount();      // returns the number of available sampling rates (>0)
    virtual bool setMode(byte mode);  // set mode 0 - getModeCount()-1. True if successful.
    virtual unsigned long getClock(); // return ADC clock in Hz
    virtual unsigned long getSampleRate(); // sampling rate in Hz (actual read speed may vary)
    virtual unsigned read();          // = analogRead(input)
};

#ifdef __MK20DX256__
#include "adc_teensy3.h"
#elif __AVR__
#include "adc_avr.h"
#endif

#endif /* ADC_H_ */
