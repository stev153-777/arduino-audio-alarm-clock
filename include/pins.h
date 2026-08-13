#pragma once

#include <Arduino.h>


// ============================================================================
// Arduino Audio Alarm Clock - Pinbelegung
// Board: NUCLEO-F446RE
//
// Diese Datei dient gleichzeitig als Pinbelegung / Verdrahtungsschema.
//
// Allgemeine Versorgung:
//   +5V  = Nucleo 5V
//   +3V3 = Nucleo 3V3
//   GND  = Nucleo GND
// ============================================================================



// ============================================================================
// LCD 1602A - 16x2 Character LCD
// Betriebsart: 4-Bit
// ============================================================================
//
// LCD Pin   Bezeichnung   Anschluss
// ---------------------------------------------------------------------------
// 1         VSS           -> GND
// 2         VDD           -> +5V
// 3         VO            -> Schleifer Kontrast-Potentiometer
// 4         RS            -> Nucleo D4
// 5         RW            -> GND
// 6         E             -> Nucleo D5
// 7         D0            -> nicht verbunden
// 8         D1            -> nicht verbunden
// 9         D2            -> nicht verbunden
// 10        D3            -> nicht verbunden
// 11        D4            -> Nucleo D6
// 12        D5            -> Nucleo D11
// 13        D6            -> Nucleo D9
// 14        D7            -> Nucleo D10
// 15        BLA / LED+    -> +5V
// 16        BLK / LED-    -> GND
//
// Kontrast-Potentiometer:
//   Ende 1   -> +5V
//   Ende 2   -> GND
//   Schleifer -> LCD VO (Pin 3)
//

constexpr uint8_t LCD_RS = D4;
constexpr uint8_t LCD_E  = D5;

constexpr uint8_t LCD_D4 = D6;
constexpr uint8_t LCD_D5 = D11;
constexpr uint8_t LCD_D6 = D9;
constexpr uint8_t LCD_D7 = D10;



// ============================================================================
// DFRobot DFPlayer Pro (DF1201S)
// Kommunikation: UART, 115200 Baud
// ============================================================================
//
// DFPlayer             Anschluss
// ---------------------------------------------------------------------------
// VCC / VIN            -> +5V
// GND                  -> GND
//
// RX                   <- Nucleo PA9  / D8  (TX)
// TX                   -> Nucleo PA10 / D2  (RX)
//
// Lautsprecher:
//   L+                  -> Lautsprecher +
//   L-                  -> Lautsprecher -
//
// Für Mono-Betrieb wird nur L+/L- verwendet.
//
// WICHTIG:
//   L- NICHT mit GND verbinden!
//   Der Lautsprecher wird direkt zwischen L+ und L- angeschlossen.
//
// Im Code:
//   HardwareSerial DFSerial(DF_RX, DF_TX);
//

constexpr uint8_t DF_RX = PA10;   // D2  - Nucleo RX  <- DFPlayer TX
constexpr uint8_t DF_TX = PA9;    // D8  - Nucleo TX  -> DFPlayer RX



// ============================================================================
// D-Pad - 5 einzelne Taster
// ============================================================================
//
// Alle Taster werden als INPUT_PULLUP betrieben.
//
// Verdrahtung jedes Tasters:
//
//       GPIO -------- Taster -------- GND
//
// Nicht gedrückt = HIGH
// Gedrückt       = LOW
//
// Funktion             Anschluss
// ---------------------------------------------------------------------------
// UP                    -> A0
// LEFT                  -> A1
// DOWN                  -> A2
// CENTER                -> A3
// RIGHT / OK            -> D3
//
// Gemeinsamer GND       -> GND
//

constexpr uint8_t BUTTON_UP     = A0;
constexpr uint8_t BUTTON_LEFT   = A1;
constexpr uint8_t BUTTON_DOWN   = A2;
constexpr uint8_t BUTTON_CENTER = A3;
constexpr uint8_t BUTTON_RIGHT  = D3;



// ============================================================================
// Alarm STOP Button
// ============================================================================
//
// Separater großer Taster zum endgültigen Ausschalten des Alarms.
//
// Verdrahtung:
//
//       D7 -------- Taster -------- GND
//
// Konfiguration:
//   pinMode(BUTTON_STOP, INPUT_PULLUP);
//
// Nicht gedrückt = HIGH
// Gedrückt       = LOW
//

constexpr uint8_t BUTTON_STOP = D7;



// ============================================================================
// Nucleo On-Board User Button (blau)
// ============================================================================
//
// Bereits auf dem NUCLEO-F446RE vorhanden.
// Keine externe Verdrahtung notwendig.
//
// Nur für Tests / Debug vorgesehen.
//

constexpr uint8_t BUTTON_USER = PC13;



// ============================================================================
// RTC - DS3231
// Kommunikation: I2C
// ============================================================================
//
// DS3231               Anschluss
// ---------------------------------------------------------------------------
// VCC                  -> +5V
// GND                  -> GND
// SDA                  -> Nucleo SDA / D14 / PB9
// SCL                  -> Nucleo SCL / D15 / PB8
//
// SQW / INT            -> nicht verbunden (vorerst)
// 32K                  -> nicht verbunden
//
// Backup-Batterie:
//   CRxxxx              -> Batteriehalter des RTC-Moduls
//   Nicht mit Nucleo verbinden.
//

constexpr uint8_t RTC_SDA = SDA;   // D14 / PB9
constexpr uint8_t RTC_SCL = SCL;   // D15 / PB8



// ============================================================================
// NUCLEO-F446RE Versorgung
// ============================================================================
//
// AKTUELLER TESTAUFBAU:
//
//   USB
//    |
//    v
// NUCLEO-F446RE
//    |
//    +---- 5V  ------> LCD VDD
//    |            \--> LCD Backlight +
//    |            \--> DFPlayer VCC
//    |            \--> DS3231 VCC
//    |
//    +---- GND -------> LCD VSS
//                 \--> LCD RW
//                 \--> LCD Backlight -
//                 \--> DFPlayer GND
//                 \--> DS3231 GND
//                 \--> D-Pad Common
//                 \--> STOP Button
//
//
// Spätere externe 5-V-Versorgung:
//
//   +5V Netzteil  -> Nucleo E5V
//   GND Netzteil  -> Nucleo GND
//
// Vor externer Einspeisung Power-Konfiguration/Jumper des Nucleo beachten.
// ============================================================================



// ============================================================================
// Pinübersicht NUCLEO-F446RE
// ============================================================================
//
// Nucleo Pin     Verwendung
// ---------------------------------------------------------------------------
// D2 / PA10      DFPlayer TX -> Nucleo RX
// D3             D-Pad RIGHT
// D4             LCD RS
// D5             LCD E
// D6             LCD D4
// D7             Alarm STOP
// D8 / PA9       Nucleo TX -> DFPlayer RX
// D9             LCD D6
// D10            LCD D7
// D11            LCD D5
//
// D14 / PB9      RTC SDA
// D15 / PB8      RTC SCL
//
// A0             D-Pad UP
// A1             D-Pad LEFT
// A2             D-Pad DOWN
// A3             D-Pad CENTER / OK
//
// PC13           On-Board User Button
//
// D0 / D1        FREI / nicht verwenden (Serial / ST-Link)
// D12            FREI
// D13            FREI
// A4             FREI
// A5             FREI
// ============================================================================