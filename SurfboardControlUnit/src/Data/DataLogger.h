#ifndef DATA_LOGGER_H 
#define DATA_LOGGER_H

using namespace std;
#include <cstdint>
#include <string>
#include "SDCardSampleLogger.h"

// todo: add SD card connection here

class DataLogger{
    private:
        static SDCardSampleLogger sampleLogger;
    public:
        static FILE createSamplingFile(int timestamp); // automatically creates the above header in the file
        static void logSample(FILE logFile, string samplingUnitID, string sensorID, string sampleData, string sampleUnits);
        static void error(string message);
        static void info(string message);
};


#endif /* DATA_LOGGER_H */