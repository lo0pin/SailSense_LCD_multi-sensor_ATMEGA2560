/////////////////////////////////////////
#include "testfile.h"

#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "RTClib.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <MPU9250_WE.h>

/////////////////////////////////////////

#define DEBUG 0
#define SETTIMEONCE 0

/////////////////////////////////////////



void systemInit(Adafruit_BME280& bme_var, RTC_DS3231& rtc_var){
#IF DEBUG
  Serial.begin(9600);
  while (!Serial) { /* warten, falls nötig */ }
#ENDIF
  wire.begin();
  while(!initBME280(bme_var)){
#IF DEBUG
    Serial.println("BME nicht gefunden")
#ENDIF 
    delay(1000);
  }
  while(!initRTC(rtc_var)){
#IF DEBUG
    Serial.println(F("FEHLER: DS3231 nicht gefunden. Wiring/Adresse prüfen!"));
#ENDIF 
    delay(1000);
  }
  while(!initDISPLAY){
#IF DEBUG    
    Serial.println(F("SSD1306 nicht gefunden. Check Verkabelung/Adresse!"));
#ENDIF 
    delay(1000);
  }
}


bool initBME280(Adafruit_BME280& bme_var) {
  // Erst Adresse 0x76 versuchen
  if (bme_var.begin(0x76)) {
#IF DEBUG
    Serial.println(F("BME280 gefunden auf Adresse 0x76."));
#ENDIF
    return true;
  }
  // Dann 0x77
  if (bme_var.begin(0x77)) {
#IF DEBUG
    Serial.println(F("BME280 gefunden auf Adresse 0x77."));
#ENDIF
    return true;
  }

  return false;
}


bool initRTC(RTC_DS3231& rtc_var){
  if(rtc_var.begin()){
    if (rtc_var.lostPower()) {
#IF DEBUG
      Serial.println(F("RTC meldet Stromverlust → Zeit wird neu gesetzt."));
#ENDIF  
      rtc_var.adjust(DateTime(F(__DATE__), F(__TIME__)));
    } else {
#IF DEBUG
      Serial.println(F("RTC hatte keinen Stromverlust (Zeit gültig)."));
#ENDIF
    }
#IF SETTIMEONCE    
    rtc_var.adjust(DateTime(F(__DATE__), F(__TIME__)));
#ENDIF
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
