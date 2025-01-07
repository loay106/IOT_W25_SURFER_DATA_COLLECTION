#include "SamplingDataWriter.h";
#include "../Exceptions/UnitExceptions.h";
#include "SPI.h";
#include "Wire.h";

string generateUniqueFileName(int timestamp){
    return "sampling_" + std::to_string(timestamp) + ".csv";
}

SamplingDataWriter::SamplingDataWriter(const uint8_t SDCardChipSelectPin, Logger logger): SDCardChipSelectPin(SDCardChipSelectPin), logger(logger){}

void SamplingDataWriter::initialize(){
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

string SamplingDataWriter::createSamplingFile(int timestamp){
    string fileName = generateUniqueFileName(timestamp);
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

void SamplingDataWriter::writeSamples(string fileName, string samplingUnitID, string sensorID, vector<string> sampleDataVec, string sampleUnits){
    string filePath = "/samplings/" + fileName;
    File logFile = SD.open(filePath.c_str(), FILE_APPEND);
    if (!logFile) {
        logger.error("Failed to open file: " + fileName);
        return;
    }
    
    for(const string& sampleData : sampleDataVec){
        string logEntry = sampleData + "," + sampleUnits + "," + samplingUnitID + "," + sensorID;
        logger.info(logEntry);
        logFile.println(logEntry.c_str());
    }
    logFile.flush();
    logFile.close();
    //logger.info("Sample written!!");
}
