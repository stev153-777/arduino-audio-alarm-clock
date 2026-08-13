#include "AlarmManager.h"
#include "../audio/AudioManager.h"

void AlarmManager::begin(AudioManager& audio)
{
    _audio = &audio;

    for (uint8_t i = 0; i < MAX_ALARMS; i++)
    {
        _alarms[i].hour = 7;
        _alarms[i].minute = 0;
        _alarms[i].volume = 15;
        _alarms[i].repeatDays = 0;
        _alarms[i].enabled = false;

        _lastTriggerMinute[i] = UINT32_MAX;
    }
}

void AlarmManager::update(const DateTime& now)
{
    if (_ringing)
    {
        return;
    }

    // Snooze wartet auf den neuen Zeitpunkt.
    if (_snoozeActive)
    {
        if (now.unixtime() >= _snoozeUntil)
        {
            _snoozeActive = false;

            if (_activeAlarm >= 0)
            {
                trigger(static_cast<uint8_t>(_activeAlarm));
            }
        }

        return;
    }

    for (uint8_t i = 0; i < MAX_ALARMS; i++)
    {
        if (shouldTrigger(i, now))
        {
            trigger(i);
            return;
        }
    }
}

bool AlarmManager::shouldTrigger(
    uint8_t index,
    const DateTime& now
)
{
    const AlarmConfig& alarm = _alarms[index];

    if (!alarm.enabled)
    {
        return false;
    }

    if (
        now.hour() != alarm.hour ||
        now.minute() != alarm.minute
    )
    {
        return false;
    }

    if (!isDayEnabled(alarm, now.dayOfTheWeek()))
    {
        return false;
    }

    uint32_t currentMinute =
        now.unixtime() / 60UL;

    if (_lastTriggerMinute[index] == currentMinute)
    {
        return false;
    }

    _lastTriggerMinute[index] = currentMinute;

    return true;
}

void AlarmManager::trigger(uint8_t index)
{
    if (_audio == nullptr)
    {
        return;
    }

    _activeAlarm = index;
    _ringing = true;

    _audio->setVolume(
        _alarms[index].volume
    );

    // Momentan benutzen alle Alarme Datei 1.
    _audio->playAlarm(1);
}

void AlarmManager::stop()
{
    if (_audio != nullptr)
    {
        _audio->stopAlarm();
    }

    _ringing = false;

    _snoozeActive = false;
    _snoozeUntil = 0;

    _activeAlarm = -1;
}

void AlarmManager::snooze(const DateTime& now)
{
    if (
        !_ringing ||
        !_snoozeEnabled ||
        _activeAlarm < 0
    )
    {
        return;
    }

    if (_audio != nullptr)
    {
        _audio->stopAlarm();
    }

    _ringing = false;

    _snoozeActive = true;

    _snoozeUntil =
        now.unixtime() +
        static_cast<uint32_t>(SNOOZE_MINUTES) * 60UL;
}

AlarmConfig& AlarmManager::getAlarm(uint8_t index)
{
    if (index >= MAX_ALARMS)
    {
        index = 0;
    }

    return _alarms[index];
}

const AlarmConfig& AlarmManager::getAlarm(uint8_t index) const
{
    if (index >= MAX_ALARMS)
    {
        index = 0;
    }

    return _alarms[index];
}

uint8_t AlarmManager::getAlarmCount() const
{
    return MAX_ALARMS;
}

bool AlarmManager::isRinging() const
{
    return _ringing;
}

int8_t AlarmManager::getActiveAlarmIndex() const
{
    return _activeAlarm;
}

bool AlarmManager::isSnoozeEnabled() const
{
    return _snoozeEnabled;
}

void AlarmManager::setSnoozeEnabled(bool enabled)
{
    _snoozeEnabled = enabled;
}

uint8_t AlarmManager::rtcDayToAlarmBit(uint8_t rtcDay) const
{
    // RTClib:
    // 0 = Sonntag
    // 1 = Montag
    // ...
    // 6 = Samstag

    switch (rtcDay)
    {
        case 0: return SUNDAY;
        case 1: return MONDAY;
        case 2: return TUESDAY;
        case 3: return WEDNESDAY;
        case 4: return THURSDAY;
        case 5: return FRIDAY;
        case 6: return SATURDAY;
        default: return 0;
    }
}

bool AlarmManager::isDayEnabled(
    const AlarmConfig& alarm,
    uint8_t rtcDay
) const
{
    uint8_t dayBit =
        rtcDayToAlarmBit(rtcDay);

    return (alarm.repeatDays & dayBit) != 0;
}

bool AlarmManager::getNextAlarm(
    const DateTime& now,
    NextAlarmInfo& result
) const
{
    result = NextAlarmInfo();

    uint32_t smallestDifference = UINT32_MAX;

    uint16_t currentMinutes =
        now.hour() * 60U +
        now.minute();

    for (uint8_t alarmIndex = 0;
         alarmIndex < MAX_ALARMS;
         alarmIndex++)
    {
        const AlarmConfig& alarm =
            _alarms[alarmIndex];

        // Alarm ausgeschaltet
        if (!alarm.enabled)
        {
            continue;
        }

        // Keine Wochentage ausgewählt
        if (alarm.repeatDays == 0)
        {
            continue;
        }

        uint16_t alarmMinutes =
            alarm.hour * 60U +
            alarm.minute;

        // 0 = heute
        // 7 = gleicher Wochentag nächste Woche
        for (uint8_t daysAhead = 0;
             daysAhead <= 7;
             daysAhead++)
        {
            uint8_t rtcDay =
                (now.dayOfTheWeek() + daysAhead) % 7;

            if (!isDayEnabled(alarm, rtcDay))
            {
                continue;
            }

            int32_t difference =
                static_cast<int32_t>(daysAhead) * 1440L +
                static_cast<int32_t>(alarmMinutes) -
                static_cast<int32_t>(currentMinutes);

            // Heute bereits vorbei bzw. genau jetzt:
            // nach dem nächsten passenden Tag suchen.
            if (difference <= 0)
            {
                continue;
            }

            if (
                static_cast<uint32_t>(difference) <
                smallestDifference
            )
            {
                smallestDifference =
                    static_cast<uint32_t>(difference);

                result.valid = true;
                result.alarmIndex = alarmIndex;
                result.hour = alarm.hour;
                result.minute = alarm.minute;
                result.daysAhead = daysAhead;
                result.minutesUntil =
                    static_cast<uint32_t>(difference);
            }

            // Für diesen Alarm wurde der nächste
            // gültige Termin gefunden.
            break;
        }
    }

    return result.valid;
}