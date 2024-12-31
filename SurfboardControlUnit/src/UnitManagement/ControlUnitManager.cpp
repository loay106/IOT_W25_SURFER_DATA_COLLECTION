#include "ControlUnitManager.h"
#include "src/Utils/Adresses.h"
#include "src/Exceptions/UnitExceptions.h"

ControlUnitManager::ControlUnitManager(const uint8_t SDCardChipSelectPin, const int serialBaudRate)
{
    this->logger = Logger(serialBaudRate);
    this->espSyncManager = ESPNowSyncManager(logger);
    this->samplingDataWriter = SamplingDataWriter(SDCardChipSelectPin, logger);
    this->timeManager = TimeManager(logger);

    this->samplingFileName = NULL;
    this->status = SystemStatus::STARTING;
}

void ControlUnitManager::initialize(uint8_t samplingUnits[][], int samplingUnitsNum){
    status = SystemStatus::INITILAZING;
    try{
        logger.initialize();
        espSyncManager.initialize();
        samplingDataWriter.initialize();
        timeManager.initialize();
        // add sampling units
        for(int i=0;i<samplingUnitsNum;i++){
            SamplingUnitRep samplingUnit;
            memcpy(samplingUnit.mac, mac, 6);
            samplingUnit.status = SamplingUnitStatus::DISCONNECTED;
            macString = macToString(samplingUnit.mac);
            samplingUnits[macString] = samplingUnit;
        }
        status = SystemStatus::STAND_BY;
    }catch(InitError& e){
        status = SystemStatus::ERROR;
    }
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
    try{
        SamplingUnitRep& samplingUnit = samplingUnits.at(unitID);
        samplingUnit.status = status;
    }
    catch(const std::out_of_range& ex){
        logger.error("Unit not found")
    }
}
