#ifndef SENSOR_BASE_H
#define SENSOR_BASE_H

#include <Arduino.h>
#include <string>
#include <sstream>

#include "../IO/SDCardHandler.h"
#include "../../Utils/Logger.h"
#include "../../Utils/Exceptions.h"

using namespace std;

enum class SensorStatus{
    STANDBY,
    SAMPLING,
    ERROR,
};

const int MAX_SAMPLES_BUFFER_LENGTH = 400;

class SensorBase{
    private:
        SDCardHandler sdcardHandler;
        string* sampleBuffer;

    protected:
        Logger logger;
        string model;
        int dataPin;
        string* samplingFileName;

    public:
        SensorBase(){};
        SensorBase(Logger logger, SDCardHandler sdcardHandler, string model, int dataPin){
            this-> dataPin = dataPin;
            this->logger = logger;
            this->sdcardHandler = sdcardHandler;
            this->model = model;
            samplingFileName = nullptr;
            sampleBuffer = new string("");
        };

        int getDataPin(){
            return dataPin;
        }

        string getModel(){
            return model;
        }

        void startSampling(string outputFilePath, int rate){
            samplingFileName = new string(outputFilePath);
            enableSensor(rate);
        }

        void stopSampling(){
            delete samplingFileName;
            samplingFileName = nullptr;
            disableSensor();
        }

        void writeSamples(){
            if(!samplingFileName){
                logger.error("samplingFileName is empty!");
                return;
            }
            try{
                string sample = getSample();
                sampleBuffer->append(sample);
                if(sampleBuffer->length() >= MAX_SAMPLES_BUFFER_LENGTH){
                    string* temp = sampleBuffer;
                    sampleBuffer = new string();
                    sdcardHandler.writeData(*samplingFileName, temp->c_str());
                    delete temp;
                }else{
                    sampleBuffer->append("|");
                }
            }catch(NotReadyError& err){
                return;
            }
        }
    
        virtual void enableSensor(int rate) = 0;
        virtual void disableSensor() = 0;
        virtual string getSample() = 0;
        virtual void init() = 0;
};

#endif // SENSOR_BASE_H