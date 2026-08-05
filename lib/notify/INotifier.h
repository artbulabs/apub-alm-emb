#pragma once

// ---------------------------------------------------------------------------
// INotifier - Strategy pattern interface for alarm notifications.
// ---------------------------------------------------------------------------
class INotifier {
public:
    virtual ~INotifier() = default;

    // Called when a sensor triggers the alarm.
    // sensorId: e.g. "PIR1", "REED1"
    virtual void sendAlert(const char* sensorId) = 0;

    // Called when the alarm is armed/disarmed (optional status update).
    virtual void sendStatus(const char* status) = 0;
};
