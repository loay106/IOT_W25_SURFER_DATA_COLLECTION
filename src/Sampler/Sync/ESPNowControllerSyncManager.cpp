#include "ESPNowControllerSyncManager.h"

ESPNowControllerSyncManager* ESPNowControllerSyncManager::instance = nullptr;

void ESPNowControllerSyncManager::initialize(uint8_t controlUnitDeviceMac[]){
    instance = this;
    memcpy(instance->controlUnitMac, controlUnitDeviceMac, 6);
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(onDataReceivedCallback);
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, controlUnitDeviceMac, 6);
    peerInfo.channel = 0; // default channel
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer!");
    }
    Serial.println("peering successful!");
}

void ESPNowControllerSyncManager::sendData(const uint8_t *data, size_t len) {
    esp_err_t result = esp_now_send(instance->controlUnitMac, data, len);
    if (result == ESP_OK) {
        Serial.println("Data sent successfully!");
    } 
    else 
    {
        Serial.println("Failed to send data.");
    }
}

void ESPNowControllerSyncManager::sendSamples(const string sample , const string pattern , const string id)
{
    std::string message = "SAMPLE_SYNC|"+id + "|" + pattern + "|" + sample;
    Serial.println(message.c_str());
    sendData((uint8_t *) message.c_str(),message.length());
}


void ESPNowControllerSyncManager::onDataReceived(const uint8_t* mac, const uint8_t* data, int len)
{
    Serial.println("data recieved:");
    char charData[len + 1]; // Create a buffer with space for null-terminator
    memcpy(charData, data, len); // Copy the data
    charData[len] = '\0'; // Null-terminate the string
    std::string command = charData;
    Serial.println(charData);
    commands.push(command);

}

void ESPNowControllerSyncManager::onDataReceivedCallback(const uint8_t* mac, const uint8_t* data, int len) {
   if(instance!= nullptr)
   {
       instance->onDataReceived(mac,data,len);
   }
   else
   {
       Serial.print("access to nullptr");
   }

}

//a8:42:e3:45:94:68
ControlUnitCommand ESPNowControllerSyncManager::getNextCommand()
{
    if(commands.empty())
    {
        Serial.println("NO_COMMAND");
        return ControlUnitCommand::NO_COMMAND;
    }
    std::string cmd = commands.front();
    commands.pop();
    if(cmd == "START_SAMPLING" ){
        Serial.println("START_SAMPLING");
        return ControlUnitCommand::START_SAMPLING;
    }
    if(cmd == "STOP_SAMPLING" ){
        Serial.println("STOP_SAMPLING");
        return ControlUnitCommand::STOP_SAMPLING;
    }
    Serial.println("undetected command");
}

void ESPNowControllerSyncManager::reportStatus(UnitManagerStatus status) {
    std::string stat;
    if(status == UnitManagerStatus::STANDBY){
        stat = "STAND_BY";
    }
    if (status == UnitManagerStatus::SAMPLING){
        stat = "SAMPLING";
    }
    if (status == UnitManagerStatus::ERROR){
        stat = "ERROR";
    }
    std::string message = "STATUS_UPDATE|" + stat;
    Serial.println(message.c_str());
    sendData((uint8_t *) message.c_str(),message.length());
}

