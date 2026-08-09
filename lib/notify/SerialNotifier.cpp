#include "SerialNotifier.h"

#ifdef ARDUINO
#include <Arduino.h>

void SerialNotifier::sendAlert(const char* sensorId) {
    Serial.print("[NOTIFY] ALERT: sensor=");
    Serial.println(sensorId);
}

void SerialNotifier::sendStatus(const char* status) {
    Serial.print("[NOTIFY] STATUS: ");
    Serial.println(status);
}

#else
// Native/test environment – Serial not available, do nothing.
void SerialNotifier::sendAlert(const char* /*sensorId*/) {}
void SerialNotifier::sendStatus(const char* /*status*/) {}
#endif