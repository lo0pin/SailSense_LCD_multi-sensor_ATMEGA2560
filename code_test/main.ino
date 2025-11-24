#include "testfile.h"

Adafruit_BME280 bme; // I2C
RTC_DS3231 rtc;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MPU9250_WE imu = MPU9250_WE(MPU9250_ADDR);

void setup(){
  systemInit(bme, rtc, display, imu);
}

void loop(){
  updateButtons();
  updateSensors();
  updateNavigation();
  updateMenuSystem();
  renderDisplay();
  handleAlarms();
}
