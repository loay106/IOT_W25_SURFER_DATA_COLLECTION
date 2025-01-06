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
    return SamplingButtonManager::buttonPressed;
}

void SamplingButtonManager::onButtonPress(){
    SamplingButtonManager::logger.info("Button was just pressed");
    SamplingButtonManager::buttonPressed = true;
    delay(10); // added for stability
}

void SamplingButtonManager::consumePress(){
    SamplingButtonManager::buttonPressed = false;
}

