#pragma once
#include "INotifier.h"

// ---------------------------------------------------------------------------
// NtfyNotifier – sends push notifications via ntfy.sh
//
// Transport is selected at compile time via NTFY_TRANSPORT in config.h:
//   0 = disabled
//   1 = WiFi only
//   2 = modem only
//   3 = both simultaneously
// ---------------------------------------------------------------------------
class NtfyNotifier : public INotifier {
   public:
    // server: e.g. "https://ntfy.sh"
    // topic:  your private topic name
    NtfyNotifier(const char* server, const char* topic);

    void sendAlert(const char* sensorId) override;
    void sendStatus(const char* status) override;

   private:
    const char* _server;
    const char* _topic;

    bool sendMessage(const char* title, const char* message, const char* priority = "default",
                     const char* tags = "");

#if defined(ARDUINO) && defined(BOARD_LILYGO)
    bool sendViaModem(const char* url, const char* title, const char* message, const char* priority,
                      const char* tags);
#endif

#if defined(ARDUINO) && (defined(BOARD_DEVKIT) || defined(BOARD_LILYGO))
    bool sendViaWifi(const char* url, const char* title, const char* message, const char* priority,
                     const char* tags);
#endif
};
