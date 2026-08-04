#pragma once
#include <cstdint>

// Alarm stystem states
enum class AlarmState {
    Disarmed,
    Armed,
    Triggered
};

// Triggered event data: which sensor fired
struct SensorEvent {
    const char* sensorId;  // e.g. "PIR1", "REED1"
};

class AlarmStateMachine {
private:
    AlarmState _state;
    const char* _lastTriggeredSensor;

public:
    explicit AlarmStateMachine();

    // State transition triggers
    void onArm();
    void onDisarm();
    void onSensorTriggered(const char* sensorId);
    void onNotificationSent();
    void onNotificationFailed();

    // Queries
    AlarmState getState() const;
    const char* getLastTriggeredSensor() const;
    const char* getStateName() const;
};