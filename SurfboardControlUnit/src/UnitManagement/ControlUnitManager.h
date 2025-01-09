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
#include "../Status/RGBStatusManager.h"
#include "../IO/SamplingButtonManager.h"

/*
    Class handling the logic of the device. Only one instance is needed
    // todo: change to singleton maybe...

    This device consits of the following parts:
        1. ESP controller with WIFI compatibility
        2. SD Card
        3. RTC 
        4. RGB analong light
        5. Press button
 */

typedef struct SamplingUnitRep{
    uint8_t mac[6];
    SamplingUnitStatus status;    
} SamplingUnitRep;

class ControlUnitManager{
    private:
        std::map<string, SamplingUnitRep> samplingUnits; // id to instance mapping
        ESPNowSyncManager espSyncManager;
        SamplingDataWriter samplingDataWriter;
        TimeManager timeManager;
        RGBStatusManager statusLightManager;
        SamplingButtonManager buttonManager;
        Logger logger;

        SystemStatus status;
        string* samplingFileName;
        int samplesCount; // samples count in a sampling session

    public:
        ControlUnitManager(){};
        ControlUnitManager(uint8_t SDCardChipSelectPin, int serialBaudRate, int buttonPin);
        void initialize(uint8_t samplingUnits[][6], int samplingUnitsNum, int RGBRedPin, int RGBGreenPin, int RGBBluePin);
        
        void startSampling();
        void stopSampling();

        void updateSystem();
};

#endif /* CONTROL_UNIT_MANAGER_H */