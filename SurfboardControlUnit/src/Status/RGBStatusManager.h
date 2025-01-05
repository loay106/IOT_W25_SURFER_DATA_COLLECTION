#ifndef RGB_STATUS_MANAGER_H 
#define RGB_STATUS_MANAGER_H

#include "SystemStatus.h"
#include "../Data/Logger.h"

/*
    This class is responsible for the RGB status light to show the unit's current status
*/
class RGBStatusManager{
    private:
        SystemStatus currentStatus;
        Logger logger;
        int redPin;
        int greenPin;
        int bluePin;
        void updateStatusColor();
    public:
        RGBStatusManager(){};
        RGBStatusManager(Logger logger, int redPin, int greenPin, int bluePin);
        void initialize(SystemStatus currentSystemStatus);
        void updateStatus(SystemStatus newStatus); 
};


#endif /* RGB_STATUS_MANAGER_H */