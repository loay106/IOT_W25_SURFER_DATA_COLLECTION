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
}

void loop() {
    statusLight->updateColors(RGBColors::GREEN, RGBColors::NO_COLOR);
    delay(3000);
    statusLight->updateColors(RGBColors::GREEN, RGBColors::RED);
    delay(3000);
    statusLight->updateColors(RGBColors::CYAN, RGBColors::NO_COLOR);
    delay(3000);
    statusLight->updateColors(RGBColors::CYAN, RGBColors::RED);
    delay(3000);
    //statusLight->flicker();
}
