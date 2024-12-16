#include "IMUBase.h"

IMUBase::IMUBase(const std::string name, const std::string id, const std::string model) : name(name), id(id), model(model){
    rotationVectorDataLogger = NULL;
    accelerometerDataLogger = NULL;
    status = IMUStatus::STANDBY;
}

void IMUBase::startSampling(int currentTimestamp){
    // todo: create unique file names with timestamps/id etc.....
    rotationVectorDataLogger = SampleDataLogger("test1234", "quatI,quatJ,quatK,quatReal,quatRadianAccuracy");
    accelerometerDataLogger = SampleDataLogger("test12345", "AccX,AccY,AccZ");;
}

void IMUBase::stopSampling(){
    // todo: valid way to desconstruct? ... I forgot c++....
    // might want to close file here
    rotationVectorDataLogger = NULL;
    accelerometerDataLogger = NULL;
}