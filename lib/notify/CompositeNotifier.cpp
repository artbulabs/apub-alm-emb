#include "CompositeNotifier.h"

void CompositeNotifier::add(INotifier* notifier) {
    _notifiers.push_back(notifier);
}

void CompositeNotifier::sendAlert(const char* sensorId) {
    for(auto* n: _notifiers) {
        n->sendAlert(sensorId);
    }
}

void CompositeNotifier::sendStatus(const char* status) {
    for(auto* n: _notifiers) {
        n->sendStatus(status);
    }
}