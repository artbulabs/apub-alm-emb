#include "StubNotifier.h"

void StubNotifier::sendAlert(const char* sensorId) {
    _lastAlertSensorId = sensorId;
    _alertCallCount++;
}

void StubNotifier::sendStatus(const char* status) {
    _lastStatus = status;
}