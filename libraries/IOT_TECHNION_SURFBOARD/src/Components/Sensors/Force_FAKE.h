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
        int delay_time;
        bool sensor_enabled;
    public:
        Force_FAKE(Logger* logger, SDCardHandler* sdcardHandler): SensorBase(logger, sdcardHandler, "FAKE_HX711"),delay_time(0),
        sensor_enabled(false){}
        void enableSensor(int rate) override {
            double time_delay = 1000000.0/rate;
            delay_time = ceil(time_delay);
            sensor_enabled=true;
        };

        void disableSensor() override {
            sensor_enabled=false;
            logger->info("Fake HX711 sensor disabled");
        };

        int getDelayTime(){return delay_time;}

        string getSample() override{
            if(sensor_enabled)
            {
                samples_count++;
                float mass_kg = random(0,1000);
                float force_N = mass_kg * GRAVITY;
                ostringstream oss;
                oss.precision(2);
                oss << std::fixed << force_N;
                return oss.str();
            }
            else{
                throw NotReadyError();
            }
         
        }

        void init() override{}

        void updateSensor(void* param)override{}
};


#endif // FORCE_FAKE
