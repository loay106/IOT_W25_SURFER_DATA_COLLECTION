#include "ControlUnitManager.h"
#include "src/Utils/Adresses.h"

ControlUnitManager::ControlUnitManager(const uint8_t SDCardChipSelectPin)
{
    this->espSyncManager = ESPNowSyncManager();
    this->samplingDataWriter = SamplingDataWriter(SDCardChipSelectPin);
    this->timeManager = TimeManager();
    this->samplingFileName = NULL;
    this->status = SystemStatus::STARTING;
}

void ControlUnitManager::initialize(uint8_t samplingUnits[][], int samplingUnitsNum){
    status = SystemStatus::INITILAZING;
    espSyncManager.initialize();
    samplingDataWriter.initialize();
    timeManager.initialize();
    for(int i=0;i<samplingUnitsNum;i++){
        addSamplingUnit(samplingUnits[i]);
    }
    status = SystemStatus::STAND_BY;
}

string ControlUnitManager::addSamplingUnit(uint8_t mac[]){
    SamplingUnitRep samplingUnit;
    memcpy(samplingUnit.mac, mac, 6);
    samplingUnit.status = SamplingUnitStatus::DISCONNECTED;
    macString = macToString(samplingUnit.mac);
    samplingUnits[macString] = samplingUnit;
}

void ControlUnitManager::startSampling(){
    espSyncManager.broadcastCommand(ControlUnitCommand::START_SAMPLING);
    string fileName = samplingDataWriter.createSamplingFile(timeManager.getCurrentTimestamp());
    *samplingFileName = fileName;
    status = SystemStatus::SAMPLING;
}

void ControlUnitManager::stopSampling(){
    espSyncManager.broadcastCommand(ControlUnitCommand::STOP_SAMPLING);
    samplingFileName = NULL;
    status = SystemStatus::STAND_BY;
}

void ControlUnitManager::updateSystem(){
    while(espSyncManager.hasStatusUpdateMessages()){
        StatusUpdateMessage statusMessage = espSyncManager.popStatusUpdateMessage();
        updateSamplingUnitStatus(statusMessage.from, statusMessage.status);
    };
    while(espSyncManager.hasSamplingUpdateMessages()){
        SamplingSyncMessage samplingUpdate = espSyncManager.popSamplingUpdateMessage();
        // todo: add validations...are we sampling? is there a file open...? etc...
        samplingDataWriter.writeSamples(*samplingFileName, macToString(samplingUpdate.from), samplingUpdate.sensorID, samplingUpdate.samplingData, samplingUpdate.units);
    };
}

void ControlUnitManager::updateSamplingUnitStatus(uint8_t samplingUnitMac[], SamplingUnitStatus status){
    // todo: handle exceptions - https://en.cppreference.com/w/cpp/container/map/at
    SamplingUnitRep& samplingUnit = samplingUnits.at(unitID);
    samplingUnit.status = status;
}
