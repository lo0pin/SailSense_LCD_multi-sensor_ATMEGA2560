/*
Rolle: Echtzeituhr.

Inhalt:

Wrapper um RTC-Library (DS3231/DS1307)

getTime(), setTime(), evtl. getUnixTime()

Abstraktion, damit der Rest des Codes keine RTC-Library direkt kennt
*/

struct DateTimeSimple {
  uint16_t year;
  uint8_t  month, day, hour, minute, second;
};

namespace RTCModule {
  bool begin();
  DateTimeSimple now();
}


