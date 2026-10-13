#pragma once

// ---------------------------------------------------------------------------
// config.h – system behaviour configuration
// Edit this file to enable/disable features and set operational parameters
// ---------------------------------------------------------------------------

// --- Notification channels ---
// Set to true to enable, false to disable
// Multiple channels can be active simultaneously (CompositeNotifier)
static constexpr bool  NTFY_ENABLED = true;
static constexpr bool  SMS_ENABLED = false;
static constexpr bool  SERIAL_NOTIFY_ENABLED = true;

// --- ntfy.sh transport (LILYGO only) ---
// NTFY_TRANSPORT values:
//   0 = disabled
//   1 = WiFi only
//   2 = modem only
//   3 = both simultaneously
#define NTFY_TRANSPORT 1

// --- Sensor debounce time (ms) ---
static constexpr uint16_t DEBOUNCE_MS = 50;

// --- Alarm timeouts ---
static constexpr uint8_t  NOTIFICATION_RETRY_COUNT = 3;
static constexpr uint32_t MODEM_BOOT_TIMEOUT_MS = 15000;
static constexpr uint32_t NTFY_POLL_INTERVAL_MS = 10000;

// --- APN configuration for Orange Poland ---
#define ORANGE_APN "internet"
#define ORANGE_USER "internet"
#define ORANGE_PASS "internet"
