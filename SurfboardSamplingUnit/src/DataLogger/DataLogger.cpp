#include "DataLogger.h"
#include <SD.h>

DataLogger::DataLogger(const int SDCardChipSelect){

  if (!SD.begin(chipSelect)) {
    // todo: throw exception?
    Serial.println("SD card initialization failed. Check connections.");
  }
}

FILE DataLogger::createSampleLogFile(std::string fileName, std::string fileHeader){
    FILE dataFile = SD.open(fileName, FILE_WRITE);
    if (dataFile) {
      dataFile.println(fileHeader);
    } else {
      // todo: throw exception and catch on IMUBase...
      // change status to ERROR...
      Serial.println("Error opening IMU_data.csv for writing.");
    }
    return dataFile;
}

void DataLogger::logSampleToFile(FILE dataFile, std::string sampleData, int currentTimeStamp){
      // todo: update format - add timestamp to the logged sample
    if (dataFile) {
      dataFile.println(sampleData);
    } else {
      // todo: throw exception and catch on IMUBase...
      // change status to ERROR...?
      Serial.println("Error opening file for writing.");
    }
}