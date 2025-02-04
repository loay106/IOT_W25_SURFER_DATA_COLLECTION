#include "RGBStatusHandler.h"
#include <Arduino.h>

const int FLICKERING_RATE = 500;

RGBStatusHandler* RGBStatusHandler::instance = nullptr;

volatile RGBColors firstColor = RGBColors::NO_COLOR;
volatile RGBColors secondColor = RGBColors::NO_COLOR;
volatile bool flickerState = false;
unsigned long lastFlickerMillis = 0;

void RGBStatusHandler::init(int redPin, int greenPin, int bluePin) {
    ledcSetup(0, 5000, 8); // Channel 0, 5kHz, 8-bit resolution
    ledcSetup(1, 5000, 8); // Channel 1, 5kHz, 8-bit resolution
    ledcSetup(2, 5000, 8); // Channel 2, 5kHz, 8-bit resolution

    ledcAttachPin(redPin, 0);
    ledcAttachPin(greenPin, 1);
    ledcAttachPin(bluePin, 2);
}

void showColor(){
    RGBColors color = flickerState ? firstColor : secondColor;
    switch (color){
        case RGBColors::RED:
            ledcWrite(0, 255); 
            ledcWrite(1, 0); 
            ledcWrite(2, 0);
            break;
        case RGBColors::GREEN:
            ledcWrite(0, 0); 
            ledcWrite(1, 255);
            ledcWrite(2, 0); 
            break;
        case RGBColors::BLUE:
            ledcWrite(0, 0); 
            ledcWrite(1, 0);
            ledcWrite(2, 255); 
            break;
        case RGBColors::CYAN:
            ledcWrite(0, 0); 
            ledcWrite(1, 255);
            ledcWrite(2, 255); 
            break;      
        default:
            // defaulting to NO_COLOR
            ledcWrite(0, 0); 
            ledcWrite(1, 0);
            ledcWrite(2, 0); 
            break;
    }
}

void RGBStatusHandler::updateColors(RGBColors first, RGBColors second){
    if(firstColor != first || secondColor != second){
        firstColor = first;
        secondColor = second;
        lastFlickerMillis=0;
        showColor();
    }
}

void RGBStatusHandler::flicker(){
    if(firstColor == secondColor){
        // nothing to do...
        return;
    }
    unsigned long current = millis();
    unsigned long elapsedMillis = (current - lastFlickerMillis); 
    if(elapsedMillis >= FLICKERING_RATE){
        showColor();
        flickerState = !flickerState;
        lastFlickerMillis = current;
    }
}


