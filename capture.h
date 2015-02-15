/*
 * capture.h - ADC data capture
 * Tiny Scope for Arduino project
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#ifndef CAPTURE_H_
#define CAPTURE_H_
#include "adc.h"

class Capture {
public:
    ADCInput adc;
    byte samples;
    unsigned minmV, maxmV, rangemV;
    unsigned long elapsedus;
    unsigned *data;

    Capture(ADCInput adc, byte samples, unsigned rangemV);
    int init();
    void capture();
    void tomV();
};

#endif /* CAPTURE_H_ */
