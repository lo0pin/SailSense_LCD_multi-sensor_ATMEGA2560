#include "testfile.h"

#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DEBUG 0

void systemInit(Adafruit_BME280& bme_var, ){
#IF DEBUG
  Serial.begin(9600);
  while (!Serial) { /* warten, falls nötig */ }
#ENDIF
  wire.begin();
  while(!initBME280(bme_var)){
#IF DEBUG
  Serial.println("BME nicht gefunden")
  delay(1000);
#ENDIF  
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
