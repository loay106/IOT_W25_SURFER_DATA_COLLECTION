#ifndef CLOUD_SYNC_MANAGER_H 
#define CLOUD_SYNC_MANAGER_H

#include <string>
#include <cstdint>

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "../Components/IO/WifiHandler.h"
#include "../Components/IO/SDCardHandler.h"


using namespace std;

class CloudSyncManager{
    private:
        WifiHandler* wifiHandler;
        String sampleUploadEndpoint;
        HTTPClient httpClient;
    public:
        CloudSyncManager(WifiHandler* wifiHandler,String sampleUploadEndpoint);
        void init();
        void connect(string ssid, string password); // connect to wifi
        void disconnect(); // disconnect from wifi
        /*
            Upload sample files as is to the cloud.
            // throw CloudSyncError() in case of a cloud error
        */
        void uploadSamples(SDCardHandler::SDCardFileReader sampleFileReader, String timestamp, String unitMac, String sensorID, String sensorModel);
        //void uploadSamples(String timestamp, String unitMac, String sensorID, String sensorModel, String samples,HTTPClient* http);
};


#endif /* CLOUD_SYNC_MANAGER_H */
