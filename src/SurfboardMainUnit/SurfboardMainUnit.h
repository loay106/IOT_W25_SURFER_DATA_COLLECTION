#ifndef SURFBOARD_MAIN_UNIT_H
#define SURFBOARD_MAIN_UNIT_H

#include <vector>
#include <string>
using namespace std;

#include "../Components/Sampler.h"
#include "../Sync/ControlUnitSyncManager.h"
#include "../Utils/Logger.h"
#include "../Utils/Status.h"
#include "../Components/IO/RTCTimeHandler.h"
#include "../Components/IO/RGBStatusHandler.h"
#include "../Components/IO/ButtonHandler.h"

#include "../Status/RGBStatusManager.h"
#include "../IO/SamplingButtonManager.h"

// todo: merge Controller to this class and remove Controller...
// add is_internal to SamplingUnitRep

typedef struct SamplingUnitRep{
    uint8_t mac[6];
    SamplerStatus status;    
} SamplingUnitRep;

const uint8_t[6] INTERNAL_SAMPLING_MAC = {0x0,0x0,0x0,0x0,0x0,0x0};

class SurfboardMainUnit {
    private:
        std::map<string, SamplingUnitRep> samplingUnits; // mac string to instance mapping
        ControlUnitSyncManager syncManager;
        RTCTimeHandler timeHandler;
        RGBStatusHandler statusLighthandler;
        ButtonHandler buttonHandler;
        Logger logger;
        Sampler sampler;
        SystemStatus status;
    public:
        SurfboardMainUnit(){};
        SurfboardMainUnit(int buttonPin);

        void init(uint8_t samplingUnitsAdresses[][6], int samplingUnitsNum, int RGBRedPin, int RGBGreenPin, int RGBBluePin);

        void addSensor(SensorBase sensor);

        void startSampling();
        void stopSampling();

        void updateSystem();

};

#endif // SURFBOARD_MAIN_UNIT_H