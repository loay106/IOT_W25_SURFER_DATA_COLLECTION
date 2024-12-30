#ifndef CONTROL_UNIT_MANAGER_H 
#define CONTROL_UNIT_MANAGER_H

using namespace std;
#include <map>
#include <string>
#include <src/Sync/ESPNowSyncManager.h>
#include <src/Status/SystemStatus.h>
#include <src/Data/DataLogger.h>
#include <SamplingUnitRep.h>
#include <src/Time/TimeManager.h>

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
        map<string, SamplingUnitRep> samplingUnits; // id to instance mapping
        ESPNowSyncManager espSyncManager;
        DataLogger dataLogger;
        SystemStatus status;
        TimeManager timeManager;

        void handleSyncMessages();

    public:
        ControlUnitManager(ESPNowSyncManager espSyncManager, DataLogger dataLogger, TimeManager timeManager);
        void initialize();

        string addSamplingUnit();
        void updateSamplingUnitStatus(string unitID, SamplingUnitStatus status);

        void startSampling();
        void stopSampling();

        void updateSystem();
};


#endif /* CONTROL_UNIT_MANAGER_H */