#ifndef RGB_STATUS_HANDLER_H 
#define RGB_STATUS_HANDLER_H

#include "Logger.h"
#include <Arduino.h>

const int FLICKERING_RATE = 500;

enum RGBColors{
    NO_COLOR,
    RED,
    BLUE, 
    GREEN
};

class RGBStatusHandler{
    private:
        RGBColors firstColor;
        RGBColors secondColor;
        Logger* logger;
        int lastFlickerMillis;
        void showColor();
    public:
        RGBStatusHandler(Logger* logger);
        void init(int redPin, int greenPin, int bluePin);
        void updateColors(RGBColors first, RGBColors second);
        void flicker();
};


#endif /* RGB_STATUS_HANDLER_H */