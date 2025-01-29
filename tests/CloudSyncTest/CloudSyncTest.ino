#include <IOT_TECHNION_SURFBOARD.h>

// constants
int serialBaudRate = 115200;
int SDCardChipSelectPin = 5;
uint8_t CONTROL_UNIT_MAC[6] = {0xCC, 0xDB, 0xA7, 0x5A, 0x7F, 0xC0};
String macAddress = "CC:DB:A7:5A:7F:C0";
const char* ssid = "your ssid";
const char* password = "your password";

// globals

void setup() {
    Logger* logger = Logger::getInstance();
    SamplingUnitSyncManager* syncManager = SamplingUnitSyncManager::getInstance();
    SDCardHandler* sdCardHandler = new SDCardHandler(SDCardChipSelectPin, logger);
    WifiHandler* wifiHandler = new WifiHandler();
    CloudSyncManager* cloudSyncManager = new CloudSyncManager(logger,wifiHandler,macAddress);
    Sampler* sampler = new Sampler(logger, sdCardHandler, cloudSyncManager);


    logger->init(serialBaudRate);
    syncManager->init(CONTROL_UNIT_MAC);
    wifiHandler->init();
    cloudSyncManager->init();
    sdCardHandler->init();
    sampler->init();
    sampler->uploadSampleFiles(ssid,password);

}

void loop() {}


//#include <IOT_TECHNION_SURFBOARD.h>
//
//CloudSyncManager* cloudSyncManager = nullptr;
//Logger* logger = nullptr;
//SDCardHandler* sdCardHandler = nullptr;
//uint8_t SDCardChipSelectPin = 5;
//
//string WIFIssid = "";
//string WIFIpassword = "";
//
//
//void setup() {
//    WifiHandler* wifiHandler = new WifiHandler();
//    wifiHandler->init();
//    logger = Logger::getInstance();
//    logger->init(57600);
//    cloudSyncManager = new CloudSyncManager(logger, wifiHandler);
//    cloudSyncManager->init();
//    cloudSyncManager->connect(WIFIssid, WIFIpassword);
//
//    sdCardHandler = new SDCardHandler(SDCardChipSelectPin, logger);
//
//    vector<string> sampleFiles = sdCardHandler->listFilesInDir("/samplings");
//    for(string sampleFilePath: sampleFiles){
//        SDCardHandler::SDCardFileReader fileReader = sdCardHandler->readFile(sampleFilePath);
//        // todo: get unit mac, sensor id, model etc from here...do this correctly
//        String sensorID = "00";
//        String timestamp = "123";
//        String sensorModel = "model";
//        String unitMac = "000";
//        cloudSyncManager->uploadSamples(fileReader, timestamp, unitMac, sensorID, sensorModel);
//        sdCardHandler->deleteFile(sampleFilePath);
//    }
//    cloudSyncManager->disconnect();
//}
//
//void loop() {}
