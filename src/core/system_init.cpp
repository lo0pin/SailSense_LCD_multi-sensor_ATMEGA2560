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

#include "mpu9250_module.h"
#include "display_oled.h"
#include "battery_monitor.h"
#include "rtc_module.h"
#include "buttons.h"

void systemInit() {
    Wire.begin();
    Buttons::begin();
    DisplayOLED::begin();
    RTCModule::begin();
    BatteryMonitor::begin();
    MPU9250Module::begin();
}
