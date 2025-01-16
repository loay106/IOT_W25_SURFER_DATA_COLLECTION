#ifndef IMU_BNO080_H
#define IMU_BNO080_H

#include "SensorBase.h"
#include "SparkFun_BNO080_Arduino_Library.h"

class IMU_BNO080: public SensorBase {
    private:
        BNO080 sensor;
    public:
        IMU_BNO080(Logger* logger, SDCardHandler sdcardHandler): SensorBase(logger, sdcardHandler, "IMU_BNO080") {}
        void enableSensor(int IMURate) override{
            sensor.enableAccelerometer(IMURate);
        }

        void disableSensor() override{
            sensor.enableAccelerometer(0);
        }

        void init() override{
            Wire.begin(21, 22); // SDA, SCL for ESP32
            if (sensor.begin() == false) {
                logger->error("BNO080 not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
                throw InitError();
            }
            Wire.setClock(400000);
        }

        string getSample() override{
            if (sensor.dataAvailable()) {
                float accX = sensor.getAccelX();
                float accY = sensor.getAccelY();
                float accZ = sensor.getAccelZ();

                // Create a string to store the data
                String dataString = String(accX, 2) + " " +
                                    String(accY, 2) + " " +
                                    String(accZ, 2);
                Serial.println(dataString);
                return dataString.c_str();
            }else{
                throw NotReadyError();
            }
        }

        void updateSensor(void* param){};
};

#endif // IMU_BNO080_H