#include "SamplingUnitSyncManager.h"

SamplingUnitSyncManager* SamplingUnitSyncManager::instance = nullptr;
CommandMessage* SamplingUnitSyncManager::nextCommand = nullptr;
Logger* SamplingUnitSyncManager::logger = Logger::getInstance();

void SamplingUnitSyncManager::onDataReceivedCallback(const esp_now_recv_info_t* messageInfo, const uint8_t *incomingData, int len){
    try{
        CommandMessage cmd = deserializeCommand(incomingData, len);
        SamplingUnitSyncManager::nextCommand = new CommandMessage();
        SamplingUnitSyncManager::nextCommand->command = cmd.command;
        SamplingUnitSyncManager::nextCommand->params = cmd.params;
    }catch(InvalidSyncMessage& err){
        SamplingUnitSyncManager::logger->error("Invalid command message received!");
        return;
    }
}

void SamplingUnitSyncManager::init(uint8_t controlUnitMac[]) {
    WiFi.mode(WIFI_STA);
    controlUnitPeer = new esp_now_peer_info_t();
    memcpy(controlUnitPeer->peer_addr, controlUnitMac, 6);
    controlUnitPeer->channel = 0; // default channel
    controlUnitPeer->encrypt = false;
    memcpy(this->controlUnitMac, controlUnitMac,6);
}

void SamplingUnitSyncManager::connect(){
    if(isConnected){
        return;
    }
    if (esp_now_init() != ESP_OK) {
        SamplingUnitSyncManager::logger->info("Error initializing ESP-NOW");
        throw InitError();
    }
    if (esp_now_add_peer(controlUnitPeer) != ESP_OK) {
        throw ESPNowSyncError();
    }
    esp_now_register_recv_cb(SamplingUnitSyncManager::onDataReceivedCallback);
    isConnected=true;
}

void SamplingUnitSyncManager::disconnect(){
    esp_now_deinit();
    isConnected=false;
}

void SamplingUnitSyncManager::reportStatus(SamplingUnitStatusMessage status){
    string message = serializeStatusUpdateMsg(status);
    esp_err_t result = esp_now_send(controlUnitMac, (uint8_t *) message.c_str(), message.length());
    if (result != ESP_OK) {
        SamplingUnitSyncManager::logger->error("Failed to report status!");
    }
}

CommandMessage SamplingUnitSyncManager::getNextCommand(){
    if(SamplingUnitSyncManager::nextCommand){
        CommandMessage command;
        command.command = SamplingUnitSyncManager::nextCommand->command;
        command.params = SamplingUnitSyncManager::nextCommand->params;
        delete SamplingUnitSyncManager::nextCommand;
        SamplingUnitSyncManager::nextCommand = nullptr;
        return command;
    }else{
        throw NotReadyError();
    }
}
