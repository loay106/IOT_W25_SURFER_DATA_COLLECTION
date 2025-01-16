#include "SurfboardSamplingUnit.h"

int SDCardChipSelectPin = 5;

SurfboardSamplingUnit samplingUnit; 
uint8_t CONTROL_UNIT_MAC[6] = {0xCC, 0xDB, 0xA7, 0x5A, 0x7F, 0xC0};

void setup() {
    samplingUnit = SurfboardSamplingUnit(SDCardChipSelectPin);
    samplingUnit.init(CONTROL_UNIT_MAC);

    // add sensors here....
}

void loop() {
    samplingUnit.updateSystem();
    delay(10); // update delay as needed
}
