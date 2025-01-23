#include "CloudSyncManager.h"

CloudSyncManager::CloudSyncManager(){}
void CloudSyncManager::init(){}

void CloudSyncManager::connect(string ssid, string password){
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.println("Connecting to WiFi...");

    // Wait until connected to WiFi
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi!");
}

void CloudSyncManager::disconnect() {
    WiFi.disconnect(true);
    Serial.println("Disconnected from WiFi.");
}


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

