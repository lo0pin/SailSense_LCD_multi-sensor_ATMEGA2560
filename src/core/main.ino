/*
main.ino

Rolle: Einstiegspunkt des Programms.

Inhalt:

setup() und loop()

Ruft Initialisierungsfunktionen aus system_init auf

Nutzt die öffentlichen Schnittstellen der anderen Module, aber enthält keine Sensorlogik im Detail
*/

#include "system_init.h"
#include "menu_system.h"
#include "display.h"
#include "buttons.h"

void setup() {
  systemInit();
}

void loop() {
  updateButtons();
  updateSensors();
  updateNavigation();
  updateMenuSystem();
  renderDisplay();
  handleAlarms();
}
