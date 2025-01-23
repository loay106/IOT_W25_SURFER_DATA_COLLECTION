#ifndef WIFI_HANDLER_H 
#define WIFI_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include "../../Utils/Exceptions.h"
#include "Logger.h"


using namespace std;

class WifiHandler{
    public:
        static const int TIME_OUT_MILLIS = 3000;
        WifiHandler(){};
        void init(){
            WiFi.mode(WIFI_STA);
        };
        void connect(string ssid, string password){
            WiFi.begin(ssid.c_str(), password.c_str());
            int timeElapsed = 0;
            while (WiFi.status() != WL_CONNECTED) {
                if(timeElapsed>=TIME_OUT_MILLIS){
                    throw WifiError();
                }
                delay(200);
                timeElapsed+=200;
            }
        }
        void disconnect(){
            WiFi.disconnect(true);
        }
};


#endif /* WIFI_HANDLER_H */
