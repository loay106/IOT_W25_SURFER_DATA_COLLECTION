#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#include <string>
using namespace std;

uint8_t CONTROL_UNIT_MAC[] = {0xA8, 0x42, 0xE3, 0x46, 0xE0, 0x64};

const char* IMU_SAMPLE_MOCK = "SAMPLE_SYNC|1547|AccX AccY AccZ|0.23 -9.77 -1.67|0.23 -10.77 -1.57|1.23 -9.78 -1.57|7.23 -9.77 -1.57|7.23 -9.77 -1.57|7.23 -9.77 -1.57|7.23 -9.77 -1.57|7.23 -9.77 -1.57|7.23 -9.77 -1.57";
const char* STRAIN_GAUGE_SAMPLE_MOCK = "SAMPLE_SYNC|1447|N|5456|5421|4242|4240|7540|7040|4812|3478|7484";
const char* STAND_BY_STATUS = "STATUS_UPDATE|STAND_BY";
const char* SAMPLING_STATUS = "STATUS_UPDATE|SAMPLING";
const char* sensorType = "IMU";

int MAX_SAMPLES_PER_SECOND = 100;
int samplingStartTime = 0;
int samplesSentCount = 0;
string COMMAND;

void onCommandReceived(const uint8_t* mac_addr, const uint8_t* incomingData, int len) {
    string message(reinterpret_cast<const char*>(incomingData), len);
    // Serial.println(message.c_str());
    if (message.find("START_SAMPLING") == 0) {
        COMMAND = string("START_SAMPLING");
        Serial.println("Sampling started...");
        samplingStartTime = millis();
        samplesSentCount=0;

        esp_err_t result = esp_now_send(CONTROL_UNIT_MAC, (uint8_t *) SAMPLING_STATUS, strlen(SAMPLING_STATUS));
        if (result != ESP_OK) {
            Serial.println("Failed to send SAMPLING status");
        }
    } else if (message.find("STOP_SAMPLING") == 0) {
        COMMAND = string("STOP_SAMPLING");
        Serial.println("Sampling stopped...");
        int elapsedSeconds = (millis() - samplingStartTime) / 1000;
        int sendRate = elapsedSeconds > 0 ? samplesSentCount / elapsedSeconds : 0;
        Serial.printf("Send rate: %d Hz\n", sendRate);
        esp_err_t result = esp_now_send(CONTROL_UNIT_MAC, (uint8_t *) STAND_BY_STATUS, strlen(STAND_BY_STATUS));
        if (result != ESP_OK) {
            Serial.println("Failed to send SAMPLING status");
        }
    }
}

void setup() {
    Serial.begin(115200);

    COMMAND = string("NO_COMMAND");

    WiFi.mode(WIFI_STA);

    if (esp_now_init() == ESP_OK) {
        Serial.println("ESPNow Init success!");
    }
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, CONTROL_UNIT_MAC , 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
    }
    esp_now_register_recv_cb(onCommandReceived);


    esp_err_t result = esp_now_send(CONTROL_UNIT_MAC, (uint8_t *) STAND_BY_STATUS, strlen(STAND_BY_STATUS));
    if (result != ESP_OK) {
        Serial.println("Failed to send STAND_BY status");
    }
}

void loop() {
    if (COMMAND == "START_SAMPLING") {
        esp_err_t result;
        if (strcmp(sensorType, "IMU") == 0) {
            result = esp_now_send(CONTROL_UNIT_MAC, (uint8_t*)IMU_SAMPLE_MOCK, strlen(IMU_SAMPLE_MOCK));
        } else {
            result = esp_now_send(CONTROL_UNIT_MAC, (uint8_t*)STRAIN_GAUGE_SAMPLE_MOCK, strlen(STRAIN_GAUGE_SAMPLE_MOCK));
        }
        if (result == ESP_OK) {
            samplesSentCount+=9;
        }
        delay(1000 / MAX_SAMPLES_PER_SECOND);
    }else{
        delay(100);
    }
}
