/*
Rolle: Kapselt alles rund um den BME280.

Inhalt:

Klasse oder Namespace wie BME280Sensor

begin(), update(), getData()

Interner Umgang mit der verwendeten BME-Library (Adafruit o. ä.)
*/

#pragma once
#include "types.h"

namespace BME280Sensor {
  bool begin();
  void update();
  EnvData getEnvData();
}

