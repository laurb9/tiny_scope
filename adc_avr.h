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
    byte cur_mode;
    void setPrescaler(byte mode);
public:
    static byte prescalers[];
    byte input;                           // analog input port
    byte bits;                            // ADC resolution in bits
    ADCInput(byte input);
    byte getModeCount();          // returns the number of available rates
    bool setMode(byte mode); // set sampling rate from the available values
    unsigned long getClock();       // Hz for information purposes only
    unsigned long getSampleRate();  // Hz for information purposes only

    __inline__ unsigned read(){
        return analogRead(input);
    }
};

#endif /* ADC_AVR_H_ */
