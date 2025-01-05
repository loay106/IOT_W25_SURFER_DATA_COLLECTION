#include "src/Time/TimeManager.h"

TimeManager timeManager;


void setup() {
    Serial.begin(57600);
    timeManager = TimeManager();
    timeManager.initialize();
    timeManager.adjust(DateTime(F(__DATE__), F(__TIME__)));
}


void loop() {
     int time = timeManager.getCurrentTimestamp();

    //Full Timestamp
    Serial.println(String("DateTime::TIMESTAMP_FULL:\t")+time);

    Serial.println("\n");

    //Delay 5s
    delay(5000);
}

