#ifndef CONTROL_UNIT_MANAGER_H 
#define CONTROL_UNIT_MANAGER_H

using namespace std;
#include <map>
#include <string>
#include <src/Sync/ESPNowSyncManager.h>
#include <src/Status/SystemStatus.h>
#include <src/Data/SamplingDataWriter.h>
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
        SamplingDataWriter samplingDataWriter;
        SystemStatus status;
        TimeManager timeManager;
        string* samplingFileName;

        void updateSamplingUnitStatus(uint8_t samplingUnitMac[], SamplingUnitStatus status);
        void addSamplingUnit(uint8_t mac[]);

    public:
        ControlUnitManager(const uint8_t SDCardChipSelectPin);
        void initialize(uint8_t samplingUnits[][], int samplingUnitsNum);
        void startSampling();
        void stopSampling();

        /*
            1. Handle status updates from sampling units in espSyncManager
            2. Handle sampling data from sampling units in espSyncManager
            3. 
        */
        void updateSystem();
};

typedef struct SamplingUnitRep{
    uint8_t mac[6];
    SamplingUnitStatus status;    
} SamplingUnitRep;

#endif /* CONTROL_UNIT_MANAGER_H */