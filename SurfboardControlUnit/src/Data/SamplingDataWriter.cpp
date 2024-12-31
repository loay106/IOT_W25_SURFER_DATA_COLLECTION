#include "SamplingDataWriter.h"

string generateUniqueFileName(int timestamp){
    return "sampling_" + std::to_string(timestamp) + ".csv";
}

SamplingDataWriter::SamplingDataWriter(const uint8_t SDCardChipSelectPin): SDCardChipSelectPin(SDCardChipSelectPin){}

void SamplingDataWriter::initialize(){
    if (!SD.begin(SDCardChipSelectPin)) {
        Serial.println("SD card initialization failed!");
        // todo: throw error...
        return;
    }
    Serial.println("SD card initialized successfully.");


    const char *folderName = "/samplings";
    // Check if the folder already exists
    if (!SD.exists(folderName)) {
        // Attempt to create the folder
        if (SD.mkdir(folderName)) {
            Serial.println("Folder 'samplings' created successfully.");
        } else {
            Serial.println("Failed to create folder 'samplings'.");
        }
    } else {
        Serial.println("Folder 'samplings' already exists.");
    }
}

string SamplingDataWriter::createSamplingFile(int timestamp){
    string fileName = generateUniqueFileName(timestamp);
    string filePath = "/samplings/" + fileName;
    File logFile = SD.open(filePath.c_str(), FILE_WRITE);
    if (!logFile) {
        Serial.println("Failed to create sampling file!");
        return File();
    }

    // Write the CSV header
    logFile.println("sample_data,sample_units,sampling_unit_id,sensor_id");
    logFile.flush();
    Serial.println("Sampling file created successfully: " + filePath);
    logFile.close();
    return fileName;
}

void SamplingDataWriter::writeSamples(string fileName, string samplingUnitID, string sensorID, vector<string> sampleData, string sampleUnits){
    string filePath = "/samplings/" + fileName;
    File logFile = SD.open(filePath.c_str(), FILE_APPEND);
    if (!logFile) {
        Serial.println("Failed to open file: " + fileName);
        // todo: show error...
        return;
    }
    
    for(const string& sd : sampleData){
        string logEntry = sampleData + "," + sampleUnits + "," + samplingUnitID + "," + sensorID;
        logFile.println(logEntry.c_str());
    }
    logFile.flush();
    logFile.close();
}
