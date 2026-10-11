#pragma once

// ============================================================
// Board selection is done via compile-time macros in
// platformio.ini:
//   build_flags = -D BOARD_DEVKIT
//   build_flags = -D BOARD_LILYGO
//   build_flags = -D BOARD_ESPCAM
//   build_flags = -D BOARD_NATIVE
// ============================================================

#if defined(BOARD_DEVKIT)

    // --- ESP32 DevKit V1 (38-pin, CP2102) ---
    // Test/development board. No modem available.

    #define BOARD_NAME     "ESP32 DevKit V1"
    #define HAS_MODEM      0

    // PIR sensors (HC-SR501) - 3 units
    #define PIR1_PIN       25
    #define PIR2_PIN       26
    #define PIR3_PIN       27

    // Reed switch (MC-38)
    #define REED1_PIN       32
    #define REED2_PIN       33

    // Modem pins (not available on this board)
    #define MODEM_TX_PIN        -1
    #define MODEM_RX_PIN        -1
    #define MODEM_PWRKEY_PIN    -1
    #define MODEM_POWERON_PIN   -1
    #define MODEM_RESET_PIN     -1
    #define MODEM_BAUD           0

#elif defined(BOARD_LILYGO)
    // --- LILYGO T-A7670E R2 ---
    // Production board. Has LTE modem (A7670E).
    // GPIO pins not soldered yet - sensor input via ESP-NOW only.
    // Pin numbers reserved for future use when pins are soldered.

    #define BOARD_NAME     "LILYGO T-A7670E R2"
    #define HAS_MODEM      1

    // PIR sensors (HC-SR501) - 3 units
    #define PIR1_PIN       -1
    #define PIR2_PIN       -1
    #define PIR3_PIN       -1

    // Reed switch (MC-38)
    #define REED1_PIN      -1
    #define REED2_PIN      -1

    // Camera pins (not on this board)
    #define CAM_PWDN_PIN        -1
    #define CAM_RESET_PIN       -1
    #define CAM_XCLK_PIN        -1
    #define CAM_SIOD_PIN        -1
    #define CAM_SIOC_PIN        -1
    #define CAM_Y9_PIN          -1
    #define CAM_Y8_PIN          -1
    #define CAM_Y7_PIN          -1
    #define CAM_Y6_PIN          -1
    #define CAM_Y5_PIN          -1
    #define CAM_Y4_PIN          -1
    #define CAM_Y3_PIN          -1
    #define CAM_Y2_PIN          -1
    #define CAM_VSYNC_PIN       -1
    #define CAM_HREF_PIN        -1
    #define CAM_PCLK_PIN        -1

    // A7670E modem pins (internal connections on LILYGO board)
    #define MODEM_TX_PIN        26
    #define MODEM_RX_PIN        27
    #define MODEM_PWRKEY_PIN     4   // DO NOT connect external wires to this pin
    #define MODEM_POWERON_PIN   12   // Must be HIGH on battery power
    #define MODEM_RESET_PIN      5
    #define MODEM_BAUD        115200

#elif defined(BOARD_ESPCAM)
    // --- ESP32-CAM AI Thinker ---

    #define BOARD_NAME     "ESP32-CAM AI Thinker"
    #define HAS_MODEM      0

    // Sensor pins (not used on ESP32-CAM – no external sensors)
    #define PIR1_PIN       -1
    #define PIR2_PIN       -1
    #define PIR3_PIN       -1
    #define REED1_PIN      -1
    #define REED2_PIN      -1

    // Camera interface pins (internal connections, required by esp_camera library)
    #define CAM_PWDN_PIN   32
    #define CAM_RESET_PIN  -1
    #define CAM_XCLK_PIN    0
    #define CAM_SIOD_PIN   26
    #define CAM_SIOC_PIN   27
    #define CAM_Y9_PIN     35
    #define CAM_Y8_PIN     34
    #define CAM_Y7_PIN     39
    #define CAM_Y6_PIN     36
    #define CAM_Y5_PIN     21
    #define CAM_Y4_PIN     19
    #define CAM_Y3_PIN     18
    #define CAM_Y2_PIN      5
    #define CAM_VSYNC_PIN  25
    #define CAM_HREF_PIN   23
    #define CAM_PCLK_PIN   22

    // Modem pins (not available on this board)
    #define MODEM_TX_PIN        -1
    #define MODEM_RX_PIN        -1
    #define MODEM_PWRKEY_PIN    -1
    #define MODEM_POWERON_PIN   -1
    #define MODEM_RESET_PIN     -1
    #define MODEM_BAUD           0

#elif defined(BOARD_NATIVE)
    // --- Native (PC/Linux) ---
    // Used for unit testing only. No GPIO available.

    #define BOARD_NAME     "Native (unit tests)"
    #define HAS_MODEM      0

    #define PIR1_PIN       -1
    #define PIR2_PIN       -1
    #define PIR3_PIN       -1
    #define REED1_PIN      -1
    #define REED2_PIN      -1

    #define CAM_PWDN_PIN   -1
    #define CAM_RESET_PIN  -1
    #define CAM_XCLK_PIN   -1
    #define CAM_SIOD_PIN   -1
    #define CAM_SIOC_PIN   -1
    #define CAM_Y9_PIN     -1
    #define CAM_Y8_PIN     -1
    #define CAM_Y7_PIN     -1
    #define CAM_Y6_PIN     -1
    #define CAM_Y5_PIN     -1
    #define CAM_Y4_PIN     -1
    #define CAM_Y3_PIN     -1
    #define CAM_Y2_PIN     -1
    #define CAM_VSYNC_PIN  -1
    #define CAM_HREF_PIN   -1
    #define CAM_PCLK_PIN   -1

    #define MODEM_TX_PIN        -1
    #define MODEM_RX_PIN        -1
    #define MODEM_PWRKEY_PIN    -1
    #define MODEM_POWERON_PIN   -1
    #define MODEM_RESET_PIN     -1
    #define MODEM_BAUD           0

#else
    #error "No board selected. Set BOARD_DEVKIT, BOARD_LILYGO or BOARD_NATIVE in platformio.ini build_flags."
#endif
