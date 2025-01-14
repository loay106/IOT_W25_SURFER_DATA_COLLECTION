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

typedef struct SamplingUnitRep{
    uint8_t mac[6];
    SamplerStatus status;
} SamplingUnitRep;

class SurfboardMainUnit {
    private:
        std::map<string, SamplingUnitRep> samplingUnits; // mac string to instance mapping
        ControlUnitSyncManager syncManager;
        RTCTimeHandler timeHandler;
        RGBStatusHandler statusLighthandler;
        ButtonHandler buttonHandler;
        Logger logger;
        Sampler sampler; // internal sampler
        SystemStatus status;
        map<string,string> WIFI_PARAMS;
        map<string,string> SAMPLING_PARAMS;

        void updateStatus(SystemStatus newStatus);
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