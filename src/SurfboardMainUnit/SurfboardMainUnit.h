#ifndef SURFBOARD_MAIN_UNIT_H
#define SURFBOARD_MAIN_UNIT_H

#include <vector>
#include <string>
using namespace std;

#include "../Sampler/Sampler.h"
#include "../Controller.h"

// todo: merge Controller to this class and remove Controller...
// add is_internal to SamplingUnitRep

class SurfboardMainUnit {
    private:
        Sampler sampler;
        Controller controller;
        Logger logger;
    public:
        SurfboardMainUnit(){};
        SurfboardMainUnit(int buttonPin);

        void init(uint8_t samplingUnitsAdresses[][6], int samplingUnitsNum, int RGBRedPin, int RGBGreenPin, int RGBBluePin);

        void addSensor(SensorBase sensor);

};

#endif // SURFBOARD_MAIN_UNIT_H