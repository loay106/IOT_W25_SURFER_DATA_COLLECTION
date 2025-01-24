#ifndef SAMPLING_BUTTON_MANAGER_H 
#define SAMPLING_BUTTON_MANAGER_H

#include "../Data/Logger.h"

class SamplingButtonManager{
    private:
        static Logger logger;
        int buttonPin;
        static bool buttonPressed;
        static int lastPressedAt;
        static void onButtonPress();
    public:
        SamplingButtonManager(){};
        SamplingButtonManager(Logger logger, int buttonPin);
        void initialize();
        bool wasPressed();
};

#endif /* SAMPLING_BUTTON_MANAGER_H */