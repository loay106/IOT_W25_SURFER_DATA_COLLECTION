#include "SamplesSDCardWriter.h";
#include "../../Utils/Exceptions.h"

#include "SPI.h";
#include "Wire.h";

SamplesSDCardWriter::SamplesSDCardWriter(const uint8_t SDCardChipSelectPin, Logger logger): SDCardChipSelectPin(SDCardChipSelectPin), logger(logger){}

void SamplesSDCardWriter::initialize(){
    SPI.begin(18,19,23,SDCardChipSelectPin);
    if (!SD.begin(SDCardChipSelectPin)) {
        logger.error("SD card initialization failed!");
        while(1);
        //throw InitError();
    }
    Wire.begin(21,22);
    Wire.setClock(400000);
    logger.info("SD card initialized successfully.");

    const char *folderName = "/samplings";
    // Check if the folder already exists
    if (!SD.exists(folderName)) {
        // Attempt to create the folder
        if (SD.mkdir(folderName)) {
            logger.info("Folder samplings created successfully.");
        } else {
            logger.error("Failed to create folder 'samplings'.");
            throw InitError();
        }
    }
}

string SamplesSDCardWriter::createSamplingFile(int timestamp, int sensorID){
    string fileName = "sampling_" + std::to_string(timestamp) + "_" + to_string(sensorID) + ".txt";
    string filePath = "/samplings/" + fileName;
    File logFile = SD.open(filePath.c_str(), FILE_WRITE);
    if (!logFile) {
        logger.error("Failed to create sampling file!");
        // todo: catch this error when using the method....
        throw SDCardError();
    }

    // Write the CSV header
    logFile.println("sample_data,sample_units,sampling_unit_id,sensor_id");
    logFile.flush();
    logger.info("Sampling file created successfully: " + filePath);
    logFile.close();
    return fileName;
}

void SamplesSDCardWriter::writeSamples(string fileName, string samples){
    string filePath = "/samplings/" + fileName;
    File logFile = SD.open(filePath.c_str(), FILE_APPEND);
    if (!logFile) {
        logger.error("Failed to open file: " + fileName);
        return;
    }
    logFile.println(samples.c_str());
    logFile.flush();
    logFile.close();
}
