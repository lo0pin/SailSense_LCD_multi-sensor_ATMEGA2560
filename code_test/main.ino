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
  //renderDisplay_everyLoop(display);
  BMEData current_bme;
  IMUData current_imu;


  if (millis() - globaltimer > delaytime_for_loop) {
    buttoninput = updateButtons();
    if (counter_for_measurment_within_loop % 2 == 1){
      current_bme = updateSensors(bme);
      counter_for_measurment_within_loop = 0;
    } else {
      counter_for_measurment_within_loop++;
    }
    
    current_imu = updateNavigation(imu);

    current_display = updateMenuSystem(buttoninput);


    if (right_now.minute() != old_minute) {
      mittelw_bme = get_mittelwert(current_bme);
      old_minute = right_now.minute();
    }
    uint8_t right_now_hour = right_now.hour();
    if (right_now_hour != old_hour) {
      if (old_hour == 99) {
        temp_messungen[right_now_hour] = mittelw_bme.temp;
        humid_messungen[right_now_hour] = mittelw_bme.humi;
        baro_messungen[right_now_hour] = mittelw_bme.baro;
        
      } else {
        uint8_t index_minus_one_hour = (right_now_hour + 24 - 1) % 24;
        temp_messungen[index_minus_one_hour] = mittelw_bme.temp;
        humid_messungen[index_minus_one_hour] = mittelw_bme.humi;
        baro_messungen[index_minus_one_hour] = mittelw_bme.baro;
      }

      old_hour = right_now.hour();
    }

    renderDisplay(display, current_bme, current_imu, right_now, current_display);


#if DEBUG
    Serial.print(F("current_bme\t"));
    Serial.print(mittelw_bme.temp);
    Serial.print(F("\t"));
    Serial.print(mittelw_bme.humi);
    Serial.print(F("\t"));
    Serial.println(mittelw_bme.baro);

    Serial.print(F("current_imu\t"));
    Serial.print(current_imu.roll);
    Serial.print(F("\t"));
    Serial.print(current_imu.pitch);
    Serial.print(F("\t"));
    Serial.println(current_imu.heading);
#endif
    globaltimer = millis();
  }
  
  handleAlarms();
}
