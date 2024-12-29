#include "ESPNowControlUnitSyncManager.h"

ESPNowControlUnitSyncManager::ESPNowControlUnitSyncManager(uint8_t controlUnitDeviceMac[])
{
    memcpy(controlUnitMac, controlUnitDeviceMac, 6);

    // Initialize WiFi in station mode
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    // Initialize ESPNow for communication
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESPNow initialization failed!");
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




void ESPNowControlUnitSyncManager::onDataReceivedCallback(const uint8_t* mac, const uint8_t* data, int len) {
    Serial.print("Received data: ");
    int min_len = min(len ,MAX_SIZE);
    memccpy(buffer , data , min_len);
    for(int i=0 ; i<min_len ; i++)
    {
        commands.push(static_cast<ControlUnitCommand>(buffer[i]));
    }
}


ControlUnitCommand ESPNowControlUnitSyncManager::getNextCommand()
{
    if(commands.empty())
    {
        return STOP_SAMPLING;
    }
    ControlUnitCommand cmd = commands.front();
    commands.pop();
    return cmd;
}


void ESPNowControlUnitSyncManager::reportStatus(UnitManagerStatus status) {

    uint8_t statusData[1] = {static_cast<uint8_t>(status)};
    sendData(statusData, sizeof(statusData));
}

