#include <IOT_TECHNION_SURFBOARD.h>

RGBStatusHandler* statusLight = nullptr;
Logger* logger = nullptr;

int RGBRedPin = 25;
int RGBGreenPin = 26;
int RGBBluePin = 27;

void setup() {
    logger = Logger::getInstance();
    logger->init(57600);
    statusLight = RGBStatusHandler::getInstance();
    statusLight->init(RGBRedPin, RGBGreenPin, RGBBluePin);
    statusLight->updateColors(RGBColors::GREEN, RGBColors::RED);
}

void loop() {
    delay(50);
    statusLight->flicker();
}
