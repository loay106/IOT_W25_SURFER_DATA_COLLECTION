#include <Arduino.h>
#include "ButtonHandler.h"

Logger ButtonHandler::logger* = Logger::getInstance();
bool ButtonHandler::buttonPressed = false;
int ButtonHandler::lastPressedAt = 0;

void ButtonHandler::init(int buttonPin){
    this->buttonPin = buttonPin;
    pinMode(buttonPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonPin), ButtonHandler::onButtonPress, FALLING);
}

bool ButtonHandler::wasPressed(){
    bool pressed = ButtonHandler::buttonPressed;
    ButtonHandler::buttonPressed = false;  // Clear the flag
    return pressed;
}

void IRAM_ATTR ButtonHandler::onButtonPress(){
    int currentMillis = millis();
    if(currentMillis - ButtonHandler::lastPressedAt >= 500){ // Only one press is considered every 0.5 seconds!
        ButtonHandler::buttonPressed = true;
        ButtonHandler::lastPressedAt = currentMillis;
    }
}

