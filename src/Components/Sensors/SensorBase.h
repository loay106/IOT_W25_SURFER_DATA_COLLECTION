#ifndef SENSOR_BASE_H
#define SENSOR_BASE_H

#include <Arduino.h>
#include <string>
#include <sstream>

#include "../SDCardHandler.h"
#include "../../Utils/Logger.h"
#include "../../Utils/Exceptions.h"

using namespace std;

enum class SensorStatus{
    STANDBY,
    SAMPLING,
    ERROR,
};


class SensorBase{
    private:
        static int INSTANCE_COUNT = 0;
        SDCardHandler sdcardHandler;
        SemaphoreHandle_t bufferMutex;
        ostringstream sampleBuffer;

        void appendSample(){
            try{
                xSemaphoreTake(bufferMutex, portMAX_DELAY);
                sampleBuffer << getSample() << "|";
                xSemaphoreGive(bufferMutex);
            }catch(NotReadyError& err){
                return;
            }
        }

    protected:
        int id; // This should be incremental
        Logger logger;
        string sensorType;
        int dataPin;
        string* samplingFileName;

    public:
        SensorBase(){};
        SensorBase(Logger logger, SDCardHandler sdcardHandler, string sensorType, int dataPin){
            bufferMutex = xSemaphoreCreateMutex();
            this-> dataPin = dataPin;
            this->logger = logger;
            this->sdcardHandler = sdcardHandler;
            this->sensorType = sensorType;
            samplingFileName = nullptr;
            id = SensorBase::INSTANCE_COUNT;
            SensorBase::INSTANCE_COUNT++;
        };

        int getId(){
            return id;
        }

        void startSampling(int timestamp, string outputFilePath){
            samplingFileName = new string(outputFilePath);
            attachInterrupt(digitalPinToInterrupt(dataPin), this->appendSample, FALLING);
            enableSensor();
        }

        void stopSampling(){
            samplingFileName = nullptr;
            detachInterrupt(dataPin);
            disableSensor();
        }

        void writeSamples(){
            xSemaphoreTake(bufferMutex, portMAX_DELAY);
            string samples = sampleBuffer.str();
            sampleBuffer.clear();
            xSemaphoreGive(bufferMutex);
            sdcardHandler.writeData(*samplingFileName, samples.c_str());
        }
    
        virtual void enableSensor() = 0;
        virtual void disableSensor() = 0;
        virtual string getSample() = 0;
        virtual void init() = 0;
};

#endif // SENSOR_BASE_H