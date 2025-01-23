#ifndef CLOUD_SYNC_MANAGER_H 
#define CLOUD_SYNC_MANAGER_H

#include <string>
#include <map>
#include <cstdint>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>


using namespace std;

class CloudSyncManager{
    public:
        CloudSyncManager();
        void init();
        void connect(string ssid, string password); // connect to wifi
        void disconnect(); // disconnect from wifi
        /*
            Leave this for later....
        */
        std::map<string, string> getSensorParams(uint8_t unitMac[]){};
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
