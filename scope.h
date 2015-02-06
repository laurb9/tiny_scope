/*
 * scope.h
 * Tiny Scope for Arduino
 * (C)2015 Laurentiu Badea
 *
 *  Created on: Feb 3, 2015
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
    unsigned gridX, gridY; /* grid density */
    unsigned timeBase;
    void renderGrid();
    void renderGraph(unsigned *data, int logicMode);
    void renderStatusBar(Capture capture);
    void calcTimeBase(unsigned long elapsed, byte samples);
public:
    Scope(Display display);
    int isFlatLine(Capture capture);
    int getLogicMode(Capture capture);
    void displayScope(Capture capture);
    void displayVoltMeter(Capture capture);
};

#endif /* SCOPE_H_ */
