/*
Rolle: Allgemeine Filter (wiederverwendbar).

Inhalt:

Klassen/Funktionen für:

gleitender Mittelwert

Exponential Moving Average

ggf. einfachen 1. Ordnung Lowpass
*/

#pragma once

class MovingAverage {
public:
  MovingAverage(size_t size);
  float update(float value);
private:
  // Buffer, Index, Sum etc.
};

