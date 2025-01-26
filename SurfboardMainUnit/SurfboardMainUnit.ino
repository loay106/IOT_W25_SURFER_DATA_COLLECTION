#include "SurfboardMainUnit.h"

// constants
uint8_t SDCardChipSelectPin = 5;
int serialBaudRate = 57600;
int RGBRedPin = 26;
int RGBGreenPin = 25;
int RGBBluePin = 27;

int buttonPin = 32;

// globals
SurfboardMainUnit* mainUnit;

uint8_t samplingUnitsMacAddresses[1][6] =  {
   // {0x08, 0xB6, 0x1F, 0x33, 0x49, 0xE4}, // Shada's esp board
    {0xCC, 0xDB, 0xA7, 0x5A, 0x7F, 0xC0} // Loay's esp testing board
};

void setup() {
    Logger* logger = Logger::getInstance();
    logger->init(serialBaudRate);
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

    ControlUnitSyncManager* syncManager = ControlUnitSyncManager::getInstance();
    RTCTimeHandler* timeHandler = new RTCTimeHandler(logger);
    RGBStatusHandler* statusLighthandler = new RGBStatusHandler(logger);
    ButtonHandler* buttonHandler = new ButtonHandler(logger, buttonPin);
    WifiHandler* wifiHandler = new WifiHandler();
    CloudSyncManager* cloudSyncManager = new CloudSyncManager(logger, wifiHandler);
    Sampler* sampler = new Sampler(logger, sdCardHandler, cloudSyncManager, WIFI_SSID, WIFI_PASSWORD);
    mainUnit = new SurfboardMainUnit(syncManager, timeHandler, statusLighthandler, buttonHandler, logger, sampler, sdCardHandler);

    // declare sensors here....
    Force_FAKE* fake_force_0 = new Force_FAKE(logger,sdCardHandler);
    Force_FAKE* fake_force_1 = new Force_FAKE(logger,sdCardHandler);
    Force_FAKE* fake_force_2 = new Force_FAKE(logger,sdCardHandler);

    try{
        // don't change the order of the init
        syncManager->init(samplingUnitsMacAddresses, 1);
        timeHandler->init();
        statusLighthandler->init(RGBRedPin, RGBGreenPin, RGBBluePin);
        buttonHandler->init();
        wifiHandler->init();
        cloudSyncManager->init();
        sampler->init();
        mainUnit->init(samplingUnitsMacAddresses, 1);

        // init sensors here..
        // you can pass params from the config file - sensor[i] should have param sensorsParams[i]
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
}

void loop() {
    mainUnit->updateSystem();
    delay(5); // update as needed
}

