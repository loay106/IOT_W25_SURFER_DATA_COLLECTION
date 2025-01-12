#include "src/Data/Logger.h"

using namespace std;
#include <string>;

Logger logger;

void setup() {
    logger = Logger(57600);
    logger.initialize();
};


void loop() {
    logger.info("PASSED!");

    //Delay 5s
    delay(5000);
};

