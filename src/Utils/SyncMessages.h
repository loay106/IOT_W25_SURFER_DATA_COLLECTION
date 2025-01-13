#ifndef SYNC_MESSAGES_H 
#define SYNC_MESSAGES_H

#include <map>
#include <string>
#include <cstdint>
#include <vector>
#include <sstream>
#include <stdexcept>

#include "Exceptions.h"

using namespace std;

// constant params
const char* IMU_RATE = "IMU_RATE";
const char* TIMESTAMP = "TIMESTAMP";
const char* WIFI_SSID = "WIFI_SSID";
const char* WIFI_PASSWORD = "WIFI_PASSWORD";

enum ControlUnitCommand {
    START_SAMPLING, // attached IMU_RATE, TIMESTAMP params
    STOP_SAMPLING,
    UPLOAD_SAMPLE_FILES // attached WIFI params
};

typedef struct CommandMessage {
    ControlUnitCommand command;
    map<string, string> params;    
} CommandMessage;

// Convert CommandMessage to a message string
string serializeCommand(const CommandMessage& commandMsg) {
    /*
        Parse ControlUnitCommand object to a string:
        Format: [command]|[param_key_1]:[param_value_1];[param_key_2]:[param_value_2]...
    */
    stringstream ss;

    // Add the command
    ss << commandMsg.command << "|";

    // Add the parameters
    for (const auto& param : commandMsg.params) {
        ss << param.first << ":" << param.second << ";";
    }

    // Remove trailing semicolon (if any)
    string result = ss.str();
    if (result.back() == ';') {
        result.pop_back();
    }
    return result;
}

// Convert message string to CommandMessage
CommandMessage deserializeCommand(const uint8_t* msg, int len) {
    /*
        Parse the message in the format generated by commandToMsg
        back into a CommandMessage object.
        Expected Format: [command]|[param_key_1]:[param_value_1];[param_key_2]:[param_value_2]...
    */
    if (len <= 0 || msg == nullptr) {
        throw invalid_argument("Invalid message length or null message.");
    }

    string rawMsg(reinterpret_cast<const char*>(msg), len);
    CommandMessage commandMsg;

    // Split by '|' to separate command and parameters
    size_t delimiterPos = rawMsg.find('|');
    if (delimiterPos == string::npos) {
        throw invalid_argument("Malformed message: Missing '|'.");
    }

    // Extract and parse the command
    string commandPart = rawMsg.substr(0, delimiterPos);
    int commandValue = stoi(commandPart);
    if (commandValue < START_SAMPLING || commandValue > STOP_SAMPLING) {
        throw invalid_argument("Invalid command value.");
    }
    commandMsg.command = static_cast<ControlUnitCommand>(commandValue);

    // Extract and parse the parameters
    string paramsPart = rawMsg.substr(delimiterPos + 1);
    stringstream paramsStream(paramsPart);
    string param;

    while (getline(paramsStream, param, ';')) {
        size_t colonPos = param.find(':');
        if (colonPos == string::npos) {
            throw invalid_argument("Malformed parameter: Missing ':'.");
        }

        string key = param.substr(0, colonPos);
        string value = param.substr(colonPos + 1);
        commandMsg.params[key] = value;
    }

    return commandMsg;
}

// Enum for sampler statuses
enum SamplerStatus {
    UNIT_STAND_BY,
    UNIT_SAMPLING,
    UNIT_ERROR  
};

#endif /* SYNC_MESSAGES_H */
