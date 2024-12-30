#ifndef TIME_MANAGER_H 
#define TIME_MANAGER_H

#include "RTClib.h"

class TimeManager{
    private:
        RTC_DS1307 rtc;
    public:
        TimeManager();
        void initialize();
        DateTime getCurrentDatetime();
        void adjust(DateTime current);
};


#endif /* TIME_MANAGER_H */