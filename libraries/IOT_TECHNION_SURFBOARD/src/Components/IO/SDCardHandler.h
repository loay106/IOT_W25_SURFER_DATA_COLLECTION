#ifndef SDCARD_HANDLER_H 
#define SDCARD_HANDLER_H

using namespace std;
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <SD.h>
#include <Arduino.h>

#include "../../Utils/Exceptions.h"
#include "../../Utils/Logger.h"

class SDCardHandler{
    private:
        Logger logger;
        uint8_t SDCardChipSelectPin;
    public:
        SDCardHandler(){};

        SDCardHandler(const uint8_t SDCardChipSelectPin, Logger logger): SDCardChipSelectPin(SDCardChipSelectPin), logger(logger){}

        void init(){
            SPI.begin(18,19,23,SDCardChipSelectPin);
            if (!SD.begin(SDCardChipSelectPin)) {
                throw InitError();
            }
            Wire.begin(21,22);
            Wire.setClock(400000);
            logger.info("SD card initialized successfully.");
        }

        void createFolder(string folderName){
            const char *folderNameCStr = folderName.c_str();
            // Check if the folder already exists
            if (!SD.exists(folderNameCStr)) {
                // Attempt to create the folder
                if (SD.mkdir(folderNameCStr)) {
                    logger.info("Folder created successfully.");
                } else {
                    logger.error("Failed to create folder.");
                    throw SDCardError();
                }
            }
        }

        void createFile(string filePath){
            File file = SD.open(filePath.c_str(), FILE_WRITE);
            if (!file) {
                logger.error("Failed to create file!");
                throw SDCardError();
            }
            file.close();
        }

        void writeData(string filePath, char* data){
            File file = SD.open(filePath.c_str(), FILE_APPEND);
            if (!file) {
                logger.error("Failed to open file: " + filePath);
                throw SDCardError();
            }
            file.println(data);
            logFile.flush();
            logFile.close();
        }

        std::map<string, string> readConfigFile(string filePath) {
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
                        logger.error("invalid line in config file: " + line.c_str());
                    }
                }
            }

            configFile.close(); // Close the file
            return configMap;
        }

        vector<string> listFilesInDir(string dirName) {
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
};


#endif /* SDCARD_HANDLER_H */