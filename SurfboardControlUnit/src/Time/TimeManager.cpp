#include "TimeManager.h"
#include "../Exceptions/UnitExceptions.h"

TimeManager::TimeManager(Logger logger): logger(logger){}

void TimeManager::initialize()
{
    if (!rtc.begin()) {
        logger.error("Couldn't find RTC device. Check the wiring");
        throw InitError();
    }
}

int TimeManager::getCurrentTimestamp(){
    DateTime time = rtc.now();
    return time.unixtime();
}

void TimeManager::adjust(DateTime current){
    rtc.adjust(current);
}
