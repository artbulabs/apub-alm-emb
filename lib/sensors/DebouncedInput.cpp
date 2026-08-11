#include "DebouncedInput.h"

DebouncedInput::DebouncedInput(PinReader pinReader, uint32_t debounceMs)
    : _pinReader(pinReader), _debounceMs(debounceMs) {}

bool DebouncedInput::update(uint32_t nowMs) {
    bool rawState = _pinReader();

    if (rawState != _lastRawState) {
        // Raw state changed – reset the debounce timer
        _lastRawState = rawState;
        _lastChangeMs = nowMs;
        return false;
    }

    if (rawState != _stableState) {
        if ((nowMs - _lastChangeMs) >= _debounceMs) {
            // Signal has been stable long enough – accept the change
            _stableState = rawState;
            return true;
        }
    }

    return false;
}

bool DebouncedInput::getState() const {
    return _stableState;
}
