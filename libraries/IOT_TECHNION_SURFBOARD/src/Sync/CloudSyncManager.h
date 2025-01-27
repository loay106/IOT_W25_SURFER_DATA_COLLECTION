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
    // todo: add firebase client here.......
    private:
        Logger* logger;
        WifiHandler* wifiHandler;
        String sampleUploadEndpoint;
        HTTPClient httpClient;
    public:
        CloudSyncManager(Logger* logger, WifiHandler* wifiHandler);
        void init();
        void connect(string ssid, string password);
        void disconnect();
        bool isWifiConnected(){
            return wifiHandler->isWifiConnected();
        }
        void uploadSamples(String timestamp, String sensorID, String sensorModel, String samples);
};


#endif /* CLOUD_SYNC_MANAGER_H */
