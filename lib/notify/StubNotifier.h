#pragma once
#include "INotifier.h"

// Silent stub implementation for unit testing.
// Records last call for verification in tests.
class StubNotifier : public INotifier {
   public:
    void sendAlert(const char* sensorId) override;
    void sendStatus(const char* status) override;

    // Test helpers
    [[nodiscard]] const char* lastAlertSensorId() const { return _lastAlertSensorId; }
    [[nodiscard]] const char* lastStatus() const { return _lastStatus; }
    [[nodiscard]] int alertCallCount() const { return _alertCallCount; }

   private:
    const char* _lastAlertSensorId = nullptr;
    const char* _lastStatus = nullptr;
    int _alertCallCount = 0;
};
