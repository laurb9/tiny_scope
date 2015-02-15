/*
 * capture.cpp - ADC data capture
 * Tiny Scope for Arduino project
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include "capture.h"

Capture::Capture(ADCInput adc, byte samples, unsigned rangemV)
:adc(adc),
 samples(samples),
 rangemV(rangemV)
{
}

int Capture::init(){
    data = (unsigned*)calloc(samples+2, sizeof(unsigned)); // two extra reads: 0 and a non-zero
    if (!data){
        samples = 0;
    }
    return data != 0;
}

/*
 * ADC allowable float for zero-detection (because of unclean ground) [mV]
 */
#define ADC_JITTER 4
/*
 * Capture the requested number of samples.
 * on return, data contains the raw capture data.
 */
void Capture::capture(){
    unsigned long start;
    unsigned *dataCur;
    unsigned v;
    byte i = samples;

    dataCur = data;
    /*
     * Attempt to latch on a zero transition
     */
    for (byte j=255; j; j--){
        v = adc.read();
        if (v < ADC_JITTER){
            *dataCur++ = v;
            break;
        }
    }
    for (byte j=255; j; j--){
        v = adc.read();
        if (v >= ADC_JITTER){
            *dataCur++ = v;
            break;
        } else {
            *data = v; // overwrite a previous "0" at dataCur-1
        }
    }
    start = micros();
    for (; i; i--){
        *dataCur++ = adc.read();
    }
    elapsedus = micros() - start;
}

/*
 * Convert captured data to mV and extract min/max values
 */
void Capture::tomV(){
    unsigned v;
    minmV = rangemV;
    maxmV = 0;
    for (int i=0; i<samples; i++){
        v = map(data[i], 0, (1L<<adc.bits)-1, 0, long(rangemV));
        data[i] = v;
        if (v < minmV){
            minmV = v;
        };
        if (v > maxmV){
            maxmV = v;
        }
    }
}
