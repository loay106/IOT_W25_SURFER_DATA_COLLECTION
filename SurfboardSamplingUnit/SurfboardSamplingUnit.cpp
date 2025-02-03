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

/*void SamplerFileUploadTask(void *param) {
    Sampler* samp = static_cast<Sampler*>(param);
    samp->uploadSampleFiles();
    vTaskDelete(NULL);
}*/

void SurfboardSamplingUnit::handleNextCommand(){
  // todo: add status report here after each command handling
    try{
        CommandMessage command = syncManager->getNextCommand();
        switch(command.command){
            case ControlUnitCommand::START_SAMPLING:
                try{
                    int timestamp = stoi(command.params["TIMESTAMP"]);
                    if(sampler->getStatus()==UNIT_ERROR || sampler->getStatus()==UNIT_STAND_BY )
                    {
                        sampler->startSampling(timestamp);
                        reportStatus(SamplingUnitStatusMessage::SAMPLING,true);
                    } 
                    if(sampler->getStatus()==UNIT_SAMPLE_FILES_UPLOAD)
                    {
                        sampler->disconnect();
                        sampler->startSampling(timestamp);
                        reportStatus(SamplingUnitStatusMessage::SAMPLING,true);
                    }
                }catch(const exception& ex){
                    logger->error("Invalid command params");
                    reportStatus(SamplingUnitStatusMessage::ERROR,true);                  
                    return;
                }
                break;
            case ControlUnitCommand::STOP_SAMPLING:
                if(sampler->getStatus()==SamplerStatus::UNIT_SAMPLING)
                {
                    sampler->stopSampling();
                    reportStatus(SamplingUnitStatusMessage::STAND_BY,true);
                    return;
                }
                break;        
            case ControlUnitCommand::START_SAMPLE_FILES_UPLOAD:
                if(sampler->hasFilesToCloudUpload()){
                    if(sampler->getStatus() == SamplerStatus::UNIT_SAMPLING)
                    {
                        sampler->stopSampling();
                    }
                    if(sampler->getStatus() != SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD){
                        logger->debug("Uploading internal sampler data started");
                        try{
                            sampler->connect();
                            reportStatus(SamplingUnitStatusMessage::SAMPLE_FILES_UPLOAD,true);
                        }
                        catch(WifiError& er){
                            reportStatus(SamplingUnitStatusMessage::ERROR,true);
                            lastStatusReported = SamplerStatus::UNIT_ERROR;     
                            return;
                        }
                    }
                }
                else{
                    reportStatus(SamplingUnitStatusMessage::SAMPLE_FILES_UPLOAD_COMPLETE,true);
                    lastStatusReported = SamplerStatus::UNIT_STAND_BY;     
                    return;
                }
                break;
            case ControlUnitCommand::STOP_SAMPLE_FILES_UPLOAD:
                if(sampler->getStatus() == SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD)
                {
                    sampler->disconnect();
                    reportStatus(SamplingUnitStatusMessage::STAND_BY,true);
                    lastStatusReported = SamplerStatus::UNIT_STAND_BY;   
                    break;
                }
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
        }else{
            logger->debug("Uploading next file batch...");
            sampler->uploadNextSamples();
        }
    }
    else{
        reportStatus(SamplingUnitStatusMessage::SAMPLE_FILES_UPLOAD_COMPLETE,true);
    }
}

void SurfboardSamplingUnit::reportStatus(SamplingUnitStatusMessage status_message , bool force ){
    unsigned long currentTime = millis(); 
    SamplerStatus currentStatus = sampler->getStatus();
    if ( (currentTime - lastStatusReportTime >= REPORT_STATUS_INTERVAL_MILLIS) || force ) 
    {
        syncManager->reportStatus(status_message);
        lastStatusReportTime = currentTime;     
    }
}