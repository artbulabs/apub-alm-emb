#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#include "espnow_message.h"

// ---------------------------------------------------------------------------
// ESP-NOW receive callback
// ---------------------------------------------------------------------------
void onDataReceived(const uint8_t* macAddr, const uint8_t* data, int dataLen) {
    if (dataLen != sizeof(EspNowMessage)) {
        Serial.println("[ESP-NOW] Received message with unexpected size");
        return;
    }

    EspNowMessage msg;
    memcpy(&msg, data, sizeof(msg));

    Serial.print("[ESP-NOW] Received trigger from sensor: ");
    Serial.print(msg.sensorId);
    Serial.print(" | timestamp: ");
    Serial.print(msg.timestampMs);
    Serial.println("ms");

    // TODO M7: trigger photo capture here
}

// ---------------------------------------------------------------------------
// Serial command handler
// ---------------------------------------------------------------------------
void handleSerialCommand() {
    if (!Serial.available()) {
        return;
    }
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "mac") {
        Serial.print("[INFO] MAC Address: ");
        Serial.println(WiFi.macAddress());
    } else if (cmd == "status") {
        Serial.print("[INFO] MAC: ");
        Serial.println(WiFi.macAddress());
        Serial.println("[INFO] Waiting for ESP-NOW triggers...");
    } else {
        Serial.print("[CMD] Unknown command: '");
        Serial.print(cmd);
        Serial.println("'. Commands: mac | status");
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("=== ESP32-CAM Node ===");

    // Initialize WiFi in station mode (required for ESP-NOW)
    WiFi.mode(WIFI_STA);
    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("[ERROR] ESP-NOW initialization failed");
        return;
    }

    // Register receive callback
    esp_now_register_recv_cb(onDataReceived);

    Serial.println("[INFO] ESP-NOW initialized, waiting for triggers...");
    Serial.println("Commands: mac | status");
}

void loop() {
    handleSerialCommand();
    delay(10);
}
