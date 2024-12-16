#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <string>

class DataLogger {
    public:
        DataLogger(const int SDCardChipSelect);
        FILE createSampleLogFile(std::string fileName, std::string fileHeader);
        void logSampleToFile(FILE dataFile, std::string sampleData, int currentTimeStamp);
};
#endif // DATA_LOGGER_H