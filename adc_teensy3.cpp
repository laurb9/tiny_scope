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

ADCInput::ADCInput(byte input)
:input(input),
 bits(ADC_BITS)
{
    pinMode(input, INPUT);
}

/*
 * Get the number of
 */
byte ADCInput::getModeCount(){
    return 1;
}

/*
 * Configure ADC for given mode.
 */
bool ADCInput::setMode(byte mode){
    return 1;
}

/*
 * Return ADC clock in Hz. This is only useful to estimate sampling rate.
 */
unsigned long ADCInput::getClock(){
    return F_CPU/2;
}

/*
 * Return ADC sampling rate, if available, otherwise 0
 */
unsigned long ADCInput::getSampleRate(){
    return ADCInput::getClock() / ADC_CLOCK_TO_SAMPLING;
}

#endif /* __MK20DX256__ */
