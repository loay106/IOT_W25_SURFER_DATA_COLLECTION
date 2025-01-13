#include "SurfboardMainUnit.h"

SurfboardMainUnit::SurfboardMainUnit(int buttonPin): logger(logger){
    logger = Logger();
    syncManager = ControlUnitSyncManager(logger);
    timeHandler = RTCTimeHandler(logger);
    statusLighthandler = RGBStatusHandler(logger);
    buttonHandler = ButtonHandler(logger, buttonPin);
    status = SystemStatus::SYSTEM_STARTING;
    sampler = Sampler(logger);
}


void SurfboardMainUnit::init(uint8_t samplingUnitsAdresses[][6], int samplingUnitsNum, int RGBRedPin, int RGBGreenPin, int RGBBluePin) {

    status = SystemStatus::SYSTEM_INITIALIZING;
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

        // add internal sampling unit
        SamplingUnitRep samplingUnit;
        samplingUnit.mac = INTERNAL_SAMPLING_MAC;
        samplingUnit.status = SamplerStatus::UNIT_STAND_BY;
        string macString = macToString(samplingUnit.mac);
        samplingUnits[macString] = samplingUnit;

        status = SystemStatus::SYSTEM_STAND_BY;
        logger.info("System initalization complete!");
        statusLighthandler.updateStatus(status);
    }catch(exception& e){
        status = SystemStatus::SYSTEM_ERROR;
        statusLighthandler.updateStatus(status);
    }
}
void SurfboardMainUnit::addSensor(SensorBase sensor) {
    try{
        sampler.addSensor(sensor);
    }catch(InitError& err){
        logger.error("Failed to add sensor");
        // todo: report error status here...
    }
}
void SurfboardMainUnit::startSampling() {
    if(status == SystemStatus::SYSTEM_STAND_BY){
        map<string,string> params;
        int timestamp = timeHandler.getCurrentTimestamp();
        params[TIMESTAMP] = timestamp;
        params[IMU_RATE] = 100; // todo: change???
        try{
            syncManager.broadcastCommand(ControlUnitCommand::START_SAMPLING, params);
            sampler.startSampling(timestamp, IMU_RATE);
        }catch(ESPNowSyncError& error){
            logger.error("Failed to send command to sampling units!");
            return;
        }
        status = SystemStatus::SYSTEM_SAMPLING;
        statusLighthandler.updateStatus(status);
        logger.info("Sampling started...");
    }
}
void SurfboardMainUnit::stopSampling() {
    if(status == SystemStatus::SYSTEM_SAMPLING){
        try{
            map<string,string> params;
            syncManager.broadcastCommand(ControlUnitCommand::STOP_SAMPLING, params);
            sampler.stopSampling();
        }catch(ESPNowSyncError& error){
            logger.error("Failed to send command to sampling units!");
            return;
        }
        status = SystemStatus::SYSTEM_STAND_BY;        
        statusLightManager.updateStatus(status);
    }
}
void SurfboardMainUnit::updateSystem() {
    // status update
    while(syncManager.hasStatusUpdateMessages()){
        StatusUpdateMessage statusMessage = syncManager.popStatusUpdateMessage();
        string unitID = macToString(statusMessage.from);
        try{
            SamplingUnitRep& samplingUnit = samplingUnits.at(unitID);
            samplingUnit.status = statusMessage.status;
        }
        catch(const std::out_of_range& ex){
            logger.error("Status update message received from unknown unit " + unitID);
        }
    };

    // internal unit status update
    string unitID = macToString(INTERNAL_SAMPLING_MAC);
    try{
        SamplingUnitRep& samplingUnit = samplingUnits.at(unitID);
        samplingUnit.status = sampler.getStatus();
    }
    catch(const std::out_of_range& ex){
        logger.error("Status update message received from unknown unit " + unitID);
    }

    if(buttonManager.wasPressed()){
        switch(status){
            case SystemStatus::SYSTEM_SAMPLING:{
                stopSampling();
                return;
            }
            case SystemStatus::SYSTEM_STAND_BY:{
                startSampling();
                return;
            }
            default:{
                // ignore the press
                logger.info("Pressed button was ignored because system is not in a ready state");
            }
        }
    }

    std::map<string, SamplingUnitRep>::iterator it = samplingUnits.begin();
    while (it != samplingUnits.end()) {
        if(it->second.mac == INTERNAL_SAMPLING_MAC){
            continue;
        }else if(status == SystemStatus::SYSTEM_SAMPLING && it->second.status != SamplerStatus::UNIT_SAMPLING){
            try{
                espSyncManager.sendCommand(ControlUnitCommand::START_SAMPLING, it->second.mac);
            }catch(ESPNowSyncError& error){
                it->second.status = SamplerStatus::UNIT_ERROR;
            }  
        }else if(status != SystemStatus::SYSTEM_SAMPLING && it->second.status == SamplerStatus::UNIT_SAMPLING){
            try{
                espSyncManager.sendCommand(ControlUnitCommand::STOP_SAMPLING, it->second.mac);
            }catch(ESPNowSyncError& error){
                it->second.status = SamplerStatus::UNIT_ERROR;
            }    
        }
        it++;
    }
};