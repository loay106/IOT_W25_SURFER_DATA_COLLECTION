#include "SurfboardSamplingUnit.h"

// constants
int serialBaudRate = 57600;
int SDCardChipSelectPin = 5;
//uint8_t CONTROL_UNIT_MAC[6] = {0xCC, 0xDB, 0xA7, 0x5A, 0x7F, 0xC0};
uint8_t CONTROL_UNIT_MAC[6] = {0x10, 0x06, 0x1C, 0x86, 0x00, 0x18};

int doutPin = 12;
 int sckPin = 13;

// globals
SurfboardSamplingUnit* samplingUnit; 

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
        std::map<string, string> configMap = sdCardHandler->readConfigFile("/unit.config");
        WIFI_SSID = configMap["WIFI_SSID"];
        WIFI_PASSWORD = configMap["WIFI_PASSWORD"];
        sensorsParams = parseSensorParams(configMap["SENSORS_PARAMS"]);
    }catch(exception& e){
        logger->error("Error reading config file! Place unit.config in your sd card top level with the required configs!");
        while(true){delay(500);};
    }


    WifiHandler* wifiHandler = new WifiHandler(WIFI_SSID, WIFI_PASSWORD);
    CloudSyncManager* cloudSyncManager = new CloudSyncManager(logger, wifiHandler, wifiHandler->getMacAddress());
    Sampler* sampler = new Sampler(logger, sdCardHandler, cloudSyncManager);

    SamplingUnitSyncManager* syncManager = SamplingUnitSyncManager::getInstance();
    samplingUnit = new SurfboardSamplingUnit(syncManager,sdCardHandler,sampler, logger);

    // declare sensors here....
    Force_HX711* sensor0 = new Force_HX711(logger,sdCardHandler,stoi(sensorsParams[0]) ,doutPin,sckPin);
    IMU_BNO080* sensor1 = new IMU_BNO080(logger, sdCardHandler,stoi(sensorsParams[1]));
    Force_FAKE* sensor2  = new Force_FAKE(logger,sdCardHandler);

    try{
        // don't change the order of the init
        logger->init(serialBaudRate);
        syncManager->init(CONTROL_UNIT_MAC);
        sampler->init();
        sensor0->init();
        sensor1->init();
        sensor2->init();
        // init sensors here..
        // you can pass params from the config file
        // sensor[i] should have the param in sensorsParams[i]
    }catch(InitError& err){
        while(true){
            // don't proceed, there's a wiring error!
            logger->error("Init error! Check your wiring!");
            delay(100);
        }
        
    }

    // add sensors here....
    //samplingUnit->addSensor(sensor0);
    samplingUnit->addSensor(sensor1);
    //samplingUnit->addSensor(sensor2);
    syncManager->connect();
    logger->info("Unit setup complete!");
}

void loop() {
    samplingUnit->handleNextCommand();
    SamplerStatus status = samplingUnit->getStatus();
    switch(status){
        case UNIT_STAND_BY:
            break;
        case UNIT_ERROR:
            delay(5);
            break;
        case UNIT_SAMPLING:
            samplingUnit->loopSampling();
            delay(2);
            break;
        case UNIT_SAMPLE_FILES_UPLOAD:
            samplingUnit->loopFileUpload();
            delay(10);
            break;
    }
    samplingUnit->reportStatus();
}
