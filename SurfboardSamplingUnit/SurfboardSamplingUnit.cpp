#include "SurfboardSamplingUnit.h"

SurfboardSamplingUnit::SurfboardSamplingUnit(SamplingUnitSyncManager *syncManager, SDCardHandler *sdCardHandler, Sampler *sampler, Logger *logger){
    this->syncManager=syncManager;
    this->sdCardHandler=sdCardHandler;
    this->sampler=sampler;
    this->sdCardHandler;
    lastStatusReportTime = 0;
}

void SurfboardSamplingUnit::addSensor(SensorBase *sensor)
{
    sampler.addSensor(sensor);
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
                    sampler->startSampling(timestamp, imuRate);
                }catch(const exception& ex){
                    logger->error("Invalid command params");
                    return;
                }    
            case ControlUnitCommand::UPLOAD_SAMPLE_FILES:
                try{
                    sampler->stopSampling();
                    sampler->uploadSampleFiles(command.params[WIFI_SSID], command.params[WIFI_PASSWORD]);
                }catch(exception& err){
                    logger->error("Invalid command params");
                    return;
                }
            case ControlUnitCommand::UPDATE_SENSOR_PARAMS:
                try{
                    sampler->stopSampling();
                    sampler->uploadSampleFiles(command.params[WIFI_SSID], command.params[WIFI_PASSWORD]);
                }catch(exception& err){
                    logger->error("Invalid command params");
                    return;
                }
        }
    }catch(NotReadyError& err){
        // ignore, no command received yet
    }

    // status report - update every STATUS_REPORT_DELAY_MILLIS
    int current = millis();
    if((current - lastStatusReportTime) >= STATUS_REPORT_DELAY_MILLIS){
        syncManager->reportStatus(sampler.getStatus());
        lastStatusReportTime = current;
    }
}