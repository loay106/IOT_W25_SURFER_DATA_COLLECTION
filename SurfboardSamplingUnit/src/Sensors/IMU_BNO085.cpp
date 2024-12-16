#include "IMU_BNO085.h"


IMU_BNO085::IMU_BNO085(const std::string name, const std::string id): IMUBase(name, id, "BNO085"){
    // todo....
}

std::string IMU_BNO085::logAccelerometerSample(int currentTimestamp){
    // todo: get sampleData...
    std::string sampleData;
    accelerometerDataLogger->logSample(sampleData)
}
std::string IMU_BNO085::logRotationVectorSample(int currentTimestamp){
    // todo: get sampleData...
    std::string sampleData;
    rotationVectorDataLogger->logSample(sampleData)
    
}
