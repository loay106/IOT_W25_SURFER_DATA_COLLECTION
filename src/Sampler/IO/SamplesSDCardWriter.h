#ifndef SAMPLING_SDCARD_WRITER_H 
#define SAMPLING_SDCARD_WRITER_H

using namespace std;
#include <cstdint>
#include <string>
#include <vector>
#include <SD.h>

#include "../../Utils/Logger.h"

class SamplesSDCardWriter{
    private:
        Logger logger;
        uint8_t SDCardChipSelectPin;
    public:
        SamplesSDCardWriter(){};
        SamplesSDCardWriter(uint8_t SDCardChipSelectPin, Logger logger);
        void initialize();
        string createSamplingFile(int timestamp, int sensorID); // automatically creates the above header in the file
        void writeSamples(string fileName, vector<string> samples);
};


#endif /* SAMPLING_SDCARD_WRITER_H */