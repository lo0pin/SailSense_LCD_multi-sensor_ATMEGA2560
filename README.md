# **SailSense – Modular Sailing Companion (Arduino Mega)**

*A structured exploration of embedded design, sensor fusion, and thoughtful engineering.*

## **Overview**

**SailSense** is an experimental, modular Arduino project designed to become a compact sailing companion device.
Its heart is an **Arduino Mega**, chosen for its generous I/O, stable ecosystem, and room for architectural experiments.

The project grows organically: from a few sensors to an extensible, power-aware tool for navigation, weather awareness, and onboard utilities.
This repository documents that journey—architecture, concepts, and code evolving together.

---

## **Current Hardware Targets**

SailSense currently uses the following components:

### **Core System**

* **Arduino Mega 2560**

### **Sensors**

* **BME280**
  Temperature, humidity, and pressure (environmental conditions)
* **DS3231 Real-Time Clock**
  Accurate timekeeping for logs and timers
* **MPU9250 (IMU)**
  9-axis sensor:
  *3-axis gyro + 3-axis accelerometer + 3-axis magnetometer*
  → Provides roll, pitch, yaw, and heading in a single device

### **User Interface**

* **0.96" OLED Display (SSD1306, 128×64, I²C)**
  Graphical UI: compass rose, icons, trend lines, menus
* **Four push buttons**
  Navigation, confirmation, settings
* **Piezo buzzer**
  Alerts and interaction feedback

### **Power**

* **18650 Li-Ion battery (planned)**
  With analog voltage monitoring and low-power modes

---

## **Planned Extensions**

* **GPS module (NEO-6M)** – SOG, COG, UTC sync
* **CAN, UART, or I²C expansion** – potential networking or modular sensor hubs
* **Data logging** – EEPROM or external flash/SD

---

## **Features (Current & Planned)**

### **Navigation & Motion**

* Real-time heading using IMU magnetometer
* Heel and pitch angle visualization
* Smooth filtered angle outputs
* Motion-based alarms (gust, rolling, anchor drift)

### **Environment & Weather**

* Temperature / humidity / pressure readings
* Pressure trend estimation for short-term forecasting
* Customizable thresholds and warnings

### **Utility Tools**

* Stopwatch
* Countdown timer
* Alarm clock
* Sailing race start timer (5–4–1–GO)

### **Power & Efficiency**

* Battery voltage measurement
* OLED dimming
* Low-power operating modes (planned)

---

## **Architecture Goals**

SailSense is as much a **software architecture experiment** as it is a gadget:

* Clean, layered structure
* Hardware abstraction modules
* Minimal dependencies and resource footprint
* Easy feature expansion
* Separation of UI, sensors, and core logic
* Maintainability first

Ultimately, the device should feel like a **digital pocket sextant**—simple, compact, dependable.

---

## **Repository Structure**

```
/src
  /core            ← system init, main loop, global control
  /sensors         ← BME280, MPU9250, DS3231 wrappers
  /ui              ← OLED renderer, buttons, menus
  /navigation      ← heading, motion, sensor fusion
  /alerts          ← buzzer patterns
  /utils           ← math, filters, helpers
/docs
/examples
/hardware
```

Each module aims to be self-contained, readable, and easy to replace.

---

## **Why Arduino Mega?**

* Large Flash & RAM → freedom to design clean abstractions
* Plenty of hardware interrupts and pins
* Stable and forgiving for architectural exploration
* Perfect for testing modular design without needing to optimize too early

Future versions may migrate to more compact or more efficient microcontrollers once the architecture stabilizes.

---

## **Roadmap**

* [ ] Sensor abstraction layer (MPU9250, BME280, RTC)
* [ ] Unified IMU fusion → Roll, pitch, yaw, heading
* [ ] OLED UI framework (icons, screens, menus)
* [ ] Button event system (short/long press)
* [ ] Buzzer feedback patterns
* [ ] Battery measurement module
* [ ] Environmental & motion dashboard
* [ ] Compass screen with tilt compensation
* [ ] Stopwatch / timer / alarm suite
* [ ] GPS integration
* [ ] Data logging system
* [ ] Power saving modes
* [ ] External communication tests (CAN/UART/I²C)

---

## **Vision**

A rugged little sailing companion:
responsive, quiet, and clear—
a tool that speaks the language of wind and water.

Not just hardware and code,
but a study of **clarity, elegance, and engineering discipline**
on a small embedded canvas.



Just say the word.
