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

    public:
        Sampler(Logger* logger, SDCardHandler* sdCardHandler, CloudSyncManager* cloudSyncManager);

        void addSensor(SensorBase* sensor);

        void init();

        void startSampling(int timestamp, int IMURate);
        void stopSampling();

        SamplerStatus getStatus();

        void updateSensorParams(int sensorID, void* param);

        // use this when you want the unit to enter error state for external reasons
        void enterErrorState();

        void uploadSampleFiles(string wifi_ssid, string wifi_password);

        void writeSensorsData();

        void printAcutalRates(unsigned long sampling_time);
};

#endif // SAMPLER_H