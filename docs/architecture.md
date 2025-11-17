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
