#ifndef IMU_BNO085_H
#define IMU_BNO085_H

#include "IMUBase.h"
#include <string>

class IMU_BNO085: public IMUBase {   
    // todo: implement and add methods as needed
    public:
        IMU_BNO085(const std::string id, int samplingRate);
        void enableAccelerometer() override;
        void disableAccelerometer() override;
        void enableRotationVector() override;
        void disableRotationVector() override;
        std::string getgAccelerometerSample() override;
        std::string getgRotationVectorSample() override;
};

#endif // IMU_BNO085_H