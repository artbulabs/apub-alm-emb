#include "CompositeNotifier.h"

void CompositeNotifier::add(INotifier* notifier) {
    _notifiers.push_back(notifier);
}

void CompositeNotifier::sendAlert(const char* sensorId) {
    for (auto* notifier : _notifiers) {
        notifier->sendAlert(sensorId);
    }
}

void CompositeNotifier::sendStatus(const char* status) {
    for (auto* notifier : _notifiers) {
        notifier->sendStatus(status);
    }
}
