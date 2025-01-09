#include "ControlUnitManager.h"
#include "../Utils/Adresses.h"
#include "../Exceptions/UnitExceptions.h"

ControlUnitManager::ControlUnitManager(uint8_t SDCardChipSelectPin, int serialBaudRate, int buttonPin)
{
    this->logger = Logger(serialBaudRate);
    this->espSyncManager = ESPNowSyncManager(logger);
    this->samplingDataWriter = SamplingDataWriter(SDCardChipSelectPin, logger);
    this->timeManager = TimeManager(logger);
    this->statusLightManager = RGBStatusManager(logger);
    this->buttonManager = SamplingButtonManager(logger, buttonPin);
    this->samplesCount = 0;

    this->samplingFileName = NULL;
    this->status = SystemStatus::SYSTEM_STARTING;
}

void ControlUnitManager::initialize(uint8_t samplingUnitsAdresses[][6], int samplingUnitsNum, int RGBRedPin, int RGBGreenPin, int RGBBluePin){
    status = SystemStatus::SYSTEM_INITIALIZING;
    try{
        logger.initialize();
        statusLightManager.initialize(status, RGBRedPin, RGBGreenPin, RGBBluePin);
        espSyncManager.initialize(samplingUnitsAdresses,samplingUnitsNum);
        samplingDataWriter.initialize();
        buttonManager.initialize();
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
        logger.info("System initalization complete!");
        statusLightManager.updateStatus(status);
    }catch(InitError& e){
        status = SystemStatus::SYSTEM_ERROR;
        statusLightManager.updateStatus(status);
    }catch(exception& e){
        status = SystemStatus::SYSTEM_ERROR;
        statusLightManager.updateStatus(status);
    }
}

void ControlUnitManager::startSampling(){
    if(status == SystemStatus::SYSTEM_STAND_BY){
        try{
            string fileName = samplingDataWriter.createSamplingFile(timeManager.getCurrentTimestamp());
            samplingFileName = new string(fileName);
        }catch(SDCardError& error){
            logger.error("Failed to create sampling file!");
            return;
        }
        try{
            espSyncManager.broadcastCommand(ControlUnitCommand::START_SAMPLING);
        }catch(ESPNowSyncError& error){
            logger.error("Failed to send command to sampling units!");
            return;
        }
        status = SystemStatus::SYSTEM_SAMPLING;
        statusLightManager.updateStatus(status);
        logger.info("Sampling started...");
    }
}

void ControlUnitManager::stopSampling(){
    if(status == SystemStatus::SYSTEM_SAMPLING){
        try{
            espSyncManager.broadcastCommand(ControlUnitCommand::STOP_SAMPLING);
        }catch(ESPNowSyncError& error){
            logger.error("Failed to send command to sampling units!");
            return;
        }
        status = SystemStatus::SYSTEM_STAND_BY;
        delete samplingFileName;
        samplingFileName = NULL;
        // todo: get samplingStartTime...
        int elapsedSeconds = (timeManager.getCurrentTimestamp() - samplingStartTime) / 1000;
        int rate = elapsedSeconds > 0 ? samplesCount / elapsedSeconds : 0;
        logger.info("Sampling stopped! Sampling rate is: " + to_string(rate) + "Hz");
        samplesCount = 0;
        
        statusLightManager.updateStatus(status);

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
            logger.error("Status update message received from unknown unit " + unitID);
        }
    };
    while(espSyncManager.hasSamplingUpdateMessages()){
        SamplingSyncMessage samplingUpdate = espSyncManager.popSamplingUpdateMessage();
        if(status == SystemStatus::SYSTEM_SAMPLING){  
            samplingDataWriter.writeSamples(*samplingFileName, macToString(samplingUpdate.from), samplingUpdate.sensorID, samplingUpdate.samplingData, samplingUpdate.units);
            samplesCount+=samplingUpdate.samplingData.size();
            //logger.info("Wrote " + to_string(samplingUpdate.samplingData.size()) + " samples to SD card!");
        }
    };

    if(buttonManager.wasPressed()){
        switch(status){
            case SystemStatus::SYSTEM_SAMPLING:{
                stopSampling();
                break;
            }
            case SystemStatus::SYSTEM_STAND_BY:{
                startSampling();
                break;
            }
            default:{
                // ignore the press
                logger.info("Pressed button was ignored because system is not in a ready state");
            }
        }
        return;
    }

    std::map<string, SamplingUnitRep>::iterator it = samplingUnits.begin();
    while (it != samplingUnits.end()) {
        if(status == SystemStatus::SYSTEM_SAMPLING && it->second.status != SamplingUnitStatus::UNIT_SAMPLING){
            try{
                espSyncManager.sendCommand(ControlUnitCommand::START_SAMPLING, it->second.mac);
            }catch(ESPNowSyncError& error){
                it->second.status = SamplingUnitStatus::UNIT_DISCONNECTED;
            }  
        }
        else if(status != SystemStatus::SYSTEM_SAMPLING && it->second.status == SamplingUnitStatus::UNIT_SAMPLING){
            try{
                espSyncManager.sendCommand(ControlUnitCommand::STOP_SAMPLING, it->second.mac);
            }catch(ESPNowSyncError& error){
                it->second.status = SamplingUnitStatus::UNIT_DISCONNECTED;
            }    
        }
        it++;
    }
}
