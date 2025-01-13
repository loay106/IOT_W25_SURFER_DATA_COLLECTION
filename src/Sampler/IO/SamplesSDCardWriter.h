#ifndef SAMPLING_SDCARD_WRITER_H 
#define SAMPLING_SDCARD_WRITER_H

using namespace std;
#include <cstdint>
#include <string>
#include <vector>
#include <SD.h>

#include "../../Utils/Logger.h"

class SamplesSDCardWriter{
    private:
        Logger logger;
        uint8_t SDCardChipSelectPin;
    public:
        SamplesSDCardWriter(){};

        SamplesSDCardWriter::SamplesSDCardWriter(const uint8_t SDCardChipSelectPin, Logger logger): SDCardChipSelectPin(SDCardChipSelectPin), logger(logger){}

        void initialize(){
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

        string createSamplingFile(int timestamp, int sensorID){
            // todo: add meta data as the first line in the file
            string fileName = "sampling_" + std::to_string(timestamp) + "_" + to_string(sensorID) + ".txt";
            string filePath = "/samplings/" + fileName;
            File logFile = SD.open(filePath.c_str(), FILE_WRITE);
            if (!logFile) {
                logger.error("Failed to create sampling file!");
                throw SDCardError();
            }

            // Write the meta data as the first line in the file
            logFile.println("METADATA HERE!!");
            logFile.flush();
            logger.info("Sampling file created successfully: " + filePath);
            logFile.close();
            return fileName;
        }

        void writeSamples(string fileName, string samples){
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
};


#endif /* SAMPLING_SDCARD_WRITER_H */