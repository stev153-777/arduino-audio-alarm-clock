#pragma once

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <RTClib.h>

class DisplayManager
{
public:
    DisplayManager();

    void begin();

    void showHomeScreen(
        const DateTime& now,
        bool nextAlarmValid,
        uint8_t nextAlarmNumber,
        uint8_t nextHour,
        uint8_t nextMinute,
        uint8_t daysAhead
    );

    void showMenuItem(
        const char* title,
        const char* value
    );

    void showTimeEditor(
        const char* title,
        uint8_t hour,
        uint8_t minute,
        bool editingHour
    );

    void showDateEditor(
        uint16_t year,
        uint8_t month,
        uint8_t day,
        uint8_t selectedField
    );

    void showRepeatEditor(
        uint8_t alarmNumber,
        const char* dayName,
        bool enabled
    );

    void showAlarmScreen(
        uint8_t alarmNumber,
        uint8_t hour,
        uint8_t minute,
        bool snoozeEnabled
    );

private:
    void printTwoDigits(uint8_t value);
    void printLine(
        uint8_t row,
        const char* text
    );

    LiquidCrystal _lcd;
};