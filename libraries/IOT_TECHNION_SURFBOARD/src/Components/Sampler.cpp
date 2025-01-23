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

 void Sampler::updateSensorParams(int sensorID, void *param){
     if(sensorID <= sensors.size()){
         sensors[sensorID]->updateSensor(param);
     }
 }

 void Sampler::enterErrorState(){
     status = SamplerStatus::UNIT_ERROR;
 }

 void Sampler::writeSensorsData(){
     for(int i= 0; i< Sampler::sensors.size(); i++){
         sensors[i]->writeSamples();
     }
 }

void uploadSampleFiles(string wifi_ssid, string wifi_password){
    /////////// use logger instead of serial
    status = SamplerStatus::UNIT_STAND_BY;
    const int BASE_DELAY_MS = 1000;
    const int MAX_RETRIES = 3;
    //////////// add http handling to cloudSyncManager
    const String endpoint = "https://us-central1-surfer-data-project.cloudfunctions.net/api/addSamples";
    HTTPClient http;
    http.begin(endpoint);
    http.addHeader("Content-Type", "application/json");
    cloudSyncManager->connect(wifi_ssid,wifi_password);
    sdCardHandler->init();
    //////////// add to SDCardHanadler
    File root = SD.open("/");
    if (!root || !root.isDirectory()) {
        Serial.println("Failed to open directory");
        throw SDCardError();
    }
    File file = root.openNextFile();
    while (file) {
        String fileName = file.name();
        Serial.println("Processing file: " + fileName);

        File tempFile = SD.open("/temp.txt", FILE_WRITE);
        if (!tempFile) {
            Serial.println("Failed to open temporary file");
            return;
        }

        bool encounteredFailure = false;

        while (file.available()) {
            String line = file.readStringUntil('\n');
            line.trim();
            if (line.length() == 0) continue;
            String sensorID = fileName.substring(0, fileName.indexOf('_'));
            String timestamp = fileName.substring(fileName.indexOf('_') + 1, fileName.lastIndexOf('_'));
            String sensorModel = fileName.substring(fileName.lastIndexOf('_') + 1);

            bool success = false;
            for (int attempt = 0; attempt < MAX_RETRIES; ++attempt) {
                try {
                    ///////// change "1" to real unit mac address
                    uploadSamples(timestamp, "1", sensorID, sensorModel,line,&http);
                    success = true;
                    break;
                } catch (...) {
                    Serial.printf("Retry %d failed for line: %s\n", attempt + 1, line.c_str());
                    delay(BASE_DELAY_MS * pow(2, attempt)); // Exponential backoff
                }
            }

            if (!success) {
                tempFile.println(line);
                while (file.available()) {
                    String remainingLine = file.readStringUntil('\n');
                    if (remainingLine.length() > 0) {
                        tempFile.println(remainingLine);
                    }
                }

                encounteredFailure = true;
                break;
            }
        }
        file.close();
        tempFile.close();
        if (encounteredFailure) {
            SD.remove(fileName);
            SD.rename("/temp.txt", fileName);
            throw std::runtime_error("Failed to upload all lines. Stopping process.");
        } else {
            SD.remove(fileName);
            SD.remove("/temp.txt");
        }

        file = root.openNextFile();
    }
    http.end();
    disconnect();
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