#include "Sampler.h"

Sampler::Sampler(Logger logger, SDCardHandler sdCardHandler): logger(logger), sdCardHandler(sdCardHandler), status(SamplerStatus::UNIT_STAND_BY){}

void Sampler::addSensor(SensorBase *sensor){
    sensor->init();
    sensors.push_back(sensor);
}

void Sampler::init(){
    try{
        sdCardHandler.createFolder("samplings");
    }catch(SDCardError& err){
        logger.error("Failed to create samplings folder");
        status = SamplerStatus::UNIT_ERROR;
        throw InitError();
    }
}

void Sampler::startSampling(int timestamp, int IMURate){
    status = SamplerStatus::UNIT_SAMPLING;
    for(int i=0;i<sensors.size(); i++){
        // sample files have this format:
        // [TIMESTAMP]_[SENSOR_ID]_[SENSOR_MODEL]
        string filePath = "samplings/" + to_string(timestamp) + "_" + to_string(i) + "_" + sensors[i]->getModel();
        sensors[i]->startSampling(filePath, IMURate);
    }
}

void Sampler::stopSampling(){
    for(SensorBase* sensor: sensors){
        sensor->stopSampling();
    }
    status = SamplerStatus::UNIT_STAND_BY;
}

SamplerStatus Sampler::getStatus(){
    return status;
}

void Sampler::enterErrorState(){
    status = SamplerStatus::UNIT_ERROR;
}

void Sampler::writeSensorsData(){
    for(int i= 0; i< Sampler::sensors.size(); i++){
        sensors[i]->writeSamples();
    }
}
