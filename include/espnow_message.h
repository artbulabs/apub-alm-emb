#pragma once
#include <cstdint>

// Message sent from DevKit via ESP-NOW wireless protocol.
struct EspNowMessage {
    char     sensorId[16];  // e.g. "PIR1", "REED2"
    uint32_t timestampMs;   // millis() on sender at trigger time
    uint8_t  messageType;   // 0 = sensor trigger, 1 = ping
};
