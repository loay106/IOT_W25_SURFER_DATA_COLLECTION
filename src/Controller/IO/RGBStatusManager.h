#ifndef RGB_STATUS_HANDLER_H 
#define RGB_STATUS_HANDLER_H

#include "SystemStatus.h"
#include "../Data/Logger.h"

/*
    This class is responsible for the RGB status light to show the unit's current status
*/
class RGBStatusHandler{
    private:
        SystemStatus currentStatus;
        Logger logger;
        void updateStatusColor();
    public:
        RGBStatusHandler(){};
        RGBStatusHandler(Logger logger);
        void initialize(SystemStatus currentSystemStatus, int redPin, int greenPin, int bluePin);
        void updateStatus(SystemStatus newStatus); 
};


#endif /* RGB_STATUS_HANDLER_H */