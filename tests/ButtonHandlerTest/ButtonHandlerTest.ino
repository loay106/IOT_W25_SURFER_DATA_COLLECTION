#include <IOT_TECHNION_SURFBOARD.h>

ButtonHandler* buttonHandler = nullptr;
Logger* logger = nullptr;

int buttonPin = 32;


void setup() {
    logger = Logger::getInstance();
    logger->init(57600);
    buttonHandler = new ButtonHandler(logger, buttonPin);
    buttonHandler->init();
}

void loop() {
    if (buttonHandler->wasPressed()) {
        logger->info("Button was pressed!");
    }
    delay(10);
}
