#ifndef IMU_BNO080_H
#define IMU_BNO080_H

#include "IMUBase.h"
#include <string>

class IMU_BNO080: public IMUBase {
    // todo: implement and add methods as needed
    public:
        IMU_BNO080(const std::string id, int samplingRate);
        void enableAccelerometer() override;
        void disableAccelerometer() override;
        void enableRotationVector() override;
        void disableRotationVector() override;
        std::string getgAccelerometerSample() override;
        std::string getgRotationVectorSample() override;
};

#endif // IMU_BNO080_H