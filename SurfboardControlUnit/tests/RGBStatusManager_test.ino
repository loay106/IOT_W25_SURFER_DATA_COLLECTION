#include "src/Status/RGBStatusManager.h"
#include "src/Data/Logger.h"

// globals
RGBStatusManager statusManager;
Logger logger;

// constants
const int redPin = 26;    // GPIO pin for Red
const int greenPin = 25;  // GPIO pin for Green
const int bluePin = 27;   // GPIO pin for Blue

void setup() {
    logger = Logger(57600);
    logger.initialize();
    statusManager = RGBStatusManager(logger, redPin, greenPin, bluePin);
    statusManager.initialize(SystemStatus::SYSTEM_SAMPLING);
}

void loop() {
    statusManager.updateStatus(SystemStatus::SYSTEM_STAND_BY);
    delay(2000);

    statusManager.updateStatus(SystemStatus::SYSTEM_SAMPLING);
    delay(2000);

    statusManager.updateStatus(SystemStatus::SYSTEM_ERROR);
    delay(2000);
}

