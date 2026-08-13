#pragma once

#include <Arduino.h>

enum class Button
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    CENTER,
    STOP
};

class ButtonManager
{
public:
    void begin();
    void update();

    bool wasPressed(Button button);

private:
    static constexpr uint8_t BUTTON_COUNT = 6;

    static constexpr unsigned long DEBOUNCE_MS = 40;

    bool _currentState[BUTTON_COUNT];
    bool _lastRawState[BUTTON_COUNT];
    bool _pressedEvent[BUTTON_COUNT];

    unsigned long _lastChangeTime[BUTTON_COUNT];

    uint8_t getPin(Button button) const;
};