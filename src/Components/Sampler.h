#ifndef SAMPLER_H
#define SAMPLER_H

#include <vector>
#include <string>
using namespace std;

#include "../Utils/Status.h"
#include "../Components/Sensors/SensorBase.h"


class Sampler {
    private:
        vector<SensorBase> sensors; // change to pointer?
        SamplerStatus status;
        Logger logger;
    public:
        Sampler(){};
        
        Sampler(Logger logger): logger(logger), status(SamplerStatus::UNIT_STAND_BY){};

        void addSensor(SensorBase sensor){
            sensor.init();
            sensors.push_back(sensor);
        }

        void startSampling(int timeStamp, int IMURate){
            status = SamplerStatus::UNIT_SAMPLING;
            for(SensorBase& sensor: sensors){
                // todo: add file name for the sensor here
                sensor.startSampling(timeStamp);
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

        void uploadSampleFiles(); // upload to the cloud
};

#endif // SAMPLER_H