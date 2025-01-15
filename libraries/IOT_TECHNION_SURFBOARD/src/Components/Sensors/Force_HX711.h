#ifndef FORCE_HX711_H
#define FORCE_HX711_H

#include "SensorBase.h"
#include "HX711.h"

using namespace std;

//#define CALIBRATION_FACTOR 430
//#define LOADCELL_DOUT_PIN 12
//#define LOADCELL_SCK_PIN 13

const float GRAVITY = 9.81;


class Force_HX711 : public SensorBase { 
    private:
        HX711 sensor;
        int calibrationFactor;
        int doutPin;
        int sckPin;
    public:
        Force_HX711(Logger logger, SDCardHandler sdcardHandler, int calibrationFactor, int doutPin, int sckPin): SensorBase(logger, sdcardHandler, "HX711"){
            this->calibrationFactor = calibrationFactor;
            this->doutPin = doutPin;
            this->sckPin = sckPin;
        }
        void enableSensor(int rate) override {};

        void disableSensor() override {};

        string getSample() override{
            if (scale.is_ready()){
                    float mass_kg = scale.get_units() / 1000;
                    float force_N = mass_kg * GRAVITY;
                    ostringstream oss;
                    oss.precision(2);
                    oss << std::fixed << force_N;
                    return oss.str();
                }else{
                    throw NotReadyError();
                }
        }

        void init() override{
            scale.begin(doutPin, sckPin);
            scale.set_scale(calibrationFactor);
            scale.tare(); // Reset scale to zero
        }

        void updateSensor(void* param){
            int calibrationF = *(int*) param;
            calibrationFactor = calibrationF;
        }
};


#endif // FORCE_HX711_H
