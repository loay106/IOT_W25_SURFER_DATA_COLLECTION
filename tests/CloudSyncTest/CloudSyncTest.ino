#include <IOT_TECHNION_SURFBOARD.h>

CloudSyncManager* cloudSyncManager = nullptr;
Logger* logger = nullptr;
SDCardHandler* sdCardHandler = nullptr;
uint8_t SDCardChipSelectPin = 5;

string WIFIssid = "";
string WIFIpassword = "";


void setup() {
    WifiHandler* wifiHandler = new WifiHandler();
    wifiHandler->init();
    logger = Logger::getInstance();
    logger->init(57600);
    cloudSyncManager = new CloudSyncManager(logger, wifiHandler);
    cloudSyncManager->init();
    cloudSyncManager->connect(WIFIssid, WIFIpassword);

    sdCardHandler = new SDCardHandler(SDCardChipSelectPin, logger);

    vector<string> sampleFiles = sdCardHandler->listFilesInDir("/samplings");
    for(string sampleFilePath: sampleFiles){
        SDCardHandler::SDCardFileReader fileReader = sdCardHandler->readFile(sampleFilePath);
        // todo: get unit mac, sensor id, model etc from here...do this correctly
        String sensorID = "00";
        String timestamp = "123";
        String sensorModel = "model";
        String unitMac = "000";
        cloudSyncManager->uploadSamples(fileReader, timestamp, unitMac, sensorID, sensorModel);
        sdCardHandler->deleteFile(sampleFilePath);
    }
    cloudSyncManager->disconnect();
}

void loop() {}
