#include "UnitManager.h"
#include "../Sensors/IMU_BNO080.h"

UnitManager::UnitManager(ESPNowControlUnitSyncManager* syncManager): status(UnitManagerStatus::STANDBY){
   this->syncManager = syncManager;
}

void UnitManager::addIMUSensor(IMUBase* sensor){
    sensor->setup();
    if(!sensor->sensorEnabled){
        sensor->enableSensor();
        sensor->sensorEnabled = true;
    }
    imuSensors.push_back(sensor);

}

void UnitManager::addForceSensor(ForceBase* sensor)
{
    sensor->setup()
    if(!sensor->getSensorStatus())
    {
        sensor->enableSensor();
    }
    forceSensors.push_back(sensor);
}

void UnitManager::startSampling(){

    for(auto imuSensor : imuSensors){
        std::string sampleString = imuSensor->getSample();
        while(sampleString == "error"){
            sampleString = imuSensor->getSample();
        }
        syncManager->sendSamples(sampleString,imuSensor->pattern,imuSensor->id);
        if(imuSensor->status == IMUStatus::ERROR ){
            status = UnitManagerStatus::ERROR;
        }
    }

}

