#include "ClockManager.h"

bool ClockManager::begin()
{
    if (!_rtc.begin())
    {
        return false;
    }

    // RTC nur bei verlorenem Backup auf Compile-Zeit setzen.
    if (_rtc.lostPower())
    {
        _rtc.adjust(
            DateTime(
                F(__DATE__),
                F(__TIME__)
            )
        );
    }

    return true;
}

DateTime ClockManager::now()
{
    return _rtc.now();
}

void ClockManager::setDateTime(
    uint16_t year,
    uint8_t month,
    uint8_t day,
    uint8_t hour,
    uint8_t minute,
    uint8_t second
)
{
    _rtc.adjust(
        DateTime(
            year,
            month,
            day,
            hour,
            minute,
            second
        )
    );
}