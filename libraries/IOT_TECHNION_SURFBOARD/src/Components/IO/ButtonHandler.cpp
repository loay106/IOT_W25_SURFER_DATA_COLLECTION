#include <Arduino.h>
#include "ButtonHandler.h"

const int ButtonHandler::DEBOUNCE_PERIOD_MILLIES = 500;

void IRAM_ATTR ButtonHandler::ButtonISTR(void* arg) {
    ButtonHandler* handler = static_cast<ButtonHandler*>(arg);
    int currentMillis = millis();
    if (currentMillis - handler->lastPressedAt >= ButtonHandler::DEBOUNCE_PERIOD_MILLIES) { 
        handler->buttonPressed = true;
        handler->lastPressedAt = currentMillis;
    }
}

ButtonHandler::ButtonHandler(Logger *logger, int buttonPin){
    this->buttonPressed = false;
    this->lastPressedAt = 0;
    this->buttonPin = buttonPin;
    this->logger = logger;
}

void ButtonHandler::init() {
    pinMode(buttonPin, INPUT_PULLUP);
    attachInterruptArg(digitalPinToInterrupt(buttonPin), ButtonHandler::ButtonISTR, this, FALLING);
}

bool ButtonHandler::wasPressed(){
    bool pressed = buttonPressed;
    buttonPressed = false; // Clear the flag
    return pressed;
}
