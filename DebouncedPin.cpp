//
// Created by lukkens on 2022-06-08.
//

#include "DebouncedPin.h"

DebouncedPin::DebouncedPin(hwlib::pin_in& wrappedPin, uint64_t minDebounceTimeUS, bool debounceHigh, bool debounceLow)
: _wrappedPin(wrappedPin), _minDebounceTimeUS(minDebounceTimeUS), _state(wrappedPin.read()), _debouncedState(_state),
_debounceHigh(debounceHigh), _debounceLow(debounceLow)
{}

bool DebouncedPin::read()
{
    return _debouncedState;
}

void DebouncedPin::refresh()
{
    _wrappedPin.refresh();

    bool newState = _wrappedPin.read();
    if (_state != newState)
    {
        if (newState && !_debounceHigh || !newState && !_debounceLow)
            _debouncedState = newState;
        else
        {
            _state = newState;
            _lastStateChangeTime = hwlib::now_us();
        }

        return;
    }

    if (GetCurrentStateTimeMS() < _minDebounceTimeUS || _state == _debouncedState)
        return;

    _debouncedState = _state;
}

uint64_t DebouncedPin::GetCurrentStateTimeMS() const
{
    return hwlib::now_us() - _lastStateChangeTime;
}
