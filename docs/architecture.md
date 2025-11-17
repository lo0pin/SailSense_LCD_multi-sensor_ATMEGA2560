# **SailSense – Project Architecture**

This document describes the overall structure and responsibilities of each module in the SailSense project.
The goal is clean modularity, low resource usage and simple extensibility for future features like timers, alarms, GPS integration and communication interfaces.

---

# **Directory Overview**

```
SailSense/
│
├─ src/
│   ├─ core/
│   │   ├─ main.ino
│   │   ├─ system_init.h
│   │   ├─ system_init.cpp
│   │   ├─ types.h
│   │   └─ config.h
│   │
│   ├─ sensors/
│   │   ├─ bme280/
│   │   │   ├─ bme280_sensor.h
│   │   │   └─ bme280_sensor.cpp
│   │   ├─ mpu6050/
│   │   │   ├─ mpu6050_sensor.h
│   │   │   └─ mpu6050_sensor.cpp
│   │   ├─ gy271/
│   │   │   ├─ gy271_sensor.h
│   │   │   └─ gy271_sensor.cpp
│   │   ├─ rtc/
│   │   │   ├─ rtc_module.h
│   │   │   └─ rtc_module.cpp
│   │   └─ battery/
│   │       ├─ battery_monitor.h
│   │       └─ battery_monitor.cpp
│   │
│   ├─ ui/
│   │   ├─ display/
│   │   │   ├─ display.h
│   │   │   └─ display.cpp
│   │   ├─ buttons/
│   │   │   ├─ buttons.h
│   │   │   └─ buttons.cpp
│   │   └─ menus/
│   │       ├─ menu_system.h
│   │       └─ menu_system.cpp
│   │
│   ├─ alerts/
│   │   ├─ buzzer.h
│   │   └─ buzzer.cpp
│   │
│   ├─ navigation/
│   │   ├─ heading.h
│   │   ├─ heading.cpp
│   │   ├─ motion.h
│   │   └─ motion.cpp
│   │
│   └─ utils/
│       ├─ filter.h
│       ├─ filter.cpp
│       ├─ math_utils.h
│       └─ math_utils.cpp
│
├─ docs/
│   ├─ architecture.md
│   ├─ hardware.md
│   ├─ roadmap.md
│   └─ wiring_diagrams/
│
├─ examples/
│   ├─ simple_bme280_test/
│   ├─ simple_mpu6050_test/
│   ├─ simple_gy271_test/
│   └─ display_demo/
│
├─ hardware/
│   ├─ schematics/
│   ├─ board_layout/
│   └─ power/
│
├─ tools/
│   ├─ calibration_scripts/
│   └─ data_export/
│
└─ README.md

```

Each folder encapsulates one logical subsystem of the project.

---

# **1. src/core – System Core**

### **main.ino**

Entry point of the application. Contains `setup()` and `loop()`, orchestrates system behavior by calling high-level update routines.

### **system_init.h / system_init.cpp**

Central initialization module.
Responsible for:

* Starting I²C, serial (optional)
* Initializing all sensors
* Setting up display, buttons, buzzer
* Initializing system state

Also provides periodic update functions used by the main loop.

### **types.h**

Shared data structures used throughout the system (e.g. `EnvData`, `MotionData`, `BatteryStatus`, enums).

### **config.h**

Central configuration file containing pin definitions, I²C addresses, calibration constants and global system settings.

---

# **2. src/sensors – Sensor Drivers**

Each sensor is isolated in its own submodule with a consistent interface:
`begin()`, `update()`, and data retrieval functions.

## **bme280/**

Environmental sensor: temperature, humidity, pressure.

* `bme280_sensor.h/.cpp`

## **mpu6050/**

Gyroscope + accelerometer.

* `mpu6050_sensor.h/.cpp`

## **gy271/**

Magnetometer (QMC5883L/HMC5883L).

* `gy271_sensor.h/.cpp`

## **rtc/**

Real-time clock (DS3231/DS1307).

* `rtc_module.h/.cpp`

## **battery/**

18650 voltage monitoring via analog input.

* `battery_monitor.h/.cpp`

---

# **3. src/ui – Display, Input & Menu System**

## **display/**

Wrapper around the 20×4 HD44780 I²C display.
Provides basic text output and higher-level rendering functions.

## **buttons/**

Handles the four physical buttons:

* Debouncing
* Short/long press detection
* Event queue for the menu system

## **menus/**

UI state machine controlling screen navigation, menu logic and screen rendering.

---

# **4. src/alerts – Acoustic Feedback**

## **buzzer.h / buzzer.cpp**

Controls the piezo buzzer for:

* Button feedback
* Alarm signals
* Time-based patterns (non-blocking)

---

# **5. src/navigation – Heading & Motion Processing**

## **heading.h / heading.cpp**

Calculates course/heading from magnetometer data, including optional tilt compensation and filtering.

## **motion.h / motion.cpp**

Combines accelerometer and gyroscope readings to estimate roll, pitch and yaw using complementary or low-pass filters.

---

# **6. src/utils – Helpers & Algorithms**

## **filter.h / filter.cpp**

Reusable filters, such as:

* Moving average
* Exponential smoothing
* Low-pass filters

## **math_utils.h / math_utils.cpp**

Mathematical helper functions:
angle normalization, linear interpolation, conversions.

---

# **7. docs – Documentation**

* **architecture.md** (this file)
* **hardware.md** – wiring, pinouts, battery circuits
* **roadmap.md** – future features
* **wiring_diagrams/** – images, PDFs, schematics

---

# **8. examples – Minimal Test Sketches**

Small standalone sketches for verifying:

* BME280
* MPU6050
* GY-271
* Display output

---

# **9. hardware – Electronics Resources**

* **schematics/** – KiCad or Fritzing diagrams
* **board_layout/** – PCB layouts
* **power/** – battery handling, regulators, protection

---

# **10. tools – Utilities**

* **calibration_scripts/** – e.g., compass calibration tools
* **data_export/** – scripts for logging/serial data extraction

---

# **Purpose of This Architecture**

* Clean separation of hardware, logic and UI
* Predictable and minimal memory footprint
* Easy addition of new modules (GPS, timers, alarms, communication)
* High readability and long-term maintainability
* Strong focus on safe patterns for embedded development

