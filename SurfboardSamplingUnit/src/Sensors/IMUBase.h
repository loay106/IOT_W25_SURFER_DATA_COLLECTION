#ifndef IMU_BASE_H
#define IMU_BASE_H

#include <string>
#include <Arduino.h>
#include <SparkFun_BNO080_Arduino_Library.h>

enum class IMUStatus{
    STANDBY,
    SAMPLING,
    ERROR,
};

enum SupportedIMUModels{
    BNO0800,
};
enum SamplingModes{
    ACCELEROMETER,
    ROTATION_VECTOR
};

class IMUBase {
    public:
        std::string id;
        SupportedIMUModels model;
        IMUStatus status;
        SamplingModes mode;
        std::string pattern;
        int samplingRate;
        bool sensorEnabled;
        IMUBase(SupportedIMUModels model, SamplingModes mode, int samplingRate);
        virtual void setup() = 0;
        virtual void enableSensor() = 0;
        virtual void enableSensor() = 0;
        virtual std::string getSample() = 0;

};



#endif // IMU_BASE_H