#include "ESPNowSyncManager.h"
#include "../Exceptions/UnitExceptions.h"
#include "../Utils/Adresses.h"
#include <sstream>

Logger ESPNowSyncManager::logger = Logger(57600);
std::queue<StatusUpdateMessage> ESPNowSyncManager::statusUpdateQueue;
std::queue<SamplingSyncMessage> ESPNowSyncManager::samplingSyncQueue;

void ESPNowSyncManager::processReceivedMessages(const uint8_t *mac_addr, const uint8_t *incomingData, int len){
    string message(reinterpret_cast<const char *>(incomingData), len); // Convert incoming data to string
    //ESPNowSyncManager::logger.info("MESSAGE RECEIVED: " + message);
    vector<string> tokens;
    stringstream ss(message);
    string token;

    // Split the message by '|'
    while (getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        //ESPNowSyncManager::logger.error("Invalid message format.");
        return;
    }

    string subject = tokens[0];

    if (subject.compare("STATUS_UPDATE") == 0) {
        if (tokens.size() < 2) {
            //ESPNowSyncManager::logger.error("Invalid STATUS_UPDATE message format.");
            return;
        }

        // Parse STATUS_UPDATE
        SamplingUnitStatus status;
        if (tokens[1].compare("SAMPLING") == 0) {
            status = SamplingUnitStatus::UNIT_SAMPLING;
        } else if (tokens[1].compare("STAND_BY") == 0) {
            status = SamplingUnitStatus::UNIT_STAND_BY;
        } else if (tokens[1].compare("ERROR") == 0) {
            status = SamplingUnitStatus::UNIT_ERROR;
        } else {
            //ESPNowSyncManager::logger.error("Unknown STATUS_UPDATE status: " + tokens[1]);
            return;
        }

        // Create StatusUpdateMessage
        StatusUpdateMessage statusMessage;
        memcpy(statusMessage.from, mac_addr, 6); // Copy sender MAC address
        statusMessage.status = status;

        ESPNowSyncManager::statusUpdateQueue.push(statusMessage);
    } else if (subject.compare("SAMPLE_SYNC") == 0) {
        if (tokens.size() < 4) {
            //ESPNowSyncManager::logger.error("Invalid SAMPLE_SYNC message format.");
            return;
        }

        // Parse SAMPLE_SYNC
        SamplingSyncMessage syncMessage;
        memcpy(syncMessage.from, mac_addr, 6); // Copy sender MAC address
        syncMessage.sensorID = tokens[1];
        syncMessage.units = tokens[2];

        // Parse sampling data (remaining tokens tokens starting from index 3)
        for (size_t i = 3; i < tokens.size(); i++) {
            syncMessage.samplingData.push_back(tokens[i]);
        }
        ESPNowSyncManager::samplingSyncQueue.push(syncMessage);
        //logger.info("Added sampling sync message with " + to_string(syncMessage.samplingData.size()) + " samples");
    } else {
        // ESPNowSyncManager::logger.error("Unknown message received: " + message);
    }
}

void ESPNowSyncManager::initialize(uint8_t samplingUnits[][6], int samplingUnitsNum){
    WiFi.mode(WIFI_OFF); // Turn off WiFi first
    delay(100);
    WiFi.mode(WIFI_STA); // Then set to station mode

    if (esp_now_init() == ESP_OK) {
        ESPNowSyncManager::logger.info("ESPNow Init success!");
    }else {
        ESPNowSyncManager::logger.error("ESPNow Init failed!");
        throw InitError();
    }

    for (int i = 0; i < samplingUnitsNum; i++) {
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, samplingUnits[i], 6);
        peerInfo.channel = 0; // Default channel
        peerInfo.encrypt = false;

        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            ESPNowSyncManager::logger.error("Failed to add peer");
        } else {
            ESPNowSyncManager::logger.info("Added peer " + macToString(peerInfo.peer_addr) + " successfully!");
        }
    }

    esp_now_register_recv_cb(ESPNowSyncManager::processReceivedMessages);
}

void ESPNowSyncManager::sendCommand(ControlUnitCommand command, uint8_t samplingUnitMac[6]){
    string messageToSend;
    switch (command){
        case ControlUnitCommand::START_SAMPLING:{
            messageToSend="START_SAMPLING";
            break;
        }
        case ControlUnitCommand::STOP_SAMPLING:{
            messageToSend="STOP_SAMPLING";
            break;
        }
        default:{
            logger.error("Unknown command to send!");
            return;
        }
    }

    esp_err_t result = esp_now_send(samplingUnitMac, (uint8_t *) messageToSend.c_str(), messageToSend.length());
    if (result != ESP_OK) {
        // ESPNowSyncManager::logger.error("Failed to send command");
        throw ESPNowSyncError();
    }
}

void ESPNowSyncManager::broadcastCommand(ControlUnitCommand command){
    string messageToSend;
    switch (command){
        case ControlUnitCommand::START_SAMPLING:{
            messageToSend="START_SAMPLING";
            break;
        }
        case ControlUnitCommand::STOP_SAMPLING:{
            messageToSend="STOP_SAMPLING";
            break;
        }
        default:{
            logger.error("Unknown command to broadcast!");
            return;
        }
    }

    esp_err_t result = esp_now_send(NULL, (uint8_t *) messageToSend.c_str(), messageToSend.length());
    if (result != ESP_OK) {
        // ESPNowSyncManager::logger.error("Failed to broadcast command!");
        throw ESPNowSyncError();
    }
}

bool ESPNowSyncManager::hasStatusUpdateMessages(){
    return !ESPNowSyncManager::statusUpdateQueue.empty();
}

bool ESPNowSyncManager::hasSamplingUpdateMessages(){
    return !ESPNowSyncManager::samplingSyncQueue.empty();
}

StatusUpdateMessage ESPNowSyncManager::popStatusUpdateMessage(){
    // todo: validate for exceptions...
    StatusUpdateMessage item = ESPNowSyncManager::statusUpdateQueue.front();
    ESPNowSyncManager::statusUpdateQueue.pop();
    return item;
}

SamplingSyncMessage ESPNowSyncManager::popSamplingUpdateMessage(){
    SamplingSyncMessage item = ESPNowSyncManager::samplingSyncQueue.front();
    ESPNowSyncManager::samplingSyncQueue.pop();
    return item;
}
