#include "SurfboardMainUnit.h"

SurfboardMainUnit::SurfboardMainUnit(int buttonPin, int SDCardChipSelectPin){
    logger = Logger();
    sdCardHandler = SDCardHandler(SDCardChipSelectPin, logger);
    syncManager = ControlUnitSyncManager(logger);
    timeHandler = RTCTimeHandler(logger);
    statusLighthandler = RGBStatusHandler(logger);
    buttonHandler = ButtonHandler(logger, buttonPin);
    status = SystemStatus::SYSTEM_STARTING;
    sampler = Sampler(logger, sdCardHandler);
}

void SurfboardMainUnit::init(uint8_t samplingUnitsAdresses[][6], int samplingUnitsNum, int RGBRedPin, int RGBGreenPin, int RGBBluePin) {
    try{
        logger.init();
        syncManager.init(samplingUnitsAdresses, samplingUnitsNum);
        statusLighthandler.init(RGBRedPin, RGBGreenPin, RGBBluePin);
        timeHandler.init();
        buttonHandler.init();
        sdCardHandler.init();

        // add external sampling units
        for(int i=0;i<samplingUnitsNum;i++){
            SamplingUnitRep samplingUnit;
            memcpy(samplingUnit.mac, samplingUnitsAdresses[i], 6);
            samplingUnit.status = SamplerStatus::UNIT_STAND_BY;
            string macString = macToString(samplingUnit.mac);
            samplingUnits[macString] = samplingUnit;
        }
    }catch(exception& e){
        updateStatus(SystemStatus::SYSTEM_ERROR);
        return;
    }

    std::map<string,string> configParams;
    try{
        // set system parameters
        SAMPLING_PARAMS[TIMESTAMP] = to_string(timeHandler.getCurrentTimestamp());
        configParams = sdCardHandler.readConfigFile(CONFIG_FILE_NAME);
        SAMPLING_PARAMS[IMU_RATE] = configParams[IMU_RATE];
        WIFI_PARAMS[WIFI_SSID] = configParams[WIFI_SSID];
        WIFI_PARAMS[WIFI_PASSWORD] = configParams[WIFI_PASSWORD];
    }catch(exception& err){
        logger.error("Failed to read config file from SD card");
        updateStatus(SystemStatus::SYSTEM_ERROR);
        return;
    }
    logger.info("System initalization complete!");
    updateStatus(SystemStatus::SYSTEM_STAND_BY);
}

void SurfboardMainUnit::updateStatus(SystemStatus newStatus){
    if(status == newStatus){
        return;
    }
    status = newStatus;
    switch (status){
        case SystemStatus::SYSTEM_STARTING:
            statusLighthandler.updateColors(RGBColors::NO_COLOR, RGBColors::NO_COLOR);
            break;
        case SystemStatus::SYSTEM_STAND_BY:
            statusLighthandler.updateColors(RGBColors::GREEN, RGBColors::GREEN);
            break;  
        case SystemStatus::SYSTEM_SAMPLING:
            statusLighthandler.updateColors(RGBColors::GREEN, RGBColors::NO_COLOR);
            break;     
        case SystemStatus::SYSTEM_SAMPLING_PARTIAL_ERROR:
            statusLighthandler.updateColors(RGBColors::GREEN, RGBColors::RED);
            break;    
        case SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD:
            statusLighthandler.updateColors(RGBColors::BLUE, RGBColors::NO_COLOR);
            break;
        case SystemStatus::SYSTEM_ERROR:
            statusLighthandler.updateColors(RGBColors::RED, RGBColors::RED);
            break;              
        default:
            statusLighthandler.updateColors(RGBColors::NO_COLOR, RGBColors::NO_COLOR);
            break;
    }
}

void SurfboardMainUnit::addSensor(SensorBase* sensor) {
    try{
        sampler.addSensor(sensor);
    }catch(InitError& err){
        logger.error("Failed to add sensor");
        updateStatus(SystemStatus::SYSTEM_ERROR);
    }
}

void SurfboardMainUnit::startSampling() {
    int timestamp = timeHandler.getCurrentTimestamp();
    SAMPLING_PARAMS[TIMESTAMP] = to_string(timestamp);
    try{
        syncManager.broadcastCommand(ControlUnitCommand::START_SAMPLING, SAMPLING_PARAMS);
        sampler.startSampling(timestamp, stoi(SAMPLING_PARAMS[IMU_RATE]));
    }catch(ESPNowSyncError& error){
        logger.error("Failed to send command to sampling units! Try again!");
        return;
    }
    updateStatus(SystemStatus::SYSTEM_SAMPLING);
    logger.info("Sampling started...");
}

void SurfboardMainUnit::stopSampling() {
    try{
        syncManager.broadcastCommand(ControlUnitCommand::UPLOAD_SAMPLE_FILES, WIFI_PARAMS);
        sampler.stopSampling();
        sampler.uploadSampleFiles(WIFI_PARAMS[WIFI_SSID], WIFI_PARAMS[WIFI_PASSWORD]);
        updateStatus(SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD);
    }catch(ESPNowSyncError& error){
        logger.error("Failed to send command to sampling units! Try again!");
        return;
    }
    updateStatus(SystemStatus::SYSTEM_STAND_BY);
}
void SurfboardMainUnit::updateSystem() {
    // status update
    while(syncManager.hasStatusUpdateMessages()){
        StatusUpdateMessage statusMessage = ControlUnitSyncManager::popStatusUpdateMessage();
        string unitID = macToString(statusMessage.from);
        try{
            SamplingUnitRep& samplingUnit = samplingUnits.at(unitID);
            samplingUnit.status = statusMessage.status;
        }
        catch(const std::out_of_range& ex){
            logger.error("Status update message received from unknown unit " + unitID);
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
                logger.info("Pressed button was ignored because system is not in a ready state");
            }
        }
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
                    syncManager.sendCommand(ControlUnitCommand::START_SAMPLING, SAMPLING_PARAMS, it->second.mac);
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
                syncManager.sendCommand(ControlUnitCommand::UPLOAD_SAMPLE_FILES, WIFI_PARAMS, it->second.mac);
            }
        }
        it++;        
    }

    statusLighthandler.flicker();
};