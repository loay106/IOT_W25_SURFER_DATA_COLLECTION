#include "SurfboardMainUnit.h"

SurfboardMainUnit::SurfboardMainUnit(int buttonPin): logger(logger){
    logger = Logger();
    controller = Controller(logger, buttonPin);
    sampler = Sampler(logger);
}
void SurfboardMainUnit::init(uint8_t samplingUnitsAdresses[][6], int samplingUnitsNum, int RGBRedPin, int RGBGreenPin, int RGBBluePin) {
    logger.init();
    controller.init(samplingUnitsAdresses, samplingUnitsNum, RGBRedPin, RGBGreenPin, RGBBluePin);
}
void SurfboardMainUnit::addSensor(SensorBase sensor) {
    try{
        sampler.addSensor(sensor);
    }catch(InitError& err){
        logger.error("Failed to add sensor");
        // todo: report error status here...
    }
};