/*
 * scope.c
 * Tiny Scope for Arduino
 * (C)2015 Laurentiu Badea
 *
 *  Created on: Feb 3, 2015
 */

#include "scope.h"

#define OLED_RESET 4
Display display(OLED_RESET);

Scope::Scope(Display display)
    :display(display),
     gridX(10),
     gridY(PIXELS_PER_VOLT),
     timeBase(0)
{};

// Since we round the timebase, it means the grid pixel size must be off.
// Use a binary multiplier to increase its precision so we don't have to use float.
// More than about 4 will cause the grid to shift around at high sampling rates
#define GRID_X_EXTRA_BITS 3
/*
 * Calculate nearest round timeBase to fit the grid
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
    gridX = ((long(timeBase) * samples) << GRID_X_EXTRA_BITS) / elapsed;
}

/*
 * Render graph grid with square units
 */
void Scope::renderGrid(){
    for (unsigned x=0; x<SCREEN_WIDTH << GRID_X_EXTRA_BITS; x+=gridX){
        for (unsigned y=0; y<=VOLTS_RANGE; y++){
            display.drawPixel(x >> GRID_X_EXTRA_BITS, y*gridY, WHITE);
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
void Scope::renderGraph(unsigned *data, int logicMode){
    int x, y, lastY;

    // render data graph
    for (x=0; x<SCREEN_WIDTH; x++, data++){
        y = VOLTS_RANGE * gridY - round((*data)*gridY/1000);
        if (logicMode && x>0 && abs(lastY-y)>4){
            if (y-lastY>0){
                display.drawFastVLine(x-1,lastY,y-lastY+1,WHITE);
            } else {
                display.drawFastVLine(x-1,y,lastY-y+1,WHITE);
            }
        }
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
        if (sps >= 1000000)
            display.print(F("+1 Msps"));
        else if (sps >= 1000)
            display.printf(F("%lu Ksps"), sps/1000);
        else
            display.printf(F("%lu sps"), sps);
    } else {
        if (timeBase < 100){
            display.printf(F("%u us"), timeBase);
        } else {
            display.printf(F("%u.%02u ms"), timeBase/1000, (timeBase % 1000)/10);
        }
    }
    display.setCursor(8*CHR_WIDTH+4, SCREEN_HEIGHT-CHR_HEIGHT);
    unsigned minmV = capture.minmV, maxmV = capture.maxmV;
    display.printf(F(" %d.%02d:%d.%02d V"), minmV/1000, (minmV%1000)/10, maxmV/1000, (maxmV%1000)/10);
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
    Scope::renderGraph(capture.data, Scope::getLogicMode(capture));
    Scope::renderStatusBar(capture);
}

/*
 * Display a large voltmeter
 */
void Scope::displayVoltMeter(Capture capture){
    unsigned minmV = capture.minmV, maxmV = capture.maxmV;
    display.setCursor(2 * CHR_WIDTH, 2 * CHR_HEIGHT);
    display.setTextSize(3);
    display.printf(F("%u.%02lu V"), maxmV/1000, round((maxmV%1000)/10.0));
    display.setTextSize(1);
    display.setCursor(2 * CHR_WIDTH, 7 * CHR_HEIGHT);

    display.printf(F("-%umV"), maxmV-minmV);
    display.setCursor(16 * CHR_WIDTH, 7 * CHR_HEIGHT);
    display.printf(F("0.%u%%"), (maxmV-minmV)/VOLTS_RANGE);
}
