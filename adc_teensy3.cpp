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
#ifdef __MK20DX256__
#include "adc.h"

#define ADC_CLOCK_TO_SAMPLING 15

static unsigned int averagingTable[] = {32, 16,  8,  4,  0, 0};
static unsigned int adcBits[]        = {12, 12, 10, 10, 10, 8};

ADCInput::ADCInput(uint8_t input)
:input(input)
{
    pinMode(input, INPUT);
}

/*
 * Get the number of
 */
uint8_t ADCInput::getModeCount(){
    return sizeof(averagingTable);
}

/*
 * Configure ADC for given mode.
 */
bool ADCInput::setMode(uint8_t mode){
    if (mode < ADCInput::getModeCount()){
        cur_mode = mode;
        bits = adcBits[cur_mode];
        analogReadRes(bits);
        analogReadAveraging(averagingTable[cur_mode]);
        return true;
    } else {
        return false;
    }
}

/*
 * Return ADC clock in Hz. This is only useful to estimate sampling rate.
 */
uint32_t ADCInput::getClock(){
    return F_CPU/8;
}

/*
 * Return ADC sampling rate, if available, otherwise 0
 */
uint32_t ADCInput::getSampleRate(){
    return ADCInput::getClock() / ADC_CLOCK_TO_SAMPLING;
}

#endif /* __MK20DX256__ */
