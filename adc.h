/*
 * setup high frequency DAC
 * (C)2015 Laurentiu Badea
 * http://forum.arduino.cc/index.php/topic,6549.0.html
 * NOTE: this will only work on AVR
 */
/*
 */
#ifndef ADC_H_
#define ADC_H_
#include <Arduino.h>

#define ADC_BITS 10

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
    byte resolution;                      // ADC resolution in bits
    AVR_ADC(byte input);
    byte getModeCount();          // returns the number of available rates
    unsigned long setMode(byte mode); // set sampling rate from the available values
    unsigned long getClock();       // Hz for information purposes only
    unsigned long getSampleRate();  // Hz for information purposes only
};

#endif /* ADC_H_ */
