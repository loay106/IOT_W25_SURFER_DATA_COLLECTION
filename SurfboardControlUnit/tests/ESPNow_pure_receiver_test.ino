#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

volatile int messageCount = 0;
unsigned long lastReportTime = 0;

void onDataReceive(const uint8_t* mac_addr, const uint8_t* data, int len) {
    messageCount++; // Increment the counter for each received message
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA); // Set ESP32 to station mode
    Serial.println("ESP-NOW Receiver");

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW initialization failed!");
        return;
    }

    esp_now_register_recv_cb(onDataReceive); // Register the receive callback
    Serial.println("Ready to receive messages...");
}

void loop() {
    unsigned long currentTime = millis();
    if (currentTime - lastReportTime >= 1000) { // Report every second
        Serial.printf("Messages received in last second: %d\n", messageCount);
        messageCount = 0; // Reset the counter
        lastReportTime = currentTime;
    }
}
