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
 maxY(minY),
 gridX(10),
 gridY(PIXELS_PER_VOLT),
 timeBase(0)
{};

// Since we round the timebase, it means the grid pixel size must be off.
// Use a binary multiplier to increase its precision so we don't have to use float.
// More than about 4 will cause the grid to shift around at high sampling rates
#define GRID_X_EXTRA_BITS 3
/*
 * Calculate nearest round timeBase(us) to fit the grid
 */
void Scope::calcTimeBase(unsigned long elapsed, byte samples){
    // TODO: maybe do this smarter (log to pick scale) ?
    static byte values[] = {10, 20, 25, 50};
    timeBase = 0;
    for (unsigned long scale=1; !timeBase && scale < elapsed/8; scale*=10){
        for (unsigned i=0; i<sizeof(values); i++){
            if (MAX_X_GRID_LINES * values[i] * scale >= elapsed){
                timeBase = values[i] * scale;
                break;
            }
        }
    }
    // calculate number of samples per grid unit
    gridX = ((long(timeBase) * samples) << GRID_X_EXTRA_BITS) / elapsed;
}

/*
 * Render graph grid with square units
 */
void Scope::renderGrid(){
    for (unsigned x=minX << GRID_X_EXTRA_BITS; x<maxX << GRID_X_EXTRA_BITS; x+=gridX){
        for (unsigned y=0; y<=VOLTS_RANGE; y++){
            display.drawPixel(x >> GRID_X_EXTRA_BITS, map(y, 0, VOLTS_RANGE, minY, maxY), WHITE);
        }
    }
}

/*
 * Return the value of the logic mode flag.
 * 1: enable, 0: disable
 */
#define CONNECT_LIMIT 5*VISIBLE_RESOLUTION_MV
int Scope::getLogicMode(Capture capture){
    unsigned *data = capture.data;
    int x, y, lastY = *data;

    // TODO: turn on logicMode if most of the values are near minmV and maxmV

    // turn off logicMode if the display would be too cluttered with vertical lines
    int transitionsUp = 0, transitionsDown = 0;
    for (x=1; x<SCREEN_WIDTH; x++){
        data++;
        y = *data;
        if (y-lastY>CONNECT_LIMIT){
            transitionsUp++;
        } else if (y-lastY<-CONNECT_LIMIT){
            transitionsDown++;
        }
        lastY = y;
    }
    return (transitionsUp+transitionsDown < SCREEN_WIDTH/4);
}

/*
 * Draw the mV values on the graph
 * The screen is inverted so Y=0 corresponds to 5V
 */
void Scope::renderGraph(unsigned *data, byte samples, int logicMode){
    int i, x, y, lastY;

    // render data graph
    for (i=0; i<samples; i++, data++){
        x = map(i, 0, samples, minX, maxX);
        y = map(*data, 0, VOLTS_RANGE*1000, minY, maxY);
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

    if (timedToggle && timeBase){
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
    return capture.maxmV-capture.minmV < VISIBLE_RESOLUTION_MV;
}

/*
 * Display scope
 */
void Scope::displayScope(Capture capture){
    display.setCursor(0,0);
    Scope::calcTimeBase(capture.elapsedus, capture.samples);
    Scope::renderGrid();
    Scope::renderGraph(capture.data, capture.samples, Scope::getLogicMode(capture));
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

    display.printf(F("-%umV"), maxmV-minmV);
    display.setCursor(16 * CHR_WIDTH, 7 * CHR_HEIGHT);
    display.printf(F("0.%u%%"), (maxmV-minmV)/VOLTS_RANGE);
}
