#ifndef CONTROLLER_H 
#define CONTROLLER_H

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

typedef struct SamplingUnitRep{
    uint8_t mac[6];
    SamplingUnitStatus status;    
} SamplingUnitRep;

class Controller{
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
        Controller(){};
        Controller(uint8_t SDCardChipSelectPin, int serialBaudRate, int buttonPin);
        void initialize(uint8_t samplingUnits[][6], int samplingUnitsNum, int RGBRedPin, int RGBGreenPin, int RGBBluePin);
        
        void startSampling();
        void stopSampling();

        void updateSystem();
};

#endif /* CONTROLLER_H */