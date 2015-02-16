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

Scope::Scope(Display display, uint16_t minX, uint16_t maxX, uint16_t minY, uint16_t maxY)
:display(display),
 minX(minX),
 maxX(maxX),
 minY(maxY), /* reverse min/max Y because screen coordinates are upside-down */
 maxY(minY)
{};

/*
 * Helper method to calculate nearest round grid step to fit "count" grid lines over "length"
 */
uint32_t Scope::calcGridStep(uint32_t length, uint16_t count){
    // TODO: maybe do this smarter (log to pick scale) ?
    static uint8_t values[] = {10, 20, 25, 50};
    uint32_t gridSize = 0;
    for (uint32_t scale=1; !gridSize && scale < length/10; scale*=10){
        for (uint16_t i=0; i<sizeof(values); i++){
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
void Scope::renderGrid(uint32_t elapsed, uint16_t rangemV){
    // Since we round the measurement units, it means the grid pixel size must be off.
    // Use a multiplier to increase its precision so we don't have to use float.
    // More than about 16 will cause the grid to shift around at high sampling rates
    #define PREC_MULT 8

    // calculate grid step in pixels
    timeBase = Scope::calcGridStep(elapsed, MAX_X_GRID_LINES);
    uint16_t gridX = ((long(timeBase) * (maxX-minX)) * PREC_MULT) / elapsed;
    uint16_t gridY = Scope::calcGridStep(rangemV, MAX_Y_GRID_LINES);
    mVperPixel = rangemV / ((maxY>minY) ? maxY-minY : minY-maxY);

    for (uint16_t x=minX * PREC_MULT; x<maxX * PREC_MULT; x+=gridX){
        for (uint16_t y=0; y<=rangemV; y+=gridY){
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
    uint16_t *data = capture.data;
    uint16_t x, y, lastY = *data;

    // TODO: turn on logicMode if most of the values are near minmV and maxmV

    // turn off logicMode if the display would be too cluttered with vertical lines
    unsigned transitionsUp = 0, transitionsDown = 0;
    for (x=1; x<capture.samples; x++, data++){
        y = *data;
        uint16_t diff = (y > lastY) ? y-lastY : lastY-y;
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
void Scope::renderGraph(uint16_t *data, uint16_t rangemV, unsigned samples, int logicMode){
    uint16_t i, x, y, lastY = 0;

    // render data graph
    for (i=0; i<samples; i++, data++){
        x = map(i, 0, samples-1, minX, maxX);
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
    int timedToggle = (millis() >> 12) & 1; // bounces between 0 and 1 slowly

    display.setTextCursor(7, 0);
    if (timedToggle){
        display.printSmallUnits(timeBase, "s/div");
        uint16_t mVpp = capture.maxmV-capture.minmV+5;
        display.printf(F("  %u.%02u Vpp"), mVpp/1000, (mVpp%1000)/10);
    } else {
        if (capture.elapsedus){
            uint32_t sps = round(1000000.0 * capture.samples / capture.elapsedus);
            display.printLargeUnits(sps, "sps");
        }
        display.setTextCursor(7, 9);
        uint16_t minmV = capture.minmV+5, maxmV = capture.maxmV+5;
        display.printf(F(" %u.%02u:%u.%02u V"), minmV/1000, (minmV%1000)/10, maxmV/1000, (maxmV%1000)/10);
    }
}

/*
 * Return true if rendered graph would be a flat line (resolution-dependent)
 * Take rolling average Vpp over HISTORY_SIZE readings
 */
int Scope::isFlatLine(Capture capture){
#define VPP_BUF_SIZE (sizeof(vpp)/sizeof(uint16_t))
    static uint16_t vpp[] = {0,0,0,0};
    static int count = 0;
    static uint32_t sumVpp = 0;
    sumVpp -= vpp[count];
    vpp[count] = capture.maxmV - capture.minmV;
    sumVpp += vpp[count];
    count = (count+1) % VPP_BUF_SIZE;
    return (sumVpp/VPP_BUF_SIZE < mVperPixel/2);
#undef VPP_BUF_SIZE
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
    uint16_t minmV = capture.minmV, maxmV = capture.maxmV;
    display.setTextCursor(2, 2);
    display.setTextSize(3);
    display.printf(F("%u.%02u V"), (maxmV+5)/1000, ((maxmV+5)%1000)/10);
    display.setTextSize(1);
    display.setTextCursor(7, 2);

    uint16_t diff = maxmV - minmV;
    display.printf(F("-%umV"), maxmV-minmV);
    display.setTextCursor(7, 16);
    // compute diff in tenths of %
    diff = diff*1000/capture.rangemV;
    display.printf(F("%u.%u%%"), diff/10, (diff%10));
}
