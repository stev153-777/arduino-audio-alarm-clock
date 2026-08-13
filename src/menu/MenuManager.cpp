#include "MenuManager.h"

#include "../rtc/ClockManager.h"
#include "../display/DisplayManager.h"
#include "../buttons/ButtonManager.h"
#include "../alarm/AlarmManager.h"
#include "../audio/AudioManager.h"


static const char* DAY_NAMES[7] =
{
    "Mo",
    "Di",
    "Mi",
    "Do",
    "Fr",
    "Sa",
    "So"
};

static const uint8_t DAY_BITS[7] =
{
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    SUNDAY
};


void MenuManager::begin(
    ClockManager& clock,
    DisplayManager& display,
    ButtonManager& buttons,
    AlarmManager& alarm,
    AudioManager& audio
)
{
    _clock = &clock;
    _display = &display;
    _buttons = &buttons;
    _alarm = &alarm;
    _audio = &audio;
}


bool MenuManager::isOpen() const
{
    return _screen != Screen::CLOSED;
}


void MenuManager::open()
{
    _screen = Screen::MAIN;
    _menuIndex = 0;

    drawMain();
}


void MenuManager::close()
{
    _screen = Screen::CLOSED;
}


void MenuManager::update()
{
    if (_screen == Screen::CLOSED)
    {
        if (_buttons->wasPressed(Button::CENTER))
        {
            open();
        }

        return;
    }

    switch (_screen)
    {
        case Screen::MAIN:
            updateMain();
            break;

        case Screen::SET_TIME:
            updateTime();
            break;

        case Screen::SET_DATE:
            updateDate();
            break;

        case Screen::ALARM_LIST:
            updateAlarmList();
            break;

        case Screen::ALARM_MENU:
            updateAlarmMenu();
            break;

        case Screen::ALARM_TIME:
            updateAlarmTime();
            break;

        case Screen::ALARM_VOLUME:
            updateAlarmVolume();
            break;

        case Screen::ALARM_REPEAT:
            updateAlarmRepeat();
            break;

        case Screen::ALARM_ENABLED:
            updateAlarmEnabled();
            break;

        case Screen::SNOOZE:
            updateSnooze();
            break;

        default:
            break;
    }
}


// ============================================================================
// Hauptmenü
// ============================================================================

void MenuManager::drawMain()
{
    static const char* ITEMS[] =
    {
        "Uhrzeit",
        "Datum",
        "Alarme",
        "Snooze"
    };

    _display->showMenuItem(
        "Einstellungen",
        ITEMS[_menuIndex]
    );
}


void MenuManager::updateMain()
{
    if (_buttons->wasPressed(Button::UP))
    {
        _menuIndex =
            (_menuIndex == 0)
                ? MAIN_MENU_COUNT - 1
                : _menuIndex - 1;

        drawMain();
    }

    if (_buttons->wasPressed(Button::DOWN))
    {
        _menuIndex =
            (_menuIndex + 1) %
            MAIN_MENU_COUNT;

        drawMain();
    }

    if (_buttons->wasPressed(Button::LEFT))
    {
        close();
        return;
    }

    if (!_buttons->wasPressed(Button::CENTER))
    {
        return;
    }

    DateTime now = _clock->now();

    switch (_menuIndex)
    {
        case 0:
            _editHour = now.hour();
            _editMinute = now.minute();
            _editHourSelected = true;

            _screen = Screen::SET_TIME;

            _display->showTimeEditor(
                "Uhrzeit",
                _editHour,
                _editMinute,
                _editHourSelected
            );
            break;

        case 1:
            _editYear = now.year();
            _editMonth = now.month();
            _editDay = now.day();

            _dateField = 0;

            _screen = Screen::SET_DATE;

            _display->showDateEditor(
                _editYear,
                _editMonth,
                _editDay,
                _dateField
            );
            break;

        case 2:
            _selectedAlarm = 0;

            _screen = Screen::ALARM_LIST;

            drawAlarmList();
            break;

        case 3:
            _editSnooze =
                _alarm->isSnoozeEnabled();

            _screen = Screen::SNOOZE;

            drawSnooze();
            break;
    }
}


// ============================================================================
// Uhrzeit einstellen
// ============================================================================

void MenuManager::updateTime()
{
    bool changed = false;

    if (_buttons->wasPressed(Button::LEFT))
    {
        _editHourSelected = true;
        changed = true;
    }

    if (_buttons->wasPressed(Button::RIGHT))
    {
        _editHourSelected = false;
        changed = true;
    }

    if (_buttons->wasPressed(Button::UP))
    {
        if (_editHourSelected)
        {
            _editHour =
                (_editHour + 1) % 24;
        }
        else
        {
            _editMinute =
                (_editMinute + 1) % 60;
        }

        changed = true;
    }

    if (_buttons->wasPressed(Button::DOWN))
    {
        if (_editHourSelected)
        {
            _editHour =
                (_editHour == 0)
                    ? 23
                    : _editHour - 1;
        }
        else
        {
            _editMinute =
                (_editMinute == 0)
                    ? 59
                    : _editMinute - 1;
        }

        changed = true;
    }

    if (_buttons->wasPressed(Button::CENTER))
    {
        DateTime old =
            _clock->now();

        _clock->setDateTime(
            old.year(),
            old.month(),
            old.day(),
            _editHour,
            _editMinute,
            0
        );

        _screen = Screen::MAIN;

        drawMain();
        return;
    }

    if (changed)
    {
        _display->showTimeEditor(
            "Uhrzeit",
            _editHour,
            _editMinute,
            _editHourSelected
        );
    }
}


// ============================================================================
// Datum
// ============================================================================

uint8_t MenuManager::daysInMonth(
    uint16_t year,
    uint8_t month
) const
{
    switch (month)
    {
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;

        case 2:
        {
            bool leap =
                (year % 4 == 0) &&
                (
                    year % 100 != 0 ||
                    year % 400 == 0
                );

            return leap ? 29 : 28;
        }

        default:
            return 31;
    }
}


void MenuManager::clampDate()
{
    uint8_t maxDay =
        daysInMonth(
            _editYear,
            _editMonth
        );

    if (_editDay > maxDay)
    {
        _editDay = maxDay;
    }
}


void MenuManager::updateDate()
{
    bool changed = false;

    if (_buttons->wasPressed(Button::LEFT))
    {
        if (_dateField > 0)
        {
            _dateField--;
        }

        changed = true;
    }

    if (_buttons->wasPressed(Button::RIGHT))
    {
        if (_dateField < 2)
        {
            _dateField++;
        }

        changed = true;
    }

    if (_buttons->wasPressed(Button::UP))
    {
        switch (_dateField)
        {
            case 0:
            {
                uint8_t maxDay =
                    daysInMonth(
                        _editYear,
                        _editMonth
                    );

                _editDay++;

                if (_editDay > maxDay)
                {
                    _editDay = 1;
                }

                break;
            }

            case 1:
                _editMonth++;

                if (_editMonth > 12)
                {
                    _editMonth = 1;
                }

                clampDate();
                break;

            case 2:
                _editYear++;

                if (_editYear > 2099)
                {
                    _editYear = 2000;
                }

                clampDate();
                break;
        }

        changed = true;
    }

    if (_buttons->wasPressed(Button::DOWN))
    {
        switch (_dateField)
        {
            case 0:
            {
                uint8_t maxDay =
                    daysInMonth(
                        _editYear,
                        _editMonth
                    );

                _editDay =
                    (_editDay <= 1)
                        ? maxDay
                        : _editDay - 1;

                break;
            }

            case 1:
                _editMonth =
                    (_editMonth <= 1)
                        ? 12
                        : _editMonth - 1;

                clampDate();
                break;

            case 2:
                _editYear =
                    (_editYear <= 2000)
                        ? 2099
                        : _editYear - 1;

                clampDate();
                break;
        }

        changed = true;
    }

    if (_buttons->wasPressed(Button::CENTER))
    {
        DateTime old =
            _clock->now();

        _clock->setDateTime(
            _editYear,
            _editMonth,
            _editDay,
            old.hour(),
            old.minute(),
            old.second()
        );

        _screen = Screen::MAIN;

        drawMain();
        return;
    }

    if (changed)
    {
        _display->showDateEditor(
            _editYear,
            _editMonth,
            _editDay,
            _dateField
        );
    }
}


// ============================================================================
// Alarm-Auswahl
// ============================================================================

void MenuManager::drawAlarmList()
{
    const AlarmConfig& alarm =
        _alarm->getAlarm(
            _selectedAlarm
        );

    char title[17];
    char value[17];

    snprintf(
        title,
        sizeof(title),
        "Alarm %u/%u",
        _selectedAlarm + 1,
        _alarm->getAlarmCount()
    );

    snprintf(
        value,
        sizeof(value),
        "%02u:%02u %s",
        alarm.hour,
        alarm.minute,
        alarm.enabled
            ? "EIN"
            : "AUS"
    );

    _display->showMenuItem(
        title,
        value
    );
}


void MenuManager::updateAlarmList()
{
    if (_buttons->wasPressed(Button::UP))
    {
        _selectedAlarm =
            (_selectedAlarm == 0)
                ? _alarm->getAlarmCount() - 1
                : _selectedAlarm - 1;

        drawAlarmList();
    }

    if (_buttons->wasPressed(Button::DOWN))
    {
        _selectedAlarm =
            (_selectedAlarm + 1) %
            _alarm->getAlarmCount();

        drawAlarmList();
    }

    if (_buttons->wasPressed(Button::LEFT))
    {
        _screen = Screen::MAIN;

        drawMain();
        return;
    }

    if (_buttons->wasPressed(Button::CENTER))
    {
        _alarmMenuIndex = 0;

        _screen = Screen::ALARM_MENU;

        drawAlarmMenu();
    }
}


// ============================================================================
// Alarm-Untermenü
// ============================================================================

void MenuManager::drawAlarmMenu()
{
    static const char* ITEMS[] =
    {
        "Zeit",
        "Lautstaerke",
        "Wochentage",
        "An / Aus",
        "Zurueck"
    };

    char title[17];

    snprintf(
        title,
        sizeof(title),
        "Alarm %u",
        _selectedAlarm + 1
    );

    _display->showMenuItem(
        title,
        ITEMS[_alarmMenuIndex]
    );
}


void MenuManager::updateAlarmMenu()
{
    if (_buttons->wasPressed(Button::UP))
    {
        _alarmMenuIndex =
            (_alarmMenuIndex == 0)
                ? ALARM_MENU_COUNT - 1
                : _alarmMenuIndex - 1;

        drawAlarmMenu();
    }

    if (_buttons->wasPressed(Button::DOWN))
    {
        _alarmMenuIndex =
            (_alarmMenuIndex + 1) %
            ALARM_MENU_COUNT;

        drawAlarmMenu();
    }

    if (_buttons->wasPressed(Button::LEFT))
    {
        _screen = Screen::ALARM_LIST;

        drawAlarmList();
        return;
    }

    if (!_buttons->wasPressed(Button::CENTER))
    {
        return;
    }

    AlarmConfig& alarm =
        _alarm->getAlarm(
            _selectedAlarm
        );

    switch (_alarmMenuIndex)
    {
        case 0:
            _editHour =
                alarm.hour;

            _editMinute =
                alarm.minute;

            _editHourSelected =
                true;

            _screen =
                Screen::ALARM_TIME;

            drawAlarmTime();
            break;

        case 1:
            _editVolume =
                alarm.volume;

            _screen =
                Screen::ALARM_VOLUME;

            drawAlarmVolume();
            break;

        case 2:
            _editRepeatDays =
                alarm.repeatDays;

            _repeatDayIndex = 0;

            _screen =
                Screen::ALARM_REPEAT;

            drawAlarmRepeat();
            break;

        case 3:
            _editEnabled =
                alarm.enabled;

            _screen =
                Screen::ALARM_ENABLED;

            drawAlarmEnabled();
            break;

        case 4:
            _screen =
                Screen::ALARM_LIST;

            drawAlarmList();
            break;
    }
}


// ============================================================================
// Alarmzeit
// ============================================================================

void MenuManager::drawAlarmTime()
{
    char title[17];

    snprintf(
        title,
        sizeof(title),
        "Alarm %u Zeit",
        _selectedAlarm + 1
    );

    _display->showTimeEditor(
        title,
        _editHour,
        _editMinute,
        _editHourSelected
    );
}


void MenuManager::updateAlarmTime()
{
    bool changed = false;

    if (_buttons->wasPressed(Button::LEFT))
    {
        _editHourSelected = true;
        changed = true;
    }

    if (_buttons->wasPressed(Button::RIGHT))
    {
        _editHourSelected = false;
        changed = true;
    }

    if (_buttons->wasPressed(Button::UP))
    {
        if (_editHourSelected)
        {
            _editHour =
                (_editHour + 1) % 24;
        }
        else
        {
            _editMinute =
                (_editMinute + 1) % 60;
        }

        changed = true;
    }

    if (_buttons->wasPressed(Button::DOWN))
    {
        if (_editHourSelected)
        {
            _editHour =
                (_editHour == 0)
                    ? 23
                    : _editHour - 1;
        }
        else
        {
            _editMinute =
                (_editMinute == 0)
                    ? 59
                    : _editMinute - 1;
        }

        changed = true;
    }

    if (_buttons->wasPressed(Button::CENTER))
    {
        AlarmConfig& alarm =
            _alarm->getAlarm(
                _selectedAlarm
            );

        alarm.hour =
            _editHour;

        alarm.minute =
            _editMinute;

        _screen =
            Screen::ALARM_MENU;

        drawAlarmMenu();
        return;
    }

    if (changed)
    {
        drawAlarmTime();
    }
}


// ============================================================================
// Lautstärke
// ============================================================================

void MenuManager::drawAlarmVolume()
{
    char title[17];
    char value[17];

    snprintf(
        title,
        sizeof(title),
        "Alarm %u Lautst",
        _selectedAlarm + 1
    );

    snprintf(
        value,
        sizeof(value),
        "%u / 30",
        _editVolume
    );

    _display->showMenuItem(
        title,
        value
    );
}


void MenuManager::updateAlarmVolume()
{
    if (_buttons->wasPressed(Button::UP))
    {
        if (_editVolume < 30)
        {
            _editVolume++;
        }

        drawAlarmVolume();
    }

    if (_buttons->wasPressed(Button::DOWN))
    {
        if (_editVolume > 0)
        {
            _editVolume--;
        }

        drawAlarmVolume();
    }

    if (_buttons->wasPressed(Button::LEFT))
    {
        _screen =
            Screen::ALARM_MENU;

        drawAlarmMenu();
        return;
    }

    if (_buttons->wasPressed(Button::CENTER))
    {
        _alarm->getAlarm(
            _selectedAlarm
        ).volume =
            _editVolume;

        _screen =
            Screen::ALARM_MENU;

        drawAlarmMenu();
    }
}


// ============================================================================
// Wochentage
// ============================================================================

void MenuManager::drawAlarmRepeat()
{
    bool enabled =
        (_editRepeatDays &
         DAY_BITS[_repeatDayIndex])
        != 0;

    _display->showRepeatEditor(
        _selectedAlarm + 1,
        DAY_NAMES[_repeatDayIndex],
        enabled
    );
}


void MenuManager::updateAlarmRepeat()
{
    if (_buttons->wasPressed(Button::LEFT))
    {
        _repeatDayIndex =
            (_repeatDayIndex == 0)
                ? 6
                : _repeatDayIndex - 1;

        drawAlarmRepeat();
    }

    if (_buttons->wasPressed(Button::RIGHT))
    {
        _repeatDayIndex =
            (_repeatDayIndex + 1) %
            7;

        drawAlarmRepeat();
    }

    if (
        _buttons->wasPressed(Button::UP) ||
        _buttons->wasPressed(Button::DOWN)
    )
    {
        _editRepeatDays ^=
            DAY_BITS[_repeatDayIndex];

        drawAlarmRepeat();
    }

    if (_buttons->wasPressed(Button::CENTER))
    {
        _alarm->getAlarm(
            _selectedAlarm
        ).repeatDays =
            _editRepeatDays;

        _screen =
            Screen::ALARM_MENU;

        drawAlarmMenu();
    }
}


// ============================================================================
// Alarm EIN / AUS
// ============================================================================

void MenuManager::drawAlarmEnabled()
{
    char title[17];

    snprintf(
        title,
        sizeof(title),
        "Alarm %u Status",
        _selectedAlarm + 1
    );

    _display->showMenuItem(
        title,
        _editEnabled
            ? "EIN"
            : "AUS"
    );
}


void MenuManager::updateAlarmEnabled()
{
    if (
        _buttons->wasPressed(Button::UP) ||
        _buttons->wasPressed(Button::DOWN)
    )
    {
        _editEnabled =
            !_editEnabled;

        drawAlarmEnabled();
    }

    if (_buttons->wasPressed(Button::LEFT))
    {
        _screen =
            Screen::ALARM_MENU;

        drawAlarmMenu();
        return;
    }

    if (_buttons->wasPressed(Button::CENTER))
    {
        _alarm->getAlarm(
            _selectedAlarm
        ).enabled =
            _editEnabled;

        _screen =
            Screen::ALARM_MENU;

        drawAlarmMenu();
    }
}


// ============================================================================
// Snooze
// ============================================================================

void MenuManager::drawSnooze()
{
    _display->showMenuItem(
        "Snooze",
        _editSnooze
            ? "EIN"
            : "AUS"
    );
}


void MenuManager::updateSnooze()
{
    if (
        _buttons->wasPressed(Button::UP) ||
        _buttons->wasPressed(Button::DOWN)
    )
    {
        _editSnooze =
            !_editSnooze;

        drawSnooze();
    }

    if (_buttons->wasPressed(Button::LEFT))
    {
        _screen =
            Screen::MAIN;

        drawMain();
        return;
    }

    if (_buttons->wasPressed(Button::CENTER))
    {
        _alarm->setSnoozeEnabled(
            _editSnooze
        );

        _screen =
            Screen::MAIN;

        drawMain();
    }
}