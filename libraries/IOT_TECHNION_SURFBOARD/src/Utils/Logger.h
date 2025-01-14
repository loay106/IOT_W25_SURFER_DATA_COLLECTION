#ifndef LOGGER_H 
#define LOGGER_H

#include <Arduino.h>

using namespace std;
#include <string>

const int SERIAL_BAUD_RATE = 57600;

class Logger{
    // todo: change to singleton
    public:
        Logger(){};
        void init(){
            Serial.begin(SERIAL_BAUD_RATE);
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