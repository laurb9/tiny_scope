/*
 * adc_teensy3.h - Interface to configure ADC speed and capture data for Teensy 3.x boards.
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 *
 * http://forum.arduino.cc/index.php/topic,6549.0.html
 */
#ifndef ADC_TEENSY3_
#define ADC_TEENSY3_
#include "adc.h"

#define ADC_BITS 10
#define ADC_CLOCK_TO_SAMPLING 15
#define ADC_AREF_MV 3300

class ADCInput : public ADCBase {
public:
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

#endif /* ADC_TEENSY3_ */
