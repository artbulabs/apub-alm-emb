#include "NtfyNotifier.h"
#include <stdint.h>
#include "config.h"

#ifdef ARDUINO
#include <Arduino.h>

// ---------------------------------------------------------------------------
// Modem transport (LILYGO only)
// ---------------------------------------------------------------------------
#if defined(BOARD_LILYGO) && ((NTFY_TRANSPORT == 2) || (NTFY_TRANSPORT == 3))

#include <HardwareSerial.h>

extern HardwareSerial modemSerial;  // defined in main_lilygo.cpp

static String urlEncode(const char* str) {
    String encoded;
    for (size_t i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        if (c == ' ') {
            encoded += '+';
        } else if (c == ':') {
            encoded += "%3A";
        } else if (c == ',') {
            encoded += "%2C";
        } else {
            encoded += c;
        }
    }
    return encoded;
}

bool NtfyNotifier::sendViaModem(const char* url, const char* title, const char* message,
                                const char* priority, const char* tags) {
    Serial.println("[NTFY] Sending via modem...");

    // Initialize HTTP
    modemSerial.println("AT+HTTPINIT");
    delay(500);

    // zmiana start
    // Build URL with query parameters instead of custom headers
    String fullUrl =
        String(url) + "?title=" + urlEncode(title) + "&priority=" + priority + "&tags=" + tags;

    String urlCmd = String("AT+HTTPPARA=\"URL\",\"") + fullUrl + "\"";
    modemSerial.println(urlCmd);
    delay(300);
    // zmiana stop
    /*
    // Set URL
    String urlCmd = String("AT+HTTPPARA=\"URL\",\"") + url + "\"";
    modemSerial.println(urlCmd);
    delay(300);
    */

    // Set content type
    modemSerial.println("AT+HTTPPARA=\"CONTENT\",\"text/plain\"");
    delay(300);
    /*
// Set custom headers for ntfy
String titleHeader = String("AT+HTTPHEAD=\"Title: ") + title + "\"";
modemSerial.println(titleHeader);
delay(300);

String prioHeader = String("AT+HTTPHEAD=\"Priority: ") + priority + "\"";
modemSerial.println(prioHeader);
delay(300);

String tagsHeader = String("AT+HTTPHEAD=\"Tags: ") + tags + "\"";
modemSerial.println(tagsHeader);
delay(300);

// Set POST data (message body)
uint16_t msgLen = strlen(message);
String dataCmd = String("AT+HTTPDATA=") + msgLen + ",5000";
modemSerial.println(dataCmd);
delay(1000);
modemSerial.print(message);
delay(2000);
*/

    // zmiana start
    uint16_t msgLen = strlen(message);
    String dataCmd = String("AT+HTTPDATA=") + msgLen + ",5000";
    modemSerial.println(dataCmd);
    delay(1000);
    modemSerial.print(message);
    delay(2000);
    // zmiana stop
    //  Execute POST (method=1)
    modemSerial.println("AT+HTTPACTION=1");
    delay(8000);

    // Read response
    String response;
    uint32_t start = millis();
    while (millis() - start < 3000) {
        while (modemSerial.available()) {
            char c = modemSerial.read();
            if (response.length() < 512) {
                response += c;
            }
        }
    }
    Serial.print("[NTFY] Modem response: ");
    Serial.println(response);

    modemSerial.println("AT+HTTPTERM");
    delay(300);

    return response.indexOf("+HTTPACTION: 1,200") >= 0 ||
           response.indexOf("+HTTPACTION: 1,201") >= 0;
}
#endif

// ---------------------------------------------------------------------------
// WiFi transport (DevKit or LILYGO)
// ---------------------------------------------------------------------------
#if (defined(BOARD_DEVKIT) && (NTFY_TRANSPORT != 0)) || \
    (defined(BOARD_LILYGO) && ((NTFY_TRANSPORT == 1) || (NTFY_TRANSPORT == 3)))
#include <HTTPClient.h>
#include <WiFi.h>
#include "secrets.h"

bool NtfyNotifier::sendViaWifi(const char* url, const char* title, const char* message,
                               const char* priority, const char* tags) {
    Serial.println("[NTFY] Sending via WiFi...");

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[NTFY] WiFi not connected, attempting...");
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        uint32_t start = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
            delay(500);
        }
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[NTFY] WiFi connection failed");
            return false;
        }
    }

    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "text/plain");
    http.addHeader("Title", title);
    http.addHeader("Priority", priority);
    http.addHeader("Tags", tags);

    int code = http.POST(message);
    Serial.print("[NTFY] WiFi HTTP code: ");
    Serial.println(code);
    http.end();

    return code == 200 || code == 201;
}
#endif

// ---------------------------------------------------------------------------
// NtfyNotifier implementation
// ---------------------------------------------------------------------------
NtfyNotifier::NtfyNotifier(const char* server, const char* topic)
    : _server(server), _topic(topic) {}

bool NtfyNotifier::sendMessage(const char* title, const char* message, const char* priority,
                               const char* tags) {
    String url = String(_server) + "/" + _topic;
    bool ok = false;

    Serial.println("xxxx01");
#if defined(BOARD_LILYGO)
    Serial.println(String("xxxx02") + NTFY_TRANSPORT);
#if (NTFY_TRANSPORT == 2) || (NTFY_TRANSPORT == 3)
    Serial.println(String("xxxx03") + NTFY_TRANSPORT);
    ok |= sendViaModem(url.c_str(), title, message, priority, tags);
#endif
#if (NTFY_TRANSPORT == 1) || (NTFY_TRANSPORT == 3)
    Serial.println("xxxx04");
    ok |= sendViaWifi(url.c_str(), title, message, priority, tags);
#endif
#elif defined(BOARD_DEVKIT) && (NTFY_TRANSPORT != 0)
    Serial.println("xxxx05");
    ok = sendViaWifi(url.c_str(), title, message, priority, tags);
#else
    // No transport available for this board configuration
    (void)title;
    (void)message;
    (void)priority;
    (void)tags;
    (void)url;
#endif

    if (ok) {
        Serial.println("[NTFY] Notification sent OK");
    } else {
        Serial.println("[NTFY] Failed to send notification");
    }
    return ok;
}

void NtfyNotifier::sendAlert(const char* sensorId) {
    String title = String("ALARM: ") + sensorId;
    String message = String("Motion detected: ") + sensorId;
    sendMessage(title.c_str(), message.c_str(), "high", "warning,rotating_light");
}

void NtfyNotifier::sendStatus(const char* status) {
    sendMessage("Alarm status", status, "default", "information_source");
}

#else
// Native environment
NtfyNotifier::NtfyNotifier(const char* server, const char* topic)
    : _server(server), _topic(topic) {}

bool NtfyNotifier::sendMessage(const char* /*title*/, const char* /*message*/,
                               const char* /*priority*/, const char* /*tags*/) {
    return true;
}
void NtfyNotifier::sendAlert(const char* /*sensorId*/) {}
void NtfyNotifier::sendStatus(const char* /*status*/) {}
#endif
