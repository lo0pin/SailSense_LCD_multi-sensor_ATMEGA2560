/*
Verdrahtung (I2C)
=================
VCC → 3.3 V (5 V geht bei vielen Breakouts auch, aber 3.3 V ist eleganter)
GND → GND
SDA → SDA
SCL → SCL
SDO/ADDR bestimmt 0x76/0x77 (wenn überhaupt rausgeführt)

Library
=======
Arduino IDE → Bibliotheksverwaltung → “Adafruit BME280 Library” + “Adafruit Unified Sensor” installieren.
*/



/******************************************************************************
 * BME280_WEATHER_SHOWCASE.ino
 *
 * Ziel: Möglichst viele relevante Funktionen der Adafruit_BME280 Library
 *       in EINEM Sketch demonstrieren.
 *
 * Gezeigt werden:
 *  1) Sensor finden / begin() mit Adressen-Scan
 *  2) Sensor-Metadaten via getSensor() (Unified Sensor)
 *  3) setSampling(): Oversampling, Filter, Standby, Normal vs Forced Mode
 *  4) takeForcedMeasurement() im FORCED-Mode
 *  5) readTemperature(), readPressure(), readHumidity()
 *  6) readAltitude(seaLevel_hPa) + seaLevelForAltitude()
 *  7) abgeleitete Größen: Taupunkt (simple Formel), absolute Feuchte (optional)
 *  8) Profil-Umschaltung (Weather Station / Humidity / Indoor Nav)
 *
 * Hinweis:
 *  - Druck kommt in Pascal (Pa) -> wir rechnen für Ausgabe in hPa um.
 *  - readAltitude() braucht eine Referenz für Meeresspiegel-Druck (hPa).
 ******************************************************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // I2C

// Typischer Standard-Meeresspiegeldruck.
// Für genaue Höhenangaben IMMER lokalen Wert verwenden,
// z.B. von Wetterstation/Met-Service.
#define SEALEVELPRESSURE_HPA 1013.25f

// --- Laufzeitsteuerung ---
enum Profile {
  PROFILE_WEATHER_STATION,
  PROFILE_HUMIDITY_SENSING,
  PROFILE_INDOOR_NAV
};

Profile currentProfile = PROFILE_WEATHER_STATION;

unsigned long lastMeasure = 0;
unsigned long measureIntervalMs = 1000; // wird von Profilen geändert

// ---------- Hilfsfunktionen ----------

// versucht beide üblichen I2C-Adressen.
// Gibt true zurück, wenn ein Sensor reagiert.
bool initBME280() {
  // Erst Adresse 0x76 versuchen
  if (bme.begin(0x76)) {
    Serial.println(F("BME280 gefunden auf Adresse 0x76."));
    return true;
  }
  // Dann 0x77
  if (bme.begin(0x77)) {
    Serial.println(F("BME280 gefunden auf Adresse 0x77."));
    return true;
  }

  return false;
}

// gibt Sensor-Infos aus dem Unified-Sensor-Interface aus
void printSensorDetails() {
  sensor_t temp, press, hum;

  bme.getTemperatureSensor(&temp);
  bme.getPressureSensor(&press);
  bme.getHumiditySensor(&hum);

  Serial.println(F("\n--- SENSOR DETAILS (Unified Sensor) ---"));

  Serial.print(F("Temp-Sensor:   "));
  Serial.print(temp.name);
  Serial.print(F("  Range: "));
  Serial.print(temp.min_value);
  Serial.print(F(" .. "));
  Serial.print(temp.max_value);
  Serial.println(F(" °C"));

  Serial.print(F("Press-Sensor:  "));
  Serial.print(press.name);
  Serial.print(F("  Range: "));
  Serial.print(press.min_value);
  Serial.print(F(" .. "));
  Serial.print(press.max_value);
  Serial.println(F(" hPa"));

  Serial.print(F("Hum-Sensor:    "));
  Serial.print(hum.name);
  Serial.print(F("  Range: "));
  Serial.print(hum.min_value);
  Serial.print(F(" .. "));
  Serial.print(hum.max_value);
  Serial.println(F(" %"));
}

// setzt „Szenarien“ ähnlich wie Adafruit advancedsettings example.
// Das ist die wichtigste „Profi“-Funktion der Library.
void applyProfile(Profile p) {
  currentProfile = p;

  switch (p) {

    // ------------- Wetterstation / Langsam, sauber -------------
    case PROFILE_WEATHER_STATION:
      Serial.println(F("\n[Profil] WEATHER STATION"));
      Serial.println(F("Forced mode, Oversampling x1/x1/x1, Filter OFF"));
      bme.setSampling(
        Adafruit_BME280::MODE_FORCED,   // misst nur, wenn wir es anstoßen
        Adafruit_BME280::SAMPLING_X1,   // Temp oversampling
        Adafruit_BME280::SAMPLING_X1,   // Press oversampling
        Adafruit_BME280::SAMPLING_X1,   // Hum oversampling
        Adafruit_BME280::FILTER_OFF     // IIR Filter aus
        // Standby entfällt im FORCED-Mode
      );

      // Empfehlung: 1 Messung pro Minute. Wir machen hier demohaft 5 s.
      measureIntervalMs = 5000;
      break;

    // ------------- Feuchtesensor / schnell, Druck egal -------------
    case PROFILE_HUMIDITY_SENSING:
      Serial.println(F("\n[Profil] HUMIDITY SENSING"));
      Serial.println(F("Forced mode, Temp x1, Hum x1, Pressure OFF"));
      bme.setSampling(
        Adafruit_BME280::MODE_FORCED,
        Adafruit_BME280::SAMPLING_X1,       // Temp
        Adafruit_BME280::SAMPLING_NONE,     // Pressure aus (spart Zeit+Strom)
        Adafruit_BME280::SAMPLING_X1,       // Hum
        Adafruit_BME280::FILTER_OFF
      );

      // Empfehlung: ~1 Hz
      measureIntervalMs = 1000;
      break;

    // ------------- Indoor Navigation / schnell + geglättet -------------
    case PROFILE_INDOOR_NAV:
      Serial.println(F("\n[Profil] INDOOR NAVIGATION"));
      Serial.println(F("Normal mode, Press x16, Temp x2, Hum x1, Filter x16, Standby 0.5ms"));
      bme.setSampling(
        Adafruit_BME280::MODE_NORMAL,   // Sensor misst ständig im Hintergrund
        Adafruit_BME280::SAMPLING_X2,   // Temp
        Adafruit_BME280::SAMPLING_X16,  // Pressure hoch oversampled für Höhenauflösung
        Adafruit_BME280::SAMPLING_X1,   // Hum
        Adafruit_BME280::FILTER_X16,    // IIR-Filter glättet Druckrauschen
        Adafruit_BME280::STANDBY_MS_0_5 // Pause zwischen Messungen
      );

      // In Normal-Mode können wir schneller lesen.
      measureIntervalMs = 200; // ~5 Hz
      break;
  }
}

// sehr einfache Taupunkt-Schätzung (Magnus-Formel light)
// gut genug für Bastelzwecke
float dewPointC(float tempC, float relHum) {
  const float a = 17.62;
  const float b = 243.12; // °C
  float gamma = (a * tempC) / (b + tempC) + log(relHum / 100.0);
  return (b * gamma) / (a - gamma);
}

// optional: Meeresspiegel-Druck aus lokaler Höhe rückrechnen
float seaLevelFromLocal(float pressure_hPa, float altitude_m, float tempC) {
  // Adafruit bietet dafür direkt seaLevelForAltitude()
  // Aber die Version mit Temp ist physikalisch näher dran.
  // Hier nutzen wir die Library-Funktion:
  return bme.seaLevelForAltitude(altitude_m, pressure_hPa);
}

// ---------- Arduino Standard ----------

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Serial.println(F("\nBME280 Showcase startet..."));

  Wire.begin();
  Wire.setClock(400000); // Fast I2C

  // 1) Sensor finden
  if (!initBME280()) {
    Serial.println(F("Keinen BME280 gefunden. Check Wiring/Adresse!"));
    while (1) {}
  }

  // 2) Sensor-Infos
  printSensorDetails();

  // 3) Default-Profil setzen
  applyProfile(PROFILE_WEATHER_STATION);

  lastMeasure = millis();
}

void loop() {

  // 4) Profil-Umschalten alle 20 Sekunden (Demo)
  //    Damit siehst du live, wie Sampling/Filter das Verhalten ändern.
  static unsigned long lastProfileSwitch = 0;
  if (millis() - lastProfileSwitch > 20000) {
    lastProfileSwitch = millis();

    if (currentProfile == PROFILE_WEATHER_STATION)
      applyProfile(PROFILE_HUMIDITY_SENSING);
    else if (currentProfile == PROFILE_HUMIDITY_SENSING)
      applyProfile(PROFILE_INDOOR_NAV);
    else
      applyProfile(PROFILE_WEATHER_STATION);
  }

  // 5) Mess-Intervall berücksichtigen
  if (millis() - lastMeasure < measureIntervalMs) return;
  lastMeasure = millis();

  // 6) FORCED-Mode braucht takeForcedMeasurement(),
  //    sonst liefert read... alte Werte.
  if (currentProfile != PROFILE_INDOOR_NAV) {
    bool ok = bme.takeForcedMeasurement();
    if (!ok) {
      Serial.println(F("takeForcedMeasurement() fehlgeschlagen (I2C-Problem?)."));
      return;
    }
  }

  // 7) Basiswerte lesen
  float tempC = bme.readTemperature();         // °C
  float humRH = bme.readHumidity();            // %
  float pressPa = bme.readPressure();          // Pa
  float presshPa = pressPa / 100.0f;           // hPa

  // 8) Höhe aus Druck (nur sinnvoll, wenn sea level pressure halbwegs stimmt)
  float altitudeM = bme.readAltitude(SEALEVELPRESSURE_HPA);

  // 9) Meeresspiegel-Druck rückrechnen, wenn du lokale Höhe kennst.
  // Beispiel: du weißt, du bist auf 450m Seehöhe:
  const float localAltitudeM = 450.0f;
  float seaLevel_hPa = seaLevelFromLocal(presshPa, localAltitudeM, tempC);

  // 10) Taupunkt
  float dewC = dewPointC(tempC, humRH);

  // 11) Ausgabe
  Serial.println(F("\n--- MEASUREMENT ---"));

  Serial.print(F("Temp:     "));
  Serial.print(tempC, 2);
  Serial.println(F(" °C"));

  Serial.print(F("Humidity: "));
  Serial.print(humRH, 1);
  Serial.println(F(" %"));

  Serial.print(F("Pressure: "));
  Serial.print(presshPa, 2);
  Serial.println(F(" hPa"));

  Serial.print(F("Altitude (ref "));
  Serial.print(SEALEVELPRESSURE_HPA, 2);
  Serial.print(F(" hPa): "));
  Serial.print(altitudeM, 1);
  Serial.println(F(" m"));

  Serial.print(F("SeaLevel pressure (from local "));
  Serial.print(localAltitudeM, 0);
  Serial.print(F(" m): "));
  Serial.print(seaLevel_hPa, 2);
  Serial.println(F(" hPa"));

  Serial.print(F("Dew point: "));
  Serial.print(dewC, 2);
  Serial.println(F(" °C"));

  // 12) Kleiner Plausibilitäts-Check
  if (humRH > 95.0)
    Serial.println(F("Hinweis: Sehr hohe rel. Feuchte — Kondensationsgefahr möglich."));
}
