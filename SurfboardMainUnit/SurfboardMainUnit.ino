#include "SurfboardMainUnit.h"

// constants
uint8_t SDCardChipSelectPin = 5;
int serialBaudRate = 57600;
int RGBRedPin = 25;
int RGBGreenPin = 26;
int RGBBluePin = 27;

int buttonPin = 4;

// globals
SurfboardMainUnit* mainUnit;

uint8_t samplingUnitsMacAddresses[1][6] =  {
   // {0x08, 0xB6, 0x1F, 0x33, 0x49, 0xE4}, // Shada's esp board
    {0xCC, 0xDB, 0xA7, 0x5A, 0x7F, 0xC0} // Loay's esp testing board
};

void setup() {
    Logger* logger = Logger::getInstance();
    logger->init(serialBaudRate);
    logger->setLogLevel(LogLevel::DEBUG);
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

    try{
        std::map<string, string> configMap = sdCardHandler->readConfigFile("//main_unit.config");
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
        wifiHandler->connect();
        logger->info("Wifi connection established!");
    }catch(WifiError& err){
          logger->error("Wifi connection failed! Check your ssid and password!");
          while(true){delay(500);};
    }

    int WIFI_ESP_NOW_CHANNEL = wifiHandler->getChannel();

    ControlUnitSyncManager* syncManager = ControlUnitSyncManager::getInstance();
    RTCTimeHandler* timeHandler = new RTCTimeHandler(logger);
    RGBStatusHandler* statusLighthandler = new RGBStatusHandler(logger);
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
        syncManager->init(samplingUnitsMacAddresses, 0);
        timeHandler->init();
        statusLighthandler->init(RGBRedPin, RGBGreenPin, RGBBluePin);
        buttonHandler->init();
        wifiHandler->init();
        cloudSyncManager->init();
        sampler->init();
        mainUnit->init(samplingUnitsMacAddresses, 0);

        // init sensors here..
        // you can pass params from the config file
        // sensor[i] should have the param in sensorsParams[i]
        fake_force_0->init();
        fake_force_1->init();
        fake_force_2->init();
    }catch(InitError& err){
        while(true){
            // don't proceed, there's a wiring error!
            logger->error("Init error! Check your wiring!");
            while(true){delay(500);};
        }
        
    }



    // add sensors here....
    mainUnit->addSensor(fake_force_0);
    mainUnit->addSensor(fake_force_1);
    mainUnit->addSensor(fake_force_2);
    logger->info("Unit setup complete!");
}

void loop() {
    mainUnit->loopStatusLight();
    mainUnit->handleButtonPress();
    mainUnit->readStatusUpdateMessages();
    SystemStatus status = mainUnit->getStatus();
    switch(status){
      case SystemStatus::SYSTEM_STAND_BY:
      case SystemStatus::SYSTEM_STARTING:
      case SystemStatus::SYSTEM_ERROR: 
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
        delay(10);
        break;
    }
}

