#ifndef RGB_STATUS_MANAGER_H 
#define RGB_STATUS_MANAGER_H

using namespace std;
#include <cstdint>
#include <string>
#include "SystemStatus.h"

/*
    This class is responsible for the RGB status light to show the unit's current status
*/
class RGBStatusManager{
    private:
        SystemStatus currentStatus;
    public:
        RGBStatusManager(SystemStatus currentStatus);
        void updateStatus(SystemStatus newStatus); 
};


#endif /* RGB_STATUS_MANAGER_H */