#include "AudioManager.h"
#include "pins.h"

static HardwareSerial DFSerial(
    DF_RX,
    DF_TX
);

static DFRobot_DF1201S player;


bool AudioManager::begin()
{
    DFSerial.begin(115200);

    delay(1000);

    if (!player.begin(DFSerial))
    {
        return false;
    }

    // Sprachansagen deaktivieren
    player.setPrompt(false);

    delay(200);

    // Music Mode
    player.switchFunction(
        DFRobot_DF1201S::MUSIC
    );

    delay(500);

    // Verstärker einschalten
    player.enableAMP();

    player.setVol(_volume);

    // Alarmdatei endlos wiederholen
    player.setPlayMode(
        DFRobot_DF1201S::SINGLECYCLE
    );

    return true;
}


void AudioManager::playAlarm(uint16_t fileNumber)
{
    if (_playing)
    {
        return;
    }

    // Verstärker wieder einschalten
    player.enableAMP();

    delay(50);

    // Sicherstellen, dass der Alarm wiederholt wird
    player.setPlayMode(
        DFRobot_DF1201S::SINGLECYCLE
    );

    delay(50);

    char command[24];

    snprintf(
        command,
        sizeof(command),
        "AT+PLAYNUM=%u",
        fileNumber
    );

    sendRawCommand(command);

    _playing = true;

    Serial.print("Audio START: Datei ");
    Serial.println(fileNumber);
}


void AudioManager::stopAlarm()
{
    if (!_playing)
    {
        return;
    }

    /*
     * Der DFPlayer besitzt keinen echten STOP-Befehl.
     *
     * AT+PLAY=PP ist nur ein Play/Pause-Toggle.
     * Deshalb zuerst AMP ausschalten:
     * Dadurch ist garantiert sofort Ruhe.
     */

    player.disableAMP();

    delay(50);

    // Danach Wiedergabe pausieren.
    sendRawCommand("AT+PLAY=PP");

    _playing = false;

    Serial.println("Audio STOP");
}


void AudioManager::setVolume(uint8_t volume)
{
    if (volume > 30)
    {
        volume = 30;
    }

    _volume = volume;

    player.setVol(_volume);
}


uint8_t AudioManager::getVolume() const
{
    return _volume;
}


bool AudioManager::isPlaying() const
{
    return _playing;
}


void AudioManager::sendRawCommand(
    const char* command
)
{
    DFSerial.print(command);
    DFSerial.print("\r\n");
}