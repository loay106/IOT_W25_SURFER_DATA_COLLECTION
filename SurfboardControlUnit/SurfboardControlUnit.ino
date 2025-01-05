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
        8. having sampling rates as a parameter...maybe add it to user interface + send rates in start_sampling command?
*/

// constants
uint8_t SDCardChipSelectPin = 5;
int serialBaudRate = 57600;

int RGBRedPin = 26;
int RGBGreenPin = 25;
int RGBBluePin = 27;

// globals
ControlUnitManager controlUnit;
uint8_t samplingUnitsMacAddresses[1][6] =  {
    {0x08, 0xB6, 0x1F, 0x33, 0x49, 0xE4} // Shada's esp board
};

void setup() {
    controlUnit = ControlUnitManager(SDCardChipSelectPin, serialBaudRate, RGBRedPin, RGBGreenPin, RGBBluePin); 
    controlUnit.initialize(samplingUnitsMacAddresses, sizeof(samplingUnitsMacAddresses)/sizeof(uint8_t));
    controlUnit.startSampling(); 
}

void loop() {
    controlUnit.updateSystem();
    delay(50);
}

