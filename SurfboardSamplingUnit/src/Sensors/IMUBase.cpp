#include "IMUBase.h"

std::string createUniqueID() {
    return std::to_string(std::rand());
}

IMUBase::IMUBase(SupportedIMUModels model, SamplingModes mode, int samplingRate): model(model),status(IMUStatus::STANDBY),mode(mode),samplingRate(samplingRate),sensorEnabled(false){
    this->id = createUniqueID();

}




