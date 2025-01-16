#include "SurfboardSamplingUnit.h"

SurfboardSamplingUnit::SurfboardSamplingUnit(int SDCardChipSelectPin){
    logger = Logger::getInstance();
    sdCardHandler = SDCardHandler(SDCardChipSelectPin, logger);
    syncManager = SamplingUnitSyncManager::getInstance();
    sampler = Sampler(logger, sdCardHandler);
}

void SurfboardSamplingUnit::init(uint8_t controlUnitMac[]){
    try{
        logger->init();
        syncManager->init(controlUnitMac);
        sdCardHandler.init();
        sampler.init();
    }catch(exception& e){
        sampler.enterErrorState();
        return;
    }   
}

void SurfboardSamplingUnit::addSensor(SensorBase *sensor){
    try{
        sampler.addSensor(sensor);
    }catch(InitError& err){
        logger->error("Failed to add sensor");
        sampler.enterErrorState();
    }
}

void SurfboardSamplingUnit::updateSystem(){
    // command handling
    try{
        CommandMessage command = syncManager->getNextCommand();
        switch(command.command){
            case ControlUnitCommand::START_SAMPLING:
                try{
                    int timestamp = stoi(command.params[TIMESTAMP]);
                    int imuRate = stoi(command.params[IMU_RATE]);
                    sampler.startSampling(timestamp, imuRate);
                }catch(const exception& ex){
                    logger->error("Invalid command params");
                    return;
                }    
            case ControlUnitCommand::UPLOAD_SAMPLE_FILES:
                try{
                    sampler.stopSampling();
                    sampler.uploadSampleFiles(command.params[WIFI_SSID], command.params[WIFI_PASSWORD]);
                }catch(exception& err){
                    logger->error("Invalid command params");
                    return;
                }
            case ControlUnitCommand::UPDATE_SENSOR_PARAMS:
                try{
                    sampler.stopSampling();
                    sampler.uploadSampleFiles(command.params[WIFI_SSID], command.params[WIFI_PASSWORD]);
                }catch(exception& err){
                    logger->error("Invalid command params");
                    return;
                }
        }
    }catch(NotReadyError& err){

    }

    // status report - update every STATUS_REPORT_DELAY_MILLIS
    int current = millis();
    if((current - lastStatusReportTime) >= STATUS_REPORT_DELAY_MILLIS){
        syncManager->reportStatus(sampler.getStatus());
        lastStatusReportTime = current;
    }
}