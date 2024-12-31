#include "ControlUnitManager.h"
#include "../Utils/Adresses.h"
#include "../Exceptions/UnitExceptions.h"

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
    }catch(exception& e){
        status = SystemStatus::ERROR;
    }
}

void ControlUnitManager::startSampling(){
    if(status == SystemStatus::STAND_BY){
        espSyncManager.broadcastCommand(ControlUnitCommand::START_SAMPLING);
        string fileName = samplingDataWriter.createSamplingFile(timeManager.getCurrentTimestamp());
        *samplingFileName = fileName;
        status = SystemStatus::SAMPLING;
    }
}

void ControlUnitManager::stopSampling(){
    if(status == SystemStatus::SAMPLING){
        espSyncManager.broadcastCommand(ControlUnitCommand::STOP_SAMPLING);
        samplingFileName = NULL;
        status = SystemStatus::STAND_BY;
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
            logger.error("Status update message received from unknown unit")
        }
    };
    while(espSyncManager.hasSamplingUpdateMessages()){
        if(status == SystemStatus::SAMPLING){
            SamplingSyncMessage samplingUpdate = espSyncManager.popSamplingUpdateMessage();
            samplingDataWriter.writeSamples(*samplingFileName, macToString(samplingUpdate.from), samplingUpdate.sensorID, samplingUpdate.samplingData, samplingUpdate.units);
        }
    };

    map<string, SamplingUnitRep>::iterator it = samplingUnits.begin();
    while (it != samplingUnits.end()) {
        if(status == SystemStatus::SAMPLING && it->second.status != SamplingUnitStatus::SAMPLING){
            espSyncManager.sendCommand(ControlUnitCommand::START_SAMPLING, it->second.mac);
        }
        if(status != SystemStatus::SAMPLING && it->second.status == SamplingUnitStatus::SAMPLING){
            espSyncManager.sendCommand(ControlUnitCommand::STOP_SAMPLING, it->second.mac);
        }
        it++;
    }
}
