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

#include "system_init.h"
#include "bme280_sensor.h"
#include "mpu6050_sensor.h"
#include "gy271_sensor.h"
#include "rtc_module.h"
#include "battery_monitor.h"
#include "display.h"
#include "buttons.h"
#include "buzzer.h"

void systemInit() {
  // Wire, Serial, Sensor-Init, Display-Init etc.
}


