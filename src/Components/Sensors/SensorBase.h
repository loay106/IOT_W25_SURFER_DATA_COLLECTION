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
        Logger logger;
        string model;
        int dataPin;
        string* samplingFileName;

    public:
        SensorBase(){};
        SensorBase(Logger logger, SDCardHandler sdcardHandler, string model, int dataPin){
            bufferMutex = xSemaphoreCreateMutex();
            this-> dataPin = dataPin;
            this->logger = logger;
            this->sdcardHandler = sdcardHandler;
            this->model = model;
            samplingFileName = nullptr;
        };

        string getModel(){
            return model;
        }

        void startSampling(string outputFilePath, int rate){
            samplingFileName = new string(outputFilePath);
            attachInterrupt(digitalPinToInterrupt(dataPin), this->appendSample, FALLING);
            enableSensor(rate);
        }

        void stopSampling(){
            delete samplingFileName;
            samplingFileName = nullptr;
            detachInterrupt(dataPin);
            disableSensor();
        }

        void writeSamples(){
            if(!samplingFileName){
                logger.error("samplingFileName is empty!");
                return;
            }
            xSemaphoreTake(bufferMutex, portMAX_DELAY);
            string samples = sampleBuffer.str();
            sampleBuffer.str(""); // clear the buffer
            sampleBuffer.clear(); // reset state flags
            xSemaphoreGive(bufferMutex);
            sdcardHandler.writeData(*samplingFileName, samples.c_str());
        }
    
        virtual void enableSensor(int rate) = 0;
        virtual void disableSensor() = 0;
        virtual string getSample() = 0;
        virtual void init() = 0;
};

#endif // SENSOR_BASE_H