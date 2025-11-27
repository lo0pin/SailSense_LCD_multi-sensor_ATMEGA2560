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


#define initialize_delay 20
#define initialize_fail_delay 1000

uint16_t delaytime = 300;
unsigned long globaltimer =0;
uint16_t minite = 0;


/////////////////////////////////////////

void systemInit(Adafruit_BME280& bme_var, RTC_DS3231& rtc_var, Adafruit_SSD1306& display_var, MPU9250_WE& imu_var){
#if DEBUG
  /////SERIAL
  Serial.begin(9600);
  delay(initialize_delay);
  // Hinweis: on Mega/2560 ist while(!Serial) normalerweise nicht nötig und kann blockieren.
  while (!Serial) { /* warten, falls nötig */ }
#endif
  /////WIRE
  Wire.begin();
  Wire.setClock(400000);   
  delay(initialize_delay);
  /////BME
  while(!initBME280(bme_var)){
#if DEBUG
    Serial.println("BME-Sensor nicht gefunden");
#endif 
    delay(initialize_fail_delay);
  }
#if DEBUG
    Serial.println("BME-Sensor initialisiert!");
#endif 
  delay(initialize_delay);
  /////RTC
  while(!initRTC(rtc_var)){
#if DEBUG
    Serial.println(F("FEHLER: DS3231-RTC nicht gefunden. Wiring/Adresse prüfen!"));
#endif 
    delay(initialize_fail_delay);
  }
#if DEBUG
    Serial.println(F("DS3231-RTC initialisiert!"));
#endif 
  delay(initialize_delay);
  /////DISPLAY
  while(!initDISPLAY(display_var)){
#if DEBUG    
    Serial.println(F("SSD1306-Display nicht gefunden. Check Verkabelung/Adresse!"));
#endif 
    delay(initialize_fail_delay);
  }
  delay(initialize_delay);
#if DEBUG    
    Serial.println(F("SSD1306-Display initialisiert!"));
#endif 
  /////IMU
  while(!initIMU(imu_var)){
#if DEBUG    
    Serial.println(F("FEHLER: MPU9250-Sensor antwortet nicht. Verkabelung/Adresse prüfen!"));
#endif   
    delay(initialize_fail_delay);
  }
#if DEBUG    
    Serial.println(F("MPU9250-Sensor initialisiert!"));
#endif     

  /////TASTERPINS  
  for (int i = 8; i<=12; ++i){
    pinMode(i, INPUT_PULLUP);
  }
#if DEBUG    
  Serial.println(F("Tasterpins initialisiert!"));
#endif 
}

/////////////////////////////////////////

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
      rtc_var.adjust(DateTime(__DATE__, __TIME__));
    } else {
#if DEBUG
      Serial.println(F("RTC hatte keinen Stromverlust (Zeit gültig)."));
#endif
    }
#if SETTIMEONCE    
    rtc_var.adjust(DateTime(__DATE__, __TIME__));
    Serial.println(F("RTC auf Compile-Time gesetzt!"));
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

    imu_var.autoOffsets();  // Kalibrieren
    return true;
  }
  return false;
}

//////////////////////////////////



uint8_t updateButtons(){
  for (uint8_t i= 8; i<=12; ++i){
    if (digitalRead(i)==1){
      return i;
    }
  }
}

//////////////////////////////////


BMEData updateSensors(Adafruit_BME280& bme_var){
  BMEData m;
  m.temp = bme_var.readTemperature();         // °C
  m.humi = bme_var.readHumidity();            // %
  m.baro = bme_var.readPressure() / 100;    // hPa   
  return m;       
}

//////////////////////////////////
/*
HEADING (Kompasskurs)
=====================
0° bis 360°  (oder -180° bis +180°)

       0° / 360°
      (Norden)
           |
           |
270° ------+------ 90°
(Westen)   |     (Osten)
           |
         180°
       (Süden)

Heading	Richtung
0°	    Norden
45°	    Nordosten
90°	    Osten
135°	  Südosten
180°	  Süden
225°	  Südwesten
270°	  Westen
315°	  Nordwesten


ROLL (Krängung)
===============
-180° bis +180°  (oder 0° bis 360°)

     +90°
      |
      | (links hoch)
      |
------+------ 0° (waagrecht)
      |
      | (rechts hoch)
      |
    -90°

Roll	Bedeutung	                    Visualisierung
0°	  Perfekt waagrecht	            ───
+15°	Leichte Neigung nach links	  ╱──
+45°	Starke Neigung nach links	    │╱
+90°	Auf linker Seite liegend	    │
-15°	Leichte Neigung nach rechts	  ──╲
-45°	Starke Neigung nach rechts	  ╲│
-90°	Auf rechter Seite liegend	    │
±180°	Auf dem Kopf	                ───


PITCH (Stampfen)
================
-90° bis +90°  (oder -180° bis +180°)

     +90°
      |
      | (Bug/Nase hoch)
      |
------+------ 0° (waagrecht)
      |
      | (Bug/Nase runter)
      |
    -90°

Pitch	Bedeutung	                                  Visualisierung
0°	  Horizontal	                                →
+10°	Bug leicht oben	                            ↗
+30°	Bug stark oben (Steigflug/Welle)	          ⭱
+90°	Senkrecht nach oben	                        ↑
-10°	Bug leicht unten	                          ↘
-30°	Bug stark unten (Sturzflug/Wellenabfahrt)	  ⭳
-90°	Senkrecht nach unten	                      ↓

*/

IMUData updateNavigation(MPU9250_WE& imu_var){
  IMUData result;
  //Lageerkennung 
  xyzFloat acc = imu_var.getGValues();
  float roll = atan2(acc.y, acc.z) * 180 / PI;
  float pitch = atan2(-acc.x, sqrt(acc.y*acc.y + acc.z*acc.z)) * 180 / PI;
  //Heading
  xyzFloat mag = imu.getMagValues();
  float heading = atan2(mag.y, mag.x) * 180 / PI;
  if (heading < 0) heading += 360;
  //if (heading >360) heading -= 360;

  result.roll = roll; // Roll (Krängung)
  result.pitch = pitch; // Pitch (Stampfen)
  result.heading = heading; //Kompasskurs

  return result;
}

//////////////////////////////////

void updateMenuSystem(uint8_t button){
  
}

void renderDisplay(Adafruit_SSD1306& dis, BMEData& bme_struct, IMUData& imu_var){
  dis.clearDisplay();
  dis.setCursor(0, 0);
  
  dis.print(F("T:   "));
  dis.println(bme_struct.temp,1);
  dis.print(F("H:   "));
  dis.println(bme_struct.humi,1);
  dis.print(F("P:   "));
  dis.println(bme_struct.baro,1);

  

  dis.display();
}

//////////////////////////////////

void handleAlarms(){
  
}


