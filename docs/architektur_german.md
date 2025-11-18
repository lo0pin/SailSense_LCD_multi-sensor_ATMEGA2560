## `src/core/`

### `main.ino`

**Rolle:** Einstiegspunkt des Programms.

**Inhalt:**

* `setup()` und `loop()`
* Ruft Initialisierungsfunktionen aus `system_init` auf
* Nutzt die öffentlichen Schnittstellen der anderen Module, aber enthält **keine** Sensorlogik im Detail

**Typisch:**

```cpp
#include "system_init.h"
#include "menu_system.h"
#include "display.h"
#include "buttons.h"

void setup() {
  systemInit();
}

void loop() {
  updateButtons();
  updateSensors();
  updateNavigation();
  updateMenuSystem();
  renderDisplay();
  handleAlarms();
}
```

---

### `system_init.h` / `system_init.cpp`

**Rolle:** Alle Initialisierungen an einem Ort.

**Inhalt:**

* Funktionen wie `void systemInit();`
* Initialisierung von:

  * Serial (optional)
  * I2C (`Wire.begin()`)
  * Sensoren (BME280, MPU6050, GY-271, RTC)
  * Display
  * Buttons
  * Buzzer
  * globale Zustandsobjekte

**Beispiele (konzeptionell):**

```cpp
// system_init.h
#pragma once
void systemInit();
void updateSensors();
void updateNavigation();
void handleAlarms();
```

```cpp
// system_init.cpp
#include "system_init.h"
#include "bme280_sensor.h"
#include "mpu6050_sensor.h"
#include "gy271_sensor.h"
#include "rtc_module.h"
#include "battery_monitor.h"
#include "display.h"
#include "buttons.h"
#include "buzzer.h"

void systemInit() {
  // Wire, Serial, Sensor-Init, Display-Init etc.
}
```

---

### `types.h`

**Rolle:** Gemeinsame Datentypen und Structs.

**Inhalt:**

* Structs für Messwerte und Zustände, z. B.:

```cpp
#pragma once

struct EnvData {
  float temperature;
  float humidity;
  float pressure;
};

struct MotionData {
  float roll;
  float pitch;
  float yaw;
};

struct HeadingData {
  float headingDeg;
};

struct BatteryStatus {
  float voltage;
  float percentage;
};
```

* Enums für Menüs, Zustände, Fehlercodes etc.

---

### `config.h`

**Rolle:** Zentrale Konfiguration / Konstanten.

**Inhalt:**

* I/O-Pins (Buttons, Buzzer, Batterie ADC, I2C-Adressen)
* Kalibrierkonstanten (Offsets für Kompass, MPU usw.)
* Thresholds (z. B. Low-Battery-Grenze)
* Compile-Time-Flags (z. B. `#define DEBUG 1`)

```cpp
#pragma once

// Pins
constexpr int PIN_BUTTON_1 = 2;
constexpr int PIN_BUTTON_2 = 3;
// ...

// I2C addresses
constexpr uint8_t BME280_ADDR = 0x76;
constexpr uint8_t GY271_ADDR  = 0x1E;

// Batterie
constexpr int PIN_BATTERY_ADC = A0;
constexpr float BATTERY_MAX_V = 4.2f;
constexpr float BATTERY_MIN_V = 3.0f;
```

---

## `src/sensors/`

### `bme280_sensor.h` / `.cpp`

**Rolle:** Kapselt alles rund um den BME280.

**Inhalt:**

* Klasse oder Namespace wie `BME280Sensor`
* `begin()`, `update()`, `getData()`
* Interner Umgang mit der verwendeten BME-Library (Adafruit o. ä.)

```cpp
// bme280_sensor.h
#pragma once
#include "types.h"

namespace BME280Sensor {
  bool begin();
  void update();
  EnvData getEnvData();
}
```

---

### `mpu6050_sensor.h` / `.cpp`

**Rolle:** IMU (Gyro/Accel) für Gier-/Roll-/Nickwinkel.

**Inhalt:**

* Initialisierung des MPU6050
* Rohwerte holen, filtern
* ggf. Orientation-Berechnung (oder Übergabe der Rohdaten an `navigation/motion`)

```cpp
namespace MPU6050Sensor {
  bool begin();
  void update();
  MotionData getMotionDataRaw();  // oder getrennt accel/gyro
}
```

---

### `gy271_sensor.h` / `.cpp`

**Rolle:** Magnetometer (Kompass).

**Inhalt:**

* Kommunikation mit GY-271 (QMC5883L / HMC5883L)
* Rohmagnetfeld lesen
* Optional schon Kompasswinkel berechnen oder nur Rohdaten liefern

```cpp
namespace GY271Sensor {
  bool begin();
  void update();
  float getRawX();
  float getRawY();
  float getRawZ();
}
```

---

### `rtc_module.h` / `.cpp`

**Rolle:** Echtzeituhr.

**Inhalt:**

* Wrapper um RTC-Library (DS3231/DS1307)
* `getTime()`, `setTime()`, evtl. `getUnixTime()`
* Abstraktion, damit der Rest des Codes keine RTC-Library direkt kennt

```cpp
struct DateTimeSimple {
  uint16_t year;
  uint8_t  month, day, hour, minute, second;
};

namespace RTCModule {
  bool begin();
  DateTimeSimple now();
}
```

---

### `battery_monitor.h` / `.cpp`

**Rolle:** Akku-Spannungsmessung.

**Inhalt:**

* Lesen eines ADC-Pins
* Umrechnung auf reale Spannung (Spannungsteiler, Referenzspannung)
* Berechnung eines groben Ladezustands in %

```cpp
namespace BatteryMonitor {
  void begin();
  void update();
  BatteryStatus getStatus();
}
```

---

## `src/ui/`

### `display.h` / `.cpp`

**Rolle:** Abstraktion für das 20×4 I²C-Display.

**Inhalt:**

* Initialisierung des HD44780 über PCF8574
* Funktionen wie:

  * `initDisplay()`
  * `clear()`
  * `printAt(row, col, const char* text)`
  * höhere Abstraktionen wie `showEnvScreen()`, `showNavScreen()`

```cpp
namespace Display {
  void begin();
  void clear();
  void printAt(uint8_t row, uint8_t col, const char* text);
  void renderMainScreen(const EnvData&, const HeadingData&, const BatteryStatus&);
}
```

---

### `buttons.h` / `.cpp`

**Rolle:** Verwaltung der 4 Taster.

**Inhalt:**

* Pin-Definitionen (oder inkl. `config.h`)
* Entprellung
* Erkennung von Klick, Long-Press, Repeat
* Event-Interface für Menüsystem

```cpp
enum class ButtonEventType { None, ShortPress, LongPress };

struct ButtonEvent {
  uint8_t buttonId;
  ButtonEventType type;
};

namespace Buttons {
  void begin();
  void update();
  bool getNextEvent(ButtonEvent& ev);
}
```

---

### `menus/menu_system.h` / `.cpp`

**Rolle:** Menüs, Screens, Zustandsmaschine für die UI.

**Inhalt:**

* Enum für Screens/Menüs (z. B. ENV, NAV, TIMER, SETTINGS …)
* Zustandsverwaltung der aktuellen Ansicht
* Reaktion auf Button-Events
* Aufruf von Display-Funktionen zum Rendern

```cpp
enum class ScreenId {
  ENV,
  NAV,
  TIMER,
  ALARM,
  SETTINGS
};

namespace MenuSystem {
  void begin();
  void update();               // verarbeitet Button-Events, wechselt Screens
  ScreenId getCurrentScreen();
}
```

---

## `src/alerts/`

### `buzzer.h` / `.cpp`

**Rolle:** Akustische Signale & Alarme.

**Inhalt:**

* Abstraktion für:

  * kurzer Beep (Button-Feedback)
  * Pattern für Alarm
  * evtl. non-blocking Tonsteuerung über `millis()`

```cpp
namespace Buzzer {
  void begin();
  void beepOk();
  void beepError();
  void alarmOn();
  void alarmOff();
  void update();   // für zeitbasierte Tonmuster
}
```

---

## `src/navigation/`

### `heading.h` / `.cpp`

**Rolle:** Berechnet schiffstauglichen Kurs/Heading aus Magnetometer & ggf. Neigung.

**Inhalt:**

* Kombiniert GY-271-Daten mit optionaler Neigungskorrektur (Aus Roll/Pitch)
* Filterung des Winkels (gleitender Mittelwert, Lowpass)
* Schnittstelle für den Rest des Systems:

```cpp
namespace Heading {
  void begin();
  void update();
  HeadingData getHeading();
}
```

---

### `motion.h` / `.cpp`

**Rolle:** Bewegungs-/Lageberechnung aus MPU6050.

**Inhalt:**

* Rohwerte von `MPU6050Sensor` holen
* Sensorfusion / Filter (z. B. Complementary Filter)
* Ausgabetyp: Roll-/Pitch-/Yaw-Winkel

```cpp
namespace Motion {
  void begin();
  void update();
  MotionData getMotion();
}
```

---

## `src/utils/`

### `filter.h` / `.cpp`

**Rolle:** Allgemeine Filter (wiederverwendbar).

**Inhalt:**

* Klassen/Funktionen für:

  * gleitender Mittelwert
  * Exponential Moving Average
  * ggf. einfachen 1. Ordnung Lowpass

```cpp
class MovingAverage {
public:
  MovingAverage(size_t size);
  float update(float value);
private:
  // Buffer, Index, Sum etc.
};
```

---

### `math_utils.h` / `.cpp`

**Rolle:** Mathematische Hilfsfunktionen.

**Inhalt:**

* Winkel-Normalisierung (`wrapAngle360`, `wrapAngle180`)
* Umrechnungen (z. B. mbar → hPa, Grad ↔ rad)
* Interpolation, Mapping, evtl. einfache Statistik

```cpp
namespace MathUtils {
  float wrapAngle360(float deg);
  float wrapAngle180(float deg);
  float lerp(float a, float b, float t);
}
```

---

## Andere Ordner (kurz)

### `docs/`

* `architecture.md`: Erklärung deiner Module, Datenflüsse, Zustände
* `hardware.md`: Pinouts, Module, Spannungsteiler für Batterie
* `roadmap.md`: ToDo, geplante Features
* `wiring_diagrams/`: Bilder, Fritzing, PDFs

### `examples/`

* Kleine Testsketches, z. B.:

  * `simple_bme280_test/` – nur BME auslesen
  * `display_demo/` – Text auf 20×4 ausgeben

### `hardware/`

* `schematics/`: Schaltpläne (KiCad, PDF)
* `board_layout/`: PCB-Layouts
* `power/`: Details zum Batterieteil, Laderegler, Step-Up/Down, etc.

### `tools/`

* `calibration_scripts/`: z. B. kleine Python-Skripte zur Kompasskalibrierung
* `data_export/`: Tools, um Logdaten auszulesen und als CSV zu speichern


