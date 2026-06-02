# Arduino Audio Alarm Clock

## Projektübersicht

Dieses Projekt entwickelt einen lokal betriebenen Audio-Wecker auf Basis eines Mikrocontrollers.

Ziel ist ein zuverlässiger Daily-Driver-Wecker mit folgenden Eigenschaften:

* Lokale MP3-Wiedergabe
* Keine WLAN-, Bluetooth- oder Cloud-Abhängigkeiten
* Mehrere konfigurierbare Wecker
* Wochentagsauswahl
* Einstellbare Lautstärke
* Bedienung über Hardwaretasten
* Modernes Farbdisplay
* Speicherung aller Einstellungen im internen Flash

---

## Entwicklungsstrategie

### Prototyp

* STM32 Nucleo F446RE
* Breadboard-Aufbau
* USB-Stromversorgung

### Finale Version

* Arduino Nano oder vergleichbarer kompakter Mikrocontroller
* Eigenes PCB
* Eigenes Gehäuse
* Optimierte Stromversorgung

---

## Festgelegte Hardware

### Mikrocontroller

#### Prototyp

* STM32 Nucleo F446RE

#### Finale Version

* Arduino Nano oder vergleichbarer kompakter Mikrocontroller

### Echtzeituhr (RTC)

* DS3231 RTC Modul
* I²C-Schnittstelle
* Batteriegepufferte Uhrzeit
* Hohe Genauigkeit

### Audio

* DFPlayer Pro 128MB
* Interner Speicher
* MP3-Decoding direkt im Modul
* USB-Dateiübertragung vom PC
* UART-Steuerung

### Display

* Adafruit 1.9" IPS TFT Display
* Artikelnummer: 5394
* Controller: ST7789
* Auflösung: 320 × 170 Pixel
* SPI-Schnittstelle

### Bedienung

* SparkFun Qwiic Directional Pad
* Artikelnummer: PRT-26851
* 5-Wege-D-Pad

  * Up
  * Down
  * Left
  * Right
  * Center
* I²C-Schnittstelle

---

## Systemarchitektur

```text
                 DS3231
                   │
                   │ I²C
                   │
PRT-26851 ─────────┤
                   │
             Nucleo / Nano
                   │
        ┌──────────┴──────────┐
        │                     │
       SPI                  UART
        │                     │
   ST7789 TFT         DFPlayer Pro
        │                     │
        │               Lautsprecher
        │
     Benutzer
```

---

## Speicherung

Alle Benutzereinstellungen werden im internen Flash des Controllers gespeichert.

Gespeichert werden beispielsweise:

* Weckzeiten
* Wochentage
* Track-Auswahl
* Lautstärke
* Systemeinstellungen

Es ist kein zusätzliches EEPROM vorgesehen.

---

## Geplante Funktionen

### Wecker

* Mehrere Wecker
* Aktivierbare Wochentage
* Individuelle Track-Auswahl
* Einstellbare Lautstärke

### Audio

* 10–20 MP3-Tracks
* Lokale Speicherung
* Keine Netzwerkfunktionen

### Benutzeroberfläche

* Menüführung über D-Pad
* Anzeige von Uhrzeit
* Anzeige aktiver Wecker
* Anzeige ausgewählter Tracks
* Einstellungsmenüs

---

## Noch offen

* Lautsprecher
* Audioverstärker (falls erforderlich)
* Finale Stromversorgung
* Eigenes PCB
* Gehäusedesign

---

## Projektstatus

### Festgelegt

* STM32 Nucleo F446RE (Prototyp)
* Arduino Nano oder vergleichbarer Mikrocontroller (Final)
* DS3231 RTC
* DFPlayer Pro 128MB
* Adafruit 1.9" ST7789 Display (5394)
* SparkFun Qwiic Directional Pad (PRT-26851)
* Speicherung im internen Flash
* USB-Stromversorgung für den Prototyp

### In Planung

* Lautsprecher
* Audioausgabe
* Finale Stromversorgung
* PCB-Design
* Gehäuse

## License

This project is licensed under the MIT License - see the LICENSE file for details.
