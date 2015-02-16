/*
 * adc_avr.h - Interface to configure ADC speed and capture data for Atmel AVR chips.
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 *
 * http://forum.arduino.cc/index.php/topic,6549.0.html
 */
#ifndef ADC_AVR_H_
#define ADC_AVR_H_
#include "adc.h"

#define ADC_BITS 10
#define ADC_CLOCK_TO_SAMPLING 13
#define ADC_AREF_MV 5000

class ADCInput : public ADCBase {
protected:
    uint8_t cur_mode;
    void setPrescaler(uint8_t mode);
public:
    static uint8_t prescalers[];
    uint8_t input;                           // analog input port
    uint8_t bits;                            // ADC resolution in bits
    ADCInput(uint8_t input);
    uint8_t getModeCount();          // returns the number of available rates
    bool setMode(uint8_t mode); // set sampling rate from the available values
    uint32_t getClock();       // Hz for information purposes only
    uint32_t getSampleRate();  // Hz for information purposes only

    __inline__ uint16_t read(){
        return analogRead(input);
    }
};

#endif /* ADC_AVR_H_ */
