#ifndef SAMPLE_LOGGER_H 
#define SAMPLE_LOGGER_H

using namespace std;
#include <cstdint>
#include <string>

// todo: add SD card connection here

class SampleLogger{        
    public:
        
        // todo: extract sensor id from here - important
        void logSample(FILE logFile, string samplingUnitID, const uint8_t* sampleContent, int sampleContentLen);
};


#endif /* SAMPLE_LOGGER_H */