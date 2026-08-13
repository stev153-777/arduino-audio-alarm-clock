#include <Arduino.h>

#include "rtc/ClockManager.h"
#include "display/DisplayManager.h"
#include "audio/AudioManager.h"
#include "buttons/ButtonManager.h"
#include "alarm/AlarmManager.h"
#include "menu/MenuManager.h"

ClockManager clockManager;
DisplayManager displayManager;
AudioManager audioManager;
ButtonManager buttonManager;
AlarmManager alarmManager;
MenuManager menuManager;

uint8_t lastDisplayedSecond = 255;

void fatalError(
    const char* message
)
{
    Serial.println(message);

    while (true)
    {
        delay(1000);
    }
}

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println(
        "============================"
    );
    Serial.println(
        " Arduino Audio Alarm Clock"
    );
    Serial.println(
        "============================"
    );

    // Buttons
    buttonManager.begin();

    Serial.println(
        "Buttons OK"
    );

    // RTC
    if (!clockManager.begin())
    {
        fatalError(
            "RTC Fehler!"
        );
    }

    Serial.println(
        "RTC OK"
    );

    // Display
    displayManager.begin();

    Serial.println(
        "Display OK"
    );

    // Audio
    if (!audioManager.begin())
    {
        fatalError(
            "DFPlayer Fehler!"
        );
    }

    Serial.println(
        "DFPlayer OK"
    );

    // Alarme
    alarmManager.begin(
        audioManager
    );

    /*
    alarmManager.setSnoozeEnabled(
        true
    );
    */

    // Menü
    menuManager.begin(
        clockManager,
        displayManager,
        buttonManager,
        alarmManager,
        audioManager
    );

    Serial.println(
        "System gestartet"
    );
}

void loop()
{
    // ----------------------------------------
    // Buttons
    // ----------------------------------------

    buttonManager.update();

    // ----------------------------------------
    // Zeit
    // ----------------------------------------

    DateTime now =
        clockManager.now();

    // ----------------------------------------
    // Alarm Manager
    // ----------------------------------------

    alarmManager.update(now);

    // ----------------------------------------
    // Alarm aktiv
    // ----------------------------------------

    if (alarmManager.isRinging())
    {
        int8_t activeIndex =
            alarmManager
                .getActiveAlarmIndex();

        if (activeIndex >= 0)
        {
            const AlarmConfig& alarm =
                alarmManager.getAlarm(
                    activeIndex
                );

            displayManager
                .showAlarmScreen(
                    activeIndex + 1,
                    alarm.hour,
                    alarm.minute,
                    alarmManager
                        .isSnoozeEnabled()
                );
        }

        // Separater STOP-Taster:
        // Alarm endgültig beenden.
        if (
            buttonManager.wasPressed(
                Button::STOP
            )
        )
        {
            alarmManager.stop();

            lastDisplayedSecond =
                255;

            return;
        }

        // CENTER = Snooze
        if (
            alarmManager
                .isSnoozeEnabled()
            &&
            buttonManager.wasPressed(
                Button::CENTER
            )
        )
        {
            alarmManager.snooze(
                now
            );

            lastDisplayedSecond =
                255;

            return;
        }

        delay(10);
        return;
    }

    // ----------------------------------------
    // Menü
    // ----------------------------------------

    menuManager.update();

    // ----------------------------------------
    // Home Screen
    // ----------------------------------------

    if (!menuManager.isOpen())
    {
        /*
         * LCD nur einmal pro Sekunde
         * aktualisieren.
         */
        if (
            now.second() !=
            lastDisplayedSecond
        )
        {
            lastDisplayedSecond =
                now.second();

            NextAlarmInfo nextAlarm;

            bool hasNextAlarm =
                alarmManager.getNextAlarm(
                    now,
                    nextAlarm
                );

            displayManager
                .showHomeScreen(
                    now,
                    hasNextAlarm,
                    nextAlarm.alarmIndex + 1,
                    nextAlarm.hour,
                    nextAlarm.minute,
                    nextAlarm.daysAhead
                );
        }
    }
    else
    {
        /*
         * Nach dem Verlassen des Menüs
         * Home Screen sofort neu zeichnen.
         */
        lastDisplayedSecond = 255;
    }

    delay(10);
}