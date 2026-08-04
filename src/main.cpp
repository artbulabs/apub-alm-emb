#include <Arduino.h>
#include "board_config.h"
#include "DebouncedInput.h"
#include "AlarmStateMachine.h"

// ---------------------------------------------------------------------------
// PinReader callbacks – one per sensor.
// Each function reads the raw GPIO state for its assigned pin.
// ---------------------------------------------------------------------------
static bool readPir1() { return digitalRead(PIR1_PIN) == HIGH; }
static bool readPir2() { return digitalRead(PIR2_PIN) == HIGH; }
static bool readPir3() { return digitalRead(PIR3_PIN) == HIGH; }
static bool readReed1() { return digitalRead(REED1_PIN) == HIGH; }

// ---------------------------------------------------------------------------
// Sensor objects
// ---------------------------------------------------------------------------
static DebouncedInput pir1(readPir1, 50);
static DebouncedInput pir2(readPir2, 50);
static DebouncedInput pir3(readPir3, 50);
static DebouncedInput reed1(readReed1, 50);

// ---------------------------------------------------------------------------
// Alarm state machine
// ---------------------------------------------------------------------
static AlarmStateMachine alarmSM;


// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------

// checkAndAlert helper for clean per-sensor event handling
void checkAndAlert(DebouncedInput& sensor, const char* name, uint32_t now) {
    if (sensor.update(now)) {
        if (sensor.getState()) {
            Serial.print("[ALERT] ");
            Serial.print(name);
            Serial.println(" triggered!");
        } else {
            Serial.print("[INFO] ");
            Serial.print(name);
            Serial.println(" cleared.");
        }
    }
}

void printStatus() {
    Serial.print("[STATUS] State: ");
    Serial.print(alarmSM.getStateName());
    const char* last = alarmSM.getLastTriggeredSensor();
    if(last != nullptr) {
        Serial.print(" | Last sensor: ");
        Serial.print(last);
    }
    Serial.println();
}

void checkSensor(DebouncedInput& sensor, const char* sensorId, uint32_t now) {
    if(sensor.update(now) && sensor.getState()) {
        Serial.print("[SENSOR] ");
        Serial.print(sensorId);
        Serial.println(" activated");
        alarmSM.onSensorTriggered(sensorId);
        if(alarmSM.getState() == AlarmState::Triggered) {
            Serial.print("[ALERT] Alarm triggered by: ");
            Serial.println(sensorId);
            // TODO M5: call notifier.sendAlert(sensorId) here
            alarmSM.onNotificationSent(); // placeholder: instant "sent"
        }
    }
}

void handleSerialCommand() {
    if (!Serial.available()) return;
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "arm") {
        alarmSM.onArm();
        Serial.println("[CMD] Armed");
        printStatus();
    } else if (cmd == "disarm") {
        alarmSM.onDisarm();
        Serial.println("[CMD] Disarmed");
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
    delay(2000); // Wait 2 seconds to allow the Serial Monitor to connect.
    Serial.println("=== Alpha Alarm System ===");
    Serial.println("Board: " BOARD_NAME);
    // Serial.println("Monitoring sensors...");
    Serial.println("Commands: arm | disarm | status");
    printStatus();

    pinMode(PIR1_PIN, INPUT);
    pinMode(PIR2_PIN, INPUT);
    pinMode(PIR3_PIN, INPUT);
    pinMode(REED1_PIN, INPUT_PULLUP);
}

// ---------------------------------------------------------------------------
// Loop
// ---------------------------------------------------------------------------
void loop() {
    uint32_t now = millis();

    handleSerialCommand();

    checkSensor(pir1,  "PIR1",  now);
    checkSensor(pir2,  "PIR2",  now);
    checkSensor(pir3,  "PIR3",  now);
    checkSensor(reed1, "REED1", now);

    // checkAndAlert(pir1, "PIR1", now);
    // checkAndAlert(pir2, "PIR2", now);
    // checkAndAlert(pir3, "PIR3", now);
    // checkAndAlert(reed1, "REED1", now);

    delay(10); // small delay to avoid busy-looping
}