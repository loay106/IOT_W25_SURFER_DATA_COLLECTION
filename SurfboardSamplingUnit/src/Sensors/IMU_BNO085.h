#ifndef IMU_BNO085_H
#define IMU_BNO085_H

#include "IMUBase.h"
#include <string>
#include "SparkFun_BNO080_Arduino_Library.h"

class IMU_BNO085: public IMUBase {   
    private:
        BNO080 myIMU;
    public:
        IMU_BNO085(const std::string id, DataLogger dataLogger, int samplingRatio);
};

#endif // IMU_BNO085_H