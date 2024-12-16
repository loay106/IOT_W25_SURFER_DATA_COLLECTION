#ifndef SAMPLE_DATA_LOGGER_H
#define SAMPLE_DATA_LOGGER_H

#include <string>

class SampleDataLogger {
    private:
        FILE dataFile;
    
    public:
        SampleDataLogger(std::string fileName, std::string fileHeader);
        void logSample(std::string sampleData);
        ~SampleDataLogger();
};
#endif // SAMPLE_DATA_LOGGER_H