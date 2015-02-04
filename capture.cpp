/*
 * Tiny Scope for Arduino
 * (C)2015 Laurentiu Badea
 */
#include "capture.h"

Capture::Capture(byte adc_pin, byte samples, unsigned rangemV)
    :adc_pin(adc_pin),
     samples(samples),
     rangemV(rangemV),
     elapsedus(0)
{
    data = (unsigned*)calloc(samples+2, sizeof(unsigned)); // two extra reads: 0 and a non-zero
}

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
    *dataCur = 0;
    /*
     * Attempt to latch on a zero transition
     */
    for (byte j=samples; j; j--){
        v = analogRead(adc_pin);
        if (v==0){
            dataCur++;
            break;
        }
    }
    for (byte j=samples; j; j--){
        v = analogRead(adc_pin);
        if (v>0){
            *dataCur++ = v;
            break;
        }
    }
    start = micros();
    for (; i; i--){
        *dataCur++ = analogRead(adc_pin);
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
        v = (data[i]+1) * long(rangemV) / (1L<<ADC_BITS);
        data[i] = v;
        if (v < minmV){
            minmV = v;
        };
        if (v > maxmV){
            maxmV = v;
        }
    }
}
