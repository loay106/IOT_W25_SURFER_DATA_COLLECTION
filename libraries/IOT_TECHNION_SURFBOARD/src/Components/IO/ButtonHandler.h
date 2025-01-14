#ifndef BUTTON_HANDLER_H 
#define BUTTON_HANDLER_H

#include "../../Utils/Logger.h"

class ButtonHandler{
    private:
        static Logger logger;
        int buttonPin;
        static bool buttonPressed;
        static int lastPressedAt;
        static void onButtonPress();
    public:
        ButtonHandler(){};
        ButtonHandler(Logger logger, int buttonPin);
        void init();
        bool wasPressed();
};

#endif /* BUTTON_HANDLER_H */