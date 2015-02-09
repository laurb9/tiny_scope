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

#ifdef __AVR__
#define ADC_BITS 10
#endif /* __AVR__ */

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

class AVR_ADC {
protected:
    byte cur_mode;
    void setPrescaler(byte mode);
public:
    static byte prescalers[];
    byte input;                           // analog input port
    byte bits;                            // ADC resolution in bits
    AVR_ADC(byte input);
    byte getModeCount();          // returns the number of available rates
    unsigned long setMode(byte mode); // set sampling rate from the available values
    unsigned long getClock();       // Hz for information purposes only
    unsigned long getSampleRate();  // Hz for information purposes only

    __inline__ unsigned read(){
        return analogRead(input);
    }
};

#endif /* ADC_H_ */
