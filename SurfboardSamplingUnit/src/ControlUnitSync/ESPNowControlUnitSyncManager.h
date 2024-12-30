#include <cstdint>
//#include <ESPNow.h>
#include <WiFi.h>
#include <string>
#include <cstring>
#include <vector>
#include <queue>
#include <Arduino.h>

#ifndef CONTROL_UNIT_SYNC_MANAGER_H
#define CONTROL_UNIT_SYNC_MANAGER_H

using namespace std;

#define MAX_SIZE 250

class ESPNowControlUnitSyncManager {
    private:
        uint8_t buffer[MAX_SIZE];
        uint8_t controlUnitMac[6];
        void sendData(const uint8_t *data, size_t len);
        queue<ControlUnitCommand> commands;
        // todo: handle messaging between this device and the control unit...
        // save in a buffer, or an array etc....
        // different messages have differnet formats and data! handle this too!
    public:
        ESPNowControlUnitSyncManager(uint8_t controlUnitDeviceMac[]);
        void reportStatus(UnitManagerStatus status);
        void sendSamples(const string sample , const string pattern , const string id);
        ControlUnitCommand getNextCommand();
        static void onDataReceivedCallback(const uint8_t* mac, const uint8_t* data, int len);
};


#endif // CONTROL_UNIT_SYNC_MANAGER_H
