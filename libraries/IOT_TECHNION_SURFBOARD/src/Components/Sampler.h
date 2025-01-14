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
    private:
        vector<SensorBase> sensors; // change to pointer?
        SamplerStatus status;
        Logger logger;
        SDCardHandler sdCardHandler;
    public:
        Sampler(){};
        
        Sampler(Logger logger, SDCardHandler sdCardHandler): logger(logger), sdCardHandler(sdCardHandler), status(SamplerStatus::UNIT_STAND_BY){};

        void addSensor(SensorBase sensor){
            sensor.init();
            sensors.push_back(sensor);
        }

        void init(){
            try{
                sdCardHandler.createFolder("samplings");
            }catch(SDCardError& err){
                logger.error("Failed to create samplings folder");
                status = SamplerStatus::UNIT_ERROR;
                throw InitError();
            }
        }

        void startSampling(int timestamp, int IMURate){
            status = SamplerStatus::UNIT_SAMPLING;
            for(int i=0;i<sensors.size(); i++){
                // sample files have this format:
                // [TIMESTAMP]_[SENSOR_ID]_[SENSOR_MODEL]
                string filePath = "samplings/" + to_string(timestamp) + "_" + to_string(i) "_" + sensors[i].getModel();
                sensors[i].startSampling(filePath, IMURate);
            }
        }

        void stopSampling(){
            for(SensorBase& sensor: sensors){
                sensor.stopSampling();
            }
            status = SamplerStatus::UNIT_STAND_BY;
        }

        SamplerStatus getStatus(){
            return status;
        }

        void enterErrorState(){
            // use this when you want the unit to enter error state for external reasons
            status = SamplerStatus::UNIT_ERROR;
        }

        void uploadSampleFiles(string wifi_ssid, string wifi_password){
            // upload to the cloud
            // todo: implement...
            status = SamplerStatus::UNIT_STAND_BY;
        }; 
};

#endif // SAMPLER_H