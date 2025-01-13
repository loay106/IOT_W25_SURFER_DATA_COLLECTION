#ifndef IMU_BNO080_H
#define IMU_BNO080_H

#include "SensorBase.h"
#include "SparkFun_BNO080_Arduino_Library.h"

class IMU_BNO080: public SensorBase {
    private:
        BNO080 sensor;
        int samplingRate;
    public:
    // todo: add dout pin for SensorBase param
        IMU_BNO080(Logger logger, SamplesSDCardWriter samplesWriter, string sensorType, int samplingRate): SensorBase(logger, samplesWriter, "IMU_BNO080") {
            this->samplingRate = samplingRate;
        }
        void enableSensor() override{
            sensor.enableAccelerometer(samplingRate);
        }

        void disableSensor() override;

        void init() override{
            Wire.begin(21, 22); // SDA, SCL for ESP32
            if (sensor.begin() == false) {
                logger.error("BNO080 not detected at default I2C address. Check your jumpers and the hookup guide. Freezing...");
                throw InitError();
            }
            Wire.setClock(400000);
        }

        string getSample() override{
            if (sensor.dataAvailable()) {
                status = IMUStatus::SAMPLING;
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
};

#endif // IMU_BNO080_H