#include "ButtonHandler.h"

const int ButtonHandler::DEBOUNCE_PERIOD_MILLIS = 50;  // Debounce time
const int ButtonHandler::LONG_PRESS_THRESHOLD_MILLIS = 1000; // Long press threshold

void IRAM_ATTR ButtonHandler::ButtonISR(void* arg) {
    ButtonHandler* handler = static_cast<ButtonHandler*>(arg);
    unsigned long currentMillis = millis();

    if (digitalRead(handler->buttonPin) == LOW) {
        if (!handler->isPressing && (currentMillis - handler->pressStartTime > ButtonHandler::DEBOUNCE_PERIOD_MILLIS)) {
            handler->pressStartTime = currentMillis; // Record press start time
            handler->isPressing = true;
            handler->isReleased = false;
        }
    }else {
        if (handler->isPressing && (currentMillis - handler->pressStartTime > ButtonHandler::DEBOUNCE_PERIOD_MILLIS)) {
            handler->pressDuration = currentMillis - handler->pressStartTime; // Calculate press duration
            handler->isPressing = false;
            handler->isReleased = true;
            handler->isPressHandled = false; // Ready to process
        }
    }
}


ButtonHandler::ButtonHandler(Logger* logger, int buttonPin) {
    this->logger = logger;
    this->buttonPin = buttonPin;
    this->pressStartTime = 0;
    this->pressDuration = 0;
    this->isPressing = false;
    this->isReleased = false;
    this->isPressHandled = true;
}

void ButtonHandler::init() {
    pinMode(buttonPin, INPUT_PULLUP);
    attachInterruptArg(digitalPinToInterrupt(buttonPin), ButtonHandler::ButtonISR, this, CHANGE);
}

ButtonPressType ButtonHandler::getLastPressType(){
    if (!isPressing && isReleased && !isPressHandled) {
        isPressHandled = true; // Mark this press as handled
        if (pressDuration >= LONG_PRESS_THRESHOLD_MILLIS) {
            return ButtonPressType::LONG_PRESS; // Detected a soft press
        }else{
            return ButtonPressType::SOFT_PRESS;
        }
    }
    return ButtonPressType::NO_PRESS;
}