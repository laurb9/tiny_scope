/*
 * scope.h - Scope/Voltmeter view of captured data
 * Tiny Scope for Arduino project
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#ifndef SCOPE_H_
#define SCOPE_H_
#include "display.h"
#include "capture.h"

// Maximum number of horizontal grid lines
#define MAX_X_GRID_LINES 10
// Maximum number of vertical grid lines
#define MAX_Y_GRID_LINES 6

class Scope {
protected:
    Display display;
    uint16_t minX, maxX;
    uint16_t minY, maxY;
    uint16_t timeBase;     // grid spacing in microseconds
    uint16_t mVperPixel;   // visible mV resolution on screen
    void renderGrid(uint32_t elapsed, uint16_t rangemV);
    void renderGraph(uint16_t *data, uint16_t rangemV, unsigned samples);
    void renderStatusBar(Capture capture);
    uint32_t calcGridStep(uint32_t length, uint16_t count);
public:
    Scope(Display display, uint16_t minX, uint16_t maxX, uint16_t minY, uint16_t maxY);
    int isFlatLine(Capture capture);
    void displayScope(Capture capture);
    void displayVoltMeter(Capture capture);
};

#endif /* SCOPE_H_ */
