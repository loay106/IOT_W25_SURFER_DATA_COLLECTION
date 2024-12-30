#include "src/Time/TimeManager.h"

TimeManager timeManager;


void setup() {
    Serial.begin(57600);
    timeManager = TimeManager();
    timeManager.initialize();
    timeManager.adjust(DateTime(F(__DATE__), F(__TIME__)));
}


void loop() {
     DateTime time = timeManager.getCurrentDatetime();

    //Full Timestamp
    Serial.println(String("DateTime::TIMESTAMP_FULL:\t")+time.timestamp(DateTime::TIMESTAMP_FULL));

    //Date Only
    Serial.println(String("DateTime::TIMESTAMP_DATE:\t")+time.timestamp(DateTime::TIMESTAMP_DATE));

    //Full Timestamp
    Serial.println(String("DateTime::TIMESTAMP_TIME:\t")+time.timestamp(DateTime::TIMESTAMP_TIME));

    Serial.println("\n");

    //Delay 5s
    delay(5000);
}

