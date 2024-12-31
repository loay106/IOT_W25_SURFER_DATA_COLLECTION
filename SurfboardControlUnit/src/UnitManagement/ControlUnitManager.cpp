#include "ControlUnitManager.h"
#include "../Utils/Adresses.h"
#include "../Exceptions/UnitExceptions.h"

ControlUnitManager::ControlUnitManager(uint8_t SDCardChipSelectPin, int serialBaudRate)
{
    this->logger = Logger(serialBaudRate);
    this->espSyncManager = ESPNowSyncManager(logger);
    this->samplingDataWriter = SamplingDataWriter(SDCardChipSelectPin, logger);
    this->timeManager = TimeManager(logger);

    this->samplingFileName = NULL;
    this->status = SystemStatus::SYSTEM_STARTING;
}

void ControlUnitManager::initialize(uint8_t samplingUnitsAdresses[][6], int samplingUnitsNum){
    status = SystemStatus::SYSTEM_INITIALIZING;
    try{
        logger.initialize();
        espSyncManager.initialize(samplingUnitsAdresses,samplingUnitsNum);
        samplingDataWriter.initialize();
        timeManager.initialize();
        // add sampling units
        for(int i=0;i<samplingUnitsNum;i++){
            SamplingUnitRep samplingUnit;
            memcpy(samplingUnit.mac, samplingUnitsAdresses[i], 6);
            samplingUnit.status = SamplingUnitStatus::UNIT_DISCONNECTED;
            string macString = macToString(samplingUnit.mac);
            samplingUnits[macString] = samplingUnit;
        }
        status = SystemStatus::SYSTEM_STAND_BY;
    }catch(InitError& e){
        status = SystemStatus::SYSTEM_ERROR;
    }catch(exception& e){
        status = SystemStatus::SYSTEM_ERROR;
    }
}

void ControlUnitManager::startSampling(){
    if(status == SystemStatus::SYSTEM_STAND_BY){
        espSyncManager.broadcastCommand(ControlUnitCommand::START_SAMPLING);
        string fileName = samplingDataWriter.createSamplingFile(timeManager.getCurrentTimestamp());
        *samplingFileName = fileName;
        status = SystemStatus::SYSTEM_SAMPLING;
    }
}

void ControlUnitManager::stopSampling(){
    if(status == SystemStatus::SYSTEM_SAMPLING){
        espSyncManager.broadcastCommand(ControlUnitCommand::STOP_SAMPLING);
        samplingFileName = NULL;
        status = SystemStatus::SYSTEM_STAND_BY;
    }

}

void ControlUnitManager::updateSystem(){
    while(espSyncManager.hasStatusUpdateMessages()){
        StatusUpdateMessage statusMessage = espSyncManager.popStatusUpdateMessage();

        string unitID = macToString(statusMessage.from);
        try{
            SamplingUnitRep& samplingUnit = samplingUnits.at(unitID);
            samplingUnit.status = statusMessage.status;
        }
        catch(const std::out_of_range& ex){
            logger.error("Status update message received from unknown unit");
        }
    };
    while(espSyncManager.hasSamplingUpdateMessages()){
        if(status == SystemStatus::SYSTEM_SAMPLING){
            SamplingSyncMessage samplingUpdate = espSyncManager.popSamplingUpdateMessage();
            samplingDataWriter.writeSamples(*samplingFileName, macToString(samplingUpdate.from), samplingUpdate.sensorID, samplingUpdate.samplingData, samplingUpdate.units);
        }
    };

    std::map<string, SamplingUnitRep>::iterator it = samplingUnits.begin();
    while (it != samplingUnits.end()) {
        if(status == SystemStatus::SYSTEM_SAMPLING && it->second.status != SamplingUnitStatus::UNIT_SAMPLING){
            espSyncManager.sendCommand(ControlUnitCommand::START_SAMPLING, it->second.mac);
        }
        if(status != SystemStatus::SYSTEM_SAMPLING && it->second.status == SamplingUnitStatus::UNIT_SAMPLING){
            espSyncManager.sendCommand(ControlUnitCommand::STOP_SAMPLING, it->second.mac);
        }
        it++;
    }
}
