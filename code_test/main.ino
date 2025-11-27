#include "testfile.h"


Adafruit_BME280     bme; // I2C
RTC_DS3231          rtc;
Adafruit_SSD1306    display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MPU9250_WE imu =    MPU9250_WE(MPU9250_ADDR);

void setup() {
  systemInit(bme, rtc, display, imu);
  renderDisplay_Setup(display);
}


void loop() {
  DateTime right_now = rtc.now();
  renderDisplay_everyLoop(display);
  BMEData current_bme;
  BMEData mittelw_bme;
  IMUData current_imu;
  
  if (millis() - globaltimer > delaytime_for_loop) {
    buttoninput = updateButtons();

    current_bme = updateSensors(bme);
    current_imu = updateNavigation(imu); 
    current_display = updateMenuSystem(buttoninput); 
    /*
    mittelw_bme.temp += current_bme.temp;
    mittelw_bme.humi += current_bme.humi;
    mittelw_bme.baro += current_bme.baro;
    
    mittelw_bme.temp /= 2;
    mittelw_bme.humi /= 2;
    mittelw_bme.baro /= 2;
     */
    globaltimer = millis();

  }


  
  
  renderDisplay(display, current_bme, current_imu, right_now, current_display); 
  handleAlarms();
}


