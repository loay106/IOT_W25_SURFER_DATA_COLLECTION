#include "SurfboardMainUnit.h"

SurfboardMainUnit::SurfboardMainUnit(ControlUnitSyncManager *syncManager, RTCTimeHandler* timeHandler, RGBStatusHandler* statusLighthandler, ButtonHandler *buttonHandler, Logger *logger, Sampler* sampler, SDCardHandler* sdCardHandler){
    this->logger = logger;
    this->syncManager = syncManager;
    this->timeHandler = timeHandler;
    this->statusLighthandler = statusLighthandler;
    this->buttonHandler = buttonHandler;
    this->logger = logger;
    this->sampler = sampler;
    this->sdCardHandler = sdCardHandler;
    status = SystemStatus::SYSTEM_STARTING;
    currentSamplingSession = 0;
}

void SurfboardMainUnit::init(uint8_t samplingUnitsAdresses[][6], int samplingUnitsNum) {
    // add sampling units
    for(int i=0;i<samplingUnitsNum;i++){
        SamplingUnitRep samplingUnit;
        memcpy(samplingUnit.mac, samplingUnitsAdresses[i], 6);
        samplingUnit.status = SamplerStatus::UNIT_STAND_BY;
        samplingUnit.hasFilesToUpload = false;
        string macString = macToString(samplingUnit.mac);
        samplingUnits[macString] = samplingUnit;
    }
    logger->info("System initalization complete!");
    updateStatus(SystemStatus::SYSTEM_STAND_BY);
}

void SurfboardMainUnit::updateStatus(SystemStatus newStatus){
    if(status == newStatus){
        return;
    }
    status = newStatus;
    switch (status){
        case SystemStatus::SYSTEM_STARTING:
            statusLighthandler->updateColors(RGBColors::NO_COLOR, RGBColors::NO_COLOR);
            break;
        case SystemStatus::SYSTEM_STAND_BY:
            statusLighthandler->updateColors(RGBColors::GREEN, RGBColors::GREEN);
            break;  
        case SystemStatus::SYSTEM_SAMPLING:
            statusLighthandler->updateColors(RGBColors::GREEN, RGBColors::NO_COLOR);
            break;     
        case SystemStatus::SYSTEM_SAMPLING_PARTIAL_ERROR:
            statusLighthandler->updateColors(RGBColors::GREEN, RGBColors::RED);
            break;    
        case SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD:
            statusLighthandler->updateColors(RGBColors::BLUE, RGBColors::NO_COLOR);
            break;
        case SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD_PARTIAL_ERROR:
            statusLighthandler->updateColors(RGBColors::BLUE, RGBColors::RED);
            break;
        case SystemStatus::SYSTEM_ERROR:
            statusLighthandler->updateColors(RGBColors::RED, RGBColors::RED);
            break;              
        default:
            statusLighthandler->updateColors(RGBColors::NO_COLOR, RGBColors::NO_COLOR);
            break;
    }
}

void SurfboardMainUnit::addSensor(SensorBase* sensor) {
    sampler->addSensor(sensor);
}

void SurfboardMainUnit::startSampling() {
    currentSamplingSession = timeHandler->getCurrentTimestamp();
    std::map<string, string> samplingParams;
    samplingParams["TIMESTAMP"] = to_string(currentSamplingSession);
    try{
        syncManager->broadcastCommand(ControlUnitCommand::START_SAMPLING, samplingParams);
    }catch(ESPNowSyncError& error){
        logger->error("Failed to send command to sampling units! Try again!");
        return;
    }
    std::map<string, SamplingUnitRep>::iterator it = samplingUnits.begin();
    while (it != samplingUnits.end()) {
      it->second.hasFilesToUpload = true;
      it++;   
    }
    sampler->startSampling(currentSamplingSession);
    updateStatus(SystemStatus::SYSTEM_SAMPLING);
    logger->info("Sampling started...");
}

void SurfboardMainUnit::stopSampling() {
    try{
        std::map<string, string> params;
        syncManager->broadcastCommand(ControlUnitCommand::STOP_SAMPLING, params);
    }catch(ESPNowSyncError& error){
        logger->error("Failed to send command to sampling units! Try again!");
        return;
    }
    sampler->stopSampling();
    updateStatus(SystemStatus::SYSTEM_STAND_BY);
}

void SurfboardMainUnit::uploadSampleFiles() {
    try{
        std::map<string, string> params;
        syncManager->broadcastCommand(ControlUnitCommand::START_SAMPLE_FILES_UPLOAD, params);
    }catch(ESPNowSyncError& error){
        logger->error("Failed to send command to sampling units! Try again!");
        return;
    }
    sampler->uploadSampleFiles();
    updateStatus(SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD);
}

void SurfboardMainUnit::stopUploadSampleFiles() {
    try{
        std::map<string, string> params;
        syncManager->broadcastCommand(ControlUnitCommand::STOP_SAMPLE_FILES_UPLOAD, params);
    }catch(ESPNowSyncError& error){
        logger->error("Failed to send command to sampling units! Try again!");
        return;
    }
    updateStatus(SystemStatus::SYSTEM_STAND_BY);
}

void SurfboardMainUnit::updateSystem() {
    // update status light flicker
    statusLighthandler->flicker();

    // run sampler methods
    if(status == SystemStatus::SYSTEM_SAMPLING || status == SystemStatus::SYSTEM_SAMPLING_PARTIAL_ERROR){
        sampler->writeSensorsData();
    }else if(status == SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD || status == SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD_PARTIAL_ERROR){
        sampler->uploadSampleFiles();
    }

    // read status update messages from sampling units
    while(syncManager->hasStatusUpdateMessages()){
        StatusUpdateMessage statusMessage = ControlUnitSyncManager::popStatusUpdateMessage();
        string unitID = macToString(statusMessage.from);
        try{
            SamplingUnitRep& samplingUnit = samplingUnits.at(unitID);
            switch(statusMessage.status){
                case SamplingUnitStatusMessage::STAND_BY:
                  samplingUnit.status = SamplerStatus::UNIT_STAND_BY;
                case SamplingUnitStatusMessage::SAMPLING:
                  samplingUnit.status = SamplerStatus::UNIT_SAMPLING;
                case SamplingUnitStatusMessage::SAMPLE_FILES_UPLOAD:
                  samplingUnit.status = SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD;
                case SamplingUnitStatusMessage::SAMPLE_FILES_UPLOAD_COMPLETE:
                  samplingUnit.hasFilesToUpload = false;
                  samplingUnit.status = SamplerStatus::UNIT_STAND_BY;
                case SamplingUnitStatusMessage::SAMPLE_FILES_UPLOAD_ERROR:
                case SamplingUnitStatusMessage::ERROR:
                  samplingUnit.status = SamplerStatus::UNIT_ERROR;
            }
        }
        catch(const std::out_of_range& ex){
            logger->error("Status update message received from unknown unit " + unitID);
        }
    };

    // handle button press
    ButtonPressType press = buttonHandler->getLastPressType();
    if(press != ButtonPressType::NO_PRESS){
        switch(status){
            case SystemStatus::SYSTEM_SAMPLING:
            case SystemStatus::SYSTEM_SAMPLING_PARTIAL_ERROR:
                stopSampling();
                return;
            case SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD:
            case SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD_PARTIAL_ERROR:
                stopUploadSampleFiles();
                return;
            case SystemStatus::SYSTEM_STAND_BY:
                if(press == ButtonPressType::SOFT_PRESS){
                    startSampling();
                }else{
                    // long press
                    uploadSampleFiles();
                }
                return;
            case SystemStatus::SYSTEM_ERROR:
                return;
            default:{
                logger->info("Unknown state, shouldn't get here");
            }
        }
    }

    // make sure all units are on the same status
    std::map<string, SamplingUnitRep>::iterator it = samplingUnits.begin();
    while (it != samplingUnits.end()) {
        switch(status){
            case SystemStatus::SYSTEM_SAMPLING:
            case SystemStatus::SYSTEM_SAMPLING_PARTIAL_ERROR:
                if(it->second.status != SamplerStatus::UNIT_SAMPLING){
                    std::map<string, string> samplingParams;
                    samplingParams["TIMESTAMP"] = to_string(currentSamplingSession);
                    syncManager->sendCommand(ControlUnitCommand::START_SAMPLING, samplingParams, it->second.mac);
                }
                break;
            case SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD:
            case SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD_PARTIAL_ERROR:
                if(it->second.status != SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD && it->second.hasFilesToUpload){
                    std::map<string, string> params; // empty params, just to pass
                    syncManager->sendCommand(ControlUnitCommand::START_SAMPLE_FILES_UPLOAD, params, it->second.mac);
                }
                break;
            case SystemStatus::SYSTEM_STAND_BY:
                if(it->second.status != SamplerStatus::UNIT_SAMPLING){
                    std::map<string, string> params; // empty params, just to pass
                    syncManager->sendCommand(ControlUnitCommand::STOP_SAMPLING, params, it->second.mac);
                }else if(it->second.status != SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD){
                      std::map<string, string> params; // empty params, just to pass
                    syncManager->sendCommand(ControlUnitCommand::STOP_SAMPLE_FILES_UPLOAD, params, it->second.mac);
                }
                break;
            case SystemStatus::SYSTEM_ERROR:
                return;
            default:{
                logger->info("Unknown state, shouldn't get here");
            }
        }
        it++;        
    }
};