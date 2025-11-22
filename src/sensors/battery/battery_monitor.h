/*
Rolle: Akku-Spannungsmessung.

Inhalt:

Lesen eines ADC-Pins

Umrechnung auf reale Spannung (Spannungsteiler, Referenzspannung)

Berechnung eines groben Ladezustands in %
*/

namespace BatteryMonitor {
  void begin();
  void update();
  BatteryStatus getStatus();
}

