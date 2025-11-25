#pragma once

// Bibliotheken einbinden, damit die Typen vollständig sind wenn main.ino
// die globalen Objekte (z.B. Adafruit_BME280 bme;) deklariert.
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "RTClib.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MPU9250_WE.h>

constexpr uint8_t  SCREEN_WIDTH =    128;
constexpr uint8_t  SCREEN_HEIGHT =   64;
constexpr int8_t   OLED_RESET =      -1;      // Reset-Pin (oder -1, wenn über Arduino-Reset)
constexpr uint8_t  SCREEN_ADDRESS =  0x3C;    // I2C-Adresse deines Displays

constexpr uint8_t MPU9250_ADDR =      0x68;
//constexpr uint8_t INT_PIN           2          // optional, falls INT verbunden ist

// Funktionsdeklarationen
void systemInit(Adafruit_BME280& bme_var, RTC_DS3231& rtc_var, Adafruit_SSD1306& oled_var, MPU9250_WE& imu_var);
bool initBME280(Adafruit_BME280& bme_var);
bool initRTC(RTC_DS3231& rtc_var);
bool initDISPLAY(Adafruit_SSD1306& display_var);
bool initIMU(MPU9250_WE& imu_var);

extern unsigned long globaltimer;
extern uint16_t delaytime;
extern uint16_t minite;

struct BMEData{
  float temp;
  float humi;
  float baro;
};

uint8_t updateButtons();
BMEData updateSensors(Adafruit_BME280& bme_var);
void updateNavigation();
void updateMenuSystem(uint8_t button);
void renderDisplay(Adafruit_SSD1306& dis, BMEData& bme_struct);
void handleAlarms();
