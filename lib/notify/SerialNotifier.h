#pragma once
#include "INotifier.h"

// Logs notifications to Serial port.
// Used on DevKit for development and as a permanent debug channel.
class SerialNotifier : public INotifier {
public:
    void sendAlert(const char* sensorId) override;
    void sendStatus(const char* status) override;
};