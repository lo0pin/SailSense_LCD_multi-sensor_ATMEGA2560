/*
Rolle: Menüs, Screens, Zustandsmaschine für die UI.

Inhalt:

Enum für Screens/Menüs (z. B. ENV, NAV, TIMER, SETTINGS …)

Zustandsverwaltung der aktuellen Ansicht

Reaktion auf Button-Events

Aufruf von Display-Funktionen zum Rendern
*/

#pragma once

enum class ScreenId {
  ENV,
  NAV,
  TIMER,
  ALARM,
  SETTINGS
};

namespace MenuSystem {
  void begin();
  void update();               // verarbeitet Button-Events, wechselt Screens
  ScreenId getCurrentScreen();
}

