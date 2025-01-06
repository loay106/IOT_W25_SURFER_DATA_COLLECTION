#include "src/UnitManagement/ControlUnitManager.h"

/*
    Missing for now:
        1. better edge cases handling
        2. Wifi connection + a way to download files wirelessly...
        3. a user interface....
        4. having sampling rates as a parameter...maybe add it to user interface + send rates in start_sampling command?
*/

// constants
uint8_t SDCardChipSelectPin = 5;
int serialBaudRate = 57600;

int RGBRedPin = 26;
int RGBGreenPin = 25;
int RGBBluePin = 27;

int buttonPin = 32;

// globals
ControlUnitManager controlUnit;
uint8_t samplingUnitsMacAddresses[1][6] =  {
    {0x08, 0xB6, 0x1F, 0x33, 0x49, 0xE4} // Shada's esp board
};

void setup() {
    controlUnit = ControlUnitManager(SDCardChipSelectPin, serialBaudRate, RGBRedPin, RGBGreenPin, RGBBluePin, buttonPin); 
    controlUnit.initialize(samplingUnitsMacAddresses, 1);
}

void loop() {
    controlUnit.updateSystem();
    delay(50);
}

