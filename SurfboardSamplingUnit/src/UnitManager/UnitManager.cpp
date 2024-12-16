#include "UnitManager.h"
#include <../Sensors/IMUBase.h>
#include <../Sensors/IMU_BNO085.h>
#include <../TimeManager/TimeManager.h>

std::string createUniqueID(){
    // todo: implement....
    return "123";
}

int calculateGCD(int a, int b) {
    while (b != 0) {
        int remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

UnitManager::UnitManager(DataLogger dataLogger, TimeManager timeManager){
    this->dataLogger = dataLogger;
    this->timeManager = timeManager;
    samplingDelayTime = 50;
}

void UnitManager::configure(int currentTimeStamp){
    timeManager.adjust(currentTimeStamp);
    status = UnitManagerStatus::STANDBY;
}

void UnitManager::addIMUSensor(SupportedIMUModels model, int samplingRatio){
    if(model == SupportedIMUModels::BNO085){
        IMU_BNO085 sensor = IMU_BNO085(createUniqueID(), dataLogger, samplingRatio);
        imuSensors.push_back(sensor);

        if(imuSensors.size() == 0){
            samplingDelayTime = samplingRatio;
        }else{
            samplingDelayTime = calculateGCD(samplingDelayTime, samplingRatio);
        }
    }else{
        // todo: throw exception...not a supported model...
        return;
    }
}

UnitManagerStatus UnitManager::getStatus(){
    return status;
}

void UnitManager::startSampling(){
    // todo: add force sensors later on...
    int sampleStartTime = timeManager.getCurrentTimeStamp();
    for(i=0; i<imuSensors.size();i++){
        imuSensors[i].startSampling(sampleStartTime, true, true);
    }

    status = UnitManagerStatus::SAMPLING;
}

int UnitManager::getSamplingDelayTime()
{
    return 0;
}

void UnitManager::logSamples(){
        // todo: add force sensors later on...
    int currentTimeStamp = timeManager.getCurrentTimeStamp();
    for(i=0; i<imuSensors.size();i++){
        imuSensors[i].logSamples(currentTimeStamp);
    }
}

void UnitManager::stopSampling(){
    int currentTimeStamp = timeManager.getCurrentTimeStamp();
    for(i=0; i<imuSensors.size();i++){
        imuSensors[i].stopSampling(currentTimeStamp);
    }

    status = UnitManagerStatus::STANDBY;
}
