Hier ist ein schön strukturiertes, professionelles und gut lesbares **README.md** für dein EEPROM-Demo-Projekt.
Perfekt für GitHub oder jedes andere Dokumentationssystem.

---

# EEPROM-Demo für Arduino

**Demonstration der wichtigsten Funktionen der Arduino-EEPROM-Bibliothek**

Dieses Projekt zeigt übersichtlich und leicht verständlich, wie man den EEPROM eines Arduino-Boards verwendet.
Es richtet sich an Einsteiger sowie an Fortgeschrittene, die den Umgang mit persistentem Speicher wiederholen möchten.

Der gesamte Beispielcode befindet sich in der Datei `eeprom_demo.ino`.

---

## 🔧 Voraussetzungen

* Arduino UNO, Nano, Mega oder kompatibles Board
* Installierte Arduino-IDE
* EEPROM vorhanden (bei den meisten AVR-Boards integriert)

Bibliothek wird automatisch mit der Arduino-IDE installiert:

```cpp
#include <EEPROM.h>
```

---

## 🎯 Ziele des Beispielcodes

Der Sketch demonstriert folgende Funktionen der `EEPROM.h` Bibliothek:

1. **Ermitteln der EEPROM-Größe**
2. **Lesen und Schreiben einzelner Bytes** mit

   * `EEPROM.write()`
   * `EEPROM.read()`
3. **Schonendes Schreiben mit `EEPROM.update()`**, das unnötige Schreibvorgänge vermeidet
4. **Speichern komplexerer Datenstrukturen** (z. B. Structs) mit

   * `EEPROM.put()`
   * `EEPROM.get()`
5. **Komplettes Löschen des EEPROM** über eine Schleife
6. **Ausgabe aller Schritte im seriellen Monitor**

---

## ⚠️ Wichtige Hinweise zur Lebensdauer des EEPROM

EEPROM-Speicher hat eine begrenzte Anzahl an Schreibzyklen pro Zelle (meist ca. 100.000).
Daher:

* Nicht in schnellen Loops ständig schreiben
* `EEPROM.update()` bevorzugen, da es nur schreibt, wenn sich der Wert ändert
* Strukturen nur "bei Bedarf" speichern
* Löschen des gesamten EEPROM nur zu Testzwecken durchführen

---

## 📦 Datenstruktur-Beispiel

Der Sketch speichert eine Konfigurationsstruktur mit mehreren Datentypen:

```cpp
struct ConfigurationData {
  int versionNumber;
  float calibrationFactor;
  char deviceName[16];
};
```

Diese Struktur wird im EEPROM abgelegt und später wieder geladen.

Verwendete Funktionen:

* `EEPROM.put(address, structObj);`
* `EEPROM.get(address, structObj);`

Damit lassen sich komplette Konfigurationsblöcke sicher und leicht speichern.

---

## 🧪 Funktionsübersicht des Sketches

### 1. Größe des EEPROM ermitteln

```cpp
EEPROM.length();
```

Gibt die Gesamtzahl der verfügbaren Bytes aus.

---

### 2. Einzelne Bytes lesen und schreiben

```cpp
EEPROM.write(address, value);  // schreibt immer
EEPROM.read(address);          // liest ein Byte
```

---

### 3. Schonendes Schreiben mit `update()`

```cpp
EEPROM.update(address, value);  // schreibt nur, wenn der Wert sich verändert hat
```

Schont die Speicherzellen erheblich.

---

### 4. Strukturen speichern und laden

```cpp
EEPROM.put(startAddress, configObject);
EEPROM.get(startAddress, loadedObject);
```

Sehr nützlich für:

* Sensorkonfigurationen
* Kalibrierwerte
* Geräteeinstellungen
* Namen, IDs, Flags

---

### 5. Gesamten EEPROM löschen

Nicht für Dauerbetrieb gedacht – dient nur der Demonstration:

```cpp
for (int address = 0; address < EEPROM.length(); address++) {
  EEPROM.update(address, 0);
}
```

---

## 🖥️ Serielle Ausgabe

Der Sketch gibt alle Schritte klar strukturiert auf dem seriellen Monitor aus.
Eine kleine Hilfsfunktion formatiert Abschnitte wie Überschriften:

```cpp
void printSeparator(const char *title);
```

Damit bleibt die Ausgabe übersichtlich und leicht nachvollziehbar.

---

## 📁 Projektstruktur

```
/EEPROM-Demo
│
├── eeprom_demo.ino   → vollständiger Beispielcode
└── README.md         → Dokumentation (dieses Dokument)
```

---

## 🚀 Erweiterungsmöglichkeiten

Hier ein paar Ideen, die du leicht auf Basis des Beispielsketches umsetzen kannst:

* Automatische Versionierung der gespeicherten Konfiguration
* Prüfsummen (CRC) für sichere EEPROM-Daten
* Failsafe-Konfigurationssystem (Default-Werte bei Fehler)
* Speicherung von Sensordaten oder Nutzungszählern
* Backup/Restore von Einstellungen

Wenn du möchtest, erstelle ich dir auch gerne eine **saubere EEPROM-Konfigurationsklasse**, z. B.:

```cpp
class DeviceConfig {
  bool load();
  void save();
  void resetToFactory();
};
```

---

Wenn du willst, formatiere ich dir auch ein professionelles **GitHub-Repository-Gerüst**, inkl.

* `LICENSE`
* `example/` Ordner
* `src/` Struktur
* Badge-Vorschlägen (Arduino, PlatformIO usw.).
