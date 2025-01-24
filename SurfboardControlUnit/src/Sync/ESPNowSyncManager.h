#ifndef ESP_NOW_SYNC_MANAGER_H 
#define ESP_NOW_SYNC_MANAGER_H

using namespace std;
#include <cstdint>
#include <string>
#include <queue>
#include <vector>

#include <esp_now.h>
#include <WiFi.h>

#include "../Status/SystemStatus.h"
#include "../Data/Logger.h"

/* 
    NOTE: ESP NOW DOES NOT ENSURE MESSAGES HAVE BEEN HANDLED - NO SYN/ACK MEACHANISM

    This class syncs data and/or sends commands to the sampling units.
    Message parts are seperated by DELIMETER.

    **** MESSAGES ARE LIMITED TO 250 BYTES IN ESP NOW!  ****

    Supported commands and formats:
        1. STATUS_UPDATE: "STATUS_UPDATE"|[SAMPLE_UNIT_STATUS]
            example messages:
                "STATUS_UPDATE|SAMPLING"
                "STATUS_UPDATE|STAND_BY"
                "STATUS_UPDATE|ERROR"
        2. SAMPLE_SYNC: "SAMPLE_SYNC"|[SENSOR_ID]|[SAMPLE_UNITS]|[SAMPLE_DATA]
            example message:
                "SAMPLE_SYNC|1547|AccX AccY AccZ|0.23 -9.77 -1.67|0.23 -10.77 -1.57|1.23 -9.78 -1.57|7.23 -9.77 -1.57"
*/

typedef struct SamplingSyncMessage{
    uint8_t from[6]; 
    string sensorID;
    string units;
    vector<string> samplingData;
} SamplingSyncMessage;

typedef struct StatusUpdateMessage{
    uint8_t from[6]; 
    SamplingUnitStatus status;
} StatusUpdateMessage;

enum MessageSubject{
    STATUS_UPDATE, 
    SAMPLE_SYNC, 
};

enum ControlUnitCommand{
    START_SAMPLING,
    STOP_SAMPLING,
};

class ESPNowSyncManager{
    private:
        static Logger logger;
        static queue<StatusUpdateMessage> statusUpdateQueue;
        static queue<SamplingSyncMessage> samplingSyncQueue; // todo: change to <string>
           
    public:
        static const char DELIMETER = '|';

        ESPNowSyncManager(){};
        ESPNowSyncManager(Logger logger) {
            ESPNowSyncManager::logger = logger;
        };
        void initialize(uint8_t samplingUnits[][6], int samplingUnitsNum);
        void sendCommand(ControlUnitCommand command, uint8_t samplingUnitMac[6]);
        void broadcastCommand(ControlUnitCommand command); 

        bool hasStatusUpdateMessages();
        bool hasSamplingUpdateMessages();

        static void processReceivedMessages(const uint8_t *mac_addr, const uint8_t *incomingData, int len); 

        static addSamplingUpdateMessage(SamplingSyncMessage msg); // todo: add mutex A here
        static addStatusUpdateMessage(StatusUpdateMessage msg); // todo: add mutex B here
        StatusUpdateMessage popStatusUpdateMessage(); // todo: add mutex B here
        SamplingSyncMessage popSamplingUpdateMessage(); // todo: add mutex A here

};


#endif /* ESP_NOW_SYNC_MANAGER_H */