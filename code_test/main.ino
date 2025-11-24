#include "testfile.h"

Adafruit_BME280 bme; // I2C
RTC_DS3231 rtc;

void setup(){
  systemInit();
}

void loop(){
  updateButtons();
  updateSensors();
  updateNavigation();
  updateMenuSystem();
  renderDisplay();
  handleAlarms();
}
