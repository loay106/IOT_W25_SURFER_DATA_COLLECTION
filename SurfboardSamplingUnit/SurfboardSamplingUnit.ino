#include "SurfboardSamplingUnit.h"

// constants
int serialBaudRate = 57600;
int SDCardChipSelectPin = 5;
uint8_t CONTROL_UNIT_MAC[6] = {0xCC, 0xDB, 0xA7, 0x5A, 0x7F, 0xC0};

// globals
SurfboardSamplingUnit* samplingUnit; 

void setup() {
    Logger* logger = Logger::getInstance();
    SamplingUnitSyncManager* syncManager = SamplingUnitSyncManager::getInstance();
    SDCardHandler* sdCardHandler = new SDCardHandler(SDCardChipSelectPin, logger);
    Sampler* sampler = new Sampler(logger, sdCardHandler);
    samplingUnit = new SurfboardSamplingUnit(syncManager,sdCardHandler,sampler, logger);

    // declare sensors here....
    // IMU_BNO080* sensor0 = new IMU_BNO080(logger, sdCardHandler);

    try{
        // don't change the order of the init
        logger->init(serialBaudRate);
        syncManager->init(CONTROL_UNIT_MAC);
        sdCardHandler->init();
        sampler->init();

        // init sensors here..
        // sensor0.init();
    }catch(InitError& err){
        while(true){
            // don't proceed, there's a wiring error!
            logger->error("Init error! Check your wiring!");
            delay(100);
        }
        
    }

    // add sensors here....
    //samplingUnit->addSensor(sensor0);
}

void loop() {
    samplingUnit->updateSystem();
    delay(5); // update delay as needed
}
