/*
Rolle: Akustische Signale & Alarme.

Inhalt:

Abstraktion für:

kurzer Beep (Button-Feedback)

Pattern für Alarm

evtl. non-blocking Tonsteuerung über millis()
*/

#pragma once

namespace Buzzer {
  void begin();
  void beepOk();
  void beepError();
  void alarmOn();
  void alarmOff();
  void update();   // für zeitbasierte Tonmuster
}

