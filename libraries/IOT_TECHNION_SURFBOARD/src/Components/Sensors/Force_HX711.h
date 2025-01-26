#ifndef FORCE_HX711_H
#define FORCE_HX711_H

#include "SensorBase.h"
#include "HX711.h"

using namespace std;

//#define CALIBRATION_FACTOR 430
//#define LOADCELL_DOUT_PIN 12
//#define LOADCELL_SCK_PIN 13


class Force_HX711 : public SensorBase { 
    private:
        HX711 sensor;
        int calibrationFactor;
        int doutPin;
        int sckPin;
        int delay_time;
    public:
        Force_HX711(Logger* logger, SDCardHandler* sdcardHandler, int calibrationFactor, int doutPin, int sckPin): SensorBase(logger, sdcardHandler, "HX711"){
            this->calibrationFactor = calibrationFactor;
            this->doutPin = doutPin;
            this->sckPin = sckPin;
        }
        void enableSensor() override {};

        int getDelayTime(){
            double time_delay = 1000000.0/80; // max rate for this sensor
            return delay_time;
        }

        void disableSensor() override {
            sensor.power_down();
            logger->info("HX711 sensor disabled");
        };

        string getSample() override{
            if (sensor.is_ready()){
                samples_count++;
                float mass_kg = sensor.get_units() / 1000;
                float force_N = mass_kg * GRAVITY;
                ostringstream oss;
                oss.precision(2);
                oss << std::fixed << force_N;
                return oss.str();
            }
            else
            {
                throw NotReadyError();
            }
        }

        void init() override{
            sensor.begin(doutPin, sckPin);
            sensor.set_scale(calibrationFactor);
            sensor.tare(); // Reset scale to zero
        }
};


#endif // FORCE_HX711_H
