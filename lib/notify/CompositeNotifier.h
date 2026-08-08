#pragma once
#include <vector>
#include "INotifier.h"

// Composite pattern: forwards notifications to all registered notifiers.
// Allows sending alerts via multiple channels simultaneously.
class CompositeNotifier : public INotifier {
   public:
    // Takes raw pointer – ownership stays with caller.
    void add(INotifier* notifier);

    void sendAlert(const char* sensorId) override;
    void sendStatus(const char* status) override;

   private:
    std::vector<INotifier*> _notifiers;
};
