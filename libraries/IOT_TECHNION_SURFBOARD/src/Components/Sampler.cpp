#include "Sampler.h"


Sampler::Sampler(Logger* logger, SDCardHandler* sdCardHandler, CloudSyncManager* cloudSyncManager){
     status = SamplerStatus::UNIT_STAND_BY;
     this->logger = logger;
     this->sdCardHandler = sdCardHandler;
     this->cloudSyncManager = cloudSyncManager;
     hasFilesToUpload = false;
}

void Sampler::addSensor(SensorBase *sensor){
    sensors.push_back(sensor);
}

void Sampler::init(){
    try{
        sdCardHandler->createFolder("/samplings");
        if(sdCardHandler->listFilesInDir("/samplings").size() > 0){
            hasFilesToUpload = true;
        }
    }catch(SDCardError& err){
        logger->error("Failed to create samplings folder");
        status = SamplerStatus::UNIT_ERROR;
        throw InitError();
    }
}

void Sampler::startSampling(int timestamp){
    status = SamplerStatus::UNIT_SAMPLING;
    logger->info("Sampling started!");
    for(int i=0;i<sensors.size(); i++){
        // sample files have this format:
        // [TIMESTAMP]_[SENSOR_ID]_[SENSOR_MODEL]
        string filePath = "/samplings/" + to_string(timestamp) + "_" + to_string(i) + "_" + sensors[i]->getModel();
        sensors[i]->startSampling(filePath);
    }
    if(sensors.size() > 0){
        hasFilesToUpload=true;
    }
    
}


void Sampler::stopSampling(){
    logger->info("Sampling stopped!");
    for(int i=0; i<sensors.size(); i++){
        string message = "Sensor id=" + to_string(i) + ", model=" + sensors[i]->getModel() + " stopped!";
        logger->info(message);
        sensors[i]->stopSampling();
    }
    status = SamplerStatus::UNIT_STAND_BY;
 }

 SamplerStatus Sampler::getStatus(){
    SamplerStatus res = status;
    return res;
 }

 void Sampler::enterErrorState(){
     status = SamplerStatus::UNIT_ERROR;
 }

void Sampler::uploadSampleFiles(){
    if(!hasFilesToUpload){
        status = SamplerStatus::UNIT_STAND_BY;
        return;
    }
    try{
        if(!cloudSyncManager->isWifiConnected()){
            cloudSyncManager->connect();
        }
    }catch(...){ 
        status = SamplerStatus::UNIT_ERROR;
        logger->error("Connecting to wifi failed!");
        return;
    }

    status = SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD;
    File root;
    sdCardHandler->getFolder("/samplings",&root);
    File file = root.openNextFile();
    int* fileSampleIndex = new int(0);
    while (file) {
        String fileName = file.name();
        String timestamp = fileName.substring(0, fileName.indexOf('_'));
        String sensorID = fileName.substring(fileName.indexOf('_') + 1, fileName.lastIndexOf('_'));
        String sensorModel = fileName.substring(fileName.lastIndexOf('_') + 1);
        logger->info("Processing file: " + string(fileName.c_str()));
        while (file.available()) {
            try {
                String line = file.readStringUntil('\n');
                line.trim();
                if (line.length() == 0) continue;
                cloudSyncManager->uploadSamples(timestamp, sensorID, sensorModel,line, fileSampleIndex);
            } catch (CloudSyncError& err) {
                logger->error("Failed to upload samples from CloudSyncManager");
                if(file){
                    file.close();
                }
                status = SamplerStatus::UNIT_ERROR;
                delete fileSampleIndex;
                return;
            } catch(...){
                logger->error("Failed to upload samples!");
                if(file){
                    file.close();
                }
                status = SamplerStatus::UNIT_ERROR;
                delete fileSampleIndex;
                return;
            }
        }
        logger->info("Finished uploading file: "+ string(fileName.c_str()));
        file.close();
        sdCardHandler->deleteFile("/samplings/" + fileName);
        file = root.openNextFile();
        *fileSampleIndex = 0;
    }
    
    // finished uploading files
    logger->info("Finished uploading all sample files.");
    cloudSyncManager->disconnect();
    hasFilesToUpload=false;
    delete fileSampleIndex;
    status = SamplerStatus::UNIT_STAND_BY;
}

 void Sampler::writeSensorsData(){
     for(int i= 0; i< Sampler::sensors.size(); i++){
         sensors[i]->writeSamples();
     }
 }
