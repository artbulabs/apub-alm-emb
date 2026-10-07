#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "AlarmStateMachine.h"
#include "CompositeNotifier.h"
#include "DebouncedInput.h"
#include "SerialNotifier.h"
#include "board_config.h"
#include "espnow_message.h"

// MAC address of ESP32-CAM node.
// Run 'mac' command on ESP32-CAM Serial Monitor to get this value.
// Replace XX with actual values from your ESP32-CAM module.
// static uint8_t CAM_MAC_ADDRESS[] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};
static uint8_t CAM_MAC_ADDRESS[] = {0x68, 0x09, 0x47, 0x9B, 0x87, 0x6C};

// ---------------------------------------------------------------------------
// PinReader callbacks - one per sensor.
// Each function reads the raw GPIO state for its assigned pin.
// ---------------------------------------------------------------------------
static bool readPir1() {
    return digitalRead(PIR1_PIN) == HIGH;
}
static bool readPir2() {
    return digitalRead(PIR2_PIN) == HIGH;
}
static bool readPir3() {
    return digitalRead(PIR3_PIN) == HIGH;
}
static bool readReed1() {
    return digitalRead(REED1_PIN) == HIGH;
}
static bool readReed2() {
    return digitalRead(REED2_PIN) == HIGH;
}

// ---------------------------------------------------------------------------
// Sensor objects
// ---------------------------------------------------------------------------
static DebouncedInput pir1(readPir1, 50);
static DebouncedInput pir2(readPir2, 50);
static DebouncedInput pir3(readPir3, 50);
static DebouncedInput reed1(readReed1, 50);
static DebouncedInput reed2(readReed2, 50);

// ---------------------------------------------------------------------------
//  Notifiers (Composite pattern - add more channels here in future)
// ---------------------------------------------------------------------------
static SerialNotifier serialNotifier;
static CompositeNotifier notifier;

// ---------------------------------------------------------------------------
// Alarm state machine
// ---------------------------------------------------------------------------
static AlarmStateMachine alarmSM;

// ---------------------------------------------------------------------------
// ESP-NOW
// ---------------------------------------------------------------------------
void onDataSent(const uint8_t* macAddr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        Serial.println("[ESP-NOW] Message delivered");
    } else {
        Serial.println("[ESP-NOW] Message delivery failed");
    }
}

void initEspNow() {
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("[ERROR] ESP-NOW initialization failed");
        return;
    }

    esp_now_register_send_cb(onDataSent);

    // Register ESP32-CAM as peer
    esp_now_peer_info_t peerInfo{};
    memcpy(peerInfo.peer_addr, CAM_MAC_ADDRESS, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("[ERROR] Failed to add ESP-NOW peer");
        return;
    }

    Serial.println("[ESP-NOW] Initialized, peer registered");
}

void sendEspNowTrigger(const char* sensorId) {
    EspNowMessage msg{};
    strncpy(msg.sensorId, sensorId, sizeof(msg.sensorId) - 1);
    msg.timestampMs = millis();
    msg.messageType = 0;

    esp_now_send(CAM_MAC_ADDRESS,
                 reinterpret_cast<uint8_t*>(&msg),
                 sizeof(msg));
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// checkAndAlert helper for clean per-sensor event handling
// void checkAndAlert(DebouncedInput& sensor, const char* name, uint32_t now) {
//     if (sensor.update(now)) {
//         if (sensor.getState()) {
//             Serial.print("[ALERT] ");
//             Serial.print(name);
//             Serial.println(" triggered!");
//         } else {
//             Serial.print("[INFO] ");
//             Serial.print(name);
//             Serial.println(" cleared.");
//         }
//     }
// }

void printStatus() {
    Serial.print("[STATUS] State: ");
    Serial.print(alarmSM.getStateName());
    const char* last = alarmSM.getLastTriggeredSensor();
    if (last != nullptr) {
        Serial.print(" | Last sensor: ");
        Serial.print(last);
    }
    Serial.println();
}

void checkSensor(DebouncedInput& sensor, const char* sensorId, uint32_t now) {
    if (sensor.update(now) && sensor.getState()) {
        Serial.print("[SENSOR] ");
        Serial.print(sensorId);
        Serial.println(" activated");
        alarmSM.onSensorTriggered(sensorId);
        if (alarmSM.getState() == AlarmState::Triggered) {
            // Serial.print("[ALERT] Alarm triggered by: ");
            // Serial.println(sensorId);
            notifier.sendAlert(sensorId);
            sendEspNowTrigger(sensorId);  // send to ESP32-CAM
            alarmSM.onNotificationSent();
        }
    }
}

void handleSerialCommand() {
    if (!Serial.available()) {
        return;
    }
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "arm") {
        alarmSM.onArm();
        // Serial.println("[CMD] Armed");
        notifier.sendStatus("Armed");
        printStatus();
    } else if (cmd == "disarm") {
        alarmSM.onDisarm();
        // Serial.println("[CMD] Disarmed");
        notifier.sendStatus("Disarmed");
        printStatus();
    } else if (cmd == "status") {
        printStatus();
    } else {
        Serial.print("[CMD] Unknown command: '");
        Serial.print(cmd);
        Serial.println("'. Commands: arm | disarm | status");
    }
}

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    delay(2000);  // Wait 2 seconds to allow the Serial Monitor to connect.

    // Register notifiers (add NtfyNotifier, SmsNotifier here in future)
    notifier.add(&serialNotifier);

    Serial.println("=== Alpha Alarm System ===");
    Serial.println("Board: " BOARD_NAME);
    // Serial.println("Monitoring sensors...");
    Serial.println("Commands: arm | disarm | status");
    printStatus();

    pinMode(PIR1_PIN, INPUT);
    pinMode(PIR2_PIN, INPUT);
    pinMode(PIR3_PIN, INPUT);
    pinMode(REED1_PIN, INPUT_PULLUP);
    pinMode(REED2_PIN, INPUT_PULLUP);

    initEspNow();
}

// ---------------------------------------------------------------------------
// Loop
// ---------------------------------------------------------------------------
void loop() {
    uint32_t now = millis();

    handleSerialCommand();

    checkSensor(pir1, "PIR1", now);
    checkSensor(pir2, "PIR2", now);
    checkSensor(pir3, "PIR3", now);
    checkSensor(reed1, "REED1", now);
    checkSensor(reed2, "REED2", now);

    // checkAndAlert(pir1, "PIR1", now);
    // checkAndAlert(pir2, "PIR2", now);
    // checkAndAlert(pir3, "PIR3", now);
    // checkAndAlert(reed1, "REED1", now);

    delay(10);  // small delay to avoid busy-looping
}
