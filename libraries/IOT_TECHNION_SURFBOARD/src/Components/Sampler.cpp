#include "Sampler.h"


Sampler::Sampler(Logger* logger, SDCardHandler* sdCardHandler, CloudSyncManager* cloudSyncManager, string WIFI_SSID, string WIFI_PASSWORD){
     status = SamplerStatus::UNIT_STAND_BY;
     this->logger = logger;
     this->sdCardHandler = sdCardHandler;
     this->cloudSyncManager = cloudSyncManager;
     this->WIFI_SSID = WIFI_SSID;
     this->WIFI_PASSWORD = WIFI_PASSWORD;
}

void Sampler::addSensor(SensorBase *sensor){
    sensors.push_back(sensor);
}

void Sampler::init(){
    try{
        sdCardHandler->createFolder("/samplings");
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
    return status;
 }

 void Sampler::enterErrorState(){
     status = SamplerStatus::UNIT_ERROR;
 }

bool Sampler::uploadSampleFiles(){
     status = SamplerStatus::UNIT_STAND_BY;
    cloudSyncManager->connect(WIFI_SSID,WIFI_PASSWORD);
    File root;
    sdCardHandler->getFolder("/samplings",&root);
    File file = root.openNextFile();
    if (file) {
        String fileName = file.name();
        logger->info("Processing file: " + string(fileName.c_str()));
        while (file.available()) {
            String line = file.readStringUntil('\n');
            line.trim();
            if (line.length() == 0) continue;
            String timestamp = fileName.substring(0, fileName.indexOf('_'));
            String sensorID = fileName.substring(fileName.indexOf('_') + 1, fileName.lastIndexOf('_'));
            String sensorModel = fileName.substring(fileName.lastIndexOf('_') + 1);
            try {
                cloudSyncManager->uploadSamples(timestamp, sensorID, sensorModel,line);
            } catch (...) {
                logger->error("Failed to upload samples.");
                throw CloudSyncError();
            }
        }
        logger->info("Finished uploading file: "+ string(fileName.c_str()));
        file.close();
        sdCardHandler->deleteFile("/samplings/" + fileName);
        return false;
    }else{
        logger->info("Finished uploading all sample files.");
        return true;
    }

    cloudSyncManager->disconnect();
}

 void Sampler::writeSensorsData(){
     for(int i= 0; i< Sampler::sensors.size(); i++){
         sensors[i]->writeSamples();
     }
 }
