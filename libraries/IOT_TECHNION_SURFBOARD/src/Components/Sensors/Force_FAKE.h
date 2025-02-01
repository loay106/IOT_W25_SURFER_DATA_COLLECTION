#ifndef FORCE_FAKE_H
#define FORCE_FAKE_H

#include "SensorBase.h"
#include "HX711.h"

using namespace std;

class Force_FAKE : public SensorBase { 
    private:
        /*int calibrationFactor;
        int doutPin;
        int sckPin;*/
        bool sensor_enabled;
    public:
        Force_FAKE(Logger* logger, SDCardHandler* sdcardHandler): SensorBase(logger, sdcardHandler, "FAKE-HX711"),
        sensor_enabled(false){}
        void enableSensor() override {
            sensor_enabled=true;
        };

        void disableSensor() override {
            sensor_enabled=false;
            logger->info("Fake HX711 sensor disabled");
        };

        string getSample() override{
            if(sensor_enabled){
                ostringstream oss;
                oss.precision(2);
                oss << std::fixed << random(0,1000) * 9.81;
                return oss.str();
            }else{
                throw NotReadyError();
            }
        }

        void init() override{}
};


#endif // FORCE_FAKE
