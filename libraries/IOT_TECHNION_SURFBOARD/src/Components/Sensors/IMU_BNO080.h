#ifndef IMU_BNO080_H
#define IMU_BNO080_H

#include "SensorBase.h"
#include "SparkFun_BNO080_Arduino_Library.h"

class IMU_BNO080: public SensorBase {
    private:
        BNO080 sensor;
        volatile bool dataReady;
        int samplingRate;
    public:
        IMU_BNO080(Logger* logger, SDCardHandler* sdcardHandler, int rate): SensorBase(logger, sdcardHandler, "BNO080") {
            this->dataReady = false;
            this->samplingRate = rate;
        }
        void enableSensor() override{
            sensor.enableAccelerometer(samplingRate);
        }

        void disableSensor() override{
            sensor.enableAccelerometer(0);
            logger->info("BNO080 sensor disabled");
        }

        void init() override{
            Wire.begin(21, 22); // SDA, SCL for ESP32
            if (sensor.begin() == false) {
                logger->error("BNO080 not detected at default I2C address. Check your jumpers and the hookup guide");
                throw InitError();
            }
            Wire.setClock(400000);
        }

        string getSample() override{
            if (dataReady && sensor.dataAvailable()) {
                dataReady=false;
                samples_count++;
                float accX = sensor.getAccelX();
                float accY = sensor.getAccelY();
                float accZ = sensor.getAccelZ();
                ostringstream oss;
                oss.precision(2);
                oss << std::fixed << accX << " " << accY << " " << accZ;
                return oss.str();
            }else{
                throw NotReadyError();
            }
        }
};

#endif // IMU_BNO080_H