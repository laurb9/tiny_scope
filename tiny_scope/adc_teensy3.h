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

#define ADC_AREF_MV 3300

class ADCInput : public ADCBase {
public:
    uint8_t input;                           // analog input port
    uint8_t bits;                            // ADC resolution in bits
    bool init(uint8_t input=0, uint8_t mode=0);
    uint8_t getModeCount();          // returns the number of available rates
    bool setMode(uint8_t mode); // set sampling rate from the available values
    uint32_t getClock();       // Hz for information purposes only
    uint32_t getSampleRate();  // Hz for information purposes only

    inline uint16_t read() __attribute__((always_inline)){
        return analogRead(input);
    }
    inline uint16_t readFast() __attribute__((always_inline)){
        return analogRead(input);
    }
    inline void readMulti(uint16_t *buffer, unsigned size){
        for (; size; size--){
            *buffer++ = analogRead(input);
        }
    }
    uint16_t calibrateAREF();
};

#endif /* ADC_TEENSY3_ */
