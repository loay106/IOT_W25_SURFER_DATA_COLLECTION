
 #include "SparkFun_BNO080_Arduino_Library.h"
 #include "src/Sensors/IMU_BNO080.h"

BNO080 myIMU;

// global objects
IMU_BNO080 sensor_1 = IMU_BNO080(SamplingModes::ACCELEROMETER,50);

void setup() {
    Serial.begin(9600);
    sensor_1.setup();
    sensor_1.enableSensor();
}
void loop(){
    sensor_1.getSample();
    delay(50);
}

