#ifndef IMU_BNO080_H
#define IMU_BNO080_H

#include "IMUBase.h"
#include <string>

class IMU_BNO080: public IMUBase {
    BNO080 sensor;
    public:
        IMU_BNO080(SamplingModes mode, int samplingRate);
        void setup() override;
        void enableSensor() override;
        void disableSensor() override;
        std::string getSample() override;
};

#endif // IMU_BNO080_H