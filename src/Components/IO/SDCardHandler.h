#ifndef SDCARD_HANDLER_H 
#define SDCARD_HANDLER_H

using namespace std;
#include <cstdint>
#include <string>
#include <vector>
#include <SD.h>

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
};


#endif /* SDCARD_HANDLER_H */