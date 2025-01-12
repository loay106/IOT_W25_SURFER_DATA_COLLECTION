#ifndef TIME_HANDLER_H 
#define TIME_HANDLER_H

#include "RTClib.h"
#include "../Data/Logger.h"

class TimeHandler{
    private:
        Logger logger;
        RTC_DS1307 rtc;
    public:
        TimeHandler(){};
        TimeHandler(Logger logger);
        void initialize();
        int getCurrentTimestamp();
        void adjust(DateTime current);
};


#endif /* TIME_HANDLER_H */