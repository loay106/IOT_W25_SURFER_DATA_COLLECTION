#include "CloudSyncManager.h"

CloudSyncManager::CloudSyncManager(Logger* logger, WifiHandler *wifiHandler){
    this->wifiHandler=wifiHandler;
    this->logger = logger;
    this->sampleUploadEndpoint = "https://us-central1-surfer-data-project.cloudfunctions.net/api/addSamples";
}

void CloudSyncManager::init() {}

void CloudSyncManager::connect(string ssid, string password){
    try{
        wifiHandler->connect(ssid, password);
        logger->info("Successfully connected to Wifi.")
        httpClient.begin(sampleUploadEndpoint);
        httpClient.addHeader("Content-Type", "application/json");
        logger->info("HTTP connection initiated Successfully.")
    }catch(WifiError& err){
        throw err;
    }
}

void CloudSyncManager::disconnect() {
    httpClient.end();
    logger->info("HTTP connection finished Successfully.")
    wifiHandler->disconnect();
    logger->info("Successfully disconnected from Wifi.")
}

void CloudSyncManager::uploadSamples(String timestamp, String sensorID, String sensorModel, String samples){
    static String lastTimestamp = "";
    static String lastSensorID = "";
    static int sampleIndex = 0;
    if (lastTimestamp != timestamp || lastSensorID != sensorID ) {
        lastTimestamp = timestamp;
        lastSensorID = sensorID;
        sampleIndex = 0;
    }
    String sampleArrayJson = "[";
    int startIndex = 0;
    int endIndex = samples.indexOf('|');
    while (endIndex > 0) {
        String sample = samples.substring(startIndex, endIndex);
        sampleArrayJson += "{\"sample\": \"" + sample + "\", \"sample_index\": " + String(sampleIndex++) + "},";
        startIndex = endIndex + 1;
        endIndex = samples.indexOf('|', startIndex);
    }
    String lastSample = samples.substring(startIndex);
    sampleArrayJson += "{\"sample\": \"" + lastSample + "\", \"sample_index\": " + String(sampleIndex++) + "}";
    sampleArrayJson += "]";
    String jsonData = "{\"timestamp\": \"" + timestamp + "\",\"sensorID\": \"" + sensorID + "\",\"sensorModel\": \"" + sensorModel + "\", \"unitMac\": \"" + wifiHandler->getUnitMac() + "\",\"samples\": " + sampleArrayJson + "}";
    logger->info(jsonData.c_str());
    int httpResponseCode = httpClient.POST(jsonData);
    if (httpResponseCode == 200) {
        logger->info("Data sent successfully!");
    } else {
        logger->error("Error sending data: " + to_string(httpResponseCode));
        throw CloudSyncError();
    }

}


