#pragma once


/*********************************************
Globale Konstanten
*********************************************/
#define DEBUG 0
#define SETTIMEONCE 0

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

constexpr uint8_t MPU9250_ADDR =      0x69;
//constexpr uint8_t INT_PIN           2          // optional, falls INT verbunden ist

constexpr uint8_t array_len = 24;
constexpr uint8_t mag_mittelwerte = 20;

extern uint8_t buttoninput;

extern unsigned long globaltimer;
extern uint16_t delaytime_for_loop;
extern uint8_t counter_for_measurment_within_loop;

extern uint8_t old_hour;
extern uint8_t old_minute;
extern uint8_t mittelwert_divisor;



extern uint8_t current_display;
extern uint8_t max_number_of_displays;

extern uint8_t moon_position_offset_x;
extern bool moon_going_right;


/*********************************************
Container
*********************************************/ 
extern float  temp_messungen[array_len];
extern float  humid_messungen[array_len];
extern float  baro_messungen[array_len];

extern float  mittelwert_magnetkurse[mag_mittelwerte];
extern uint8_t mag_mittelwert_index;
extern uint16_t mag_geglaettet;


/*********************************************
Typen
*********************************************/ 

struct BMEData{
  float temp;
  float humi;
  float baro;
};

struct IMUData{
  float roll; // Roll (Krängung)
  float pitch; // Pitch (Stampfen)
  float heading; //Kompasskurs
};


extern BMEData hourly_summe_bme;
extern BMEData mittelw_bme;

/*********************************************
Funktionsdeklarationen
*********************************************/ 
void systemInit(Adafruit_BME280& bme_var, RTC_DS3231& rtc_var, Adafruit_SSD1306& display_var, MPU9250_WE& imu_var);
bool initBME280(Adafruit_BME280& bme_var);
bool initRTC(RTC_DS3231& rtc_var);
bool initDISPLAY(Adafruit_SSD1306& display_var);
bool initIMU(MPU9250_WE& imu_var);
uint8_t updateButtons();
BMEData updateSensors(Adafruit_BME280& bme_var);
BMEData get_mittelwert(BMEData& bme_now);
IMUData updateNavigation(MPU9250_WE& imu_var);
float get_mag_mittelwert(float cur_head);

void updateMenuSystem(uint8_t button);
void renderDisplay(Adafruit_SSD1306& dis, BMEData& bme_struct, IMUData& imu_struct, DateTime dt, uint8_t displaymode);

void renderDisplay_Setup(Adafruit_SSD1306& dis, uint8_t mode);
void renderDisplay_everyLoop(Adafruit_SSD1306& dis);

void pointOnCircle(int cx, int cy, int r, float heading_deg, int &x, int &y);

const char* weekdayName(uint8_t wday);

void handleAlarms();
