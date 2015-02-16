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

Capture::Capture(ADCInput adc, unsigned samples, uint16_t rangemV)
:adc(adc),
 samples(samples),
 rangemV(rangemV)
{
}

int Capture::init(){
    data = (uint16_t*)calloc(samples+2, sizeof(uint16_t)); // two extra reads: 0 and a non-zero
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
    uint32_t start;
    uint16_t *dataCur;
    uint16_t v;
    unsigned i = samples;

    dataCur = data;
    /*
     * Attempt to latch on a zero transition
     */
    for (unsigned j=255; j; j--){
        v = adc.read();
        if (v < ADC_JITTER){
            *dataCur++ = v;
            break;
        }
    }
    for (unsigned j=255; j; j--){
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
    uint16_t v;
    minmV = rangemV;
    maxmV = 0;
    for (unsigned i=0; i<samples; i++){
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
