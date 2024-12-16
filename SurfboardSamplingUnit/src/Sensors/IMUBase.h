#ifndef IMU_BASE_H
#define IMU_BASE_H

#include <string>
#include <../DataLogger/DataLogger.h>

class IMUBase {
    protected:
        std::string id;
        std::string model;
        IMUStatus status;
        int samplingRatio;

        virtual void enableAccelerometer() = 0;
        virtual void disableAccelerometer() = 0;

        virtual void enableRotationVector() = 0;
        virtual void disableRotationVector() = 0;

        virtual std::string getgAccelerometerSample() = 0;
        virtual std::string getgRotationVectorSample() = 0;

    private:
        DataLogger dataLogger;
        FILE* accelerometerDataFile;
        FILE* rotationVectorDataFile;
        bool accelerometerEnabled;
        bool rotationVectorEnabled;
    
    public:
        IMUBase(const std::string id, const std::string model, DataLogger dataLogger);
        void startSampling(int currentTimestamp,bool accelerometereSampling, bool rotationVectorSampling);
        void logSamples(int currentTimestamp);
        void stopSampling(int currentTimestamp);
};

enum IMUStatus{
    STANDBY,
    SAMPLING,
    ERROR,
};

#endif // IMU_BASE_H