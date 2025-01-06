#include "IMU_BNO080.h"

IMU_BNO080::IMU_BNO080(SamplingModes mode, int samplingRate): IMUBase(SupportedIMUModels::BNO0800,mode,samplingRate){

}
void IMU_BNO080::setup(){
    Wire.begin(21, 22); // SDA, SCL for ESP32
    if (sensor.begin() == false) {
        Serial.println("BNO080 not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
        status = IMUStatus::ERROR;
    }
    Wire.setClock(400000);
    Serial.println("IMU setup complete");
}
void IMU_BNO080::enableSensor(){
    sensorEnabled = true;
    if(mode == SamplingModes::ACCELEROMETER){
        sensor.enableAccelerometer(samplingRate);
        Serial.println("accelerometer enabled");

    }
    else if(mode == SamplingModes::ROTATION_VECTOR){
        sensor.enableRotationVector(samplingRate);
        Serial.println("rotation vector enabled");

    }
    else{
        status = IMUStatus::ERROR;
        Serial.println("sensor enabling failed");
    }
}


std::string IMU_BNO080::getSample(){
    if(mode == SamplingModes::ACCELEROMETER){
        if (sensor.dataAvailable()) {
            status = IMUStatus::SAMPLING;
            float accX = sensor.getAccelX();
            float accY = sensor.getAccelY();
            float accZ = sensor.getAccelZ();

            // Create a string to store the data
            String dataString = String(accX, 2) + "," +
                                String(accY, 2) + "," +
                                String(accZ, 2);
            Serial.println(dataString);
            return dataString.c_str();
        }
        else{
            Serial.println("IMU data not available");
            return "error";
        }
    }
    else if(mode == SamplingModes::ROTATION_VECTOR){
        if (sensor.dataAvailable()) {
            status = IMUStatus::SAMPLING;
            float quatI = sensor.getQuatI();
            float quatJ = sensor.getQuatJ();
            float quatK = sensor.getQuatK();
            float quatReal = sensor.getQuatReal();
            float quatRadianAccuracy = sensor.getQuatRadianAccuracy();

            // Create a string to store the data
            String dataString = String(quatI, 2) + "," +
                                String(quatJ, 2) + "," +
                                String(quatK, 2) + "," +
                                String(quatReal, 2) + "," +
                                String(quatRadianAccuracy, 2);
            Serial.println(dataString);
            return dataString.c_str();
        }
        else{
            Serial.println("IMU data not available");
            return "error";
        }
    }
    else{
        status = IMUStatus::ERROR;
        Serial.println("mode unsupported");
        return "error";
    }
}