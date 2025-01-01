#include "UnitManager.h"
#include "../Sensors/IMUBase.h"
#include "../Sensors/IMU_BNO080.h"

UnitManager::UnitManager(ESPNowControlUnitSyncManager* syncManager): status(UnitManagerStatus::STANDBY){
   this->syncManager = syncManager;
}
void UnitManager::addIMUSensor(IMUBase* sensor){
    sensor->setup();
    imuSensors.push_back(sensor);

}

void UnitManager::startSampling(){

    for(auto &imuSensor : imuSensors){

        if(!imuSensor->sensorEnabled){
            imuSensor->enableSensor();
            imuSensor->sensorEnabled = true;
        }
        std::string sampleString = imuSensor->getSample();
//        syncManager.sendSamples(sampleString,imuSensor.pattern,imuSensor.id);
        if(imuSensor->status == IMUStatus::ERROR ){
            status = UnitManagerStatus::ERROR;
        }
    }


}

