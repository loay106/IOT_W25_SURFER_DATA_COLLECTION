#ifndef CONTROLLER_H 
#define CONTROLLER_H

using namespace std;
#include <map>
#include <string>

#include "Sync/ControlUnitSyncManager.h"
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

class Controller{
    private:
        std::map<string, SamplingUnitRep> samplingUnits; // mac string to instance mapping
        ControlUnitSyncManager syncManager;
        RTCTimeHandler timeHandler;
        RGBStatusHandler statusLighthandler;
        ButtonHandler buttonHandler;
        Logger logger;

        SystemStatus status;

    public:
        Controller(){};
        Controller(Logger logger, int buttonPin);
        void init(uint8_t samplingUnits[][6], int samplingUnitsNum, int RGBRedPin, int RGBGreenPin, int RGBBluePin);
        
        void startSampling();
        void stopSampling();

        void updateSystem();
};

#endif /* CONTROLLER_H */