#ifndef SDCARD_HANDLER_H 
#define SDCARD_HANDLER_H

using namespace std;
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <SD.h>
#include <Arduino.h>
#include <Wire.h>

#include "../../Utils/Exceptions.h"
#include "Logger.h"

class SDCardHandler{
    private:
        Logger* logger;
        uint8_t SDCardChipSelectPin;
    public:
        SDCardHandler(const uint8_t SDCardChipSelectPin, Logger* logger);

        void init();

        void createFolder(string folderName);
        void createFile(string filePath);
        void writeData(string filePath,const char* data);
        void openFolder(string folderName, File* root);

        std::map<string, string> readConfigFile(string filePath);

        vector<string> listFilesInDir(string dirName);
};


#endif /* SDCARD_HANDLER_H */