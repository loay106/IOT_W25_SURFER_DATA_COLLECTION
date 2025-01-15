#ifndef SYNC_MESSAGES_H 
#define SYNC_MESSAGES_H

#include <map>
#include <string>
#include <cstdint>
#include <vector>
#include <sstream>
#include <stdexcept>

#include "../Utils/Status.h"
#include "../Utils/Exceptions.h"

using namespace std;

// constant params
extern const char* IMU_RATE;
extern const char* TIMESTAMP;
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;
extern const int STATUS_REPORT_DELAY_MILLIS;

enum ControlUnitCommand {
    START_SAMPLING, // attached IMU_RATE and TIMESTAMP
    UPLOAD_SAMPLE_FILES, // attached WIFI_SSID and WIFI_PASSWORD
    UPDATE_SENSOR_PARAMS, // attached sensor id and param
};

typedef struct CommandMessage {
    ControlUnitCommand command;
    std::map<string, string> params;    
} CommandMessage;

string serializeStatusUpdateMsg(SamplerStatus status);
SamplerStatus deserializeStatusUpdateMsg(const uint8_t* msg, int len);

string serializeCommand(const ControlUnitCommand& command, const std::map<string,string>& params);
CommandMessage deserializeCommand(const uint8_t* msg, int len);

#endif /* SYNC_MESSAGES_H */
