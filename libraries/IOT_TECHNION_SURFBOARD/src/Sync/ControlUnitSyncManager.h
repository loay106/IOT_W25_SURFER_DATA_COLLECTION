#ifndef CONTROL_UNIT_SYNC_MANAGER_H 
#define CONTROL_UNIT_SYNC_MANAGER_H

using namespace std;
#include <cstdint>
#include <string>
#include <queue>
#include <vector>

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#include "../Utils/Status.h"
#include "../Components/IO/Logger.h"
#include "../Utils/Adresses.h"
#include "SyncMessages.h"


typedef struct StatusUpdateMessage{
    uint8_t from[6]; 
    SamplerStatus status;
} StatusUpdateMessage;

class ControlUnitSyncManager{
    // todo: change class to singleton
    private:
        static Logger* logger;
        static queue<StatusUpdateMessage> statusUpdateQueue;
        static SemaphoreHandle_t queueMutex;

        static void addStatusUpdateMessage(StatusUpdateMessage msg); 
        static void processReceivedMessages(const uint8_t *mac_addr, const uint8_t *incomingData, int len);   
    public:
        ControlUnitSyncManager(){};
        ControlUnitSyncManager(Logger* logger);
        void init(uint8_t samplingUnits[][6], int samplingUnitsNum);
        void sendCommand(const ControlUnitCommand& command,const std::map<string,string>& params, uint8_t samplingUnitMac[6]);
        void broadcastCommand(const ControlUnitCommand& command,const std::map<string,string>& params); 
        bool hasStatusUpdateMessages();
        static StatusUpdateMessage popStatusUpdateMessage(); 
};


#endif /* CONTROL_UNIT_SYNC_MANAGER_H */