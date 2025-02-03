#ifndef FORCE_HX711_H
#define FORCE_HX711_H

#include "SensorBase.h"
#include "HX711.h"

using namespace std;

const float GRAVITY = 9.81;

class Force_HX711 : public SensorBase { 
    private:
        HX711 sensor;
        volatile bool dataReady;
        int calibrationFactor;
        int doutPin;
        int sckPin;
        static void IRAM_ATTR Force_HX711_ISR(void* arg) {
            bool* flag = static_cast<bool*>(arg);
            *flag=true;
        }
    public:
        Force_HX711(Logger* logger, SDCardHandler* sdcardHandler, int calibrationFactor, int doutPin, int sckPin): SensorBase(logger, sdcardHandler, "HX711"){
            this->calibrationFactor = calibrationFactor;
            this->doutPin = doutPin;
            this->sckPin = sckPin;
            this->dataReady = false;
        }
        void enableSensor() override {};

        void disableSensor() override {
            logger->info("HX711 sensor disabled");
        };

        string getSample() override{
            if(dataReady){
                logger->debug("TRIGG!");
            }
            if (sensor.is_ready()){
                float mass_kg = sensor.get_units() / 1000;
                float force_N = mass_kg * GRAVITY;
                ostringstream oss;
                oss.precision(2);
                oss << std::fixed << force_N;
                return oss.str();
            }else {
                throw NotReadyError();
            }
        }

        void init() override{
            sensor.begin(doutPin, sckPin);
            sensor.set_scale(calibrationFactor);
            sensor.tare(); // Reset scale to zero
            // pinMode(doutPin, INPUT_PULLUP);
            //attachInterruptArg(digitalPinToInterrupt(doutPin), Force_HX711::Force_HX711_ISR, (void*)&dataReady, FALLING);
        }
};


#endif // FORCE_HX711_H
