#include <Arduino.h>
#include "SamplingButtonManager.h"

Logger SamplingButtonManager::logger = Logger(9600);
bool SamplingButtonManager::buttonPressed = false;

SamplingButtonManager::SamplingButtonManager(Logger logger, int buttonPin){
    SamplingButtonManager::logger = logger;
    SamplingButtonManager::buttonPressed = false;
    this->buttonPin = buttonPin;
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
    SamplingButtonManager::buttonPressed = true;
}

