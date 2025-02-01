#include "SurfboardSamplingUnit.h"

SurfboardSamplingUnit::SurfboardSamplingUnit(SamplingUnitSyncManager *syncManager, SDCardHandler *sdCardHandler, Sampler *sampler, Logger *logger){
    this->syncManager=syncManager;
    this->sdCardHandler=sdCardHandler;
    this->sampler=sampler;
    this->sdCardHandler;
    lastStatusReportTime = 0;
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
                    int timestamp = stoi(command.params["TIMESTAMP"]);
                    sampler->startSampling(timestamp);
                }catch(const exception& ex){
                    logger->error("Invalid command params");
                    return;
                }
            case ControlUnitCommand::STOP_SAMPLING:
                    sampler->stopSampling();
                    return;
            case ControlUnitCommand::START_SAMPLE_FILES_UPLOAD:
                    if(sampler->hasFilesToCloudUpload()){
                        if(sampler->getStatus() != SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD){
                            logger->debug("Uploading sampler data");
                            xTaskCreatePinnedToCore(
                                SamplerFileUploadTask,      // Task function
                                "SamplerFileUploadTask",    // Task name
                                32768,        // Stack size (bytes) - 32kb
                                sampler,        // Task param
                                1,           // Priority (higher value = higher priority)
                                NULL, // Task handle (can be NULL if not needed)
                                1            // Core ID (0 or 1 for dual-core ESP32)
                            );
                        }
                    }
                    break;
            case ControlUnitCommand::STOP_SAMPLE_FILES_UPLOAD:
                    sampler->stopUploadSampleFiles();
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
    // todo: report SAMPLE_FILES_UPLOAD_COMPLETE here when its complete - add this functionality
}

void SurfboardSamplingUnit::reportStatus(){
    // todo: report status to main unit every REPORT_STATUS_INTERVAL_MILLIS
    // *AND* everytime the status changes
    // *AND* everytime a command is received

}