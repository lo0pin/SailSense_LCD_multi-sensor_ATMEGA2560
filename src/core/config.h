/*
Rolle: Zentrale Konfiguration / Konstanten.

Inhalt:

I/O-Pins (Buttons, Buzzer, Batterie ADC, I2C-Adressen)

Kalibrierkonstanten (Offsets für Kompass, MPU usw.)

Thresholds (z. B. Low-Battery-Grenze)

Compile-Time-Flags (z. B. #define DEBUG 1)
*/

#pragma once

// Pins
constexpr int PIN_BUTTON_1 = 2;
constexpr int PIN_BUTTON_2 = 3;
// ...

// I2C addresses
constexpr uint8_t BME280_ADDR = 0x76;
constexpr uint8_t GY271_ADDR  = 0x1E;

// Batterie
constexpr int PIN_BATTERY_ADC = A0;
constexpr float BATTERY_MAX_V = 4.2f;
constexpr float BATTERY_MIN_V = 3.0f;


