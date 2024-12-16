#include "SampleDataLogger.h"
#include <SD.h>

SampleDataLogger::SampleDataLogger(std::string fileName, std::string fileHeader){
    dataFile = SD.open(fileName, FILE_WRITE);
    if (dataFile) {
      dataFile.println(fileHeader);
    } else {
      // todo: throw exception and catch on IMUBase...
      // change status to ERROR...
      Serial.println("Error opening IMU_data.csv for writing.");
    }
}

void SampleDataLogger::logSample(std::string sampleData){
    if (dataFile) {
      dataFile.println(sampleData);
    } else {
      // todo: throw exception and catch on IMUBase...
      // change status to ERROR...
      Serial.println("Error opening file for writing.");
    }
}

SampleDataLogger::~SampleDataLogger(){
    dataFile.close();
}
