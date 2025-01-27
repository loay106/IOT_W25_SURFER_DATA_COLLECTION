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
        samplingUnit.lastCommandSentMillis = 0;
        string macString = macToString(samplingUnit.mac);
        samplingUnits[macString] = samplingUnit;
    }
    syncManager->connect();
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
    int current = millis();
    std::map<string, SamplingUnitRep>::iterator it;
    for (it=samplingUnits.begin(); it!=samplingUnits.end(); it++) {
      it->second.hasFilesToUpload = true;
      it->second.lastCommandSentMillis = current;
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
    int current = millis();
    std::map<string, SamplingUnitRep>::iterator it;
    for (it=samplingUnits.begin(); it!=samplingUnits.end(); it++) {
      it->second.lastCommandSentMillis = current;
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
    int current = millis();
    std::map<string, SamplingUnitRep>::iterator it;
    for (it=samplingUnits.begin(); it!=samplingUnits.end(); it++) {
      it->second.lastCommandSentMillis = current;
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
    int current = millis();
    std::map<string, SamplingUnitRep>::iterator it;
    for (it=samplingUnits.begin(); it!=samplingUnits.end(); it++) {
      it->second.lastCommandSentMillis = current;
    }
    updateStatus(SystemStatus::SYSTEM_STAND_BY);
}

void SurfboardMainUnit::updateSystem() {
    // update status light flicker
    logger->debug("flickering light...");
    statusLighthandler->flicker();

    // read status update messages from sampling units
    logger->debug("Updating status methods...");
    while(syncManager->hasStatusUpdateMessages()){
        StatusUpdateMessage statusMessage = ControlUnitSyncManager::popStatusUpdateMessage();
        string unitID = macToString(statusMessage.from);
        try{
            SamplingUnitRep& samplingUnit = samplingUnits.at(unitID);
            switch(statusMessage.status){
                case SamplingUnitStatusMessage::STAND_BY:
                  samplingUnit.status = SamplerStatus::UNIT_STAND_BY;
                  break;
                case SamplingUnitStatusMessage::SAMPLING:
                  samplingUnit.status = SamplerStatus::UNIT_SAMPLING;
                  break;
                case SamplingUnitStatusMessage::SAMPLE_FILES_UPLOAD:
                  samplingUnit.status = SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD;
                  break;
                case SamplingUnitStatusMessage::SAMPLE_FILES_UPLOAD_COMPLETE:
                  samplingUnit.hasFilesToUpload = false;
                  samplingUnit.status = SamplerStatus::UNIT_STAND_BY;
                  break;
                case SamplingUnitStatusMessage::SAMPLE_FILES_UPLOAD_ERROR:
                case SamplingUnitStatusMessage::ERROR:
                  samplingUnit.status = SamplerStatus::UNIT_ERROR;
                  break;
            }
        }
        catch(const std::out_of_range& ex){
            logger->error("Status update message received from unknown unit " + unitID);
        }
    };

    // handle button press
    logger->debug("Handling button press...");
    ButtonPressType press = buttonHandler->getLastPressType();
    if(press != ButtonPressType::NO_PRESS){
        switch(status){
            case SystemStatus::SYSTEM_SAMPLING:
            case SystemStatus::SYSTEM_SAMPLING_PARTIAL_ERROR:
                logger->debug("Soft or long press detected while sampling");
                stopSampling();
                return;
            case SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD:
            case SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD_PARTIAL_ERROR:
                logger->debug("Soft or long press detected while file uploading");
                stopUploadSampleFiles();
                return;
            case SystemStatus::SYSTEM_STAND_BY:
                if(press == ButtonPressType::SOFT_PRESS){
                    logger->debug("Soft press detected while file on standby");
                    startSampling();
                }else{
                    // long press
                    logger->debug("Long press detected while file on standby");
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

    // run sampler methods
    logger->debug("Running sampler methods...");
    if(status == SystemStatus::SYSTEM_SAMPLING || status == SystemStatus::SYSTEM_SAMPLING_PARTIAL_ERROR){
        logger->debug("Writing sampler data");
        sampler->writeSensorsData();
    }else if(status == SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD || status == SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD_PARTIAL_ERROR){
        logger->debug("Uploading sampler data");
        sampler->uploadSampleFiles();
    }

    // make sure all units are on the same status
    logger->debug("Make sure all units are on the same system status...");
    std::map<string, SamplingUnitRep>::iterator it;
    for (it=samplingUnits.begin(); it!=samplingUnits.end(); it++) {
        switch(status){
            case SystemStatus::SYSTEM_SAMPLING:
            case SystemStatus::SYSTEM_SAMPLING_PARTIAL_ERROR:
                if(it->second.status != SamplerStatus::UNIT_SAMPLING && (millis() - it->second.lastCommandSentMillis) >= COMMAND_SEND_MIN_INTERVAL_MILLIS){
                    std::map<string, string> samplingParams;
                    samplingParams["TIMESTAMP"] = to_string(currentSamplingSession);
                    try{
                        string message = "Unit " + it->first + " not sampling. Sending START_SAMPLING command...";
                        logger->info(message);
                        syncManager->sendCommand(ControlUnitCommand::START_SAMPLING, samplingParams, it->second.mac);
                        it->second.lastCommandSentMillis = millis();
                    }catch(ESPNowSyncError& error){
                        // todo: maybe set unit status as disconnected?
                    } 
                }
                break;
            case SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD:
            case SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD_PARTIAL_ERROR:
                if(it->second.status != SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD && it->second.hasFilesToUpload && (millis() - it->second.lastCommandSentMillis) >= COMMAND_SEND_MIN_INTERVAL_MILLIS){
                    try{
                        string message = "Unit " + it->first + " not uploading files. Sending START_SAMPLE_FILES_UPLOAD command...";
                        logger->info(message);
                        std::map<string, string> params; // empty params, just to pass
                        syncManager->sendCommand(ControlUnitCommand::START_SAMPLE_FILES_UPLOAD, params, it->second.mac);
                        it->second.lastCommandSentMillis = millis();
                    }catch(ESPNowSyncError& error){
                        // todo: maybe set unit status as disconnected?
                        continue;
                    }
                }
                break;
            case SystemStatus::SYSTEM_STAND_BY:
                if(it->second.status == SamplerStatus::UNIT_STAND_BY){
                    continue;
                }else if(it->second.status == SamplerStatus::UNIT_SAMPLING && (millis() - it->second.lastCommandSentMillis) >= COMMAND_SEND_MIN_INTERVAL_MILLIS){
                    try{
                        string message = "Unit " + it->first + " is still sampling. Sending STOP_SAMPLING command...";
                        logger->info(message);
                        std::map<string, string> params; // empty params, just to pass
                        syncManager->sendCommand(ControlUnitCommand::STOP_SAMPLING, params, it->second.mac);
                        it->second.lastCommandSentMillis = millis();
                    }catch(ESPNowSyncError& error){
                        // todo: maybe set unit status as disconnected?
                    }
                }else if(it->second.status != SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD && (millis() - it->second.lastCommandSentMillis) >= COMMAND_SEND_MIN_INTERVAL_MILLIS){
                    try{
                        string message = "Unit " + it->first + "is still uploading files. Sending STOP_SAMPLE_FILES_UPLOAD command...";
                        logger->info(message);
                        std::map<string, string> params; // empty params, just to pass
                        syncManager->sendCommand(ControlUnitCommand::STOP_SAMPLE_FILES_UPLOAD, params, it->second.mac);
                        it->second.lastCommandSentMillis = millis();
                    }catch(ESPNowSyncError& error){
                        // todo: maybe set unit status as disconnected?
                    } 
                }
                break;
            case SystemStatus::SYSTEM_ERROR:
                return;
            default:{
                logger->info("Unknown state, shouldn't get here");
            }
        }
        logger->debug("Moving to the next unit...");    
    }
    logger->debug("System update complete...");
};