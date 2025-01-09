#include "RGBStatusManager.h"
#include <Arduino.h>

void RGBStatusManager::updateStatusColor(){
    switch (currentStatus){
        case SystemStatus::SYSTEM_STARTING:{
            // yellow
            ledcWrite(0, 255);
            ledcWrite(1, 255); 
            ledcWrite(2, 0); 
            break;
        }
        case SystemStatus::SYSTEM_INITIALIZING:{
            // yellow
            ledcWrite(0, 255);
            ledcWrite(1, 255); 
            ledcWrite(2, 0); 
            break;
        }
        case SystemStatus::SYSTEM_STAND_BY:{
            // green
            ledcWrite(0, 0); 
            ledcWrite(1, 255);
            ledcWrite(2, 0); 
            break;
        }
        case SystemStatus::SYSTEM_SAMPLING:{
            // blue
            ledcWrite(0, 0);
            ledcWrite(1, 0); 
            ledcWrite(2, 255); 
            break;
        }
        case SystemStatus::SYSTEM_ERROR:{
            // red
            ledcWrite(0, 255); 
            ledcWrite(1, 0); 
            ledcWrite(2, 0);
            break;
        }
        default:{
            // todo: throw an error here
            return;
        }
    }
}

RGBStatusManager::RGBStatusManager(Logger logger) : logger(logger){
    currentStatus = SystemStatus::SYSTEM_STARTING;
}

void RGBStatusManager::initialize(SystemStatus currentSystemStatus, int redPin, int greenPin, int bluePin) {
    ledcSetup(0, 5000, 8); // Channel 0, 5kHz, 8-bit resolution
    ledcSetup(1, 5000, 8); // Channel 1, 5kHz, 8-bit resolution
    ledcSetup(2, 5000, 8); // Channel 2, 5kHz, 8-bit resolution

    ledcAttachPin(redPin, 0);
    ledcAttachPin(greenPin, 1);
    ledcAttachPin(bluePin, 2);

    currentStatus = SystemStatus::SYSTEM_INITIALIZING;
    updateStatusColor();
}
void RGBStatusManager::updateStatus(SystemStatus newStatus) {
    currentStatus = newStatus;
    updateStatusColor();
};