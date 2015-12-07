/*
 * adc_teensy3.cpp - Interface to configure ADC speed and capture data for Teensy 3.x boards.
 * Tiny Scope for Arduino project
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 *
 */
#if defined(__MK20DX256__) || defined(__MKL26Z64__)
#include "adc_teensy3.h"

// Actual voltage of internal 1.2V ref in mV. This is different from chip to chip.
// = VRefMeasured*1200/3.3 for 3.3V systems.
#define INTERNAL_REF_MV (3280*1200L/3300)

#define INTERNAL_REF_PORT 39
#define ADC_CLOCK_TO_SAMPLING 15

typedef struct {
    uint8_t samplingSpeed;
    uint8_t conversionSpeed;
    uint8_t resolution;
    uint8_t averaging;
} adcSettings_t;

static adcSettings_t settings[] = {
//      {      ADC_LOW_SPEED,       ADC_LOW_SPEED, 16, 32},  //    10ms   2K
//      {      ADC_LOW_SPEED,       ADC_LOW_SPEED, 12, 16},  //     5ms   4K
        {      ADC_LOW_SPEED,       ADC_LOW_SPEED, 12,  8},  //     2ms   9K
        {      ADC_LOW_SPEED,       ADC_MED_SPEED, 12,  16}, //     1ms  18K
        {      ADC_MED_SPEED,       ADC_MED_SPEED, 12,  16}, //     1ms  22K
        {      ADC_MED_SPEED,       ADC_MED_SPEED, 12,  8},  //   0.5ms  43K
        {      ADC_MED_SPEED,       ADC_MED_SPEED, 12,  4},  //   0.2ms  87K
        {     ADC_HIGH_SPEED,      ADC_HIGH_SPEED, 12,  4},  //   0.1ms 187K
        {      ADC_MED_SPEED,       ADC_MED_SPEED, 12,  0},  //    50us 345K
        {     ADC_HIGH_SPEED, ADC_VERY_HIGH_SPEED, 10,  0},  //    20us 749K
        {ADC_VERY_HIGH_SPEED, ADC_VERY_HIGH_SPEED, 10,  0},  //    20us 815K
        {ADC_VERY_HIGH_SPEED, ADC_VERY_HIGH_SPEED,  8,  0}   //    20us 941K (1.2Msps @ 48,96MHz)
};

bool ADCInput::init(uint8_t newInput, uint8_t mode)
{
    adc = new ADC();
    input = newInput;
    pinMode(input, INPUT);
    return setMode(mode);
}

/*
 * Get the number of
 */
uint8_t ADCInput::getModeCount(){
    return sizeof(settings)/sizeof(adcSettings_t);
}

/*
 * Configure ADC for given mode.
 */
bool ADCInput::setMode(uint8_t mode=0){
    if (mode < getModeCount()){
        curMode = mode;
        adcSettings_t s = settings[mode];
        bits = s.resolution;
        adc->setAveraging(s.averaging);
        adc->setResolution(bits);
        adc->setConversionSpeed(s.conversionSpeed);
        adc->setSamplingSpeed(s.samplingSpeed);
        return true;
    } else {
        return false;
    }
}

/*
 * Read internal reference voltage.
 */
uint16_t ADCInput::calibrateAREF(){
    // reset ADC to default mode for highest precision.
    //uint8_t oldMode = curMode;
    //setMode(0);
    adc->setResolution(16, 1);
    adc->setResolution(16, 0);
    adc->analogRead(INTERNAL_REF_PORT);
    delay(200);
    uint16_t rangemV = INTERNAL_REF_MV * adc->getMaxValue() / adc->analogRead(INTERNAL_REF_PORT);

    // set ADC to previous mode
    setMode(curMode);
    read();

    return rangemV;
}

/*
 * Return ADC clock in Hz. This is only useful to estimate sampling rate.
 */
uint32_t ADCInput::getClock(){
    return 0;
}

/*
 * Return ADC sampling rate, if available, otherwise 0
 */
uint32_t ADCInput::getSampleRate(){
    return ADCInput::getClock() / ADC_CLOCK_TO_SAMPLING;
}

#endif /* defined(__MK20DX256__) || defined(__MKL26Z64__) */
