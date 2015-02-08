/*
 * scope.cpp - Scope/Voltmeter view of captured data
 * Tiny Scope for Arduino project
 *
 * Copyright (C)2015 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include "scope.h"

#define OLED_RESET 4
Display display(OLED_RESET);

Scope::Scope(Display display, unsigned minX, unsigned maxX, unsigned minY, unsigned maxY)
:display(display),
 minX(minX),
 maxX(maxX),
 minY(maxY), /* reverse min/max Y because screen coordinates are upside-down */
 maxY(minY)
{};

/*
 * Helper method to calculate nearest round grid step to fit "count" grid lines over "length"
 */
unsigned long Scope::calcGridStep(unsigned long length, unsigned count){
    // TODO: maybe do this smarter (log to pick scale) ?
    static byte values[] = {10, 20, 25, 50};
    unsigned long gridSize = 0;
    for (unsigned long scale=1; !gridSize && scale < length/10; scale*=10){
        for (unsigned i=0; i<sizeof(values); i++){
            if (scale * count * values[i] >= length){
                gridSize = values[i] * scale;
                break;
            }
        }
    }
    return gridSize;
}

/*
 * Render graph grid with square units
 */
void Scope::renderGrid(unsigned long elapsed, unsigned rangemV){
    // Since we round the measurement units, it means the grid pixel size must be off.
    // Use a multiplier to increase its precision so we don't have to use float.
    // More than about 16 will cause the grid to shift around at high sampling rates
    #define PREC_MULT 8

    // calculate grid step in pixels
    timeBase = Scope::calcGridStep(elapsed, MAX_X_GRID_LINES);
    unsigned gridX = ((long(timeBase) * (maxX-minX)) * PREC_MULT) / elapsed;
    unsigned gridY = Scope::calcGridStep(rangemV, MAX_Y_GRID_LINES);
    mVperPixel = rangemV / ((maxY>minY) ? maxY-minY : minY-maxY);

    for (unsigned x=minX * PREC_MULT; x<maxX * PREC_MULT; x+=gridX){
        for (unsigned y=0; y<=rangemV; y+=gridY){
            display.drawPixel(x / PREC_MULT, map(y, 0, rangemV, minY, maxY), WHITE);
        }
    }
    #undef PREC_MULT
}

/*
 * Return the value of the logic mode flag.
 * 1: enable, 0: disable
 */
int Scope::getLogicMode(Capture capture){
    unsigned *data = capture.data;
    int x, y, lastY = *data;

    // TODO: turn on logicMode if most of the values are near minmV and maxmV

    // turn off logicMode if the display would be too cluttered with vertical lines
    int transitionsUp = 0, transitionsDown = 0;
    for (x=1; x<capture.samples; x++, data++){
        y = *data;
        unsigned diff = (y > lastY) ? y-lastY : lastY-y;
        if (diff > 2*mVperPixel){
            if (y > lastY) transitionsUp++;
            else transitionsDown++;
        }
        lastY = y;
    }
    return (transitionsUp+transitionsDown < capture.samples/4);
}

/*
 * Draw the mV values on the graph
 * The screen is inverted so Y=0 corresponds to 5V
 */
void Scope::renderGraph(unsigned *data, unsigned rangemV, byte samples, int logicMode){
    int i, x, y, lastY;

    // render data graph
    for (i=0; i<samples; i++, data++){
        x = map(i, 0, samples, minX, maxX);
        y = map(*data, 0, rangemV, minY, maxY);
        if (logicMode && i>0 && abs(lastY-y)>4){
            if (y-lastY>0){
                display.drawFastVLine(x-1,lastY,y-lastY+1,WHITE);
            } else {
                display.drawFastVLine(x-1,y,lastY-y+1,WHITE);
            }
        };
        lastY=y;
        display.drawPixel(x, y, WHITE);
    }
}

/*
 * Display time units, volt range etc
 */
void Scope::renderStatusBar(Capture capture){
    display.setCursor(0, SCREEN_HEIGHT-CHR_HEIGHT);
    int timedToggle = (millis() >> 12) & 1; // bounces between 0 and 1 slowly

    if (!timedToggle && timeBase){
        // show samples per second
        unsigned long sps = round(1000000.0 * capture.samples / capture.elapsedus);
        display.printLargeUnits(sps, "sps");
    } else {
        display.printSmallUnits(timeBase, "s");
    }
    display.setCursor(9*CHR_WIDTH, SCREEN_HEIGHT-CHR_HEIGHT);
    unsigned minmV = capture.minmV+5, maxmV = capture.maxmV+5;
    display.printf(F(" %u.%02u:%u.%02u V"), minmV/1000, (minmV%1000)/10, maxmV/1000, (maxmV%1000)/10);
}

/*
 * Return true if rendered graph would be a flat line (resolution-dependent)
 */
int Scope::isFlatLine(Capture capture){
    return capture.maxmV-capture.minmV < mVperPixel/2;
}

/*
 * Display scope
 */
void Scope::displayScope(Capture capture){
    Scope::renderGrid(capture.elapsedus, capture.rangemV);
    Scope::renderGraph(capture.data, capture.rangemV, capture.samples, Scope::getLogicMode(capture));
    Scope::renderStatusBar(capture);
}

/*
 * Display a large voltmeter
 */
void Scope::displayVoltMeter(Capture capture){
    unsigned minmV = capture.minmV, maxmV = capture.maxmV;
    display.setCursor(2 * CHR_WIDTH, 2 * CHR_HEIGHT);
    display.setTextSize(3);
    display.printf(F("%u.%02u V"), (maxmV+5)/1000, ((maxmV+5)%1000)/10);
    display.setTextSize(1);
    display.setCursor(2 * CHR_WIDTH, 7 * CHR_HEIGHT);

    unsigned diff = maxmV - minmV;
    display.printf(F("-%umV"), maxmV-minmV);
    display.setCursor(16 * CHR_WIDTH, 7 * CHR_HEIGHT);
    // compute diff in tenths of %
    diff = diff*1000/capture.rangemV;
    display.printf(F("%u.%u%%"), diff/10, (diff%10));
}
