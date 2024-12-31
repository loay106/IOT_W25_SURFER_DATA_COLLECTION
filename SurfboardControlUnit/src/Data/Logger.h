#ifndef LOGGER_H 
#define LOGGER_H

using namespace std;
#include <string>

class Logger{
    private:
        int serialBaudRate;
    public:
        Logger(){};
        Logger(int serialBaudRate): serialBaudRate(serialBaudRate){};
        void initialize(){
            Serial.begin(serialBaudRate);
        };
        void info(string message){
            string fmessage = "[INFO] " + message;
            Serial.println(fmessage.c_str());
            Serial.flush();
        };
        void error(string message){
            string fmessage = "[ERROR] " + message;
            Serial.println(fmessage.c_str());
            Serial.flush();
        };

};

#endif /* CONTROL_UNIT_MANAGER_H */