#ifndef WIFI_HANDLER_H 
#define WIFI_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include "../../Utils/Exceptions.h"
#include "Logger.h"


using namespace std;

extern const int CONNECTION_REQUEST_INTERVAL;

class WifiHandler{
    private:
        int lastConnectionRequest;
    public:
        WifiHandler(){
            lastConnectionRequest=0;
        };

        void init(){
            WiFi.mode(WIFI_STA);
        };
        String getUnitMac(){
            return WiFi.macAddress();
        }
        void connect(string ssid, string password){
            int current = millis();
            if((current - lastConnectionRequest) < CONNECTION_REQUEST_INTERVAL){
                WiFi.begin(ssid.c_str(), password.c_str());
                lastConnectionRequest=current;
            }
            
        }
        void disconnect(){
            lastConnectionRequest=0;
            WiFi.disconnect(true);
        }

        bool isWifiConnected(){
            return WiFi.status() == WL_CONNECTED;
        }
};


#endif /* WIFI_HANDLER_H */
