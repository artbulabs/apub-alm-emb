#pragma once

// ============================================================
// Board selection is done via compile-time macros in
// platformio.ini:
//   build_flags = -D BOARD_DEVKIT
//   build_flags = -D BOARD_LILYGO
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
    #define REED1_PIN       -1

#elif defined(BOARD_NATIVE)
    // --- Native (PC/Linux) ---
    // Used for unit testing only. No GPIO available.

    #define BOARD_NAME     "Native (unit tests)"
    #define HAS_MODEM      0

    #define PIR1_PIN       -1
    #define PIR2_PIN       -1
    #define PIR3_PIN       -1
    #define REED1_PIN       -1

#else
    #error "No board selected. Set BOARD_DEVKIT, BOARD_LILYGO or BOARD_NATIVE in platformio.ini build_flags."
#endif
