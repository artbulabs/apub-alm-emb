#pragma once
#include <cstdint>

// Alarm stystem states
enum class AlarmState { Disarmed, Armed, Triggered };

// Triggered event data: which sensor fired
struct SensorEvent {
    const char* sensorId;  // e.g. "PIR1", "REED1"
};

class AlarmStateMachine {
   public:
    explicit AlarmStateMachine() = default;

    // State transition triggers
    void onArm();
    void onDisarm();
    void onSensorTriggered(const char* sensorId);
    void onNotificationSent();
    void onNotificationFailed();

    // Queries
    [[nodiscard]] AlarmState getState() const;
    [[nodiscard]] const char* getLastTriggeredSensor() const;
    [[nodiscard]] const char* getStateName() const;

   private:
    AlarmState _state{AlarmState::Disarmed};
    const char* _lastTriggeredSensor{nullptr};
};
