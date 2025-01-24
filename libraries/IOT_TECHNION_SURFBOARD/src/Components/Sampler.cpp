#include "Sampler.h"


Sampler::Sampler(Logger* logger, SDCardHandler* sdCardHandler, CloudSyncManager* cloudSyncManager){
     status = SamplerStatus::UNIT_STAND_BY;
     this->logger = logger;
     this->sdCardHandler = sdCardHandler;
     this->cloudSyncManager = cloudSyncManager;

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

void Sampler::startSampling(int timestamp, int IMURate){
    status = SamplerStatus::UNIT_SAMPLING;
    for(int i=0;i<sensors.size(); i++){
        // sample files have this format:
        // [TIMESTAMP]_[SENSOR_ID]_[SENSOR_MODEL]
        string filePath = "/samplings/" + to_string(timestamp) + "_" + to_string(i) + "_" + sensors[i]->getModel();
        sensors[i]->startSampling(filePath, IMURate);
    }
}
 void Sampler::stopSampling(){
     for(SensorBase* sensor: sensors){
         sensor->stopSampling();
     }
     status = SamplerStatus::UNIT_STAND_BY;
 }

 SamplerStatus Sampler::getStatus(){
     return status;
 }

 void Sampler::enterErrorState(){
     status = SamplerStatus::UNIT_ERROR;
 }

void Sampler::uploadSampleFiles(string wifi_ssid, string wifi_password){
     status = SamplerStatus::UNIT_STAND_BY;
    cloudSyncManager->connect(wifi_ssid,wifi_password);
    File root;
    sdCardHandler->getFolder("/samplings",&root);
    File file = root.openNextFile();
    while (file) {
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
        file = root.openNextFile();
    }
    logger->info("Finished uploading all sample files.");
    cloudSyncManager->disconnect();
}

 void Sampler::writeSensorsData(){
     for(int i= 0; i< Sampler::sensors.size(); i++){
         sensors[i]->writeSamples();
     }
 }
void Sampler::printAcutalRates(unsigned long sampling_time){
    string model="";
    unsigned long rate=0;
    unsigned long sampling_time_in_sec = sampling_time/1000;
    string message="";
    for(int i= 0; i< Sampler::sensors.size(); i++){
        model = sensors[i]->getModel();
        rate = (sensors[i]->getSamplesCount()) / (sampling_time_in_sec);
        message = model + " actual rate is: " + std::to_string(rate) + "Hz";
        logger->info(message);
    }
}