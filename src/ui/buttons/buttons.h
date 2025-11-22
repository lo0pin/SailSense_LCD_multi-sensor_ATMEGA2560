/*
Rolle: Verwaltung der 4 Taster.

Inhalt:

Pin-Definitionen (oder inkl. config.h)

Entprellung

Erkennung von Klick, Long-Press, Repeat

Event-Interface für Menüsystem
*/

enum class ButtonEventType { None, ShortPress, LongPress };

struct ButtonEvent {
  uint8_t buttonId;
  ButtonEventType type;
};

namespace Buttons {
  void begin();
  void update();
  bool getNextEvent(ButtonEvent& ev);
}

