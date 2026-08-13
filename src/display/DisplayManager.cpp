#include "DisplayManager.h"
#include "pins.h"

DisplayManager::DisplayManager()
    : _lcd(
        LCD_RS,
        LCD_E,
        LCD_D4,
        LCD_D5,
        LCD_D6,
        LCD_D7
    )
{
}

void DisplayManager::begin()
{
    _lcd.begin(16, 2);

    _lcd.clear();

    printLine(
        0,
        "Audio Alarm"
    );

    printLine(
        1,
        "Clock startet..."
    );

    delay(1000);

    _lcd.clear();
}

void DisplayManager::printTwoDigits(
    uint8_t value
)
{
    if (value < 10)
    {
        _lcd.print('0');
    }

    _lcd.print(value);
}

void DisplayManager::printLine(
    uint8_t row,
    const char* text
)
{
    _lcd.setCursor(0, row);

    uint8_t i = 0;

    while (
        text[i] != '\0' &&
        i < 16
    )
    {
        _lcd.print(text[i]);
        i++;
    }

    while (i < 16)
    {
        _lcd.print(' ');
        i++;
    }
}

void DisplayManager::showHomeScreen(
    const DateTime& now,
    bool nextAlarmValid,
    uint8_t nextAlarmNumber,
    uint8_t nextHour,
    uint8_t nextMinute,
    uint8_t daysAhead
)
{
    char line1[17];
    char line2[17];

    // RTClib:
    // 0 = Sonntag
    // 1 = Montag
    // ...
    // 6 = Samstag
    static const char* WEEKDAYS[7] =
    {
        "So",
        "Mo",
        "Di",
        "Mi",
        "Do",
        "Fr",
        "Sa"
    };

    snprintf(
        line1,
        sizeof(line1),
        "%02u:%02u:%02u   %02u.%02u",
        now.hour(),
        now.minute(),
        now.second(),
        now.day(),
        now.month()
    );

    if (nextAlarmValid)
    {
        if (daysAhead == 0)
        {
            snprintf(
                line2,
                sizeof(line2),
                "A%u %02u:%02u Heute",
                nextAlarmNumber,
                nextHour,
                nextMinute
            );
        }
        else
        {
            // Wochentag des nächsten Alarms berechnen
            uint8_t alarmWeekday =
                (now.dayOfTheWeek() + daysAhead) % 7;

            snprintf(
                line2,
                sizeof(line2),
                "A%u %02u:%02u %s",
                nextAlarmNumber,
                nextHour,
                nextMinute,
                WEEKDAYS[alarmWeekday]
            );
        }
    }
    else
    {
        snprintf(
            line2,
            sizeof(line2),
            "Kein Alarm aktiv"
        );
    }

    printLine(0, line1);
    printLine(1, line2);
}

void DisplayManager::showMenuItem(
    const char* title,
    const char* value
)
{
    printLine(0, title);
    printLine(1, value);
}

void DisplayManager::showTimeEditor(
    const char* title,
    uint8_t hour,
    uint8_t minute,
    bool editingHour
)
{
    char line[17];

    if (editingHour)
    {
        snprintf(
            line,
            sizeof(line),
            ">%02u: %02u",
            hour,
            minute
        );
    }
    else
    {
        snprintf(
            line,
            sizeof(line),
            " %02u:>%02u",
            hour,
            minute
        );
    }

    printLine(0, title);
    printLine(1, line);
}

void DisplayManager::showDateEditor(
    uint16_t year,
    uint8_t month,
    uint8_t day,
    uint8_t selectedField
)
{
    char line[17];

    switch (selectedField)
    {
        case 0:
            snprintf(
                line,
                sizeof(line),
                ">%02u.%02u.%04u",
                day,
                month,
                year
            );
            break;

        case 1:
            snprintf(
                line,
                sizeof(line),
                " %02u.>%02u.%04u",
                day,
                month,
                year
            );
            break;

        default:
            snprintf(
                line,
                sizeof(line),
                " %02u.%02u.>%04u",
                day,
                month,
                year
            );
            break;
    }

    printLine(
        0,
        "Datum einstellen"
    );

    printLine(
        1,
        line
    );
}

void DisplayManager::showRepeatEditor(
    uint8_t alarmNumber,
    const char* dayName,
    bool enabled
)
{
    char title[17];
    char line[17];

    snprintf(
        title,
        sizeof(title),
        "Alarm %u Tage",
        alarmNumber
    );

    snprintf(
        line,
        sizeof(line),
        "%s: %s",
        dayName,
        enabled ? "EIN" : "AUS"
    );

    printLine(0, title);
    printLine(1, line);
}

void DisplayManager::showAlarmScreen(
    uint8_t alarmNumber,
    uint8_t hour,
    uint8_t minute,
    bool snoozeEnabled
)
{
    char line1[17];

    snprintf(
        line1,
        sizeof(line1),
        "ALARM %u %02u:%02u",
        alarmNumber,
        hour,
        minute
    );

    printLine(
        0,
        line1
    );

    if (snoozeEnabled)
    {
        printLine(
            1,
            "OK:Snooze STOP"
        );
    }
    else
    {
        printLine(
            1,
            "STOP = Aus"
        );
    }
}