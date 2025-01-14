#include "SamplingUnitSyncManager.h"

CommandMessage* SamplingUnitSyncManager::nextCommand;
Logger SamplingUnitSyncManager::logger;

void SamplingUnitSyncManager::onDataReceivedCallback(const uint8_t *mac, const uint8_t *incomingData, int len){
    try{
        CommandMessage cmd = deserializeCommand(incomingData, len);
        SamplingUnitSyncManager::nextCommand = new CommandMessage();
        SamplingUnitSyncManager::nextCommand->command = cmd.command;
        SamplingUnitSyncManager::nextCommand->params = cmd.params;
    }catch(InvalidSyncMessage& err){
        SamplingUnitSyncManager::logger.error("Invalid command message received!");
        return;
    }
}

SamplingUnitSyncManager::SamplingUnitSyncManager(Logger logger, uint8_t controlUnitMac[]){
    SamplingUnitSyncManager::nextCommand=nullptr;
    memcpy(this->controlUnitMac, controlUnitMac, 6);
    SamplingUnitSyncManager::logger = logger;
}
void SamplingUnitSyncManager::init() {
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        SamplingUnitSyncManager::logger.info("Error initializing ESP-NOW");
        throw InitError();
    }
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, controlUnitMac, 6);
    peerInfo.channel = 0; // default channel
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        SamplingUnitSyncManager::logger.error("Failed to add peer!");
        throw InitError();
    }
    esp_now_register_recv_cb(SamplingUnitSyncManager::onDataReceivedCallback);
    SamplingUnitSyncManager::logger.info("peering successful!");
}
void SamplingUnitSyncManager::reportStatus(SamplerStatus status) {
    string message = serializeStatusUpdateMsg(status);
    esp_err_t result = esp_now_send(controlUnitMac, (uint8_t *) message.c_str(), message.length());
    if (result != ESP_OK) {
        SamplingUnitSyncManager::logger.error("Failed to report status!");
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