/*
Rolle: Alle Initialisierungen an einem Ort.

Inhalt:

Funktionen wie void systemInit();

Initialisierung von:

Serial (optional)

I2C (Wire.begin())

Sensoren (BME280, MPU6050, GY-271, RTC)

Display

Buttons

Buzzer

globale Zustandsobjekte
*/

// system_init.h
#pragma once
void systemInit();
void updateSensors();
void updateNavigation();
void handleAlarms();


