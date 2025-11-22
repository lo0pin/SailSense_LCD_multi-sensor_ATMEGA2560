/*
Rolle: Gemeinsame Datentypen und Structs.

Inhalt:

Structs für Messwerte und Zustände
*/


#pragma once

struct EnvData {
  float temperature;
  float humidity;
  float pressure;
};

struct MotionData {
  float roll;
  float pitch;
  float yaw;
};

struct HeadingData {
  float headingDeg;
};

struct BatteryStatus {
  float voltage;
  float percentage;
};

