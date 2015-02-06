/*
 * Tiny Scope for Arduino
 * (C)2015 Laurentiu Badea
 */
#ifndef CAPTURE_H_
#define CAPTURE_H_
#include "adc.h"

class Capture {
public:
    int adc_pin;
    byte samples;
    unsigned minmV, maxmV, rangemV;
    unsigned long elapsedus;
    unsigned *data;

    Capture(byte adc_pin, byte samples, unsigned rangemV);
    int init();
    void capture();
    void tomV();
};

#endif /* CAPTURE_H_ */
