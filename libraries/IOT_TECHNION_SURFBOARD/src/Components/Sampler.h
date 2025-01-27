#ifndef SAMPLER_H
#define SAMPLER_H

#include <vector>
#include <string>
#include <Arduino.h>
#include <HTTPClient.h>
#include <FS.h>
#include <SD.h>
#include <WiFi.h>
using namespace std;

#include "../Utils/Status.h"
#include "Sensors/SensorBase.h"
#include "IO/SDCardHandler.h"
#include "../Sync/CloudSyncManager.h"


class Sampler {
    // todo: change class to singleton
    private:
        vector<SensorBase*> sensors;
        SamplerStatus status;
        Logger* logger;
        SDCardHandler* sdCardHandler;
        CloudSyncManager* cloudSyncManager;
        string WIFI_SSID;
        string WIFI_PASSWORD;
        bool hasFilesToUpload;

    public:
        Sampler(Logger* logger, SDCardHandler* sdCardHandler, CloudSyncManager* cloudSyncManager, string WIFI_SSID, string WIFI_PASSWORD);

        void addSensor(SensorBase* sensor);

        void init();

        void startSampling(int timestamp);
        void stopSampling();

        bool hasFilesToCloudUpload(){
            return hasFilesToUpload;
        }

        SamplerStatus getStatus();

        // use this when you want the unit to enter error state for external reasons
        void enterErrorState();

        void uploadNextSampleFile();

        void writeSensorsData();
};

#endif // SAMPLER_H