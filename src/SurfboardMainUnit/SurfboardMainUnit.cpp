#include "SurfboardMainUnit.h"

SurfboardMainUnit::SurfboardMainUnit(int buttonPin) : logger(logger){
    logger = Logger();
    syncManager = ControlUnitSyncManager(logger);
    timeHandler = RTCTimeHandler(logger);
    statusLighthandler = RGBStatusHandler(logger);
    buttonHandler = ButtonHandler(logger, buttonPin);
    status = SystemStatus::SYSTEM_STARTING;
    sampler = Sampler(logger);
}

void SurfboardMainUnit::init(uint8_t samplingUnitsAdresses[][6], int samplingUnitsNum, int RGBRedPin, int RGBGreenPin, int RGBBluePin) {
    try{
        logger.init();
        syncManager.init(samplingUnitsAdresses, samplingUnitsNum);
        statusLighthandler.init(); // todo: update so it receives a light....
        timeHandler.init();
        buttonHandler.init();

        // add external sampling units
        for(int i=0;i<samplingUnitsNum;i++){
            SamplingUnitRep samplingUnit;
            memcpy(samplingUnit.mac, samplingUnitsAdresses[i], 6);
            samplingUnit.status = SamplerStatus::UNIT_STAND_BY;
            string macString = macToString(samplingUnit.mac);
            samplingUnits[macString] = samplingUnit;
        }

        SAMPLING_PARAMS[TIMESTAMP] = to_string(timeHandler.getCurrentTimestamp());
        // todo: read from config file in sd card here and get wifi+imu rate params
        SAMPLING_PARAMS[IMU_RATE] = to_string(100);
        WIFI_PARAMS[WIFI_SSID] = "WIFI_SSID_VAL";
        WIFI_PARAMS[WIFI_PASSWORD] = "WIFI_PASSWORD_VAL";

    }catch(exception& e){
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

void SurfboardMainUnit::addSensor(SensorBase sensor) {
    try{
        sampler.addSensor(sensor);
    }catch(InitError& err){
        logger.error("Failed to add sensor");
        updateStatus(SystemStatus::SYSTEM_ERROR);
    }
}

void SurfboardMainUnit::startSampling() {
    SAMPLING_PARAMS[TIMESTAMP] = to_string(timeHandler.getCurrentTimestamp());
    try{
        syncManager.broadcastCommand(ControlUnitCommand::START_SAMPLING, SAMPLING_PARAMS);
        sampler.startSampling(SAMPLING_PARAMS.at(TIMESTAMP), SAMPLING_PARAMS.at(IMU_RATE));
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
        sampler.uploadSampleFiles();
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
        StatusUpdateMessage statusMessage = syncManager.popStatusUpdateMessage();
        string unitID = macToString(statusMessage.from);
        try{
            SamplingUnitRep& samplingUnit = samplingUnits.at(unitID);
            samplingUnit.status = statusMessage.status;
            samplingUnit.lastStatusUpdate = millis();
        }
        catch(const std::out_of_range& ex){
            logger.error("Status update message received from unknown unit " + unitID);
        }
    };

    if(buttonManager.wasPressed()){
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

    map<string, SamplingUnitRep>::iterator it = samplingUnits.begin();
    while (it != samplingUnits.end()) {
        if(it->second.mac == INTERNAL_SAMPLING_MAC){
            continue;
        }
        if(status == SystemStatus::SYSTEM_SAMPLING || status == SystemStatus::SYSTEM_SAMPLING_PARTIAL_ERROR){
            if(it->second.status == SamplerStatus::UNIT_SAMPLING){
                continue;
            }else if(it->second.status == SamplerStatus::UNIT_ERROR){
                numUnitsInError++;
                updateStatus(SystemStatus::SYSTEM_SAMPLING_PARTIAL_ERROR);
            }else{
                try{
                    espSyncManager.sendCommand(ControlUnitCommand::START_SAMPLING, SAMPLING_PARAMS, ->second.mac);
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
                espSyncManager.sendCommand(ControlUnitCommand::UPLOAD_SAMPLE_FILES, WIFI_PARAMS, it->second.mac);
            }
        }
        it++;        
    }
};