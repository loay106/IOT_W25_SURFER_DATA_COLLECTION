#include "SurfboardMainUnit.h"

// constants
uint8_t SDCardChipSelectPin = 5;

int serialBaudRate = 115200;

int RGBRedPin = 25;
int RGBGreenPin = 26;
int RGBBluePin = 27;

int buttonPin = 4;

int MAX_RECOVERY_TRIES = 3;


// globals
SurfboardMainUnit* mainUnit;
Logger* logger;
RGBStatusHandler* statusLighthandler;
int errorRecoveryTries = 0;

uint8_t samplingUnitsMacAddresses[2][6] =  {
  //  {0xCC, 0xDB, 0xA7, 0x5A, 0x7F, 0xC0}, // Loay's esp testing board
    {0xA8, 0x42, 0xE3, 0x45, 0x94, 0x68}, // Mousa's esp board
    {0x0C, 0xB8, 0x15, 0x77, 0x84, 0x64} // Shada's esp board
};

void setup() {
    logger = Logger::getInstance();
    logger->init(serialBaudRate);
    logger->setLogLevel(LogLevel::DEBUG);

    logger->info("System init starting...");

    statusLighthandler = RGBStatusHandler::getInstance();
    try{
        statusLighthandler->init(RGBRedPin, RGBGreenPin, RGBBluePin);
    }catch(...){
        logger->error("Failed to start LED! Check your wiring!");
        while(true){delay(500);};
    }

    SDCardHandler* sdCardHandler = new SDCardHandler(SDCardChipSelectPin, logger);
    try{
        sdCardHandler->init();
    }catch(InitError& err){
        logger->error("SD Card init error! Check your SD card wiring!");
        while(true){delay(500);};
    }

    string WIFI_SSID = "";
    string WIFI_PASSWORD = "";
    vector<string> sensorsParams;
    int WIFI_ESP_NOW_CHANNEL = 0;

    try{
        std::map<string, string> configMap = sdCardHandler->readConfigFile("//unit.config");
        WIFI_SSID = configMap["WIFI_SSID"];
        WIFI_PASSWORD = configMap["WIFI_PASSWORD"];
        sensorsParams = parseSensorParams(configMap["SENSORS_PARAMS"]);
    }catch(exception& e){
        logger->error("Error reading config file!");
        while(true){delay(500);};
    }

    WifiHandler* wifiHandler = new WifiHandler(WIFI_SSID, WIFI_PASSWORD);
    try{
        logger->info("Checking wifi connection...");
        wifiHandler->init();
        wifiHandler->connect();
        logger->info("Wifi connection established!");
        WIFI_ESP_NOW_CHANNEL = wifiHandler->getChannel();
        logger->info("Setting ESP Now channel to " + to_string(WIFI_ESP_NOW_CHANNEL));
        wifiHandler->disconnect();
        logger->info("Wifi disconnected!");
    }catch(...){
          logger->error("Wifi connection failed! Check your ssid and password!");
          while(true){delay(500);};
    }


    ControlUnitSyncManager* syncManager = ControlUnitSyncManager::getInstance();
    RTCTimeHandler* timeHandler = new RTCTimeHandler(logger);
    ButtonHandler* buttonHandler = new ButtonHandler(logger, buttonPin);

    CloudSyncManager* cloudSyncManager = new CloudSyncManager(logger, wifiHandler, wifiHandler->getMacAddress());
    Sampler* sampler = new Sampler(logger, sdCardHandler, cloudSyncManager);
    mainUnit = new SurfboardMainUnit(syncManager, timeHandler, statusLighthandler, buttonHandler, logger, sampler, sdCardHandler);

    // declare sensors here....
    Force_FAKE* fake_force_0 = new Force_FAKE(logger,sdCardHandler);
    Force_FAKE* fake_force_1 = new Force_FAKE(logger,sdCardHandler);
    Force_FAKE* fake_force_2 = new Force_FAKE(logger,sdCardHandler);

    try{
        // don't change the order of the init
        syncManager->init(samplingUnitsMacAddresses, 2, WIFI_ESP_NOW_CHANNEL);
        timeHandler->init();
        buttonHandler->init();
        cloudSyncManager->init();
        sampler->init();
        mainUnit->init(samplingUnitsMacAddresses, 2);

        // init sensors here..
        // you can pass params from the config file
        // sensor[i] should have the param in sensorsParams[i]
        fake_force_0->init();
        fake_force_1->init();
        fake_force_2->init();
    }catch(InitError& err){
        logger->error("Init error! Check your wiring!");
        while(true){delay(500);};
    }

    // add sensors here....
    mainUnit->addSensor(fake_force_0);
    mainUnit->addSensor(fake_force_1);
    mainUnit->addSensor(fake_force_2);

    try{
        syncManager->connect();
    }catch(ESPNowSyncError& err){
        while(true){delay(500);};
    }
    logger->info("System init complete!");
}

void loop() {
    try{  
       // logger->info("LOOPING");
        mainUnit->handleButtonPress();
        mainUnit->readStatusUpdateMessages();
        mainUnit->loopDiscoverDisconnected();
        SystemStatus status = mainUnit->getStatus();
        switch(status){
          case SystemStatus::SYSTEM_STAND_BY:
          case SystemStatus::SYSTEM_STARTING:
            mainUnit->loopStandby();
            delay(10);
            break;
          case SystemStatus::SYSTEM_SAMPLING:
          case SystemStatus::SYSTEM_SAMPLING_PARTIAL_ERROR:
            mainUnit->loopSampling();
            delay(3);
            break;
          case SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD:
          case SystemStatus::SYSTEM_SAMPLE_FILE_UPLOAD_PARTIAL_ERROR:
            mainUnit->loopFileUpload();
            delay(3);
            break;
          case SystemStatus::SYSTEM_ERROR: 
            statusLighthandler->updateColors(RGBColors::RED, RGBColors::RED);
            delay(500);
            break;
        }
    }catch(...){
        logger->error("An unexpected error occured! Trying to recover...");
        statusLighthandler->updateColors(RGBColors::RED, RGBColors::NO_COLOR);
        delay(200); // give some time for the system to recover
    }
}

