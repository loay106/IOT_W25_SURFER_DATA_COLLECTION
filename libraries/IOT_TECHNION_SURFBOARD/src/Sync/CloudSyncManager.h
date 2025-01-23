#ifndef CLOUD_SYNC_MANAGER_H 
#define CLOUD_SYNC_MANAGER_H

#include <string>
#include <cstdint>

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "../Components/IO/WifiHandler.h"


using namespace std;

class CloudSyncManager{
    private:
        WifiHandler* wifiHandler;
    public:
        CloudSyncManager(WifiHandler* wifiHandler);
        void init();
        void connect(string ssid, string password); // connect to wifi
        void disconnect(); // disconnect from wifi
        /*
            Upload sample files from sampling units (files from the sd card).
            Files are read by lines this method will be called with its content on each line (line = sampleData)
            example sample line (you can assume they will be in this format):
                0.4 -9.77 -1.57|0.4 -9.77 -1.57|0.4 -9.77 -1.57|0.4 -9.77 -1.57|0.4 -9.77 -1.57|0.4 -9.77 -1.57
            
            Lines are then written to a file in the firebase storage named "sampling_[timestamp].csv"

            // throw WifiError() in case of an error
        */
         void uploadSamples(String timestamp, String unitMac, String sensorID, String sensorModel, String samples,HTTPClient* http);
};


#endif /* CLOUD_SYNC_MANAGER_H */
