#include "SensorBase.h"

SensorBase::SensorBase(Logger* logger, SDCardHandler* sdcardHandler, string model){
    this->logger = logger;
    this->sdcardHandler = sdcardHandler;
    this->model = model;
    samplingFileName = nullptr;
    sampleBuffer = new string("");
    samples_count=0;
}

string SensorBase::getModel()
{
    return model;
}

unsigned long SensorBase::getSamplesCount()
{
    return samples_count;
}

void SensorBase::startSampling(string outputFilePath, int rate)
{
    samplingFileName = new string(outputFilePath);
    enableSensor(rate);
}

void SensorBase::stopSampling()
{
    delete samplingFileName;
    samplingFileName = nullptr;
    disableSensor();
    samples_count=0;
}

void SensorBase::writeSamples()
{
    if(!samplingFileName){
        logger->error("samplingFileName is empty!");
        return;
    }
    try{
        string sample = getSample();
        sampleBuffer->append(sample);
        if(sampleBuffer->length() >= MAX_SAMPLES_BUFFER_LENGTH){
            string* temp = sampleBuffer;
            sampleBuffer = new string();
            sdcardHandler->writeData(*samplingFileName, temp->c_str());
            delete temp;
        }else{
            sampleBuffer->append("|");
        }
    }catch(NotReadyError& err){
        return;
    }
}