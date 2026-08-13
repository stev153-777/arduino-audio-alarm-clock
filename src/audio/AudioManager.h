#pragma once

#include <Arduino.h>
#include <DFRobot_DF1201S.h>

class AudioManager
{
public:
    bool begin();

    void playAlarm(uint16_t fileNumber = 1);
    void stopAlarm();

    void setVolume(uint8_t volume);
    uint8_t getVolume() const;

    bool isPlaying() const;

private:
    void sendRawCommand(const char* command);

    uint8_t _volume = 15;

    bool _playing = false;
};