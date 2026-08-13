#include "ButtonManager.h"
#include "pins.h"

uint8_t ButtonManager::getPin(
    Button button
) const
{
    switch (button)
    {
        case Button::UP:
            return BUTTON_UP;

        case Button::DOWN:
            return BUTTON_DOWN;

        case Button::LEFT:
            return BUTTON_LEFT;

        case Button::RIGHT:
            return BUTTON_RIGHT;

        case Button::CENTER:
            return BUTTON_CENTER;

        case Button::STOP:
            return BUTTON_STOP;
    }

    return BUTTON_CENTER;
}

void ButtonManager::begin()
{
    for (uint8_t i = 0;
         i < BUTTON_COUNT;
         i++)
    {
        Button button =
            static_cast<Button>(i);

        uint8_t pin =
            getPin(button);

        pinMode(
            pin,
            INPUT_PULLUP
        );

        bool state =
            digitalRead(pin);

        _currentState[i] =
            state;

        _lastRawState[i] =
            state;

        _pressedEvent[i] =
            false;

        _lastChangeTime[i] =
            0;
    }
}

void ButtonManager::update()
{
    for (uint8_t i = 0;
         i < BUTTON_COUNT;
         i++)
    {
        Button button =
            static_cast<Button>(i);

        uint8_t pin =
            getPin(button);

        bool rawState =
            digitalRead(pin);

        if (
            rawState !=
            _lastRawState[i]
        )
        {
            _lastRawState[i] =
                rawState;

            _lastChangeTime[i] =
                millis();
        }

        if (
            millis() -
            _lastChangeTime[i]
                >= DEBOUNCE_MS
            &&
            rawState !=
            _currentState[i]
        )
        {
            _currentState[i] =
                rawState;

            if (
                _currentState[i] ==
                LOW
            )
            {
                _pressedEvent[i] =
                    true;
            }
        }
    }
}

bool ButtonManager::wasPressed(
    Button button
)
{
    uint8_t index =
        static_cast<uint8_t>(button);

    if (_pressedEvent[index])
    {
        _pressedEvent[index] =
            false;

        return true;
    }

    return false;
}