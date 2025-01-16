#ifndef BUTTON_HANDLER_H 
#define BUTTON_HANDLER_H

#include "Logger.h"
#include <Arduino.h>

class ButtonHandler{
    // singleton class
    private:
        static Logger* logger;
        static ButtonHandler* instance;
        int buttonPin;
        static bool buttonPressed;
        static int lastPressedAt;
        static void onButtonPress();
        ButtonHandler(){
            buttonPin=false;
        };
    public:
        ButtonHandler(const ButtonHandler& obj) = delete;
        static ButtonHandler* getInstance() {
            if (instance == nullptr) {
                instance = new ButtonHandler();
            }
            return instance;
        }
        void init(int buttonPin);
        bool wasPressed();
};

#endif /* BUTTON_HANDLER_H */