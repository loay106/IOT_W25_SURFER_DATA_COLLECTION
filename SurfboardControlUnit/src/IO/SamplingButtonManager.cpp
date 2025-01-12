#include <Arduino.h>
#include "SamplingButtonManager.h"

Logger SamplingButtonManager::logger = Logger(57600);
bool SamplingButtonManager::buttonPressed = false;
int SamplingButtonManager::lastPressedAt = 0;

SamplingButtonManager::SamplingButtonManager(Logger logger, int buttonPin){
    SamplingButtonManager::logger = logger;
    SamplingButtonManager::buttonPressed = false;
    this->buttonPin = buttonPin;
    SamplingButtonManager::lastPressedAt = 0;
}

void SamplingButtonManager::initialize(){
    pinMode(buttonPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonPin), SamplingButtonManager::onButtonPress, FALLING);
}

bool SamplingButtonManager::wasPressed(){
    bool pressed = SamplingButtonManager::buttonPressed;
    SamplingButtonManager::buttonPressed = false;  // Clear the flag
    return pressed;
}

void IRAM_ATTR SamplingButtonManager::onButtonPress(){
    int currentMillis = millis();
    if(currentMillis - SamplingButtonManager::lastPressedAt >= 500){ // Only one press is considered every 0.5 seconds!
        SamplingButtonManager::buttonPressed = true;
        SamplingButtonManager::lastPressedAt = currentMillis;
    }
}

