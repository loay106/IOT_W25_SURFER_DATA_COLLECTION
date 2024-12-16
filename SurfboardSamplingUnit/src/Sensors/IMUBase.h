#ifndef IMU_BASE_H
#define IMU_BASE_H

#include <string>
#include <SampleDataLogger.h>

class IMUBase {
    protected:
        std::string name;
        std::string id;
        std::string model;
        IMUStatus status;
        SampleDataLogger* accelerometerDataLogger;
        SampleDataLogger* rotationVectorDataLogger;
    
    public:
        IMUBase(const std::string name, const std::string id, const std::string model);
        virtual void startSampling(int currentTimestamp);
        virtual std::string logAccelerometerSample(int currentTimestamp);
        virtual std::string logRotationVectorSample(int currentTimestamp);
        virtual void stopSampling();
};

enum IMUStatus{
    STANDBY,
    SAMPLING,
    ERROR,
};

#endif // IMU_BASE_H