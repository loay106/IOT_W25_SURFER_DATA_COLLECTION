#include "src/Sync/ESPNowSyncManager.h"
#include "src/Data/SamplingDataWriter.h"
#include "src/Data/Logger.h"
#include "src/Utils/Adresses.h"

// globals
ESPNowSyncManager syncManager;
Logger logger;
SamplingDataWriter dataWriter;
string fileName = "";
uint8_t samplingUnitsMacAddresses[1][6] =  {
    {0x08, 0xB6, 0x1F, 0x33, 0x49, 0xE4}
};

void setup() {
    logger = Logger(57600);
    logger.initialize();
    syncManager = ESPNowSyncManager(logger);
    syncManager.initialize(samplingUnitsMacAddresses, 1);
    dataWriter = SamplingDataWriter(5, logger);
    dataWriter.initialize();
    fileName = dataWriter.createSamplingFile(1736083807);

    string data = "SAMPLE_SYNC|1481765933|AccX AccY AccZ|-0.08 -9.93 -0.30";
    ESPNowSyncManager::processReceivedMessages(samplingUnitsMacAddresses[0], (const uint8_t *)data.c_str(), data.length());
    SamplingSyncMessage message = syncManager.popSamplingUpdateMessage();

    dataWriter.writeSamples(fileName, macToString(message.from), message.sensorID, message.samplingData, message.units);
}

void loop() {
}

