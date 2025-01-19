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


void CloudSyncManager::uploadSamples(string timestamp_str, string unitMac_str, string sensorID_str, string sensorModel_str, string samples_str){
    String timestamp = String(timestamp_str.c_str());
    String unitMac = String(unitMac_str.c_str());
    String sensorID = String(sensorID_str.c_str());
    String sensorModel = String(sensorModel_str.c_str());
    String samples = String(samples_str.c_str());

    const String endpoint = "https://us-central1-surfer-data-project.cloudfunctions.net/api/addSamples";
    const String exportEndpoint = "https://us-central1-surfer-data-project.cloudfunctions.net/api/exportAll";

    static String lastTimestamp = ""; // Static variable to store the last timestamp
    static int sampleIndex = 0;       // Static counter for sample index

    HTTPClient http;

    // Check if the timestamp has changed (new session)
    if (lastTimestamp != timestamp) {
        lastTimestamp = timestamp; // Update to the new timestamp
        sampleIndex = 0;           // Reset the sample index for the new session
    }

    // Specify the HTTP request
    http.begin(endpoint);  // Specify the endpoint URL
    http.addHeader("Content-Type", "application/json"); // Specify content type as JSON

    // Create an array to store all the sample data
    String sampleArrayJson = "[";

    // Split the samples string by '|'
    int startIndex = 0;
    int endIndex = samples.indexOf('|');

    while (endIndex > 0) {
        String sample = samples.substring(startIndex, endIndex);

        // Append the sample to the array JSON
        sampleArrayJson += "{\"sample\": \"" + sample + "\", \"sample_index\": " + String(sampleIndex++) + "},";

        // Update indices
        startIndex = endIndex + 1;
        endIndex = samples.indexOf('|', startIndex);
    }

    // Handle the last sample after the loop
    String lastSample = samples.substring(startIndex);
    sampleArrayJson += "{\"sample\": \"" + lastSample + "\", \"sample_index\": " + String(sampleIndex++) + "}";

    // Close the array JSON
    sampleArrayJson += "]";

    // Create the full JSON payload
    String jsonData = "{\"timestamp\": \"" + timestamp + "\",\"sensorID\": \"" + sensorID + "\",\"sensorModel\": \"" + sensorModel + "\", \"unitMac\": \"" + unitMac + "\",\"samples\": " + sampleArrayJson + "}";

    // Send POST request
    int httpResponseCode = http.POST(jsonData);

    // Check for success or failure
    if (httpResponseCode == 200) {
        Serial.println("Data sent successfully!");
    } else {
        Serial.println("Error sending data: " + String(httpResponseCode));
    }

    // Close the HTTP connection
    http.end();

}

