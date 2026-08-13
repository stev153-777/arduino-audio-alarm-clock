#pragma once

#include <Arduino.h>
#include <RTClib.h>

class AudioManager;

constexpr uint8_t MAX_ALARMS = 5;

// Bitmaske:
// Bit 0 = Montag
// ...
// Bit 6 = Sonntag
enum AlarmDay : uint8_t
{
    MONDAY    = 1 << 0,
    TUESDAY   = 1 << 1,
    WEDNESDAY = 1 << 2,
    THURSDAY  = 1 << 3,
    FRIDAY    = 1 << 4,
    SATURDAY  = 1 << 5,
    SUNDAY    = 1 << 6
};

constexpr uint8_t ALL_DAYS =
    MONDAY |
    TUESDAY |
    WEDNESDAY |
    THURSDAY |
    FRIDAY |
    SATURDAY |
    SUNDAY;

struct AlarmConfig
{
    uint8_t hour = 7;
    uint8_t minute = 0;

    uint8_t volume = 15;

    uint8_t repeatDays = ALL_DAYS;

    bool enabled = false;
};

struct NextAlarmInfo
{
    bool valid = false;

    uint8_t alarmIndex = 0;

    uint8_t hour = 0;
    uint8_t minute = 0;

    uint8_t daysAhead = 0;

    uint32_t minutesUntil = 0;
};

class AlarmManager
{
public:
    void begin(AudioManager& audio);

    void update(const DateTime& now);

    AlarmConfig& getAlarm(uint8_t index);
    const AlarmConfig& getAlarm(uint8_t index) const;

    uint8_t getAlarmCount() const;

    bool getNextAlarm(
        const DateTime& now,
        NextAlarmInfo& result
    ) const;

    void stop();
    void snooze(const DateTime& now);

    bool isRinging() const;

    int8_t getActiveAlarmIndex() const;

    bool isSnoozeEnabled() const;
    void setSnoozeEnabled(bool enabled);

private:
    bool shouldTrigger(
        uint8_t index,
        const DateTime& now
    );

    bool isDayEnabled(
        const AlarmConfig& alarm,
        uint8_t rtcDay
    ) const;

    void trigger(uint8_t index);

    uint8_t rtcDayToAlarmBit(uint8_t rtcDay) const;

    AudioManager* _audio = nullptr;

    AlarmConfig _alarms[MAX_ALARMS];

    uint32_t _lastTriggerMinute[MAX_ALARMS];

    int8_t _activeAlarm = -1;

    bool _ringing = false;

    bool _snoozeEnabled = false;
    bool _snoozeActive = false;

    uint32_t _snoozeUntil = 0;

    static constexpr uint8_t SNOOZE_MINUTES = 5;
};