/*
Rolle:
Das neue Power-Modul — liefert alles, was du brauchst:

Accelerometer

Gyroscope

Magnetometer

Orientation: Roll, Pitch, Yaw

ggf. Kompasswinkel

Sensorfusion (Complementary Filter, Madgwick, Mahony …)
*/

#pragma once
#include "types.h"

namespace MPU9250Module {
    bool begin();
    void update();
    IMUData getIMU();
    float getHeadingDeg();   // magnetischer Kurs
}
