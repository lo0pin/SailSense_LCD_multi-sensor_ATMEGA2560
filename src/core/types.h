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

struct IMUData {
    float roll;
    float pitch;
    float yaw;

    float magX;
    float magY;
    float magZ;
};


struct BatteryStatus {
  float voltage;
  float percentage;
};


