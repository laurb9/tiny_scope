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

#define ADC_BITS 10

/*
 * This is the portable interface that capture uses and should work on any Arduino device.
 * Chip-specific classes will build upon this.
 */

class ADCBase {
protected:
    uint8_t curMode;
public:
    uint8_t input;                       // analog input port connected to this ADC
    uint8_t bits;                        // ADC resolution in bits

    /*
     * Setup functionality
     */
    bool init(uint8_t inputPin=0, uint8_t mode=0){
        input = inputPin;
        return ADCBase::setMode(mode);
    };
    // get the number of available sampling rates (>0)
    uint8_t getModeCount(){
        return 1; // there is only one mode in portable mode (default)
    }
    // set mode to 0 - getModeCount()-1. True if successful.
    bool setMode(uint8_t mode){
        curMode = 0;
        bits = ADC_BITS;
        return (mode == curMode);
    };

    /*
     * ADC read functionality
     */
    // = analogRead(input)
    inline uint16_t read() __attribute__((always_inline)){
        return analogRead(input);
    }
    // = analogRead(input) without ADC/port setup
    inline uint16_t readFast() __attribute__((always_inline)){
        return analogRead(input);
    }
    // bulk read "size" samples into buffer
    void readMulti(uint16_t *buffer, unsigned size){
        for (; size; size--){
            *buffer++ = ADCBase::readFast();
        }
    };
    // calibration function, return corrected AREF based on Vbg measured against it.
    uint16_t calibrateAREF(){
        return 0;
    }

    /*
     * ADC Information
     * These are not really necessary, just to show the info if available on splash page
     */
    // get ADC clock in Hz
    uint32_t getClock(){
        return 0;
    }
    // sampling rate in Hz (actual read speed may vary)
    uint32_t getSampleRate(){
        return 0;
    }
};

#if defined(__MK20DX256__) || defined(__MKL26Z64__)
#include "adc_teensy3.h"
#elif defined(__AVR__) && defined(ADCSRA) && defined(ADCL)
#include "adc_avr.h"
#else
#define ADC_AREF_MV 5000
class ADCInput : public ADCBase {
};
#endif

#endif /* ADC_H_ */
