#ifndef STRAING_GUAGE_H
#define STRAIN_GUAGE_H

#include "SensorBase.h"
#include "HX711.h"

using namespace std;

//#define CALIBRATION_FACTOR 430
//#define LOADCELL_DOUT_PIN 12
//#define LOADCELL_SCK_PIN 13

const float GRAVITY = 9.81;


class StrainGuage : public SensorBase { 
    private:
        HX711 sensor;
        int calibrationFactor;
        int doutPin;
        int sckPin;
    public:
        StrainGuage(Logger logger, SamplesSDCardWriter samplesWriter, int calibrationFactor, int doutPin, int sckPin): SensorBase(logger, samplesWriter, "HX711", doutPin){
            this->calibrationFactor = calibrationFactor;
            this->doutPin = doutPin;
            this->sckPin = sckPin;
        }
        virtual void enableSensor() override {};

        virtual void disableSensor() override {};

        virtual string getSample() override{
            if (scale.is_ready()){
                    float mass_kg = scale.get_units() / 1000;
                    float force_N = mass_kg * GRAVITY;
                    Serial.print("Mass: ");
                    Serial.print(mass_kg, 3);
                    Serial.print(" kg, Force: ");
                    Serial.print(force_N, 2);
                    Serial.println(" N");
                    String force_str = String(force_N, 2);
                    return force_str;
                }else{
                    throw NotReadyError();
                }
        }

        void init() override{
            Serial.begin(115200);
            Serial.println("HX711 Force Measurement Test");
            scale.begin(doutPin, sckPin);
            scale.set_scale(calibrationFactor);
            scale.tare(); // Reset scale to zero
        }
};


#endif // STRAIN_GUAGE_H
