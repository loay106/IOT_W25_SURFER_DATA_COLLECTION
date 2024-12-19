#ifndef ESP_NOW_SYNC_MANAGER_H 
#define ESP_NOW_SYNC_MANAGER_H

using namespace std;
#include <cstdint>
#include <string>
#include <queue>

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
            where:
                [SAMPLE_DATA] is the data enclosed in parentheses and seperated by DELIMETER
            example message:
                "SAMPLE_SYNC|1547|(AccX,AccY,AccZ)|(0.23,-9.77,-1.67)|(0.23,-10.77,-1.57)|(1.23,-9.78,-1.57)|(7.23,-9.77,-1.57)"
*/

class ESPNowSyncManager{
    private:
        queue<SyncMessage> messageQueue;

        SyncMessage decodeMessage(const uint8_t *mac, const uint8_t *data, int len);
        void enqueueMessage(SyncMessage message);

    public:
        static const char DELIMETER = '|';

        ESPNowSyncManager();
        void initialize();
        void sendMessage(SyncMessage message);
        bool hasMessage();
        SyncMessage popMessage();
};

enum MessageSubject{
    STATUS_UPDATE, 
    SAMPLE_SYNC, 
};

struct SyncMessage {
    uint8_t macAddress[6];
    MessageSubject subject; // first 3 bytes
    const uint8_t *messageContent; // rest of the payload
    int messageContentLen; 
};

#endif /* ESP_NOW_SYNC_MANAGER_H */