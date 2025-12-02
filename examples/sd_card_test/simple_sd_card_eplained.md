Hier ist dein Inhalt sauber und klar als **Markdown-Datei formatiert** – perfekt für ein `.md`-File, GitHub, Obsidian oder Dokumentation deines Projekts.

---

# SD-Karten-Bibliothek für Arduino

**Kurzanleitung & Demo-Funktionen**

## Empfehlung der Bibliothek

Für das AZDelivery-SD-Modul eignet sich die Standard-Bibliothek hervorragend:

* **Empfohlen:** `#include <SD.h>`
* **Erforderlich:** `#include <SPI.h>`
* **Alternative:** `SdFat` (schneller, komplexer)

Die Standard-`SD`-Library ist gut dokumentiert und bietet alle Funktionen für:

* Logfiles
* Konfigurationsdaten
* Datei- und Ordnerverwaltung
* einfache, robuste Speicherung auf MicroSD

---

## Übersicht der unterstützten Funktionen

Der Beispielcode demonstriert:

* **Initialisierung**
* **Verzeichnis auflisten** (inkl. rekursiv)
* **Ordner anlegen / löschen** (`mkdir`, `rmdir`)
* **Dateien lesen und schreiben**
* **Append**
* **Existenzprüfung** (`exists`)
* **Dateien löschen** (`remove`)
* **Directory-Walking** mit `openNextFile()`

Diese Toolbox deckt praktisch alles ab, was ein kleines eingebettetes Dateisystem können muss.

---

## Verdrahtung des AZDelivery-SD-Moduls

*(für Arduino UNO / Nano / Mega)*

| SD-Modul | Arduino            |
| -------- | ------------------ |
| **VCC**  | 5V                 |
| **GND**  | GND                |
| **MISO** | Pin 12             |
| **MOSI** | Pin 11             |
| **SCK**  | Pin 13             |
| **CS**   | Pin 10 (empfohlen) |

**Mega-Hinweis:**
MISO/MOSI/SCK liegen dort auf Pins 50/51/52 – die SD-Library berücksichtigt das automatisch.

---

## Funktionsumfang der SD-Library (Kurzüberblick)

### Dateioperationen

* `SD.open(path, FILE_WRITE)`
* `SD.open(path, FILE_READ)`
* `File.print()`, `println()`
* `File.read()`, `available()`, `close()`

### Verzeichnisoperationen

* `SD.exists(path)`
* `SD.mkdir(path)`
* `SD.rmdir(path)` *(nur leer löschbar)*
* `File.isDirectory()`
* `File.openNextFile()` *(Directory-Auflistung)*

### Dateimanagement

* Logs schreiben
* Konfigurationswerte speichern
* Ordnerstruktur verwalten
* Dateien anhängen und erweitern

---

## Beispielanwendungen

Mit dieser Bibliothek kannst du:

* **Messwerte** regelmäßig auf SD loggen
* **Konfigurationsdateien** speichern und auslesen
* **CSV-Dateien** schreiben
* **Ordner für einzelne Tage** erstellen (`/2025-12-05/`)
* **Logger-Systeme** mit automatischer Dateirotation umsetzen

---

Wenn du willst, kann ich das komplette **Demo-Sketch** ebenfalls als `.md`, `.ino` oder sogar als Tutorial mit Bildern formatieren.
