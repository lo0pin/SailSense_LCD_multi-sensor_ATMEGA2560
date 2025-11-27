/***************************************************************************
“Showcase”-Sketch für den MPU9250, der mit der MPU9250_WE-Library (von Wolle/Ewald) sehr viele spannende Features demonstriert: Rohdaten, SI-Einheiten, Range-Settings, Low-Pass-Filter (DLPF), Sample-Rate-Divider, Auto-Offsets, Tilt-Winkel, einfache Yaw/Pitch/Roll-Schätzung, Magnetometer-Kalibrier-Nutzung, FIFO-Lesen und Data-Ready-Interrupt.
Die Library ist aktuell gepflegt, I2C+SPI, FIFO und Interrupts werden unterstützt.
Hinweis zur Realität: Der MPU9250 ist EoL (wird aber noch gut verkauft). Die Beispiele bleiben trotzdem golden
API-Hinweis: Die MPU9250_WE-Library hat viele Funktionen (≈59). Kleinigkeiten im Funktionsnamen können je nach Version minimal anders heißen — falls der Compiler meckert: im Library-Beispielordner nachschlagen und 1:1 anpassen. 

Verdrahtung (I2C, Standard-Breakout)
====================================
VCC → 3.3 V (manche Boards haben Regler, aber 3.3 V ist sauberer)
GND → GND
SDA → SDA
SCL → SCL
AD0 → GND ⇒ Adresse 0x68
(AD0 → VCC ⇒ 0x69)
Optional: INT → D2 (Data-Ready Interrupt)

Library installieren
====================
Arduino IDE → Bibliotheksverwaltung → “MPU9250_WE” suchen und installieren. 



 * MPU9250_WE Showcase Sketch
 * 
 * Zeigt viele interessante Funktionen des MPU9250:
 *  1) Verbindungstest / WHO_AM_I
 *  2) Rohdaten + SI-Einheiten (g, deg/s, µT, °C)
 *  3) Einstellen von Messbereichen (Acc/Gyro/Mag)
 *  4) Digital Low Pass Filter (DLPF) ein/aus + Bandbreite
 *  5) Sample Rate Divider (Ausgaberate beeinflussen)
 *  6) Auto-Offsets (Gyro/Acc) + Nutzung manuell gesetzter Offsets
 *  7) Tilt-Winkel aus Beschleunigung (Roll/Pitch)
 *  8) Yaw/Pitch/Roll grob mit Mag-Kompensation (simple Fusion)
 *  9) Data-Ready Interrupt (optional)
 * 10) FIFO Buffer lesen (Kurzdemo)
 * 
 * Hardware: MPU9250 Breakout via I2C (Adresse 0x68)
 * INT-Pin optional an D2
 ***************************************************************************/

#include <Wire.h>
#include <MPU9250_WE.h>

#define MPU9250_ADDR 0x68
#define INT_PIN 2          // optional, falls INT verbunden ist

MPU9250_WE imu = MPU9250_WE(MPU9250_ADDR);

// --- kleine Helfer für hübsche Ausgabe ---
void printVec3(const char* label, xyzFloat v, const char* unit) {
  Serial.print(label);
  Serial.print(":  X=");
  Serial.print(v.x, 3);
  Serial.print("  Y=");
  Serial.print(v.y, 3);
  Serial.print("  Z=");
  Serial.print(v.z, 3);
  Serial.print(" ");
  Serial.println(unit);
}

// Für simple Yaw/Pitch/Roll Fusion (nur Demo, kein EKF):
// Roll/Pitch aus Acc, Yaw aus Mag (tilt-compensated).
void computeSimpleYPR(xyzFloat acc_g, xyzFloat mag_uT, float& yaw, float& pitch, float& roll) {
  // Roll/Pitch aus Acc (klassisch)
  roll  = atan2(acc_g.y, acc_g.z);
  pitch = atan2(-acc_g.x, sqrt(acc_g.y * acc_g.y + acc_g.z * acc_g.z));

  // Tilt-Compensation fürs Mag
  float cr = cos(roll),  sr = sin(roll);
  float cp = cos(pitch), sp = sin(pitch);

  float mx = mag_uT.x * cp + mag_uT.z * sp;
  float my = mag_uT.x * sr * sp + mag_uT.y * cr - mag_uT.z * sr * cp;

  yaw = atan2(-my, mx);

  // in Grad umrechnen
  yaw   *= 180.0 / PI;
  pitch *= 180.0 / PI;
  roll  *= 180.0 / PI;
}

volatile bool dataReady = false;

// ISR: wird getriggert, wenn MPU "Data Ready" signalisiert
void IRAM_ATTR isrDataReady() {
  dataReady = true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { /* Leonardo/ESP32 etc. */ }

  Serial.println("\nMPU9250_WE Showcase gestartet.");

  Wire.begin();
  Wire.setClock(400000);       // Fast I2C, MPU kann bis 400kHz

  // ---------- 1) Init ----------
  if (!imu.init()) {
    Serial.println("FEHLER: MPU9250 antwortet nicht. Verkabelung/Adresse prüfen!");
    while (1) {}
  }
  Serial.println("MPU9250 verbunden.");

  // ---------- 2) Sensorbereiche setzen ----------
  // Acc: +/-2g,4g,8g,16g
  imu.setAccRange(MPU9250_ACC_RANGE_8G);     // guter Allround-Bereich
  // Gyro: +/-250,500,1000,2000 deg/s
  imu.setGyrRange(MPU9250_GYRO_RANGE_500);   // ausreichend dynamisch
  // Mag: full-scale wird intern gesetzt; wir wählen Betriebsmodus:
  imu.setMagOpMode(AK8963_CONT_MODE_100HZ);  // kontinuierlich, 100 Hz

  Serial.println("Ranges gesetzt: Acc 8G, Gyro 500 dps, Mag 100Hz.");

  // ---------- 3) Low Pass Filter ----------
  // Beschleuniger-LPF
  imu.enableAccDLPF(true);
  imu.setAccDLPF(MPU9250_DLPF_6);  // niedrige Bandbreite => ruhiger, aber träger
  // Gyro-LPF
  imu.enableGyrDLPF();
  imu.setGyrDLPF(MPU9250_DLPF_6);

  Serial.println("DLPF aktiviert (Stufe 6).");

  // ---------- 4) Sample Rate Divider ----------
  // Output-Rate = Grundrate / (1 + Divider)
  // Beispiel: wenn Grundrate 1kHz und Divider=9 => 100Hz.
  imu.setSampleRateDivider(9);
  Serial.println("SampleRateDivider auf 9 gesetzt (~100Hz je nach Grundrate).");

  // ---------- 5) Auto-Offets / Kalibrierung ----------
  // Gyro+Acc Offsets automatisch ermitteln:
  // * Sensor muss ruhig liegen!
  Serial.println("Auto-Offsets... bitte Sensor ruhig liegen lassen.");
  delay(1000);
  imu.autoOffsets();   // intern: Mittelwert bilden, Register setzen
  Serial.println("Auto-Offsets fertig.");

  // Optional: manuelle Offsets setzen (Demo; Werte nur Beispiel!)
  // imu.setAccOffsets(minX, maxX, minY, maxY, minZ, maxZ);
  // imu.setGyrOffsets(offX, offY, offZ);

  // ---------- 6) Interrupt (optional) ----------
  pinMode(INT_PIN, INPUT_PULLUP);
  // Data Ready Interrupt aktivieren
  imu.enableDataReadyInterrupt();
  attachInterrupt(digitalPinToInterrupt(INT_PIN), isrDataReady, RISING);

  Serial.println("DataReady Interrupt aktiviert (falls INT verdrahtet).");

  // ---------- 7) FIFO (Kurzsetup) ----------
  // FIFO kann Acc/Gyro puffern, z.B. für Burst-Reads.
  // Wir konfigurieren FIFO so, dass Acc+Gyro reingehen.
  imu.enableFifo(true);
  imu.setFifoAcc(true);
  imu.setFifoGyr(true);
  imu.resetFifo();
  Serial.println("FIFO aktiviert für Acc+Gyro.");
}

unsigned long lastPrint = 0;
const unsigned long printIntervalMs = 200; // 5 Hz Ausgabe

void loop() {

  // Wenn Interrupt nicht genutzt wird (INT nicht verbunden),
  // einfach regelmäßig pollen:
  if (!dataReady) {
    // minimal warten, um Bus zu entlasten
    delay(2);
  }

  // Sobald neue Daten da sind, Flag löschen
  if (dataReady) dataReady = false;

  // ---------- 8) Basisdaten auslesen ----------
  // Rohdaten (int16) bekommt man auch, aber Library liefert komfortabel SI:
  xyzFloat acc_g   = imu.getGValues();      // g
  xyzFloat gyro_dps= imu.getGyrValues();    // deg/s
  xyzFloat mag_uT  = imu.getMagValues();    // µT
  float tempC      = imu.getTemperature();  // °C

  // ---------- 9) Tilt-Winkel (Roll/Pitch) ----------
  // Library hat hierfür Helfer:
  xyzFloat angles = imu.getAngles(); // Grad: x=Roll, y=Pitch, z=Yaw (je nach Lib-Konvention)
  MPU9250_orientation orient = imu.getOrientation(); // enum: PORTRAIT/LANDSCAPE/... (optional)

  // ---------- 10) Simple YPR Fusion (Demo) ----------
  float yaw, pitch, roll;
  computeSimpleYPR(acc_g, mag_uT, yaw, pitch, roll);

  // ---------- 11) FIFO Demo ----------
  // Wir lesen gelegentlich den FIFO-Füllstand und holen ein paar Samples.
  // Achtung: FIFO kann überlaufen, wenn man nie liest.
  if (millis() % 2000 < 5) {  // ca. alle 2 Sekunden
    int16_t fifoCount = imu.getFifoCount(); // Bytes im FIFO
    Serial.print("\nFIFO count (bytes): ");
    Serial.println(fifoCount);

    if (fifoCount >= 12) { // 6 Bytes Acc + 6 Bytes Gyro pro Sample
      xyzFloat fifoAcc = imu.getFifoAcc();   // holt ein Sample
      xyzFloat fifoGyr = imu.getFifoGyr();
      Serial.println("FIFO sample:");
      printVec3("  AccFIFO", fifoAcc, "g");
      printVec3("  GyrFIFO", fifoGyr, "deg/s");
    }
  }

  // ---------- 12) Ausgabe throttlen ----------
  if (millis() - lastPrint >= printIntervalMs) {
    lastPrint = millis();

    Serial.println("\n--- LIVE DATA ---");
    printVec3("Acc",  acc_g,    "g");
    printVec3("Gyro", gyro_dps, "deg/s");
    printVec3("Mag",  mag_uT,   "uT");

    Serial.print("Temp: ");
    Serial.print(tempC, 2);
    Serial.println(" °C");

    Serial.print("Library Angles (deg): ");
    Serial.print("Roll=");
    Serial.print(angles.x, 1);
    Serial.print(" Pitch=");
    Serial.print(angles.y, 1);
    Serial.print(" Yaw=");
    Serial.println(angles.z, 1);

    Serial.print("Simple YPR (deg): ");
    Serial.print("Yaw=");
    Serial.print(yaw, 1);
    Serial.print(" Pitch=");
    Serial.print(pitch, 1);
    Serial.print(" Roll=");
    Serial.println(roll, 1);

    Serial.print("Orientation enum: ");
    Serial.println((int)orient);

    // Komfort: Resultate auch als "Kalibrier-Check"
    if (abs(acc_g.x) < 0.05 && abs(acc_g.y) < 0.05) {
      Serial.println("Hinweis: Sensor liegt vermutlich flach (Acc X/Y nahe 0).");
    }
  }
}

