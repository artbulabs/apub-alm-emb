#include <Arduino.h>
#include <WiFi.h>
#include <esp_camera.h>
#include <esp_now.h>

#include "board_config.h"
#include "espnow_message.h"

// ---------------------------------------------------------------------------
// Camera initialization
// ---------------------------------------------------------------------------
bool initCamera() {
    camera_config_t config{};
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = CAM_Y2_PIN;
    config.pin_d1 = CAM_Y3_PIN;
    config.pin_d2 = CAM_Y4_PIN;
    config.pin_d3 = CAM_Y5_PIN;
    config.pin_d4 = CAM_Y6_PIN;
    config.pin_d5 = CAM_Y7_PIN;
    config.pin_d6 = CAM_Y8_PIN;
    config.pin_d7 = CAM_Y9_PIN;
    config.pin_xclk = CAM_XCLK_PIN;
    config.pin_pclk = CAM_PCLK_PIN;
    config.pin_vsync = CAM_VSYNC_PIN;
    config.pin_href = CAM_HREF_PIN;
    config.pin_sccb_sda = CAM_SIOD_PIN;
    config.pin_sccb_scl = CAM_SIOC_PIN;
    config.pin_pwdn = CAM_PWDN_PIN;
    config.pin_reset = CAM_RESET_PIN;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_SVGA;  // 800x600
    config.jpeg_quality = 12;
    config.fb_count = 1;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("[CAM] Init failed: 0x%x\n", err);
        return false;
    }
    Serial.println("[CAM] Initialized OK");
    return true;
}

// ---------------------------------------------------------------------------
// Capture photo and return frame buffer
// Returns nullptr on failure – caller must call esp_camera_fb_return()
// ---------------------------------------------------------------------------
camera_fb_t* capturePhoto() {
    camera_fb_t* fb = esp_camera_fb_get();  // Get the frame buffer (image) captured by the camera
    if (!fb) {
        Serial.println("[CAM] Capture failed");
        return nullptr;
    }
    Serial.printf("[CAM] Captured JPEG: %u bytes (%dx%d)\n", fb->len, fb->width, fb->height);
    return fb;
}

// ---------------------------------------------------------------------------
// ESP-NOW receive callback
// ---------------------------------------------------------------------------
void onDataReceived(const uint8_t* macAddr, const uint8_t* data, int dataLen) {
    if (dataLen != sizeof(EspNowMessage)) {
        Serial.println("[ESP-NOW] Received message with unexpected size");
        return;
    }

    EspNowMessage msg{};
    memcpy(&msg, data, sizeof(msg));

    Serial.print("[ESP-NOW] Received trigger from sensor: ");
    Serial.print(msg.sensorId);
    Serial.print(" | timestamp: ");
    Serial.print(msg.timestampMs);
    Serial.println("ms");

    camera_fb_t* fb = capturePhoto();
    if (fb) {
        // TODO M8+: send photo via ntfy.sh or store to SD card
        esp_camera_fb_return(fb);  //  Return the frame buffer
    }
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
        Serial.print("[INFO] MAC: ");
        Serial.println(WiFi.macAddress());
    } else if (cmd == "photo") {
        Serial.println("[CMD] Manual capture...");
        camera_fb_t* fb = capturePhoto();
        if (fb) {
            esp_camera_fb_return(fb);
        }
    } else if (cmd == "status") {
        Serial.print("[INFO] MAC: ");
        Serial.println(WiFi.macAddress());
        Serial.println("[INFO] Waiting for ESP-NOW triggers...");
    } else {
        Serial.print("[CMD] Unknown command: '");
        Serial.print(cmd);
        Serial.println("'. Commands: mac | photo | status");
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("=== ESP32-CAM Node ===");

    if (!initCamera()) {
        Serial.println("[ERROR] Camera init failed – halting");
        while (true) {
            delay(1000);
        }
    }

    // Initialize WiFi in station mode (required for ESP-NOW)
    WiFi.mode(WIFI_STA);
    Serial.print("[INFO] MAC: ");
    Serial.println(WiFi.macAddress());

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("[ERROR] ESP-NOW initialization failed");
        return;
    }

    // Register receive callback
    esp_now_register_recv_cb(onDataReceived);

    Serial.println("[INFO] ESP-NOW initialized, waiting for triggers...");
    Serial.println("Commands: mac | photo | status");
}

void loop() {
    handleSerialCommand();
    delay(10);
}
