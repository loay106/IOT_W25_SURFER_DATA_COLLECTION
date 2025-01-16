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
}

SurfboardMainUnit::SurfboardMainUnit(Logger *logger, int SDCardChipSelectPin)
{
    logger = logger;
    sdCardHandler = SDCardHandler(SDCardChipSelectPin, logger);
    syncManager = ControlUnitSyncManager::getInstance();
    timeHandler = RTCTimeHandler(logger);
    statusLighthandler = RGBStatusHandler(logger);
    buttonHandler = ButtonHandler::getInstance();
    status = SystemStatus::SYSTEM_STARTING;
    sampler = Sampler(logger, sdCardHandler);
}

void SurfboardMainUnit::init(uint8_t samplingUnitsAdresses[][6], int samplingUnitsNum) {
    // add sampling units
    for(int i=0;i<samplingUnitsNum;i++){
        SamplingUnitRep samplingUnit;
        memcpy(samplingUnit.mac, samplingUnitsAdresses[i], 6);
        samplingUnit.status = SamplerStatus::UNIT_STAND_BY;
        string macString = macToString(samplingUnit.mac);
        samplingUnits[macString] = samplingUnit;
    }

    // set system parameters
    std::map<string,string> configParams;
    try{
        SAMPLING_PARAMS[TIMESTAMP] = to_string(timeHandler->getCurrentTimestamp());
        configParams = sdCardHandler->readConfigFile(CONFIG_FILE_NAME);
        SAMPLING_PARAMS[IMU_RATE] = configParams[IMU_RATE];
        WIFI_PARAMS[WIFI_SSID] = configParams[WIFI_SSID];
        WIFI_PARAMS[WIFI_PASSWORD] = configParams[WIFI_PASSWORD];
    }catch(exception& err){
        logger->error("Failed to read config file from SD card");
        updateStatus(SystemStatus::SYSTEM_ERROR);
        throw InitError();
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
        case SystemStatus::SYSTEM_ERROR:
            statusLighthandler->updateColors(RGBColors::RED, RGBColors::RED);
            break;              
        default:
            statusLighthandler->updateColors(RGBColors::NO_COLOR, RGBColors::NO_COLOR);
            break;
    }
}

void SurfboardMainUnit::addSensor(SensorBase* sensor) {
    try{
        sampler->addSensor(sensor);
    }catch(InitError& err){
        logger->error("Failed to add sensor");
        updateStatus(SystemStatus::SYSTEM_ERROR);
    }
}

void SurfboardMainUnit::startSampling() {
    int timestamp = timeHandler->getCurrentTimestamp();
    SAMPLING_PARAMS[TIMESTAMP] = to_string(timestamp);
    try{
        syncManager->broadcastCommand(ControlUnitCommand::START_SAMPLING, SAMPLING_PARAMS);
        sampler->startSampling(timestamp, stoi(SAMPLING_PARAMS[IMU_RATE]));
    }catch(ESPNowSyncError& error){
        logger->error("Failed to send command to sampling units! Try again!");
        return;
    }
    updateStatus(SystemStatus::SYSTEM_SAMPLING);
    logger->info("Sampling started...");
}

void SurfboardMainUnit::stopSampling() {
    try{
        syncManager->broadcastCommand(ControlUnitCommand::UPLOAD_SAMPLE_FILES, WIFI_PARAMS);
        sampler->stopSampling();
        sampler->uploadSampleFiles(WIFI_PARAMS[WIFI_SSID], WIFI_PARAMS[WIFI_PASSWORD]);
        updateStatus(SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD);
    }catch(ESPNowSyncError& error){
        logger->error("Failed to send command to sampling units! Try again!");
        return;
    }
    updateStatus(SystemStatus::SYSTEM_STAND_BY);
}


void SurfboardMainUnit::RecalibrateForceSensors(uint8_t mac[6] samplingUnit, map<string,string> newCalibrationFactors){
    try{
        syncManager->sendCommand(ControlUnitCommand::UPDATE_SENSOR_PARAMS, newCalibrationFactors, samplingUnit);
    }catch(ESPNowSyncError& err){
        logger->error("Failed to send recalibration factor to unit!");
        return;
    }
}


void SurfboardMainUnit::updateSystem() {
    // status update
    while(syncManager->hasStatusUpdateMessages()){
        StatusUpdateMessage statusMessage = ControlUnitSyncManager::popStatusUpdateMessage();
        string unitID = macToString(statusMessage.from);
        try{
            SamplingUnitRep& samplingUnit = samplingUnits.at(unitID);
            samplingUnit.status = statusMessage.status;
        }
        catch(const std::out_of_range& ex){
            logger->error("Status update message received from unknown unit " + unitID);
        }
    };

    if(buttonHandler.wasPressed()){
        switch(status){
            case SystemStatus::SYSTEM_SAMPLING:{
                stopSampling();
                return;
            }
            case SystemStatus::SYSTEM_SAMPLING_PARTIAL_ERROR:{
                stopSampling();
                return;
            }
            case SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD:{
                // file upload must be completed first to start another session
                return;
            }
            case SystemStatus::SYSTEM_STAND_BY:{
                startSampling();
                return;
            }
            case SystemStatus::SYSTEM_ERROR:{
                return;
            }
            default:{
                // ignore the press
                logger->info("Pressed button was ignored because system is not in a ready state");
            }
        }
    }


    if(status == SystemStatus::SYSTEM_STAND_BY){
        // todo: add code that polls from the firebase cloud the new calibration values....
        // RecalibrateForceSensors(samplingUnit,newCalibrationFactors);
        // uint8_t mac[6] samplingUnit;
        // map<string,string> newCalibrationFactors; // sensor id 
    }

    std::map<string, SamplingUnitRep>::iterator it = samplingUnits.begin();
    // todo: validate that units finished syncing...when all finish move system status to STAND_BY...
    while (it != samplingUnits.end()) {
        if(status == SystemStatus::SYSTEM_SAMPLING || status == SystemStatus::SYSTEM_SAMPLING_PARTIAL_ERROR){
            if(it->second.status == SamplerStatus::UNIT_SAMPLING){
                continue;
            }else if(it->second.status == SamplerStatus::UNIT_ERROR){
                updateStatus(SystemStatus::SYSTEM_SAMPLING_PARTIAL_ERROR);
            }else{
                try{
                    syncManager->sendCommand(ControlUnitCommand::START_SAMPLING, SAMPLING_PARAMS, it->second.mac);
                }catch(ESPNowSyncError& error){
                    // try again next iteration...maybe unit is not nearby
                    continue;
                } 
            }
        }else if(status == SystemStatus::SYSTEM_STAND_BY){
            // nothing to do, maybe connect to cloud to retrieve params? (calibration etc...)
        }else if(status == SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD){
            if(it->second.status == SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD || it->second.status == SamplerStatus::UNIT_ERROR){
                continue;
            }else{
                syncManager->sendCommand(ControlUnitCommand::UPLOAD_SAMPLE_FILES, WIFI_PARAMS, it->second.mac);
            }
        }
        it++;        
    }

    statusLighthandler.flicker();
};