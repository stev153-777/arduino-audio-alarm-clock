#pragma once

#include <Arduino.h>
#include <RTClib.h>

class ClockManager;
class DisplayManager;
class ButtonManager;
class AlarmManager;
class AudioManager;

class MenuManager
{
public:
    void begin(
        ClockManager& clock,
        DisplayManager& display,
        ButtonManager& buttons,
        AlarmManager& alarm,
        AudioManager& audio
    );

    void update();

    bool isOpen() const;

private:
    enum class Screen
    {
        CLOSED,

        MAIN,

        SET_TIME,
        SET_DATE,

        ALARM_LIST,
        ALARM_MENU,

        ALARM_TIME,
        ALARM_VOLUME,
        ALARM_REPEAT,
        ALARM_ENABLED,

        SNOOZE
    };

    void open();
    void close();

    void updateMain();
    void updateTime();
    void updateDate();

    void updateAlarmList();
    void updateAlarmMenu();

    void updateAlarmTime();
    void updateAlarmVolume();
    void updateAlarmRepeat();
    void updateAlarmEnabled();

    void updateSnooze();

    void drawMain();
    void drawAlarmList();
    void drawAlarmMenu();

    void drawAlarmTime();
    void drawAlarmVolume();
    void drawAlarmRepeat();
    void drawAlarmEnabled();

    void drawSnooze();

    uint8_t daysInMonth(
        uint16_t year,
        uint8_t month
    ) const;

    void clampDate();

    ClockManager* _clock = nullptr;
    DisplayManager* _display = nullptr;
    ButtonManager* _buttons = nullptr;
    AlarmManager* _alarm = nullptr;
    AudioManager* _audio = nullptr;

    Screen _screen = Screen::CLOSED;

    uint8_t _menuIndex = 0;

    uint8_t _selectedAlarm = 0;
    uint8_t _alarmMenuIndex = 0;

    uint8_t _editHour = 0;
    uint8_t _editMinute = 0;

    bool _editHourSelected = true;

    uint16_t _editYear = 2026;
    uint8_t _editMonth = 1;
    uint8_t _editDay = 1;

    uint8_t _dateField = 0;

    uint8_t _editVolume = 15;

    uint8_t _editRepeatDays = 0;
    uint8_t _repeatDayIndex = 0;

    bool _editEnabled = false;
    bool _editSnooze = true;

    static constexpr uint8_t MAIN_MENU_COUNT = 4;
    static constexpr uint8_t ALARM_MENU_COUNT = 5;
};