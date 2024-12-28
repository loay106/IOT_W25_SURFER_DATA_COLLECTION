#ifndef IMU_BASE_H
#define IMU_BASE_H

#include <string>

class IMUBase {
    protected:
        std::string id;
        SupportedIMUModels model;
        IMUStatus status;
        SamplingModes mode;
        int samplingRate;
        bool samplingEnabled;

        virtual void setup() = 0;
        virtual void enableAccelerometer() = 0;
        virtual void disableAccelerometer() = 0;

        virtual void enableRotationVector() = 0;
        virtual void disableRotationVector() = 0;

        virtual std::string getgAccelerometerSample() = 0;
        virtual std::string getgRotationVectorSample() = 0;

    public:
        IMUBase(SupportedIMUModels model, SamplingModes mode, int samplingRate);
        void setup();
        void enableSampling();
        void getSample();
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