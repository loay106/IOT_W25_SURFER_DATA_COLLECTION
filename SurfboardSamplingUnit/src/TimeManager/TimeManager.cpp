#include "TimeManager.h"

#include <Arduino.h>

TimeManager::TimeManager(int currentTimeStamp){
    receivedTimestamp = currentTimeStamp;
    syncMillis = millis();
}

int TimeManager::getCurrentTimeStamp(){
    // todo: test this......
    return receivedTimestamp + (millis() - syncMillis) / 1000;
}
