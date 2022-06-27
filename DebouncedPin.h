//
// Created by lukkens on 2022-06-08.
//

#ifndef IPASS_NFC_DEVICE_DEBOUNCEDPIN_H
#define IPASS_NFC_DEVICE_DEBOUNCEDPIN_H

#include <hwlib.hpp>

class DebouncedPin : hwlib::pin_in
{
public:
    explicit DebouncedPin(hwlib::pin_in &wrappedPin, uint64_t minDebounceTimeUS, bool debounceHigh = true, bool debounceLow = true);

    bool read() override;
    void refresh() override;

    // Gets how long the current state has not changed.
    [[nodiscard]]
    uint64_t GetCurrentStateTimeMS() const;

private:
    bool _debounceHigh;
    bool _debounceLow;

    bool _state;
    bool _debouncedState;

    uint64_t _lastStateChangeTime = 0;
    uint64_t _minDebounceTimeUS = 0;

    hwlib::pin_in &_wrappedPin;
};


#endif //IPASS_NFC_DEVICE_DEBOUNCEDPIN_H
