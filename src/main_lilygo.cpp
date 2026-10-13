#include <Arduino.h>
#include <HardwareSerial.h>

#include "CompositeNotifier.h"
#include "NtfyNotifier.h"
#include "SerialNotifier.h"
#include "board_config.h"
#include "config.h"
#include "secrets.h"

// Maximum buffer size for AT command responses and URC messages
static constexpr uint16_t AT_BUFFER_MAX_LEN = 512;

// ---------------------------------------------------------------------------
//  Notifiers (Composite pattern - add more channels here in future)
// ---------------------------------------------------------------------------
static NtfyNotifier ntfyNotifier(NTFY_SERVER, NTFY_TOPIC);
static SerialNotifier serialNotifier;
static CompositeNotifier notifier;

// ---------------------------------------------------------------------------
// Modem serial port
// LILYGO T-A7670E uses UART1 for modem communication
// ---------------------------------------------------------------------------
HardwareSerial modemSerial(1);

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------
void sendAT(const char* cmd, uint32_t timeoutMs = 2000);
String sendATGetResponse(const char* cmd, uint32_t timeoutMs = 2000);

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
            if (buffer.length() < AT_BUFFER_MAX_LEN) {  // Memory overflow protection
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
void sendAT(const char* cmd, uint32_t timeoutMs) {
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
// Send AT command, return response as String
// ---------------------------------------------------------------------------
String sendATGetResponse(const char* cmd, uint32_t timeoutMs) {
    modemSerial.println(cmd);
    String response;
    uint32_t start = millis();
    while (millis() - start < timeoutMs) {
        while (modemSerial.available()) {
            char c = modemSerial.read();
            if (response.length() < AT_BUFFER_MAX_LEN) {  // Memory overflow protection
                response += c;
            }
        }
    }
    return response;
}

// ---------------------------------------------------------------------------
// Establish LTE data connection
// ---------------------------------------------------------------------------
bool connectToNetwork() {
    Serial.println("[NET] Configuring APN...");

    // Set APN
    String apnCmd = String("AT+CGDCONT=1,\"IP\",\"") + ORANGE_APN + "\"";
    sendAT(apnCmd.c_str());

    // Activate PDP context
    sendAT("AT+CGACT=1,1", 10000);

    // Check if IP assigned
    String response = sendATGetResponse("AT+CGPADDR=1", 3000);
    Serial.print("[NET] IP response: ");
    Serial.println(response);

    if (response.indexOf("+CGPADDR: 1,") >= 0 && response.indexOf("0.0.0.0") == -1) {
        Serial.println("[NET] Connected successfully");
        return true;
    }

    Serial.println("[NET] Connection failed");
    return false;
}

// ---------------------------------------------------------------------------
// Send test HTTP GET request
// ---------------------------------------------------------------------------
void testHttpGet() {
    Serial.println("[NET] Sending test HTTP GET to example.com...");

    // Initialize HTTP service
    sendAT("AT+HTTPINIT", 3000);

    // Set URL
    sendAT("AT+HTTPPARA=\"URL\",\"http://example.com\"", 3000);

    // Set connection type
    sendAT("AT+HTTPPARA=\"CONNECTTO\",60", 3000);

    // Execute GET request (method=0 for GET)
    String response = sendATGetResponse("AT+HTTPACTION=0", 10000);
    Serial.print("[NET] HTTP action response: ");
    Serial.println(response);

    // Read response data
    sendAT("AT+HTTPREAD=0,200", 5000);

    // Terminate HTTP service
    sendAT("AT+HTTPTERM", 2000);

    Serial.println("[NET] HTTP test complete");
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
    } else if (cmd == "netstatus") {
        Serial.println("[CMD] Network status...");
        sendAT("AT+CREG?");      // Registration status
        sendAT("AT+CSQ");        // Signal strength
        sendAT("AT+CGPADDR=1");  // Assigned IP address
        sendAT("AT+CGACT?");     // PDP context status
    } else if (cmd == "connect") {
        connectToNetwork();
    } else if (cmd == "ping") {
        testHttpGet();
    } else if (cmd == "notify") {
        Serial.println("[CMD] Sending test notification...");
        ntfyNotifier.sendAlert("TEST");
    } else if (cmd == "status") {
        Serial.println("[INFO] LILYGO T-A7670E R2");
        Serial.println("[INFO] Commands: modem | netstatus | connect | ping | notify | status");
    } else {
        Serial.print("[CMD] Unknown: '");
        Serial.print(cmd);
        Serial.println("'. Commands: modem | netstatus | connect | ping | notify | status");
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

    // Register notifiers
    notifier.add(&serialNotifier);
    if (NTFY_ENABLED) {
        notifier.add(&ntfyNotifier);
    }

    Serial.println("[INFO] Ready");
    Serial.println("Commands: modem | netstatus | connect | ping | notify | status");
}

void loop() {
    handleSerialCommand();

    // Forward any unsolicited modem output to Serial Monitor
    while (modemSerial.available()) {
        Serial.write(modemSerial.read());
    }

    delay(10);
}
