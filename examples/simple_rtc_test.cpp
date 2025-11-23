/*
Wichtig zur Hardware:
=====================
> DS3231 hängt wie dein BME280 an I2C (SDA/SCL).
> SQW/INT-Pin des Moduls optional auf D2 (oder anderen Interrupt-Pin), damit Alarme den Arduino wecken/triggern können.
> Eine Knopfzelle muss drin sein, sonst verliert er die Zeit beim Stromausfall.

Verdrahtung (I2C)
=================
> VCC → 3.3 V oder 5 V (die Module haben i.d.R. Regler/Levelshifter)
> GND → GND
> SDA → SDA
> SCL → SCL
> optional SQW/INT → D2

Library
=======
Arduino IDE → Bibliotheksverwaltung → “RTClib” (Adafruit) installieren. 
*/

/******************************************************************************
 * DS3231_SHOWCASE.ino  (Adafruit RTClib)
 *
 * Zeigt die wichtigsten DS3231-Funktionen:
 *  1) rtc.begin() + Fehlerfall
 *  2) rtc.lostPower() → bei Batterieverlust automatisch neu stellen
 *  3) rtc.adjust(DateTime) → Zeit setzen (Compile-Time oder fix)
 *  4) rtc.now() → aktuelle Zeit lesen
 *  5) Formatierung / Ausgabe der Zeit
 *  6) TimeSpan → bequem Zeiten addieren/subtrahieren
 *  7) DS3231-Temperatur (eingebauter Temperatursensor)
 *  8) Alarm1 + Alarm2: setzen, abfragen, quittieren
 *       - Alarm1: jede Minute bei Sekunde 0 (Demo)
 *       - Alarm2: täglich um 07:30 (Demo)
 *  9) SQW/INT-Pin-Modi (1Hz-Squarewave oder Alarm-Interrupt)
 *
 * Hinweis:
 *  - Alarm-Flags MÜSSEN nach einem Alarm gelöscht werden,
 *    sonst bleibt das SQW/INT-Signal "stuck" und es kommt kein neuer Interrupt.
 ******************************************************************************/

#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

#define INT_PIN 2   // optional: SQW/INT hier anschließen

// einfache Hilfsfunktion: zweistellig drucken
void print2(int v) {
  if (v < 10) Serial.print('0');
  Serial.print(v);
}

// ---- Alarm-ISR ----
// Der SQW/INT-Pin geht LOW, wenn ein Alarm feuert.
// Wir merken uns nur "da war was", der Rest passiert in loop().
// (Im ISR nix I2C machen!)
volatile bool alarmTriggered = false;
void alarmISR() {
  alarmTriggered = true;
}

// schöner Wochentag-String
const char* dowString(uint8_t d) {
  static const char* names[] = {
    "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"
  };
  if (d > 6) return "??";
  return names[d];
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Serial.println(F("\nDS3231 Showcase startet..."));

  Wire.begin();
  Wire.setClock(400000); // Fast I2C

  // 1) RTC initialisieren
  if (!rtc.begin()) {
    Serial.println(F("FEHLER: DS3231 nicht gefunden. Wiring/Adresse prüfen!"));
    while (1) {}
  }
  Serial.println(F("DS3231 verbunden."));

  // 2) Prüfen, ob RTC Stromverlust hatte (Batterie leer/weg)
  //    → dann sinnvoll neu stellen
  if (rtc.lostPower()) {
    Serial.println(F("RTC meldet Stromverlust → Zeit wird neu gesetzt."));

    // 3) Zeit setzen:
    // Variante A: Compile-Time (Zeit des Sketch-Kompilierens)
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // Variante B: feste Zeit (auskommentieren wenn du willst)
    // rtc.adjust(DateTime(2025, 11, 23, 12, 0, 0));
  } else {
    Serial.println(F("RTC hatte keinen Stromverlust (Zeit gültig)."));
  }

  // 4) SQW/INT-Pin-Modus konfigurieren
  // DS3231 kann hier wahlweise:
  //  - Squarewave (1Hz/1kHz/4kHz/8kHz)
  //  - Alarm-Interrupt
  // Für Alarm-Interrupt: DS3231_OFF (SQW aus, INT aktiv bei Alarm)
  rtc.writeSqwPinMode(DS3231_OFF);
  Serial.println(F("SQW Pin Mode: OFF (INT für Alarme)."));

  // optional 32kHz Ausgang aktivieren (nur falls du ihn brauchst)
  // rtc.enable32KHz(true);

  // 5) Interrupt am Arduino scharf machen (optional)
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), alarmISR, FALLING);

  // 6) Vorherige Alarme/Flags sauber löschen
  rtc.disableAlarm(1);
  rtc.disableAlarm(2);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  // 7) Alarm1 setzen → "jede Minute wenn Sekunde == 0"
  //
  // Alarm-Modi (RTClib):
  //  DS3231_A1_Second  → wenn Sekunden übereinstimmen: 1× pro Minute
  //  DS3231_A1_Minute  → wenn Min+Sek übereinstimmen: 1× pro Stunde (mit 0/0)
  //  DS3231_A1_Hour    → 1× pro Tag wenn h/m/s matchen
  //  DS3231_A1_Date    → 1× pro Monat
  //  DS3231_A1_Day     → 1× pro Woche
  //
  // Für "jede Minute bei Sekunde 0" reicht:
  rtc.setAlarm1(DateTime(0,0,0,0,0,0), DS3231_A1_Second);
  Serial.println(F("Alarm1: jede Minute bei Sekunde 0."));

  // 8) Alarm2 setzen → "täglich um 07:30"
  //
  // Alarm2 hat KEINE Sekunden-Option!
  // Modi:
  //  DS3231_A2_Minute → Minuten matchen: 1× pro Stunde (wenn minute==x)
  //  DS3231_A2_Hour   → Stunden+Minuten matchen: 1× pro Tag
  //  DS3231_A2_Date   → 1× pro Monat
  //  DS3231_A2_Day    → 1× pro Woche
  //
  rtc.setAlarm2(DateTime(0,0,0,7,30,0), DS3231_A2_Hour);
  Serial.println(F("Alarm2: täglich um 07:30."));

  Serial.println(F("Setup fertig.\n"));
}

unsigned long lastPrint = 0;

void loop() {

  // 9) Zeit lesen
  DateTime now = rtc.now();

  // alle 1 s hübsch ausgeben
  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();

    Serial.print(F("Zeit: "));
    print2(now.day()); Serial.print('.');
    print2(now.month()); Serial.print('.');
    Serial.print(now.year()); Serial.print(" ");
    print2(now.hour()); Serial.print(':');
    print2(now.minute()); Serial.print(':');
    print2(now.second());
    Serial.print("  ("); Serial.print(dowString(now.dayOfTheWeek())); Serial.println(")");

    // 10) Temperatur des DS3231
    // (nicht super-präzise absolute Temp, aber stabil als Trend)
    float t = rtc.getTemperature();
    Serial.print(F("RTC Temp: "));
    Serial.print(t, 2);
    Serial.println(F(" °C"));

    // 11) TimeSpan-Demo: "wie spät ist es in 90 Minuten?"
    TimeSpan plus90min(0, 1, 30, 0); // days, hours, minutes, seconds
    DateTime later = now + plus90min;
    Serial.print(F("In 90min: "));
    print2(later.hour()); Serial.print(':');
    print2(later.minute()); Serial.print(':');
    print2(later.second());
    Serial.println();
  }

  // 12) Wenn ISR anschlug oder wir pollen wollen:
  if (alarmTriggered || rtc.alarmFired(1) || rtc.alarmFired(2)) {
    alarmTriggered = false; // Flag zurücksetzen

    // 13) Welche Alarme haben gefeuert?
    if (rtc.alarmFired(1)) {
      Serial.println(F("\n*** Alarm1 gefeuert! (minütlich) ***"));

      // WICHTIG: Alarm-Flag löschen, sonst bleibt INT low!
      rtc.clearAlarm(1);
    }

    if (rtc.alarmFired(2)) {
      Serial.println(F("\n*** Alarm2 gefeuert! (täglich 07:30) ***"));

      rtc.clearAlarm(2);
    }

    Serial.println(F("*** Alarm-Flags quittiert.\n"));
  }

  // 14) (Optional) Squarewave-Demo:
  // Wenn du statt Alarm-INT einen 1Hz-Takt brauchst:
  // rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
  // Dann wird SQW dauerhaft mit 1Hz toggeln.
}





Alles klar — hier ist ein **DS3231-Showcase-Sketch** mit **Adafruit RTClib**, der die relevanten Funktionen einmal „auf die Bühne“ holt: Initialisierung, `lostPower()`/Neu-Stellen, Zeit lesen/formatieren, Rechnen mit `TimeSpan`, Temperatur des DS3231, **beide Alarme** (minütlich + täglich), Alarm-Flags sauber quittieren, plus **SQW-Pin-Modi** (1 Hz-Takt oder Alarm-Interrupt). Die Alarm-API in RTClib ist etwas speziell, darum sind die Kommentare extra ausführlich. ([Random Nerd Tutorials][1])

> **Wichtig zur Hardware:**
>
> * DS3231 hängt wie dein BME280 an I2C (SDA/SCL).
> * **SQW/INT-Pin** des Moduls optional auf **D2** (oder anderen Interrupt-Pin), damit Alarme den Arduino wecken/triggern können.
> * Eine Knopfzelle muss drin sein, sonst verliert er die Zeit beim Stromausfall. ([Random Nerd Tutorials][1])

---

## Verdrahtung (I2C)

* **VCC → 3.3 V oder 5 V** (die Module haben i.d.R. Regler/Levelshifter)
* **GND → GND**
* **SDA → SDA**
* **SCL → SCL**
* optional **SQW/INT → D2** ([Random Nerd Tutorials][1])

---

## Library

Arduino IDE → Bibliotheksverwaltung → **“RTClib” (Adafruit)** installieren. ([Random Nerd Tutorials][1])

---

## Showcase-Sketch (maximal kommentiert)

```cpp
/******************************************************************************
 * DS3231_SHOWCASE.ino  (Adafruit RTClib)
 *
 * Zeigt die wichtigsten DS3231-Funktionen:
 *  1) rtc.begin() + Fehlerfall
 *  2) rtc.lostPower() → bei Batterieverlust automatisch neu stellen
 *  3) rtc.adjust(DateTime) → Zeit setzen (Compile-Time oder fix)
 *  4) rtc.now() → aktuelle Zeit lesen
 *  5) Formatierung / Ausgabe der Zeit
 *  6) TimeSpan → bequem Zeiten addieren/subtrahieren
 *  7) DS3231-Temperatur (eingebauter Temperatursensor)
 *  8) Alarm1 + Alarm2: setzen, abfragen, quittieren
 *       - Alarm1: jede Minute bei Sekunde 0 (Demo)
 *       - Alarm2: täglich um 07:30 (Demo)
 *  9) SQW/INT-Pin-Modi (1Hz-Squarewave oder Alarm-Interrupt)
 *
 * Hinweis:
 *  - Alarm-Flags MÜSSEN nach einem Alarm gelöscht werden,
 *    sonst bleibt das SQW/INT-Signal "stuck" und es kommt kein neuer Interrupt.
 ******************************************************************************/

#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

#define INT_PIN 2   // optional: SQW/INT hier anschließen

// einfache Hilfsfunktion: zweistellig drucken
void print2(int v) {
  if (v < 10) Serial.print('0');
  Serial.print(v);
}

// ---- Alarm-ISR ----
// Der SQW/INT-Pin geht LOW, wenn ein Alarm feuert.
// Wir merken uns nur "da war was", der Rest passiert in loop().
// (Im ISR nix I2C machen!)
volatile bool alarmTriggered = false;
void alarmISR() {
  alarmTriggered = true;
}

// schöner Wochentag-String
const char* dowString(uint8_t d) {
  static const char* names[] = {
    "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"
  };
  if (d > 6) return "??";
  return names[d];
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Serial.println(F("\nDS3231 Showcase startet..."));

  Wire.begin();
  Wire.setClock(400000); // Fast I2C

  // 1) RTC initialisieren
  if (!rtc.begin()) {
    Serial.println(F("FEHLER: DS3231 nicht gefunden. Wiring/Adresse prüfen!"));
    while (1) {}
  }
  Serial.println(F("DS3231 verbunden."));

  // 2) Prüfen, ob RTC Stromverlust hatte (Batterie leer/weg)
  //    → dann sinnvoll neu stellen
  if (rtc.lostPower()) {
    Serial.println(F("RTC meldet Stromverlust → Zeit wird neu gesetzt."));

    // 3) Zeit setzen:
    // Variante A: Compile-Time (Zeit des Sketch-Kompilierens)
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // Variante B: feste Zeit (auskommentieren wenn du willst)
    // rtc.adjust(DateTime(2025, 11, 23, 12, 0, 0));
  } else {
    Serial.println(F("RTC hatte keinen Stromverlust (Zeit gültig)."));
  }

  // 4) SQW/INT-Pin-Modus konfigurieren
  // DS3231 kann hier wahlweise:
  //  - Squarewave (1Hz/1kHz/4kHz/8kHz)
  //  - Alarm-Interrupt
  // Für Alarm-Interrupt: DS3231_OFF (SQW aus, INT aktiv bei Alarm)
  rtc.writeSqwPinMode(DS3231_OFF);
  Serial.println(F("SQW Pin Mode: OFF (INT für Alarme)."));

  // optional 32kHz Ausgang aktivieren (nur falls du ihn brauchst)
  // rtc.enable32KHz(true);

  // 5) Interrupt am Arduino scharf machen (optional)
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), alarmISR, FALLING);

  // 6) Vorherige Alarme/Flags sauber löschen
  rtc.disableAlarm(1);
  rtc.disableAlarm(2);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  // 7) Alarm1 setzen → "jede Minute wenn Sekunde == 0"
  //
  // Alarm-Modi (RTClib):
  //  DS3231_A1_Second  → wenn Sekunden übereinstimmen: 1× pro Minute
  //  DS3231_A1_Minute  → wenn Min+Sek übereinstimmen: 1× pro Stunde (mit 0/0)
  //  DS3231_A1_Hour    → 1× pro Tag wenn h/m/s matchen
  //  DS3231_A1_Date    → 1× pro Monat
  //  DS3231_A1_Day     → 1× pro Woche
  //
  // Für "jede Minute bei Sekunde 0" reicht:
  rtc.setAlarm1(DateTime(0,0,0,0,0,0), DS3231_A1_Second);
  Serial.println(F("Alarm1: jede Minute bei Sekunde 0."));

  // 8) Alarm2 setzen → "täglich um 07:30"
  //
  // Alarm2 hat KEINE Sekunden-Option!
  // Modi:
  //  DS3231_A2_Minute → Minuten matchen: 1× pro Stunde (wenn minute==x)
  //  DS3231_A2_Hour   → Stunden+Minuten matchen: 1× pro Tag
  //  DS3231_A2_Date   → 1× pro Monat
  //  DS3231_A2_Day    → 1× pro Woche
  //
  rtc.setAlarm2(DateTime(0,0,0,7,30,0), DS3231_A2_Hour);
  Serial.println(F("Alarm2: täglich um 07:30."));

  Serial.println(F("Setup fertig.\n"));
}

unsigned long lastPrint = 0;

void loop() {

  // 9) Zeit lesen
  DateTime now = rtc.now();

  // alle 1 s hübsch ausgeben
  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();

    Serial.print(F("Zeit: "));
    print2(now.day()); Serial.print('.');
    print2(now.month()); Serial.print('.');
    Serial.print(now.year()); Serial.print(" ");
    print2(now.hour()); Serial.print(':');
    print2(now.minute()); Serial.print(':');
    print2(now.second());
    Serial.print("  ("); Serial.print(dowString(now.dayOfTheWeek())); Serial.println(")");

    // 10) Temperatur des DS3231
    // (nicht super-präzise absolute Temp, aber stabil als Trend)
    float t = rtc.getTemperature();
    Serial.print(F("RTC Temp: "));
    Serial.print(t, 2);
    Serial.println(F(" °C"));

    // 11) TimeSpan-Demo: "wie spät ist es in 90 Minuten?"
    TimeSpan plus90min(0, 1, 30, 0); // days, hours, minutes, seconds
    DateTime later = now + plus90min;
    Serial.print(F("In 90min: "));
    print2(later.hour()); Serial.print(':');
    print2(later.minute()); Serial.print(':');
    print2(later.second());
    Serial.println();
  }

  // 12) Wenn ISR anschlug oder wir pollen wollen:
  if (alarmTriggered || rtc.alarmFired(1) || rtc.alarmFired(2)) {
    alarmTriggered = false; // Flag zurücksetzen

    // 13) Welche Alarme haben gefeuert?
    if (rtc.alarmFired(1)) {
      Serial.println(F("\n*** Alarm1 gefeuert! (minütlich) ***"));

      // WICHTIG: Alarm-Flag löschen, sonst bleibt INT low!
      rtc.clearAlarm(1);
    }

    if (rtc.alarmFired(2)) {
      Serial.println(F("\n*** Alarm2 gefeuert! (täglich 07:30) ***"));

      rtc.clearAlarm(2);
    }

    Serial.println(F("*** Alarm-Flags quittiert.\n"));
  }

  // 14) (Optional) Squarewave-Demo:
  // Wenn du statt Alarm-INT einen 1Hz-Takt brauchst:
  // rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
  // Dann wird SQW dauerhaft mit 1Hz toggeln.
}

/*
## Warum genau diese Funktionen „relevant“ sind
* **`rtc.begin()`**: Basic-Handshake. Wenn das scheitert, ist alles andere egal. 
* **`rtc.lostPower()` + `rtc.adjust()`**: Der Klassiker für robuste Systeme (z.B. Logger). 
* **`rtc.now()` + `DateTime`-Getter**: Zeit sauber lesen/verwenden.
* **`TimeSpan`**: macht Zeitrechnung angenehm statt Fehlerquelle. 
* **`rtc.getTemperature()`**: nettes Bonus-Feature des DS3231.
* **`setAlarm1/2`, `alarmFired`, `clearAlarm`, `disableAlarm`**: zwei Alarme sind *der* Grund, warum man den DS3231 nimmt (Wecken, Stundenlogger, Tagesereignisse). 
* **`writeSqwPinMode()`**: SQW-Pin flexibel als Taktquelle oder Interrupt-Leitung.
*/

