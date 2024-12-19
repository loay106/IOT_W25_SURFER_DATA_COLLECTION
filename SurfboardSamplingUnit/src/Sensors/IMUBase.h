#ifndef IMU_BASE_H
#define IMU_BASE_H

#include <string>

class IMUBase {
    protected:
        std::string id;
        std::string model;
        IMUStatus status;
        int samplingRate;

        virtual void enableAccelerometer() = 0;
        virtual void disableAccelerometer() = 0;

        virtual void enableRotationVector() = 0;
        virtual void disableRotationVector() = 0;

        virtual std::string getgAccelerometerSample() = 0;
        virtual std::string getgRotationVectorSample() = 0;

    private:
        FILE* accelerometerDataFile;
        FILE* rotationVectorDataFile;
        bool accelerometerEnabled;
        bool rotationVectorEnabled;
    
    public:
        IMUBase(const std::string id, const std::string model, int samplingRate);
        void startSampling(bool accelerometereSampling, bool rotationVectorSampling);
        void getSamples();
        void stopSampling();
};

enum IMUStatus{
    STANDBY,
    SAMPLING,
    ERROR,
};

#endif // IMU_BASE_H