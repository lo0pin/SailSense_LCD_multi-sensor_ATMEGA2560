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

#define booting_display_message_delay 300

uint16_t delaytime_for_loop = 300;
unsigned long globaltimer =0;
uint8_t counter_for_measurment_within_loop = 0;

uint8_t old_hour = 99;
uint8_t old_minute = 99;
uint8_t mittelwert_divisor = 0;

uint8_t buttoninput = 1;
uint8_t current_display = 0;
uint8_t max_number_of_displays = 5;

/////////////////////////////////////////

BMEData hourly_summe_bme;
BMEData mittelw_bme;

/////////////////////////////////////////

float temp_messungen[array_len] = {
  -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1
};
float humid_messungen[array_len] = {
  -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1
};
float baro_messungen[array_len] = {
  -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1
};

float  mittelwert_magnetkurse[mag_mittelwerte] = {
  -1, -1, -1, -1, -1, 
  -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, 
  -1, -1, -1, -1, -1
};

uint8_t mag_mittelwert_index = 0;
uint16_t mag_geglaettet = 0;

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

    // WICHTIG: Magnetometer explizit aktivieren
    imu_var.initMagnetometer();  // <- Diese Zeile hinzufügen!
    
    // Mag: Betriebsmodus setzen
    imu_var. setMagOpMode(AK8963_CONT_MODE_100HZ); 
     
    delay(100);
    
    imu_var.autoOffsets();  // Kalibrieren
    return true;
  }
  return false;
}

//////////////////////////////////

uint8_t updateButtons(){
  for (uint8_t i= 8; i<=12; ++i){
    if (digitalRead(i)==0){
      return i-7;
    }
  }
  return 0;
}

//////////////////////////////////


BMEData updateSensors(Adafruit_BME280& bme_var){
  BMEData m;
  m.temp = bme_var.readTemperature();         // °C
  m.humi = bme_var.readHumidity();            // %
  m.baro = bme_var.readPressure() / 100;    // hPa   
  return m;       
}

BMEData get_mittelwert(BMEData& bme_now){
  BMEData result;
  if (mittelwert_divisor == 0){
    hourly_summe_bme.temp = bme_now.temp;
    hourly_summe_bme.humi = bme_now.humi;
    hourly_summe_bme.baro = bme_now.baro;
  } else {
    hourly_summe_bme.temp += bme_now.temp;
    hourly_summe_bme.humi += bme_now.humi;
    hourly_summe_bme.baro += bme_now.baro;        
  }
  mittelwert_divisor++;
  result.temp = hourly_summe_bme.temp/mittelwert_divisor;
  result.humi = hourly_summe_bme.humi/mittelwert_divisor;
  result.baro = hourly_summe_bme.baro/mittelwert_divisor; 
  return result; 
}

//////////////////////////////////

IMUData updateNavigation(MPU9250_WE& imu_var){
  IMUData result;

  // --- Sensorwerte holen ---
  xyzFloat acc = imu_var.getGValues();
  xyzFloat mag = imu_var.getMagValues();

  // === Achsen-Mapping gemäß deiner Hardware ===
  // Board-Aufdruck: Y zeigt nach vorne (Heading), X nach rechts
  float Ax = acc.y;   // Vorwärts (North)
  float Ay = acc.x;   // Rechts   (East)
  float Az = acc.z;   // Vertikal (Up/Down)

  float Mx = mag.y;   // Magnetfeld in Vorwärtsrichtung
  float My = mag.x;   // Magnetfeld nach rechts
  float Mz = mag.z;   // Magnetfeld vertikal

  // --- Lage (Roll & Pitch) in RAD ---
  float roll  = atan2(Ay, Az);                         // Roll um Vorwärtsachse
  float pitch = atan2(-Ax, sqrt(Ay*Ay + Az*Az));       // Pitch (Nase hoch/runter)

  // --- Tilt-Kompensation Magnetometer ---
  float cosRoll  = cos(roll);
  float sinRoll  = sin(roll);
  float cosPitch = cos(pitch);
  float sinPitch = sin(pitch);

  // In horizontale Ebene transformieren:
  float Xh = Mx * cosPitch + Mz * sinPitch;
  float Yh = Mx * sinRoll * sinPitch + My * cosRoll - Mz * sinRoll * cosPitch;

  // --- Heading berechnen ---
  // Heading = atan2(East, North)
  float headingRad = atan2(Yh, Xh);
  float headingDeg = headingRad * 180.0f / PI;
  if (headingDeg < 0) headingDeg += 360.0f;

  // Wenn Drehrichtung invertiert ist (Test!):
  // headingDeg = fmod(360.0f - headingDeg, 360.0f);

  // --- Ergebnisse in Grad ins Struct ---
  result.roll    = roll  * 180.0f / PI;    // Roll (Krängung)
  result.pitch   = pitch * 180.0f / PI;    // Pitch (Stampfen)
  result.heading = headingDeg;             // Kompasskurs

  return result;
}


//////////////////////////////////

void updateMenuSystem(uint8_t button){
  if (button == 1) {
    current_display = (current_display+1)%max_number_of_displays;
  } else if (button == 2) {
    current_display = (current_display+max_number_of_displays-1)%max_number_of_displays;
  }
}

void renderDisplay_Setup(Adafruit_SSD1306& dis, uint8_t mode){
  dis.clearDisplay();

  dis.setTextSize(2);
  dis.println(F("SailSense"));
  
  dis.setTextSize(1);
  dis.println(F("by Julian Kampitsch"));
  dis.println(F("2025"));
  dis.println(F(""));
  if (mode == 1){
    dis.print(F("booting"));
    dis.display();
    
    
    for (uint8_t i = 0; i<3; ++i){
      delay(booting_display_message_delay);
      dis.print(F("."));
      dis.display();    
    }    
  } else {
    dis.print(F("push button to continue..."));
    dis.display();  
  }

}

/*
void renderDisplay_everyLoop(Adafruit_SSD1306& dis){
  dis.clearDisplay();
  dis.setCursor(0, 0);
}
*/


void renderDisplay(Adafruit_SSD1306& dis, BMEData& bme_struct, IMUData& imu_struct, DateTime dt, uint8_t displaymode){
  dis.clearDisplay();
  dis.setCursor(0, 0);

  switch(displaymode){
    case 0:{
      renderDisplay_Setup(dis, 0);
      break;
    }
    case 1:{
      dis.setTextSize(1);
      dis.print(dt.hour()); dis.print(F(":")); dis.print(dt.minute()); dis.print(F(":")); dis.print(dt.second()); dis.print(F("   ")); 
      dis.print(dt.day()); dis.print(F(".")); dis.print(dt.month()); dis.print(F(".")); dis.println(dt.year()); 
      
      dis.print(F("Temp:    "));
      float temp_temp = bme_struct.temp;
//      if (temp_temp < 10);
      dis.println(temp_temp,1);
      dis.print(F("Hygr:    "));
      dis.println(bme_struct.humi,1);
      dis.print(F("Baro:   "));
      float temp_baro = bme_struct.baro;
      if (temp_baro < 1000) dis.print(F(" "));
      dis.println(bme_struct.baro,1);
    
      dis.print(F("Roll:    "));
      if (imu_struct.roll>=0) dis.print(F(" "));
      dis.println(imu_struct.roll,1);
      dis.print(F("Pitch:   "));
      if (imu_struct.pitch>=0) dis.print(F(" "));
      dis.println(imu_struct.pitch,1);
      dis.print(F("Magn:    "));
      float temp_heading = imu_struct.heading;
      if (temp_heading<100) dis.print(F("0"));
      if (temp_heading<10) dis.print(F("0"));
      dis.println(imu_struct.heading,1);   
      dis.display(); 
      break;
    }
    case 2:{
      dis.setTextSize(2);
      dis.print(weekdayName(dt.dayOfTheWeek())); dis.println(F(","));
      dis.print(dt.day()); dis.print(F(".")); dis.print(dt.month()); dis.print(F(".")); dis.println(dt.year()); 
      dis.print(dt.hour()); dis.print(F(":")); dis.print(dt.minute()); dis.print(F(":")); dis.println(dt.second());
      dis.display(); 
      break;
    }
    case 3:{
      dis.setTextSize(2);
      dis.print(F("T:  "));
      dis.println(bme_struct.temp,1);
      dis.print(F("H:  "));
      dis.println(bme_struct.humi,1);
      dis.print(F("B: "));
      dis.println(bme_struct.baro,1);
      dis.display(); 
      break;
      
    }
    case 4:{
      dis.setTextSize(2);
      dis.print(F("R: "));
      float temp_roll = imu_struct.roll;
      if (temp_roll>=0) dis.print(F(" "));
      if (temp_roll>=0 && temp_roll <10) dis.print(F(" "));
      if (temp_roll<0 && temp_roll >-10) dis.print(F(" "));
      dis.println(temp_roll,1);
      dis.print(F("P: "));
      float temp_pitch = imu_struct.pitch;
      if (temp_pitch>=0) dis.print(F(" "));
      if (temp_pitch>=0 && temp_pitch <10) dis.print(F(" "));
      if (temp_pitch<0 && temp_pitch >-10) dis.print(F(" "));
      dis.println(temp_pitch,1);
      dis.print(F("M: "));
      float temp_heading = imu_struct.heading;
      if (temp_heading<100) dis.print(F("0"));
      if (temp_heading<10) dis.print(F("0"));
      dis.println(imu_struct.heading,1);   
      dis.display(); 
      break;      
    }

  }  
}

float get_mag_mittelwert(float cur_head){
  float result = 0;
  uint8_t act_index = (mag_mittelwert_index+1)%mag_mittelwerte;
  mittelwert_magnetkurse[act_index] = cur_head;
  uint8_t divisor_counter = 0;
  for (uint8_t i = 0; i< mag_mittelwerte; ++i){
    float current_v = mittelwert_magnetkurse[i];
    
    if (current_v != -1){
      result += current_v;
      divisor_counter++;
      
    }
  }
  return result/divisor_counter;
}


const char* weekdayName(uint8_t wday) {
  static const char* names[] = {
    "Sonntag", "Montag", "Dienstag",
    "Mittwoch", "Donnerstag", "Freitag", "Samstag"
  };

  if (wday > 6) return "???";
  return names[wday];
}

//////////////////////////////////

void handleAlarms(){
  
}

