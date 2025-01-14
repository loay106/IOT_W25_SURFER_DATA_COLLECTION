#ifndef SAMPLING_UNIT_SYNC_MANAGER_H
#define SAMPLING_UNIT_SYNC_MANAGER_H

#include <WiFi.h>
#include <esp_now.h>
#include <string>
#include <Arduino.h>

#include "../../Utils/Logger.h"
#include "SyncMessages.h"


using namespace std;


class SamplingUnitSyncManager {
    private:
        CommandMessage* nextCommand;
        uint8_t controlUnitMac[6];
        Logger logger;

        void onDataReceivedCallback(const uint8_t* mac, const uint8_t* incomingData, int len){
            try{
                nextCommand = deserializeCommand(incomingData, len); // todo: copy object here...
            }catch(InvalidSyncMessage& err){
                logger.error("Invalid command message received!");
                return;
            }
        }
    public:

        SamplingUnitSyncManager(Logger logger, uint8_t controlUnitMac[]): logger(logger){
            nextCommand=nullptr;
            memcpy(this->controlUnitMac, controlUnitMac, 6);
        };

        void init(){
            WiFi.mode(WIFI_STA);
            if (esp_now_init() != ESP_OK) {
                logger.info("Error initializing ESP-NOW");
                throw InitError();
            }
            esp_now_peer_info_t peerInfo = {};
            memcpy(peerInfo.peer_addr, controlUnitMac, 6);
            peerInfo.channel = 0; // default channel
            peerInfo.encrypt = false;
            if (esp_now_add_peer(&peerInfo) != ESP_OK) {
                logger.error("Failed to add peer!");
                throw InitError();
            }
            esp_now_register_recv_cb(this->onDataReceivedCallback);
            logger.info("peering successful!");
        }

        void reportStatus(SamplerStatus status){
            string message = serializeStatusUpdateMsg(status);
            esp_err_t result = esp_now_send(controlUnitMac, (uint8_t *) message.c_str(), message.length());
            if (result != ESP_OK) {
                logger.error("Failed to report status!");
            } 
        }

        CommandMessage* getNextCommand(){
            CommandMessage* command = nextCommand;
            nextCommand = nullptr;
            return command;
        }

};

#endif // SAMPLING_UNIT_SYNC_MANAGER_H
