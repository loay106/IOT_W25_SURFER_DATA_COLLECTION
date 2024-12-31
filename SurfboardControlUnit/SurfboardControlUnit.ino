#include "src/UnitManagement/ControlUnitManager.h"

/*
    Missing for now:
        1. start/stop button for sampling
        2. better edge cases handling
        3. testing the code
        4. status light
        5. Wifi connection
        6. a way to download files wirelessly...
        7. a user interface....
*/

// constants
const uint8_t SDCardChipSelectPin = 5;
const int serialBaudRate = 57600;

// globals
ControlUnitManager controlUnit;
uint8_t samplingUnitsMacAddress[3][6] =  {
    {0xC8, 0xC9, 0xA3, 0xCA, 0x22, 0x70},
    {0xC8, 0xC9, 0xA3, 0xC6, 0xFE, 0x54},
    {0x94, 0xE6, 0x86, 0x0D, 0x7B, 0x80}
};

void setup() {
    controlUnit = ControlUnitManager(SDCardChipSelectPin, serialBaudRate);
    controlUnit.initialize(samplingUnitsMacAddress, 3);
}

void loop() {
    controlUnit.updateSystem();
    delay(50);
}

