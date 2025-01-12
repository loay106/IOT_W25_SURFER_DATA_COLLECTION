#ifndef CONTROLLER_SYNC_MANAGER_H
#define CONTROLLER_SYNC_MANAGER_H

#include <cstdint>
#include <WiFi.h>
#include <esp_now.h>
#include <string>
#include <cstring>
#include <vector>
#include <queue>
#include <Arduino.h>


using namespace std;
#define MAX_SIZE 250

enum ControlUnitCommand{
    START_SAMPLING, // send timestamp here
    STOP_SAMPLING,
    NO_COMMAND
};

enum class UnitManagerStatus{
    STANDBY,
    SAMPLING,
    ERROR,
};


class ESPNowControllerSyncManager {
    private:
        uint8_t buffer[MAX_SIZE];
        uint8_t controlUnitMac[6];
    public:
        queue<std::string> commands;
        void sendData(const uint8_t *data, size_t len);
        static ESPNowControllerSyncManager* instance;
        void initialize(uint8_t controlUnitDeviceMac[]);
        ESPNowControllerSyncManager(){};
        void reportStatus(UnitManagerStatus status);
        void sendSamples(const string sample , const string pattern , const string id);
        ControlUnitCommand getNextCommand();
        static void onDataReceivedCallback(const uint8_t* mac, const uint8_t* data, int len);
        void onDataReceived(const uint8_t* mac, const uint8_t* data, int len);

};

#endif // CONTROLLER_SYNC_MANAGER_H
