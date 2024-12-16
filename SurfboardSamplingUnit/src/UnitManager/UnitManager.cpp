#include "UnitManager.h"

UnitManager::UnitManager(){
    imuSensors = {};
}

void UnitManager::addIMUSensor(IMUBase sensor){
    imuSensors.push_back(sensor);
}