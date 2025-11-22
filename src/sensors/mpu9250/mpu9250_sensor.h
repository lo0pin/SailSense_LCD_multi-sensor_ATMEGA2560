/*
Rolle:
Das neue Power-Modul — liefert alles, was du brauchst:

Accelerometer

Gyroscope

Magnetometer

Orientation: Roll, Pitch, Yaw

ggf. Kompasswinkel

Sensorfusion (Complementary Filter, Madgwick, Mahony …)


Intern:

Du nutzt eine Library wie SparkFunMPU9250-DMP oder bolderflight MPU9250.

Der Code ist viel cleaner als getrennte Sensoren.
*/

#pragma once
#include "types.h"

namespace MPU9250Module {
    bool begin();
    void update();
    IMUData getIMU();
    float getHeadingDeg();   // magnetischer Kurs
}
