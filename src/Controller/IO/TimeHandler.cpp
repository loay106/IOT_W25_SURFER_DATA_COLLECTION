#include "TimeHandler.h"
#include "../Exceptions/UnitExceptions.h"

TimeHandler::TimeHandler(Logger logger): logger(logger){}

void TimeHandler::initialize()
{
    if (!rtc.begin()) {
        logger.error("Couldn't find RTC device. Check the wiring");
        throw InitError();
    }
}

int TimeHandler::getCurrentTimestamp(){
    DateTime time = rtc.now();
    return time.unixtime();
}

void TimeHandler::adjust(DateTime current){
    rtc.adjust(current);
}
