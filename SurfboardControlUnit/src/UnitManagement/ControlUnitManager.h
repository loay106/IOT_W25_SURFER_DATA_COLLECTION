#ifndef CONTROL_UNIT_MANAGER_H 
#define CONTROL_UNIT_MANAGER_H

using namespace std;
#include <map>
#include <string>

#include "../Sync/ESPNowSyncManager.h"
#include "../Status/SystemStatus.h"
#include "../Data/SamplingDataWriter.h"
#include "../Data/Logger.h"
#include "../Time/TimeManager.h"

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
        TimeManager timeManager;
        Logger logger;

        SystemStatus status;
        string* samplingFileName;

    public:
        ControlUnitManager(const uint8_t SDCardChipSelectPin, const int serialBaudRate);
        void initialize(uint8_t samplingUnits[][], int samplingUnitsNum);
        
        void startSampling();
        void stopSampling();

        void updateSystem();
};

typedef struct SamplingUnitRep{
    uint8_t mac[6];
    SamplingUnitStatus status;    
} SamplingUnitRep;

#endif /* CONTROL_UNIT_MANAGER_H */