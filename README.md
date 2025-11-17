# **SailSense – Modular Sensor Gadget for Sailors (Arduino Mega)**

*An experimental journey toward a compact, extensible, power-efficient sailing companion.*

## **Overview**

**SailSense** is a modular Arduino-based project designed to explore how far clean structure, low resource usage, and thoughtful abstraction can carry an embedded system.
The project begins as a collection of environmental and motion sensors on an **Arduino Mega**, and moves—step by elegant step—towards a fully featured sailing gadget.

This repository documents the architecture, experiments, concepts, and code as the system grows.
Think of it as both a technical study and a personal logbook.

## **Current Hardware Targets**

The project is built around an **Arduino Mega** and integrates:

* **BME280** – temperature, humidity, and pressure
* **RTC module (DS3231/DS1307)** – real-time clock
* **MPU6050** – gyroscope & accelerometer for roll, pitch & yaw
* **GY-271 (QMC5883L / HMC5883L)** – 3-axis magnetometer (heading)
* **20×4 I²C Display (HD44780 + PCF8574AT backpack)** – text output
* **4 push buttons** – user input
* **Piezo buzzer** – feedback & alarms
* **18650 Li-Ion battery** (planned) – mobile power supply with analog voltage monitoring

Future extensions include:

* **GPS module** – positioning, speed over ground, course over ground
* **Communication interfaces** – possibly CAN, I²C expansion, or UART links to companion microcontrollers.

## **Planned Features**

### **Navigation & Motion**

* Boat heading (magnetometer)
* Heel & pitch angles (IMU)
* Course stability indicators
* Motion-based alarms (e.g., anchor drift warning, strong gust roll detection)

### **Weather & Environment**

* Pressure trend for weather forecasting
* Air temperature & humidity
* Configurable thresholds and alerts

### **Utility Tools**

* Stopwatch
* Countdown timer
* Alarm clock
* Sailing-specific timers (start-sequence timing, e.g. 5-4-1-go)

### **Power & Mobile Operation**

* 18650 battery monitoring via analog input
* Power-friendly software design
* Runtime optimization and sleep modes (planned)

### **Architecture Goals**

This project is also a **software engineering experiment**:

* Highly modular structure
* Minimal memory footprint
* Clean abstractions for sensors & subsystems
* Easy extensibility for new hardware or features
* Robustness and maintainability

Ultimately, SailSense aims to become a compact, intuitive, low-power companion for skippers and hobbyists alike.

## **Project Structure (to be filled as code evolves)**

```
/src
  /sensors
  /display
  /input
  /core
/docs
/examples
```

As modules grow, they’ll be documented with diagrams, architecture notes, and benchmarks.

## **Why Arduino Mega?**

* Plenty of I/O pins for modular expansion
* Generous Flash & RAM for experimenting with abstraction layers
* Stable platform with rich documentation
* Ideal for comparing "clean code vs. resource budget" approaches

The Mega is the project’s development base; later iterations may port to more compact or more efficient microcontrollers.

## **Roadmap**

* [ ] Define sensor abstraction layer
* [ ] Basic reading of all sensors
* [ ] Display UI framework
* [ ] Button input handler
* [ ] Buzzer feedback module
* [ ] Battery voltage measurement
* [ ] First navigation readings (heading, heel angle)
* [ ] Pressure trend estimation
* [ ] Modular menu system
* [ ] Stopwatch / Timer / Alarm
* [ ] GPS integration
* [ ] Power optimization
* [ ] Communication interface experiments

## **Vision**

A rugged little companion for sailors:
simple, trustworthy, and extensible—
like a pocket sextant for the digital age.
Not just a device, but a study of clarity, efficiency, and thoughtful engineering.

