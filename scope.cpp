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
    :display(display)
{};

/*
 * Render graph grid with square units
 */
void Scope::renderGrid(){
    for (int x=0; x<SCREEN_WIDTH; x+=PIXELS_PER_TIME){
        for (int y=0; y<=VOLTS_RANGE; y++){
            display.drawPixel(x, y*PIXELS_PER_VOLT, WHITE);
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
        y = VOLTS_RANGE * PIXELS_PER_VOLT - round((*data)*PIXELS_PER_VOLT/1000);
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
void Scope::renderStatusBar(unsigned timeBase, unsigned minmV, unsigned maxmV){
    display.setCursor(0, SCREEN_HEIGHT-CHR_HEIGHT);
    int timedToggle = (millis() >> 12) & 1; // bounces between 0 and 1 slowly

    if (timedToggle && timeBase){
        // show samples per second
        unsigned long sps = round(1000000.0 * PIXELS_PER_TIME / timeBase);
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
    display.setCursor(7*CHR_WIDTH+4, SCREEN_HEIGHT-CHR_HEIGHT);
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
    unsigned timeBase = capture.elapsedus * PIXELS_PER_TIME / capture.samples;
    Scope::renderGrid();
    Scope::renderGraph(capture.data, Scope::getLogicMode(capture));
    Scope::renderStatusBar(timeBase, capture.minmV, capture.maxmV);
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
