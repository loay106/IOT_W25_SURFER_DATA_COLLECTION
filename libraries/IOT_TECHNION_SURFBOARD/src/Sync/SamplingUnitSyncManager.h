#ifndef SAMPLING_UNIT_SYNC_MANAGER_H
#define SAMPLING_UNIT_SYNC_MANAGER_H

#include <WiFi.h>
#include <esp_now.h>
#include <string>
#include <Arduino.h>

#include "../Utils/Logger.h"
#include "SyncMessages.h"


using namespace std;


class SamplingUnitSyncManager {
    // todo: change class to singleton
    private:
        static CommandMessage* nextCommand;
        uint8_t controlUnitMac[6];
        Logger logger;

        static void onDataReceivedCallback(const uint8_t* mac, const uint8_t* incomingData, int len);
    public:

        SamplingUnitSyncManager(Logger logger, uint8_t controlUnitMac[]);
        void init();
        void reportStatus(SamplerStatus status);

        CommandMessage getNextCommand();

};

#endif // SAMPLING_UNIT_SYNC_MANAGER_H
