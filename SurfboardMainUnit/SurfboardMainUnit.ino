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
    ControlUnitSyncManager* syncManager = ControlUnitSyncManager::getInstance();
    RTCTimeHandler* timeHandler = new RTCTimeHandler(logger);
    RGBStatusHandler* statusLighthandler = new RGBStatusHandler(logger);
    ButtonHandler* buttonHandler = ButtonHandler::getInstance();
    SDCardHandler* sdCardHandler = new SDCardHandler(SDCardChipSelectPin, logger);
    Sampler* sampler = new Sampler(logger, sdCardHandler);
    mainUnit = new SurfboardMainUnit(syncManager, timeHandler, statusLighthandler, buttonHandler, logger, sampler, sdCardHandler);

    // declare sensors here....
    // IMU_BNO080* sensor0 = new IMU_BNO080(logger, sdCardHandler);
    try{
        // don't change the order of the init
        logger->init(serialBaudRate);
        syncManager->init(samplingUnitsMacAddresses, 1);
        timeHandler->init();
        statusLighthandler->init(RGBRedPin, RGBGreenPin, RGBBluePin);
        buttonHandler->init(buttonPin);
        sdCardHandler->init();
        sampler->init();
        mainUnit->init(samplingUnitsMacAddresses, 1);

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
    //mainUnit->addSensor(sensor0);
}

void loop() {
    mainUnit->updateSystem();
    delay(5); // update as needed
}

