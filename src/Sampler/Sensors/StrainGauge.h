#ifndef STRAING_GUAGE_H
#define STRAIN_GUAGE_H
#include <iostream>
#include <string>
#include "ForceBase.h"
#include "HX711.h"
#include <cmath>
using namespace std;

#define CALIBRATION_FACTOR 430
#define LOADCELL_DOUT_PIN 12
#define LOADCELL_SCK_PIN 13
const float GRAVITY = 9.81;


class StrainGuage : public ForceBase { 
    private:
        HX711 sensor;
    
    public:
        StrainGuage(ForceModel Model);
        void enableSensor() override;
        void disableSensor() override;
        bool getSensorStatus() override;
        string getSamples() override;
        void setup() override;
        int calculateDelayTime(int sampling_rate);// HZ
        //void stopSampling() override;
};


#endif // STRAIN_GUAGE_H
