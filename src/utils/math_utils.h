/*
Rolle: Mathematische Hilfsfunktionen.

Inhalt:

Winkel-Normalisierung (wrapAngle360, wrapAngle180)

Umrechnungen (z. B. mbar → hPa, Grad ↔ rad)

Interpolation, Mapping, evtl. einfache Statistik
*/

#pragma once

namespace MathUtils {
  float wrapAngle360(float deg);
  float wrapAngle180(float deg);
  float lerp(float a, float b, float t);
}

