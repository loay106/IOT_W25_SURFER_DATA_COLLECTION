#include <IOT_TECHNION_SURFBOARD.h>

ButtonHandler* buttonHandler = nullptr;
Logger* logger = nullptr;

int buttonPin = 4;


void setup() {
    logger = Logger::getInstance();
    logger->init(57600);
    logger->setLogLevel(LogLevel::DEBUG);
    buttonHandler = new ButtonHandler(logger, buttonPin);
    buttonHandler->init();
}

void loop() {
    buttonHandler->getLastPressType();
    delay(10);
}
