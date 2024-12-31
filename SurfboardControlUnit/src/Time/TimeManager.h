#ifndef TIME_MANAGER_H 
#define TIME_MANAGER_H

#include "RTClib.h"
#include <src/Data/Logger.h>

class TimeManager{
    private:
        Logger logger;
        RTC_DS1307 rtc;
    public:
        TimeManager(Logger logger);
        void initialize();
        DateTime getCurrentDatetime();
        int getCurrentTimestamp();
        void adjust(DateTime current);
};


#endif /* TIME_MANAGER_H */