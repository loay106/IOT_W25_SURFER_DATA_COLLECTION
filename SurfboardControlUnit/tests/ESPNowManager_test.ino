#include "src/Sync/ESPNowSyncManager.h"
#include "src/Data/Logger.h"
#include "src/Utils/Adresses.h"

// globals
ESPNowSyncManager syncManager;
Logger logger;
uint8_t samplingUnitsMacAddresses[1][6] =  {
    {0x08, 0xB6, 0x1F, 0x33, 0x49, 0xE4}
};

void setup() {
    logger = Logger(57600);
    logger.initialize();
    syncManager = ESPNowSyncManager(logger);
    syncManager.initialize(samplingUnitsMacAddresses, 1);
}

void loop() {
    syncManager.broadcastCommand(ControlUnitCommand::START_SAMPLING);
    if(syncManager.hasStatusUpdateMessages()){
        StatusUpdateMessage updateMessage = syncManager.popStatusUpdateMessage();
        logger.info("status update received from: "+ macToString(updateMessage.from));
    }
    delay(500);
}

