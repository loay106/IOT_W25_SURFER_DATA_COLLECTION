#include "src/UnitManagement/ControlUnitManager.h"

/*
    Missing for now:
        1. Wifi connection + user interface
        2. having sampling rates as a parameter...maybe add it to user interface + send rates in start_sampling command?
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
   // {0x08, 0xB6, 0x1F, 0x33, 0x49, 0xE4}, // Shada's esp board
    {0xCC, 0xDB, 0xA7, 0x5A, 0x7F, 0xC0} // Loay's esp testing board
};

void setup() {
    controlUnit = ControlUnitManager(SDCardChipSelectPin, serialBaudRate, buttonPin); 
    controlUnit.initialize(samplingUnitsMacAddresses, 1, RGBRedPin, RGBGreenPin, RGBBluePin);
}

void loop() {
    controlUnit.updateSystem();
    delay(50);
}

