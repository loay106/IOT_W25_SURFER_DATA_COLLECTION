#ifndef CONTROL_UNIT_MANAGER_H 
#define CONTROL_UNIT_MANAGER_H

using namespace std;
#include <map>
#include <string>
#include <src/SamplingUnitSync/ESPNowSyncManager.h>
#include <src/DataManagement/SDCardSampleLogger.h>
#include <SamplingUnit.h>

/*
    Class handling the logic of the device. Only one instance is needed
    // todo: change to singleton maybe...

    This device consits of the following parts:
        1. ESP controller with WIFI compatibility
        2. SD Card
        3. RTC 
        4. RGB light (status updates)
 */

class ControlUnitManager{
    private:
        map<string, SamplingUnit> samplingUnits; // id to instance mapping
        ESPNowSyncManager espSyncManager;
        SDCardSampleLogger sampleLogger;
        SystemStatus status;

    public:
        ControlUnitManager();

        string addSamplingUnit();
        void updateSamplingUnitStatus(string unitID, SamplingUnitStatus status);

        void handleSyncMessages();
        void sendCommand(ControlUnitCommand command, string samplingUnitID); // send to specific unit   
        void broadcastCommand(ControlUnitCommand command); // send to all units
};

enum SystemStatus{
    INITILAZING,
    STAND_BY,
    SAMPLING,
    FILE_UPLOAD,
    ERROR
    // add more ass needed
};

enum ControlUnitCommand{
    START_SAMPLING, // send timestamp here
    STOP_SAMPLING,
    START_SAMPLE_FILE_TRANSFER,
    STOP_SAMPLE_FILE_TRANSFER
};


#endif /* CONTROL_UNIT_MANAGER_H */