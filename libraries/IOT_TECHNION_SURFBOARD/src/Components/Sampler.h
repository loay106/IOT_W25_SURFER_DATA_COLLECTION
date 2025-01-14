#ifndef SAMPLER_H
#define SAMPLER_H

#include <vector>
#include <string>
#include <Arduino.h>
using namespace std;

#include "../Utils/Status.h"
#include "Sensors/SensorBase.h"
#include "IO/SDCardHandler.h"


class Sampler {
    // todo: change class to singleton
    private:
        vector<SensorBase*> sensors;
        SamplerStatus status;
        Logger logger;
        SDCardHandler sdCardHandler;

    public:
        Sampler(){};
        
        Sampler(Logger logger, SDCardHandler sdCardHandler);

        void addSensor(SensorBase* sensor);

        void init();

        void startSampling(int timestamp, int IMURate);
        void stopSampling();

        SamplerStatus getStatus();

        // use this when you want the unit to enter error state for external reasons
        void enterErrorState();

        void uploadSampleFiles(string wifi_ssid, string wifi_password){
            // upload to the cloud
            // todo: implement...
            status = SamplerStatus::UNIT_STAND_BY;
        }; 

        void writeSensorsData();
};

#endif // SAMPLER_H