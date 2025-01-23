#include <IOT_TECHNION_SURFBOARD.h>

ButtonHandler* buttonHandler = nullptr;
Logger* logger = nullptr;

int buttonPin = 4;


void setup() {
    logger = Logger::getInstance();
    logger->init(57600);
    buttonHandler = new ButtonHandler(logger, buttonPin);
    buttonHandler->init();
}

void loop() {
    ButtonPressType press = buttonHandler->getLastPressType();
    if (press == ButtonPressType::SOFT_PRESS) {
        logger->info("Button was soft pressed!");
    }else if (press == ButtonPressType::LONG_PRESS) {
        logger->info("Button was long pressed!");
    }
    delay(10);
}
