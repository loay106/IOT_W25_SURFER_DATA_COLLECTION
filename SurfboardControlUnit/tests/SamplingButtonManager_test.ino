#include "src/IO/SamplingButtonManager.h"
#include "src/Data/Logger.h"

// globals
SamplingButtonManager buttonManager;
Logger logger;

// constants
const int buttonPin = 32; 

void setup() {
    logger = Logger(57600);
    logger.initialize();
    buttonManager = SamplingButtonManager(logger, buttonPin);
    buttonManager.initialize();
}

void loop() {
    if(buttonManager.wasPressed()){
        logger.info("BUTTON WAS PRESSED!");
        buttonManager.consumePress();
    }
    delay(2000);
}

