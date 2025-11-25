/////////////////////////////////////////
#include "testfile.h"

#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "RTClib.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <MPU9250_WE.h>

/////////////////////////////////////////

#define DEBUG 0
#define SETTIMEONCE 0

/////////////////////////////////////////



void systemInit(Adafruit_BME280& bme_var, RTC_DS3231& rtc_var, Adafruit_SSD1306& display_var, MPU9250_WE& imu_var){
#if DEBUG
  Serial.begin(9600);
  while (!Serial) { /* warten, falls nötig */ }
#endif
  Wire.begin();
  while(!initBME280(bme_var)){
#if DEBUG
    Serial.println("BME nicht gefunden");
#endif 
    delay(1000);
  }
  while(!initRTC(rtc_var)){
#if DEBUG
    Serial.println(F("FEHLER: DS3231 nicht gefunden. Wiring/Adresse prüfen!"));
#endif 
    delay(1000);
  }
  while(!initDISPLAY(display_var)){
#if DEBUG    
    Serial.println(F("SSD1306 nicht gefunden. Check Verkabelung/Adresse!"));
#endif 
    delay(1000);
  }
  while(!initIMU(imu_var)){
 #if DEBUG    
    Serial.println("FEHLER: MPU9250 antwortet nicht. Verkabelung/Adresse prüfen!");
 #endif   
    delay(1000);
  }

  
}


bool initBME280(Adafruit_BME280& bme_var) {
  // Erst Adresse 0x76 versuchen
  if (bme_var.begin(0x76)) {
#if DEBUG
    Serial.println(F("BME280 gefunden auf Adresse 0x76."));
#endif
    return true;
  }
  // Dann 0x77
  if (bme_var.begin(0x77)) {
#if DEBUG
    Serial.println(F("BME280 gefunden auf Adresse 0x77."));
#endif
    return true;
  }

  return false;
}


bool initRTC(RTC_DS3231& rtc_var){
  if(rtc_var.begin()){
    if (rtc_var.lostPower()) {
#if DEBUG
      Serial.println(F("RTC meldet Stromverlust → Zeit wird neu gesetzt."));
#endif  
      rtc_var.adjust(DateTime(F(__DATE__), F(__TIME__)));
    } else {
#if DEBUG
      Serial.println(F("RTC hatte keinen Stromverlust (Zeit gültig)."));
#endif
    }
#if SETTIMEONCE    
    rtc_var.adjust(DateTime(F(__DATE__), F(__TIME__)));
#endif
    return true;
  }
  return false;

}


bool initDISPLAY(Adafruit_SSD1306& display_var){
  if(display_var.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)){
    display_var.clearDisplay();
    display_var.setTextSize(1);
    display_var.setTextColor(SSD1306_WHITE);
    display_var.setCursor(0, 0);
    return true;
  }
  return false;
}

bool initIMU(MPU9250_WE& imu_var){
  if (imu_var.init()){
    // ---------- Sensorbereiche setzen ----------
    // Acc: +/-2g,4g,8g,16g
    imu_var.setAccRange(MPU9250_ACC_RANGE_8G);     // guter Allround-Bereich
    // Gyro: +/-250,500,1000,2000 deg/s
    imu_var.setGyrRange(MPU9250_GYRO_RANGE_500);   // ausreichend dynamisch
    // Mag: full-scale wird intern gesetzt; wir wählen Betriebsmodus:
    imu_var.setMagOpMode(AK8963_CONT_MODE_100HZ);  // kontinuierlich, 100 Hz
    return true;
  }
  return false;
}








void updateButtons(){
  
}

void updateSensors(){
  
}

void updateNavigation(){
  
}

void updateMenuSystem(){
  
}

void renderDisplay(){
  
}

void handleAlarms(){
  
}
