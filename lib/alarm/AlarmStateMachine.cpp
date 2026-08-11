#include "AlarmStateMachine.h"

void AlarmStateMachine::onArm() {
    if (_state == AlarmState::Disarmed) {
        _state = AlarmState::Armed;
    }
}

void AlarmStateMachine::onDisarm() {
    if (_state == AlarmState::Armed) {
        _state = AlarmState::Disarmed;
    }
}

void AlarmStateMachine::onSensorTriggered(const char* sensorId) {
    if (_state == AlarmState::Armed) {
        _state = AlarmState::Triggered;
        _lastTriggeredSensor = sensorId;
    }
}

void AlarmStateMachine::onNotificationSent() {
    if (_state == AlarmState::Triggered) {
        _state = AlarmState::Armed;
    }
}

void AlarmStateMachine::onNotificationFailed() {
    if (_state == AlarmState::Triggered) {
        _state = AlarmState::Armed;
    }
}

AlarmState AlarmStateMachine::getState() const {
    return _state;
}

const char* AlarmStateMachine::getLastTriggeredSensor() const {
    return _lastTriggeredSensor;
}

const char* AlarmStateMachine::getStateName() const {
    switch (_state) {
        case AlarmState::Disarmed:
            return "Disarmed";
        case AlarmState::Armed:
            return "Armed";
        case AlarmState::Triggered:
            return "Triggered";
        default:
            return "Unknown";
    }
}
