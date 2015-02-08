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
// range of ADC input in Volts
#define VOLTS_RANGE 5
// I picked 10 so the blue/yellow cutoff falls at 3.45V on the split-color display
#define PIXELS_PER_VOLT 10
#define PIXELS_PER_TIME 10
// smallest mV value that can be seen on display
#define VISIBLE_RESOLUTION_MV (1000/PIXELS_PER_VOLT/2)

class Scope {
protected:
    Display display;
    unsigned minX, maxX;
    unsigned minY, maxY;
    unsigned gridX, gridY; /* grid in pixels per grid unit */
    unsigned timeBase;
    void renderGrid();
    void renderGraph(unsigned *data, byte samples, int logicMode);
    void renderStatusBar(Capture capture);
    void calcTimeBase(unsigned long elapsed, byte samples);
public:
    Scope(Display display, unsigned minX, unsigned maxX, unsigned minY, unsigned maxY);
    int isFlatLine(Capture capture);
    int getLogicMode(Capture capture);
    void displayScope(Capture capture);
    void displayVoltMeter(Capture capture);
};

#endif /* SCOPE_H_ */
