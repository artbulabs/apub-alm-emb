#include <Arduino.h>
#include <HardwareSerial.h>

#include "board_config.h"

// ---------------------------------------------------------------------------
// Modem serial port
// LILYGO T-A7670E uses UART1 for modem communication
// ---------------------------------------------------------------------------
HardwareSerial modemSerial(1);

// ---------------------------------------------------------------------------
// Modem power on sequence
// Based on LILYGO official documentation
// ---------------------------------------------------------------------------
void modemPowerOn() {
    // Required on battery power: enable power rail
    pinMode(MODEM_POWERON_PIN, OUTPUT);
    digitalWrite(MODEM_POWERON_PIN, HIGH);
    delay(100);

    // PWRKEY sequence to power on modem
    Serial.println("[MODEM] Starting power on sequence...");
    pinMode(MODEM_PWRKEY_PIN, OUTPUT);
    digitalWrite(MODEM_PWRKEY_PIN, HIGH);  // Ensure starts HIGH
    delay(100);
    digitalWrite(MODEM_PWRKEY_PIN, LOW);   // Pull LOW
    delay(100);                            // Hold for 100ms (datasheet min: 50ms)
    digitalWrite(MODEM_PWRKEY_PIN, HIGH);  // Release - DONE, no more pulses

    // Wait for modem to boot (datasheet: ~11s for UART ready)
    // Exit early if all URC messages received ("PB DONE" is the last one)
    Serial.println("[MODEM] Waiting for boot ...");

    String buffer;
    uint32_t start = millis();
    const uint32_t BOOT_TIMEOUT_MS = 15000;  // 15 second timeout

    while (millis() - start < BOOT_TIMEOUT_MS) {
        while (modemSerial.available()) {
            char c = modemSerial.read();
            if (buffer.length() < 512) {  // Memory overflow protection
                buffer += c;
            }
        }
        if (buffer.indexOf("PB DONE") >= 0) {
            break;
        }
        delay(10);
    }

    if (!buffer.isEmpty()) {
        Serial.println("[MODEM] URC messages:");
        Serial.println(buffer);  // Print all collected URC messages at once
    }
    Serial.println("[MODEM] Power on sequence complete");
}

// ---------------------------------------------------------------------------
// Send AT command and print response
// ---------------------------------------------------------------------------
void sendAT(const char* cmd, uint32_t timeoutMs = 2000) {
    Serial.print("[AT] >> ");
    Serial.println(cmd);

    modemSerial.println(cmd);

    uint32_t start = millis();
    while (millis() - start < timeoutMs) {
        while (modemSerial.available()) {
            Serial.write(modemSerial.read());
        }
    }
    Serial.println();
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

    if (cmd == "modem") {
        Serial.println("[CMD] Modem diagnostics...");
        sendAT("AT");            // Basic check
        sendAT("ATI");           // Firmware version
        sendAT("AT+GMR");        // Detailed version
        sendAT("AT+SIMCOMATI");  // Hardware + firmware version
        sendAT("AT+CSQ");        // Signal strength
        sendAT("AT+CREG?");      // Network registration
        sendAT("AT+COPS?");      // Operator name
        sendAT("AT+CIMI");       // SIM IMSI number
    } else if (cmd == "status") {
        Serial.println("[INFO] LILYGO T-A7670E R2");
        Serial.println("[INFO] Type 'modem' for modem diagnostics");
    } else {
        Serial.print("[CMD] Unknown: '");
        Serial.print(cmd);
        Serial.println("'. Commands: modem | status");
    }
}

// ---------------------------------------------------------------------------
// Setup / Loop
// ---------------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("=== LILYGO T-A7670E R2 ===");
    Serial.println("Board: " BOARD_NAME);

    // Initialize modem UART
    modemSerial.begin(MODEM_BAUD, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);

    modemPowerOn();

    Serial.println("[INFO] Ready");
    Serial.println("Commands: modem | status");
}

void loop() {
    handleSerialCommand();

    // Forward any unsolicited modem output to Serial Monitor
    while (modemSerial.available()) {
        Serial.write(modemSerial.read());
    }

    delay(10);
}
