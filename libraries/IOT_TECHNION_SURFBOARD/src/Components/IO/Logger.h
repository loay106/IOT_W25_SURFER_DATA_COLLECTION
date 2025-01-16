#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <string>

using namespace std;

// Constants for logging levels
enum class LogLevel {
    NONE,    // No logging
    ERROR,   // Log only errors
    INFO,    // Log errors and info
    DEBUG    // Log errors, info, and debug messages
};

const int SERIAL_BAUD_RATE = 57600;

class Logger {
    // singleton class
    private:
        LogLevel currentLevel; // Current logging level
        static Logger* instance;

        Logger(){
            currentLevel = LogLevel::INFO;
        }
        void logMessage(LogLevel level, string prefix, string message) {
            if (static_cast<int>(level) <= static_cast<int>(currentLevel)) {
                string formattedMessage = "[" + prefix + "] " + message;
                Serial.println(String(formattedMessage.c_str()));
                Serial.flush();
            }
        }

    public:
        Logger(const Logger& obj) = delete;

        static Logger* getInstance() {
            if (instance == nullptr) {
                instance = new Logger();
            }
            return instance;
        }

        void init(int serialBaudRate) {
            Serial.begin(serialBaudRate);
        }

        void setLogLevel(LogLevel level) {
            currentLevel = level;
        }

        void info(string message) {
            if(!isInit){
                throw NotInitError();
            }
            logMessage(LogLevel::INFO, "INFO", message);
        }

        void error(string message) {
            if(!isInit){
                throw NotInitError();
            }
            logMessage(LogLevel::ERROR, "ERROR", message);
        }

        void debug(string message) {
            if(!isInit){
                throw NotInitError();
            }
            logMessage(LogLevel::DEBUG, "DEBUG", message);
        }
};

#endif /* LOGGER_H */