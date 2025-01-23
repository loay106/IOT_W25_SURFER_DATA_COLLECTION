#include "SDCardHandler.h"

SDCardHandler::SDCardHandler(const uint8_t SDCardChipSelectPin, Logger* logger): SDCardChipSelectPin(SDCardChipSelectPin), logger(logger){}

void SDCardHandler::init(){
    SPI.begin(18,19,23,SDCardChipSelectPin);
    if (!SD.begin(SDCardChipSelectPin)) {
        logger->error("Failed to init SD card!");
        throw InitError();
    }
    Wire.begin(21,22);
    Wire.setClock(400000);
    logger->info("SD card initialized successfully.");
}

void SDCardHandler::createFolder(string folderName){
    const char *folderNameCStr = folderName.c_str();
    // Check if the folder already exists
    if (!SD.exists(folderNameCStr)) {
        // Attempt to create the folder
        if (SD.mkdir(folderNameCStr)) {
            logger->info("Folder created successfully.");
        } else {
            logger->error("Failed to create folder.");
            throw SDCardError();
        }
    }
}
void SDCardHandler::openFolder(string folderName, File* root) {
    File* root = SD.open(folderName.c_str());
    if (!root || !root.isDirectory()) {
        Serial.println("Failed to open directory");
        throw SDCardError();
    }
}

void SDCardHandler::createFile(string filePath){
    File file = SD.open(filePath.c_str(), FILE_WRITE);
    if (!file) {
        logger->error("Failed to create file!");
        throw SDCardError();
    }
    file.close();
}

void SDCardHandler::writeData(string filePath, const char *data){
    File file = SD.open(filePath.c_str(), FILE_APPEND);
    if (!file) {
        logger->error("Failed to open file: " + filePath);
        throw SDCardError();
    }
    file.println(data);
    file.flush();
    file.close();
}

std::map<string, string> SDCardHandler::readConfigFile(string filePath){
    std::map<string, string> configMap; // Map to store parameter name and value

    File configFile = SD.open(filePath.c_str(), FILE_READ); // Open the file in read mode
    if (!configFile) {
        throw SDCardError();
    }

    while (configFile.available()) {
        string line = configFile.readStringUntil('\n').c_str(); // Read a line from the file
        line.erase(line.find_last_not_of(" \n\r\t") + 1); // Trim trailing whitespace

        if (!line.empty()) {
            size_t delimiterPos = line.find(':'); // Find the ':' delimiter
            if (delimiterPos != string::npos) {
                string paramName = line.substr(0, delimiterPos);        // Extract parameter name
                string paramValue = line.substr(delimiterPos + 1);     // Extract parameter value
                configMap[paramName] = paramValue;                     // Insert into map
            } else {
                logger->error("invalid line in config file");
                throw SDCardError();
            }
        }
    }

    configFile.close(); // Close the file
    return configMap;
}

vector<string> SDCardHandler::listFilesInDir(string dirName){
    vector<string> fileList;
    // Open the directory
    File dir = SD.open(dirName.c_str());
    if (!dir || !dir.isDirectory()) {
        Serial.println("Error: Directory does not exist or could not be opened.");
        throw SDCardError();
    }

    // Iterate through the files in the directory
    File entry;
    while ((entry = dir.openNextFile())) {
        if (!entry.isDirectory()) {
            // If it's a file, add the full path to the vector
            string filePath = dirName + "/" + string(entry.name());
            fileList.push_back(filePath);
        }
        entry.close(); // Close the current file to avoid resource leaks
    }

    dir.close(); // Close the directory
    return fileList;
}
