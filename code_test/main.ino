#include "testfile.h"

Adafruit_BME280 bme; // I2C

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
