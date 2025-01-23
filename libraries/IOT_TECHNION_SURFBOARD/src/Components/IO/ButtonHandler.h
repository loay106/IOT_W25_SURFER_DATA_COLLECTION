#ifndef BUTTON_HANDLER_H 
#define BUTTON_HANDLER_H

#include "Logger.h"
#include <Arduino.h>

class ButtonHandler{
    private:
        Logger* logger;
        int buttonPin;
        volatile bool buttonPressed;
        volatile int lastPressedAt;
        static void IRAM_ATTR ButtonISTR(void* arg);
    public:
        static const int DEBOUNCE_PERIOD_MILLIES;
        ButtonHandler(Logger* logger, int buttonPin);
        void init();
        bool wasPressed();
};

#endif /* BUTTON_HANDLER_H */