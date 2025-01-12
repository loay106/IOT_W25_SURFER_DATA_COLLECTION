#include "SurfboardSamplingUnit.h"

SurfboardSamplingUnit samplingUnit; 

void setup() {
    samplingUnit = SurfboardSamplingUnit(); // todo: add params....
    samplingUnit.init();

    // todo: add sensors here....
}

void loop() {
    samplingUnit.updateSystem();
    delay(5); // is this needed?
}
