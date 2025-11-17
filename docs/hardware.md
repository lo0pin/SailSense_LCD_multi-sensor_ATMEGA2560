# **SailSense – Hardware Overview**

This document provides an overview of the hardware components used in the SailSense project.
Each module is selected for reliability, low power consumption and suitability for use on sailing vessels or outdoor environments.
All components connect to an Arduino Mega, forming a modular and extensible sensor platform.

---

# **1. Main Controller**

## **Arduino Mega 2560**

Chosen for:

* Large number of GPIO pins
* Multiple hardware serial interfaces
* 256 KB Flash / 8 KB SRAM (suitable for clean modular code)
* Stable I²C and SPI support
* Excellent library ecosystem

The Mega serves as the development platform; future iterations may migrate to a smaller or more power-efficient MCU.

---

# **2. Sensors**

## **BME280 – Environmental Sensor**

**Functions:**

* Temperature
* Humidity
* Atmospheric pressure (used for weather trend estimation)

**Interface:** I²C
**Typical Address:** >>`0x76`<< or `0x77`

**Notes:**

* Best mounted away from heat sources.
* Optional passive shielding for sunlight exposure.

---

## **MPU6050 – IMU (Gyroscope + Accelerometer)**

**Functions:**

* Roll, pitch, yaw estimation
* Motion detection
* Tilt compensation for compass data

**Interface:** I²C
**Address:** >>`0x67`<< (standard)

**Notes:**

* Needs calibration at startup for best accuracy.
* Sensitive to vibration → damping recommended on moving boats.

---

## **GY-271 (QMC5883L/HMC5883L) – Magnetometer**

**Functions:**

* Heading (compass direction)
* Magnetic field strength

**Interface:** I²C
**Addresses:** ???

* **QMC5883L:** typically `0x0D`
* **HMC5883L:** typically `0x1E`

**Notes:**

* Keep away from ferromagnetic objects and wires carrying high current.
* Requires calibration (soft-iron and hard-iron correction).
* Final heading calculation done in software (`navigation/heading`).

---

## **DS3231 – Real-Time Clock (RTC)**

**Functions:**

* Accurate timekeeping (±2 ppm)
* Temperature-compensated crystal
* Battery-backed operation
* Timestamp for logs, alarms, stopwatches, timers

**Interface:** I²C
**Address:** >>`0x57`<<

**Notes:**

* Far more precise and stable than DS1307.
* Can run for years on a CR2032 backup cell.
* Useful for sleep scheduling in later power-optimization phases.

---

## **Battery Voltage Monitoring**

**Component:**

* Arduino analog input → voltage divider → 18650 lithium cell

**Notes:**

* Must use a proper resistor divider (e.g. 100k / 47k).
* Reference voltage must be known (5V USB or 3.3V internal reference).
* Software converts ADC readings to real voltage.

---

# **3. User Interface Components**

## **20×4 I²C Display (HD44780 + PCF8574AT)**

**Functions:**

* Main text output for sensor data, menus, timers, and alarms

**Interface:** I²C
**Address:**

* Typically `0x27` or `0x3F` for PCF8574AT

**Notes:**

* Backlight can be PWM-controlled for power saving.
* Clear and robust, ideal for outdoor readability.

---

## **Buttons (4× Momentary Push Buttons)**

**Functions:**

* Menu navigation
* Confirm / select
* Timer/stopwatch control
* Alarm management

**Connection:**

* Digital pins with pull-up configuration (`INPUT_PULLUP`)
* Active low

**Notes:**

* Debouncing handled in software.
* Long-press and short-press events supported.

---

## **Piezo Buzzer**

**Functions:**

* Button feedback
* Alarm tones
* Custom sound patterns (non-blocking via `millis()`)

**Connection:**

* PWM pin recommended for tone control

---

# **4. Power System**

## **18650 Li-Ion Battery (planned)**

**Functions:**

* Portable power supply for standalone operation

**Requirements:**

* Proper protection circuitry (BMS)
* Charger module (e.g. TP4056 or similar)
* Optional buck/boost converter depending on target voltage

**Notes:**

* Battery level displayed via ADC measurement
* Essential for long-term autonomy

---

# **5. Wiring Overview**

A complete pinout and wiring diagram is maintained in:

```
docs/wiring_diagrams/
```

This includes:

* I²C bus topology
* Display and sensor addresses
* Button wiring
* Buzzer pin
* Battery measurement resistor divider
* Power flow (USB → 5V → sensors → battery monitoring)

---

# **6. Expansion Possibilities**

Future hardware extensions include:

* **GPS module** (NEO-6M or UBlox series)
* **External microcontrollers** (via UART/I²C)
* **Wind sensor / anemometer**
* **CAN, RS485, or NMEA-0183 interfaces**
* **Waterproof enclosure and marine connectors**

---

# **7. Design Goals**

* Reliable sensor readings even under motion and vibration
* Low power consumption
* Modular wiring for replacement/expansion
* Marine-friendly layout (noise minimization, EMC awareness)
* Consistent I²C-based design for future extensibility
