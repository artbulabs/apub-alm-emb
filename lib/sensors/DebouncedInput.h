#pragma once
#include <cstdint>

// Callback type: a function that reads the raw pin state.
// Returns true = HIGH, false = LOW.
// Using a callback instead of direct digitalRead() makes the class
// hardware-independent and fully testable on native (PC) environment.
using PinReader = bool (*)();

class DebouncedInput {
   public:
    // pinReader  - function that returns current raw pin state
    // debounceMs - how long the signal must be stable to count as changed
    explicit DebouncedInput(PinReader pinReader, uint32_t debounceMs = 50);

    // Call this regularly (e.g. in loop()) with current time in milliseconds.
    // Returns true if the stable state changed since last call.
    bool update(uint32_t nowMs);

    // Returns the last stable (debounced) state.
    bool getState() const;

   private:
    PinReader _pinReader;
    uint32_t _debounceMs;
    bool _stableState;
    bool _lastRawState;
    uint32_t _lastChangeMs;
};
