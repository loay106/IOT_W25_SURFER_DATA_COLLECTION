#include "CloudSyncManager.h"

CloudSyncManager::CloudSyncManager(WifiHandler *wifiHandler, String sampleUploadEndpoint){
    this->wifiHandler=wifiHandler;
    this->sampleUploadEndpoint = sampleUploadEndpoint;
}

void CloudSyncManager::init() {
    httpClient.begin(sampleUploadEndpoint);
    httpClient.addHeader("Content-Type", "application/json");
}

void CloudSyncManager::connect(string ssid, string password){
    try{
        wifiHandler->connect(ssid, password);
    }catch(WifiError& err){
        throw err;
    }
}

void CloudSyncManager::disconnect() {
    wifiHandler->disconnect();
}

void CloudSyncManager::uploadSamples(SDCardHandler::SDCardFileReader sampleFileReader, String timestamp, String unitMac, String sensorID, String sensorModel){
    /*
        Upload each line as is! don't convert to json array. Leave the computing to the cloud, that's the whole point.
    */
    String lineContent = "";
    while(true){
        try{
            lineContent = sampleFileReader.readNextLine();
            // todo: upload the entire file by streaming it as a resumeable file in the storage
            // the customer will call the trigger function on the files to merge them into a single file when he wants to download the file
        }catch(EndOfFileError& err){
            // we reached end of file, this means all the content was read successfully and we finished!
            break;
        }
    }

   
}
/*
void CloudSyncManager::uploadSamples(String timestamp, String unitMac, String sensorID, String sensorModel, String samples,HTTPClient* http){
    static String lastTimestamp = "";
    static int sampleIndex = 0;
    if (lastTimestamp != timestamp) {
        lastTimestamp = timestamp;
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
    String jsonData = "{\"timestamp\": \"" + timestamp + "\",\"sensorID\": \"" + sensorID + "\",\"sensorModel\": \"" + sensorModel + "\", \"unitMac\": \"" + unitMac + "\",\"samples\": " + sampleArrayJson + "}";
    Serial.println(jsonData);
    int httpResponseCode = http->POST(jsonData);
    if (httpResponseCode == 200) {
        Serial.println("Data sent successfully!");
    } else {
        Serial.println("Error sending data: " + String(httpResponseCode));
        throw CloudSyncError();
    }

}

*/

