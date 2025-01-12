#include <Arduino.h>
#include "SamplingButtonHandler.h"

Logger SamplingButtonHandler::logger = Logger(57600);
bool SamplingButtonHandler::buttonPressed = false;
int SamplingButtonHandler::lastPressedAt = 0;

SamplingButtonHandler::SamplingButtonHandler(Logger logger, int buttonPin){
    SamplingButtonHandler::logger = logger;
    SamplingButtonHandler::buttonPressed = false;
    this->buttonPin = buttonPin;
    SamplingButtonHandler::lastPressedAt = 0;
}

void SamplingButtonHandler::initialize(){
    pinMode(buttonPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonPin), SamplingButtonHandler::onButtonPress, FALLING);
}

bool SamplingButtonHandler::wasPressed(){
    bool pressed = SamplingButtonHandler::buttonPressed;
    SamplingButtonHandler::buttonPressed = false;  // Clear the flag
    return pressed;
}

void IRAM_ATTR SamplingButtonHandler::onButtonPress(){
    int currentMillis = millis();
    if(currentMillis - SamplingButtonHandler::lastPressedAt >= 500){ // Only one press is considered every 0.5 seconds!
        SamplingButtonHandler::buttonPressed = true;
        SamplingButtonHandler::lastPressedAt = currentMillis;
    }
}

