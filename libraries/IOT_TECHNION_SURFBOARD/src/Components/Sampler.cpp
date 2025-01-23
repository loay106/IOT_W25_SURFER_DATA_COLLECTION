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
    cloudSyncManager->connect(wifi_ssid, wifi_password);
    vector<string> sampleFiles = sdCardHandler->listFilesInDir("/samplings");
    for(string sampleFilePath: sampleFiles){
        SDCardHandler::SDCardFileReader fileReader = sdCardHandler->readFile(sampleFilePath);
        // todo: get unit mac, sensor id, model etc from here...do this correctly
        String sensorID = "00";
        String timestamp = "123";
        String sensorModel = "model";
        String unitMac = "000";
        cloudSyncManager->uploadSamples(fileReader, timestamp, unitMac, sensorID, sensorModel);
        sdCardHandler->deleteFile(sampleFilePath);
    }
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