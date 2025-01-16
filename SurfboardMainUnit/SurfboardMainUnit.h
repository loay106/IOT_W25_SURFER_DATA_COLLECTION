#ifndef SURFBOARD_MAIN_UNIT_H
#define SURFBOARD_MAIN_UNIT_H

#include <vector>
#include <map>
#include <string>
using namespace std;

#include <Arduino.h>

#include <IOT_TECHNION_SURFBOARD.h>


const string CONFIG_FILE_NAME = "device.config";

typedef struct SamplingUnitRep{
    uint8_t mac[6];
    SamplerStatus status;
} SamplingUnitRep;

class SurfboardMainUnit {
    /*
        todo: add these functionlities:
            1. read sensor update params if in standby mode (every X minutes) then send commands accorindgly
            2. handle upload files functionality
            3. review updateSystem and debug it
            4. debug messages sync between the units
    */
    private:
        std::map<string, SamplingUnitRep> samplingUnits; // mac string to instance mapping
        ControlUnitSyncManager* syncManager;
        RTCTimeHandler* timeHandler;
        RGBStatusHandler* statusLighthandler;
        ButtonHandler* buttonHandler;
        Logger* logger;
        Sampler* sampler; // internal sampler
        SDCardHandler* sdCardHandler;

        SystemStatus status;
        std::map<string,string> WIFI_PARAMS;
        std::map<string,string> SAMPLING_PARAMS;

        void updateStatus(SystemStatus newStatus);

        void startSampling();
        void stopSampling();
        void UpdateSensorsParams(uint8_t samplingUnitMac[], std::map<string,string> newSensorParams);
    public:
        SurfboardMainUnit(ControlUnitSyncManager* syncManager, RTCTimeHandler* timeHandler, RGBStatusHandler* statusLighthandler, ButtonHandler* buttonHandler, Logger* logger, Sampler* sampler, SDCardHandler* sdCardHandler);
        void init(uint8_t samplingUnitsAdresses[][6], int samplingUnitsNum);
        void addSensor(SensorBase* sensor);
        void updateSystem(); // update system periodically (i.e call this method every 20 ms..)

};

#endif // SURFBOARD_MAIN_UNIT_H