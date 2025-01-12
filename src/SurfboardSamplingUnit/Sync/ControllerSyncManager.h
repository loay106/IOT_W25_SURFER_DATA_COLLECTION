#ifndef CONTROLLER_SYNC_MANAGER_H
#define CONTROLLER_SYNC_MANAGER_H

#include <WiFi.h>
#include <esp_now.h>
#include <string>
#include <Arduino.h>

#include "../../Utils/Logger.h"


using namespace std;

enum ControlUnitCommand{
    START_SAMPLING, // send timestamp here
    STOP_SAMPLING,
    UPLOAD_FILES,
    NO_COMMAND
};

enum class UnitManagerStatus{
    STANDBY,
    SAMPLING,
    ERROR,
};


class ControllerSyncManager {
    private:
        ControlUnitCommand nextCommand;
        uint8_t controlUnitMac[6];
        Logger logger;

        void onDataReceivedCallback(const uint8_t* mac, const uint8_t* incomingData, int len){
            string message(reinterpret_cast<const char *>(incomingData), len);
            // todo: add parameters to commands....
            if(message == "START_SAMPLING"){
                nextCommand = ControlUnitCommand::START_SAMPLING;
            }else if(message == "STOP_SAMPLING"){
                nextCommand = ControlUnitCommand::STOP_SAMPLING;
            }else if(message == "UPLOAD_FILES"){
                nextCommand == ControlUnitCommand::UPLOAD_FILES;
            }
        }
    public:

        ControllerSyncManager(Logger logger, uint8_t controlUnitMac[]): logger(logger){
            nextCommand=ControlUnitCommand::NO_COMMAND
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
            std::string stat;
            if(status == SamplerStatus::UNIT_STAND_BY){
                stat = "STAND_BY";
            }else if (status == SamplerStatus::UNIT_SAMPLING){
                stat = "SAMPLING";
            }else{
                stat = "ERROR";
            }
            string message = "STATUS_UPDATE|" + stat;
            esp_err_t result = esp_now_send(controlUnitMac, (uint8_t *) message.c_str(), message.length());
            if (result != ESP_OK) {
                logger.error("Failed to report status!");
            } 
        }

        ControlUnitCommand getNextCommand(){
            ControlUnitCommand command = nextCommand; // does this copy effectively? Is there a need for a mutex?
            nextCommand = ControlUnitCommand::NO_COMMAND;
            return command;

        }

};

#endif // CONTROLLER_SYNC_MANAGER_H
