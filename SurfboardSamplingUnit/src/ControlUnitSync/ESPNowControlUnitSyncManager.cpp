#include "ESPNowControlUnitSyncManager.h"

ESPNowControlUnitSyncManager* ESPNowControlUnitSyncManager::instance = nullptr;

ESPNowControlUnitSyncManager::ESPNowControlUnitSyncManager(uint8_t controlUnitDeviceMac[])
{
    memcpy(controlUnitMac, controlUnitDeviceMac, 6);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(100);
    esp_err_t result = esp_now_init();
    if (result != ESP_OK) {
        Serial.print("ESPNow initialization failed! Error code: ");
        Serial.println(result);
        return;
    }
    else
    {
        Serial.println("ESPNow initialized successfully!");
        return;
    }

    // Set callback for received data
    esp_now_register_recv_cb(onDataReceivedCallback);
    // Add the peer control unit for communication
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, controlUnitMac, 6);
    peerInfo.channel = 0; // default channel
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer!");
    }
    instance = this;
}

void ESPNowControlUnitSyncManager::sendData(const uint8_t *data, size_t len) {
    esp_err_t result = esp_now_send(controlUnitMac, data, len);
    if (result == ESP_OK) {
        Serial.println("Data sent successfully!");
    } 
    else 
    {
        Serial.println("Failed to send data.");
    }
}

void ESPNowControlUnitSyncManager::sendSamples(const string sample , const string pattern , const string id)
{

    std::string message = id + "|" + pattern + "|" + sample;
    vector<uint8_t> messageData(message.begin(), message.end());
    sendData(messageData.data() , messageData.size());
}


void ESPNowControlUnitSyncManager::onDataReceived(const uint8_t* mac, const uint8_t* data, int len)
{
    Serial.print("Received data: ");
    int min_len = min(len ,MAX_SIZE);
    memcpy(buffer , data , static_cast<size_t>(min_len));
    for(int i=0 ; i<min_len ; i++)
    {
        commands.push(static_cast<ControlUnitCommand>(buffer[i]));
    }
}

void ESPNowControlUnitSyncManager::onDataReceivedCallback(const uint8_t* mac, const uint8_t* data, int len) {
   if(instance!= nullptr)
   {
       instance->onDataReceived(mac,data,len);
   }
   else
   {
       Serial.print("access to nullptr");
   }

}


ControlUnitCommand ESPNowControlUnitSyncManager::getNextCommand()
{
    if(commands.empty())
    {
        Serial.println("NO_COMMAND");
        return NO_COMMAND;
    }
    ControlUnitCommand cmd = commands.front();
    commands.pop();
    return cmd;
}


void ESPNowControlUnitSyncManager::reportStatus(UnitManagerStatus status) {

    uint8_t statusData[1] = {static_cast<uint8_t>(status)};
    sendData(statusData, sizeof(statusData));
}

