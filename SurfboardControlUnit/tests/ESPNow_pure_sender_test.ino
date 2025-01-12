#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Replace with the actual MAC address of the receiver
uint8_t RECEIVER_MAC[] = {0xCC, 0xDB, 0xA7, 0x5A, 0x7F, 0xC0};

const char* MESSAGE = "hello world!"; // Dummy message
const int THREAD_COUNT = 2;           // Number of threads for parallel sending

// Function executed by each thread to send messages
void sendTask(void* param) {
    while (true) {
        esp_err_t result = esp_now_send(RECEIVER_MAC, (uint8_t*)MESSAGE, strlen(MESSAGE));
        if (result != ESP_OK) {
            Serial.printf("Send failed: %s\n", esp_err_to_name(result));
            delay(10); // Small delay in case of failure to prevent overload
        }
        // No delay for maximum throughput
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA); // Set ESP32 to station mode
    Serial.println("ESP-NOW Sender with Threads");

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW initialization failed!");
        return;
    }

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, RECEIVER_MAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
    Serial.println("Peer added, starting transmission...");

    // Create threads to send messages in parallel
    for (int i = 0; i < THREAD_COUNT; i++) {
        char taskName[16];
        snprintf(taskName, sizeof(taskName), "SendTask_%d", i);

        BaseType_t result = xTaskCreatePinnedToCore(
            sendTask,          // Task function
            taskName,          // Task name
            4096,              // Stack size
            NULL,              // Parameter
            1,                 // Priority
            NULL,              // Task handle
            i % 2              // Core (alternating between core 0 and 1)
        );

        if (result != pdPASS) {
            Serial.printf("Failed to create thread %d\n", i);
        }
    }
}

void loop() {
    // Nothing to do here; tasks handle the sending process
}
