#ifndef LOGGER_H 
#define LOGGER_H

#include <Arduino.h>

using namespace std;
#include <string>

class Logger{
    private:
        int serialBaudRate;
    public:
        Logger(){};
        Logger(int serialBaudRate): serialBaudRate(serialBaudRate){};
        void init(){
            Serial.begin(serialBaudRate);
        };
        void info(string message){
            string fmessage = "[INFO] " + message;
            Serial.println(String(fmessage.c_str()));
            Serial.flush();
        };
        void error(string message){
            string fmessage = "[ERROR] " + message;
            Serial.println(String(fmessage.c_str()));
            Serial.flush();
        };

};

#endif /* LOGGER_H */