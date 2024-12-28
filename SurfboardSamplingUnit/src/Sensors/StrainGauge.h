#ifndef STRAING_GUAGE_H
#define STRAIN_GUAGE_H
#include <iostream>
#include <string>
#include "ForceBase.h"
#include "HX711.h"
using namespace std;

#define CALIBRATION_FACTOR 438
#define LOADCELL_DOUT_PIN 12
#define LOADCELL_SCK_PIN 13

class StrainGuage : public ForceBase { 

    private:
        bool force_calibration_enabled;
        bool force_weight_enabled;
        HX711 sensor;
    
    public:
        StrainGuage(ForceModel Model);
        void enableForceCalibrationMeasurement();
        void disableForceCalibrationMeasurement();
        void enableForceWeightMeasurement();
        void disableForceWeightMeasurement();
        string getForceCalibrationSample();
        string getForceWeightSample();
        //void startSampling(bool EnableForceCalibration , bool EnableForceWeight );
        string getSamples() override;
        void stopSampling() override;
};


#endif // STRAIN_GUAGE_H