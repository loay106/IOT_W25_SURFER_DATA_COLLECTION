#ifndef IMU_BASE_H
#define IMU_BASE_H

#include <string>
#include "SparkFun_BNO080_Arduino_Library.h"

class IMUBase {
    protected:
        std::string id;
        SupportedIMUModels model;
        IMUStatus status;
        SamplingModes mode;
        std::string pattern;
        int samplingRate;
        bool sensorEnabled;

    public:
        IMUBase(SupportedIMUModels model, SamplingModes mode, int samplingRate);
        virtual void setup() = 0;
        virtual void enableSensor() = 0;
        virtual void disableSensor() = 0;
        virtual std::string getSample() = 0;

};

enum IMUStatus{
    STANDBY,
    SAMPLING,
    ERROR,
};

enum SupportedIMUModels{
    BNO080,
};
enum SamplingModes{
    ACCELEROMETER,
    ROTATION_VECTOR
};

#endif // IMU_BASE_H