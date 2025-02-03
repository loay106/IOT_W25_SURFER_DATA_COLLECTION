#include "SurfboardSamplingUnit.h"

SurfboardSamplingUnit::SurfboardSamplingUnit(SamplingUnitSyncManager *syncManager, SDCardHandler *sdCardHandler, Sampler *sampler, Logger *logger){
    this->syncManager=syncManager;
    this->sdCardHandler=sdCardHandler;
    this->sampler=sampler;
    this->sdCardHandler;
    lastStatusReportTime = 0;
    lastStatusReported = UNIT_STAND_BY;
}

void SurfboardSamplingUnit::addSensor(SensorBase *sensor){
    sampler->addSensor(sensor);
}

SamplerStatus SurfboardSamplingUnit::getStatus(){
    SamplerStatus stat = sampler->getStatus();
    return stat;
}

void SamplerFileUploadTask(void *param) {
    Sampler* samp = static_cast<Sampler*>(param);
    samp->uploadSampleFiles();
    vTaskDelete(NULL);
}

void SurfboardSamplingUnit::handleNextCommand(){
  // todo: add status report here after each command handling
    try{
        CommandMessage command = syncManager->getNextCommand();
        switch(command.command){
            case ControlUnitCommand::START_SAMPLING:
                try{
                    if(sampler->getStatus()!=SamplerStatus::UNIT_SAMPLING)
                    {
                        int timestamp = stoi(command.params["TIMESTAMP"]);
                        sampler->startSampling(timestamp);
                        syncManager->reportStatus(SamplingUnitStatusMessage::SAMPLING);
                        lastStatusReported = SamplerStatus::UNIT_SAMPLING;
                    }
                   
                }catch(const exception& ex){
                    logger->error("Invalid command params");
                    syncManager->reportStatus(SamplingUnitStatusMessage::ERROR);
                    lastStatusReported = SamplerStatus::UNIT_ERROR;                   
                    return;
                }
                break;
            case ControlUnitCommand::STOP_SAMPLING:
                    sampler->stopSampling();
                    syncManager->reportStatus(SamplingUnitStatusMessage::STAND_BY);
                    lastStatusReported = SamplerStatus::UNIT_STAND_BY;
                    return;
            case ControlUnitCommand::START_SAMPLE_FILES_UPLOAD:
                    if(sampler->hasFilesToCloudUpload()){
                      if(sampler->getStatus() != SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD){
                          logger->debug("Uploading internal sampler data started");
                          try{
                              sampler->connect();
                          }catch(WifiError& er){
                              updateStatus(SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD_PARTIAL_ERROR);
                              syncManager->reportStatus(SamplingUnitStatusMessage::ERROR);
                              lastStatusReported = SamplerStatus::UNIT_ERROR;     
                              return;
                          }
                    }else{
                        syncManager->reportStatus(SamplingUnitStatusMessage::SAMPLE_FILES_UPLOAD_COMPLETE);
                        lastStatusReported = SamplerStatus::STAND_BY;     
                        return;
                    }
                    break;
            case ControlUnitCommand::STOP_SAMPLE_FILES_UPLOAD:
                    sampler->disconnect();
                    syncManager->reportStatus(SamplingUnitStatusMessage::STAND_BY);
                    lastStatusReported = SamplerStatus::STAND_BY;   
                    break;
        }
    }catch(NotReadyError& err){
        // ignore, no command received yet
    }

}

void SurfboardSamplingUnit::loopSampling(){
    sampler->writeSensorsData();
}

void SurfboardSamplingUnit::loopFileUpload(){
    if(sampler->hasFilesToCloudUpload()){
        if(sampler->getStatus() != SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD){
            logger->debug("Error in internal sampler file upload");
            updateStatus(SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD_PARTIAL_ERROR);
        }else{
            logger->debug("Uploading next file batch...");
            sampler->uploadNextSamples();
        }
    }else{
        // all file uploading is complete here....
        // todo: report SamplingUnitStatusMessage::SAMPLE_FILES_UPLOAD_COMPLETE here
    }
}

void SurfboardSamplingUnit::reportStatus(){
    unsigned long currentTime = millis(); 
    SamplerStatus currentStatus = sampler->getStatus();
    if ( (currentTime - lastStatusReportTime >= REPORT_STATUS_INTERVAL_MILLIS) || lastStatusReported!=currentStatus ) 
    {
        switch (currentStatus)
        {
            case UNIT_STAND_BY:
                syncManager->reportStatus(SamplingUnitStatusMessage::STAND_BY);
                break;

            case UNIT_SAMPLING:
                syncManager->reportStatus(SamplingUnitStatusMessage::SAMPLING);
                break;

            case UNIT_ERROR:
                syncManager->reportStatus(SamplingUnitStatusMessage::ERROR);
                break;

            case UNIT_SAMPLE_FILES_UPLOAD: 
                syncManager->reportStatus(SamplingUnitStatusMessage::SAMPLE_FILES_UPLOAD);
                break;
            
            default:
                break;
        }
        lastStatusReportTime = currentTime;
        lastStatusReported = currentStatus;     
    }
    // todo: report status to main unit every REPORT_STATUS_INTERVAL_MILLIS
    // *AND* everytime the status changes
    // *AND* everytime a command is received
}