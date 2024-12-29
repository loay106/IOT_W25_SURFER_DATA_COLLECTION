#include "UnitManager.h"
#include <../Sensors/IMUBase.h>
#include <../Sensors/IMU_BNO080.h>

UnitManager::UnitManager(ESPNowControlUnitSyncManager& syncManager): status(UnitManagerStatus::STANDBY){
    this->syncManager = syncManager;
}
void UnitManager::addIMUSensor(IMUBase& sensor){
    imuSensors.push_back(sensor);
}
void UnitManager::startSampling(){
    for(IMUBase& imuSensor : imuSensors){
        if(!sensor.sensorEnabled){
            sensor.enableSensor();
            sensor.sensorEnabled = true;
        }
        std::string sampleString = imuSensor.getSample();
        syncManager.sendSamples(sampleString,imuSensor.pattern,imuSensor.id);
        if(imuSensor.status == IMUStatus::ERROR || SyncManager.status == ){
            status = UnitManagerStatus::ERROR;
        }
    }
}
void UnitManager::stopSampling(){
    for(IMUBase& imuSensor : imuSensors){
        if(sensor.sensorEnabled){
            sensor.disableSensor();
            sensor.sensorEnabled = false;
        }
    }
}
