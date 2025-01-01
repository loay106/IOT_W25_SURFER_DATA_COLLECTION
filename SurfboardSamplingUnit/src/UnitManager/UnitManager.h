#ifndef UNIT_MANAGER_H
#define UNIT_MANAGER_H

#include "../Sensors/IMUBase.h"
#include <string>
#include <list>
#include "../ControlUnitSync/ESPNowControlUnitSyncManager.h"

/* 
    This class handles the logic for the sampling unit device.
    The device consists of the following hardware:
        1. ESP controller with WiFi compatibility
        2. Sensors (as needed, IMU, Strain gauge etc...)

    Workflow:
        1. Device initializes and tries to connect to the control unit (CONFIGURING)
        2. Once connected, device stays on STAND_BY status until a command is received (STAND_BY)
        3. If START_SAMPLING command was received, the devices starts sampling (SAMPLING)
        4. If STOP_SAMPLING command was received, the devices stops sampling and moves to STAND_BY status (STAND_BY)
        5. if an error occurs that stops the unit from working, the units stays on ERROR status (ERROR)

    Notes and constraints:
        1. Samples are sent in bulks and not seperately and are limited in size (see ControlUnit docs for format) 
        2. Devices handles the ID's of all its sensors (unique ones)
        3. Device is agnostic to time and timestamp! This is handled in the Control Unit when commands are sent

*/
//shada MAC : 08:b6:1f:33:49:e4

class UnitManager {
    public:
        UnitManagerStatus status;
        ESPNowControlUnitSyncManager* syncManager;
        std::list<IMUBase*> imuSensors;
//        std::list<IMUBase> strainSensors;
        UnitManager(ESPNowControlUnitSyncManager* syncManager);
        void addIMUSensor(IMUBase* sensor);
        void startSampling();
};




#endif // UNIT_MANAGER_H