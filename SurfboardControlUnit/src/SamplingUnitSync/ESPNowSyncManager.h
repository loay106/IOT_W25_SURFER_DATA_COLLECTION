#ifndef ESP_NOW_SYNC_MANAGER_H 
#define ESP_NOW_SYNC_MANAGER_H

using namespace std;
#include <cstdint>
#include <string>
#include <queue>

// NOTE: ESP NOW DOES NOT ENSURE MESSAGES HAS BEEN HANDLED - NO ACK MEACHANISM

class ESPNowSyncManager{
    private:
        queue<SyncMessage> messageQueue;

        SyncMessage decodeMessage(const uint8_t *mac, const uint8_t *data, int len);
        void enqueueMessage(SyncMessage message);

    public:
        ESPNowSyncManager();
        void initialize();
        void sendMessage(SyncMessage message);
        bool hasMessage();
        SyncMessage popMessage();
};

enum MessageSubject{
    STATUS_UPDATE, // "001" - unit's status in messageContent + status of all the sensors?
    SAMPLE_SYNC, // "002" - file name in messageContent
};

struct SyncMessage {
    uint8_t macAddress[6];
    MessageSubject subject; // first 3 bytes
    const uint8_t *messageContent; // rest of the payload
    int messageContentLen; 
};

#endif /* ESP_NOW_SYNC_MANAGER_H */