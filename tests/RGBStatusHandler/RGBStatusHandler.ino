#include <IOT_TECHNION_SURFBOARD.h>

RGBStatusHandler* statusLight = nullptr;
Logger* logger = nullptr;

int RGBRedPin = 26;
int RGBGreenPin = 25;
int RGBBluePin = 27;

void setup() {
    logger = Logger::getInstance();
    logger->init(57600);
    statusLight = new RGBStatusHandler(logger);
    statusLight->init(RGBRedPin, RGBGreenPin, RGBBluePin);
    statusLight->updateColors(RGBColors::GREEN, RGBColors::RED);
}

void loop() {
    delay(500);
    statusLight->flicker();
}
