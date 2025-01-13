#include "SurfboardMainUnit.h"

// constants
uint8_t SDCardChipSelectPin = 5;

int RGBRedPin = 26;
int RGBGreenPin = 25;
int RGBBluePin = 27;

int buttonPin = 32;

// globals
SurfboardMainUnit mainUnit;
uint8_t samplingUnitsMacAddresses[1][6] =  {
   // {0x08, 0xB6, 0x1F, 0x33, 0x49, 0xE4}, // Shada's esp board
    {0xCC, 0xDB, 0xA7, 0x5A, 0x7F, 0xC0} // Loay's esp testing board
};

void setup() {
    mainUnit = SurfboardMainUnit(buttonPin); 
    mainUnit.init(samplingUnitsMacAddresses, 1, RGBRedPin, RGBGreenPin, RGBBluePin);
}

void loop() {
    mainUnit.updateSystem();
    delay(10);
}

