#include "UnitManager.h"
#include <../Sensors/IMUBase.h>
#include <../Sensors/IMU_BNO080.h>

UnitManager::UnitManager(int samplingDelayTime): status(UnitManagerStatus::CONFIGURING),samplingDelayTime(samplingDelayTime){

}
void UnitManager::addIMUSensor(SupportedIMUModels model){
    if(model == SupportedIMUModels::BNO080){
        IMU_BNO080 sensor = IMU_BNO080();
        imuSensors.push_back(sensor);
    }
    else{
        status = UnitManagerStatus::ERORR;
    }
}
void UnitManager::startSampling(){
    status = UnitManagerStatus::SAMPLING;
}
void UnitManager::stopSampling(){
    status = UnitManagerStatus::STANDBY;
}
