#include "SamplingDataWriter.h"
#include "src/Exceptions/UnitExceptions.h"

string generateUniqueFileName(int timestamp){
    return "sampling_" + std::to_string(timestamp) + ".csv";
}

SamplingDataWriter::SamplingDataWriter(const uint8_t SDCardChipSelectPin, Logger logger): SDCardChipSelectPin(SDCardChipSelectPin), logger(logger){}

void SamplingDataWriter::initialize(){
    if (!SD.begin(SDCardChipSelectPin)) {
        logger.error("SD card initialization failed!");
        throw InitError();
    }
    logger.info("SD card initialized successfully.");

    const char *folderName = "/samplings";
    // Check if the folder already exists
    if (!SD.exists(folderName)) {
        // Attempt to create the folder
        if (SD.mkdir(folderName)) {
            logger.info("Folder 'samplings' created successfully.");
        } else {
            logger.error("Failed to create folder 'samplings'.");
            throw InitError();
        }
    }
}

string SamplingDataWriter::createSamplingFile(int timestamp){
    string fileName = generateUniqueFileName(timestamp);
    string filePath = "/samplings/" + fileName;
    File logFile = SD.open(filePath.c_str(), FILE_WRITE);
    if (!logFile) {
        logger.error("Failed to create sampling file!");
        return File();
    }

    // Write the CSV header
    logFile.println("sample_data,sample_units,sampling_unit_id,sensor_id");
    logFile.flush();
    logger.info("Sampling file created successfully: " + filePath);
    logFile.close();
    return fileName;
}

void SamplingDataWriter::writeSamples(string fileName, string samplingUnitID, string sensorID, vector<string> sampleData, string sampleUnits){
    string filePath = "/samplings/" + fileName;
    File logFile = SD.open(filePath.c_str(), FILE_APPEND);
    if (!logFile) {
        logger.error("Failed to open file: " + fileName);
        return;
    }
    
    for(const string& sd : sampleData){
        string logEntry = sampleData + "," + sampleUnits + "," + samplingUnitID + "," + sensorID;
        logFile.println(logEntry.c_str());
    }
    logFile.flush();
    logFile.close();
}
