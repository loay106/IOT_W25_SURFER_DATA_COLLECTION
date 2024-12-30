#include "TimeManager.h"

TimeManager::TimeManager(){}

void TimeManager::initialize()
{
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        Serial.flush();
  }
}

DateTime TimeManager::getCurrentDatetime(){
    return rtc.now();
}

void TimeManager::adjust(DateTime current){
    rtc.adjust(current);
}
