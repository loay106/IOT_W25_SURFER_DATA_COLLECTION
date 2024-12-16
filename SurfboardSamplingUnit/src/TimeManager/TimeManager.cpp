#include "TimeManager.h"

#include <Arduino.h>

TimeManager::TimeManager(){
    isConfigured = false;
}

void TimeManager::adjust(int currentTimeStamp){
    receivedTimestamp = currentTimeStamp;
    this->syncMillis = millis();
    isConfigured = true;
}
int TimeManager::getCurrentTimeStamp(){
    if (!isConfigured){
        // todo: throw exception....
        return -1;
    }
    return receivedTimestamp + (millis() - syncMillis) / 1000;
}
