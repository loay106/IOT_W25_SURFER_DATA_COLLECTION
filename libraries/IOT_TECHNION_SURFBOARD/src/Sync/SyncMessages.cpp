#include "SyncMessages.h"

const char* IMU_RATE = "IMU_RATE";
const char* TIMESTAMP = "TIMESTAMP";
const char* WIFI_SSID = "WIFI_SSID";
const char* WIFI_PASSWORD = "WIFI_PASSWORD";
const int STATUS_REPORT_DELAY_MILLIS = 500;


string serializeStatusUpdateMsg(SamplerStatus status){
    string stat = "STATUS_UPDATE|";
    if(status == SamplerStatus::UNIT_STAND_BY){
        stat.append("UNIT_STAND_BY");
    }else if (status == SamplerStatus::UNIT_SAMPLING){
        stat.append("UNIT_SAMPLING");
    }else if(status == SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD){
        stat.append("UNIT_SAMPLE_FILES_UPLOAD");
    }else{
        stat.append("ERROR");
    }
    return stat;
}

SamplerStatus deserializeStatusUpdateMsg(const uint8_t* msg, int len) {
    if (msg == nullptr || len <= 0) {
        throw InvalidSyncMessage();
    }

    // Convert the message to a string
    string rawMsg(reinterpret_cast<const char*>(msg), len);

    // Check the message prefix
    const string prefix = "STATUS_UPDATE|";
    if (rawMsg.find(prefix) != 0) {
        throw InvalidSyncMessage();
    }

    // Extract the status part
    string statusStr = rawMsg.substr(prefix.length());

    // Match the status string to the enum
    if (statusStr == "UNIT_STAND_BY") {
        return SamplerStatus::UNIT_STAND_BY;
    } else if (statusStr == "UNIT_SAMPLING") {
        return SamplerStatus::UNIT_SAMPLING;
    } else if (statusStr == "UNIT_SAMPLE_FILES_UPLOAD") {
        return SamplerStatus::UNIT_SAMPLE_FILES_UPLOAD;
    } else {
        return SamplerStatus::UNIT_ERROR;
    }
}

string serializeCommand(const ControlUnitCommand& command, const std::map<string,string>& params) {
    /*
        Parse ControlUnitCommand object to a string:
        Format: [command]|[param_key_1]:[param_value_1];[param_key_2]:[param_value_2]...
    */
    stringstream ss;

    // Add the command
    ss << command;
    if(!params.empty()){
        ss << "|";
        // Add the parameters
        for (const auto& param : params) {
            ss << param.first << ":" << param.second << ";";
        }
    }
    
    // Remove trailing semicolon (if any)
    string result = ss.str();
    if (result.back() == ';') {
        result.pop_back();
    }
    return result;
}

CommandMessage deserializeCommand(const uint8_t* msg, int len) {
    /*
        Parse the message in the format generated by commandToMsg
        back into a CommandMessage object.
        Expected Format: [command]|[param_key_1]:[param_value_1];[param_key_2]:[param_value_2]...
    */
    if (len <= 0 || msg == nullptr) {
        throw InvalidSyncMessage();
    }

    string rawMsg(reinterpret_cast<const char*>(msg), len);
    CommandMessage commandMsg;

    // Split by '|' to separate command and parameters
    size_t delimiterPos = rawMsg.find('|');
    if (delimiterPos == string::npos) {
        throw InvalidSyncMessage();
    }

    // Extract and parse the command
    string commandPart = rawMsg.substr(0, delimiterPos);
    int commandValue = stoi(commandPart);
    if (commandValue < START_SAMPLING || commandValue > UPLOAD_SAMPLE_FILES) {
        throw InvalidSyncMessage();
    }
    commandMsg.command = static_cast<ControlUnitCommand>(commandValue);

    // Extract and parse the parameters
    string paramsPart = rawMsg.substr(delimiterPos + 1);
    stringstream paramsStream(paramsPart);
    string param;

    while (getline(paramsStream, param, ';')) {
        size_t colonPos = param.find(':');
        if (colonPos == string::npos) {
            throw InvalidSyncMessage();
        }

        string key = param.substr(0, colonPos);
        string value = param.substr(colonPos + 1);
        commandMsg.params[key] = value;
    }

    return commandMsg;
}