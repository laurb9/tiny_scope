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
#define MAX_Y_GRID_LINES 5

class Scope {
protected:
    Display display;
    unsigned minX, maxX;
    unsigned minY, maxY;
    unsigned timeBase;     // grid spacing in microseconds
    unsigned mVperPixel;   // visible mV resolution on screen
    void renderGrid(unsigned long elapsed, unsigned rangemV);
    void renderGraph(unsigned *data, unsigned rangemV, byte samples, int logicMode);
    void renderStatusBar(Capture capture);
    unsigned long calcGridStep(unsigned long length, unsigned count);
public:
    Scope(Display display, unsigned minX, unsigned maxX, unsigned minY, unsigned maxY);
    int isFlatLine(Capture capture);
    int getLogicMode(Capture capture);
    void displayScope(Capture capture);
    void displayVoltMeter(Capture capture);
};

#endif /* SCOPE_H_ */
