#include <SPI.h>
#include <SD.h>

const int SD_CS_PIN = 10;  // CS-Pin vom SD-Modul → an D10 (anpassen, falls anders verdrahtet)

void listDir(const char *dirname, uint8_t levels);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // für Leonardo/Micro, beim UNO ist das egal
  }

  Serial.println(F("\n=== SD-Karten-Demo startet ==="));

  // SD initialisieren
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("Fehler: SD-Karte konnte nicht initialisiert werden."));
    Serial.println(F("→ Prüfe Verdrahtung, CS-Pin, Format (FAT16/FAT32) und Karte."));
    while (true) {
      // Stop – ohne SD macht der Rest keinen Sinn
    }
  }
  Serial.println(F("SD-Karte erfolgreich initialisiert."));

  // Infos: Existiert ein bestimmter Pfad?
  Serial.println(F("\nPrüfe, ob /logs existiert..."));
  if (!SD.exists("/logs")) {
    Serial.println(F("Ordner /logs existiert noch nicht, wird angelegt..."));
    if (SD.mkdir("/logs")) {
      Serial.println(F("Ordner /logs erfolgreich angelegt."));
    } else {
      Serial.println(F("Fehler beim Anlegen von /logs."));
    }
  } else {
    Serial.println(F("Ordner /logs existiert bereits."));
  }

  // 1) Datei neu anlegen & schreiben
  Serial.println(F("\nErstelle und schreibe in /logs/demo.txt ..."));
  File file = SD.open("/logs/demo.txt", FILE_WRITE);
  if (!file) {
    Serial.println(F("Fehler: /logs/demo.txt konnte nicht geöffnet werden (zum Schreiben)."));
  } else {
    file.println(F("Dies ist eine Testdatei."));
    file.println(F("Zeile 2: Hallo von deiner SD-Karte!"));
    file.println(F("Zeile 3: Datum/Zeit koenntest du z.B. mit einem RTC-Modul dazuschreiben."));
    file.close();
    Serial.println(F("Erste Schreiboperation abgeschlossen."));
  }

  // 2) An bestehende Datei anhängen (Append)
  Serial.println(F("\nHänge eine Zeile an /logs/demo.txt an..."));
  file = SD.open("/logs/demo.txt", FILE_WRITE);  // FILE_WRITE hängt standardmäßig ans Ende an
  if (!file) {
    Serial.println(F("Fehler beim Öffnen von /logs/demo.txt für Append."));
  } else {
    file.println(F("Zusatzzeile (Append): Noch ein Eintrag ins Log.");
    file.close();
    Serial.println(F("Append abgeschlossen."));
  }

  // 3) Datei lesen
  Serial.println(F("\nLese Inhalt von /logs/demo.txt:\n--------------------------------"));
  file = SD.open("/logs/demo.txt", FILE_READ);
  if (!file) {
    Serial.println(F("Fehler beim Öffnen von /logs/demo.txt zum Lesen."));
  } else {
    while (file.available()) {
      char c = file.read();
      Serial.write(c);
    }
    file.close();
    Serial.println(F("\n--------------------------------"));
    Serial.println(F("Datei wurde vollständig gelesen."));
  }

  // 4) Verzeichnisinhalt ausgeben (rekursiv)
  Serial.println(F("\nInhalt der SD-Karte (Root-Verzeichnis):"));
  listDir("/", 2);  // 2 = wie tief rekursiv in Unterordnern gesucht wird

  // 5) Existenz einer Datei prüfen
  Serial.println(F("\nPrüfe Existenz von /logs/demo.txt ..."));
  if (SD.exists("/logs/demo.txt")) {
    Serial.println(F("→ /logs/demo.txt existiert."));
  } else {
    Serial.println(F("→ /logs/demo.txt existiert NICHT (??)."));
  }

  // 6) Datei löschen
  Serial.println(F("\nLösche /logs/demo.txt ..."));
  if (SD.remove("/logs/demo.txt")) {
    Serial.println(F("→ /logs/demo.txt wurde gelöscht."));
  } else {
    Serial.println(F("→ Fehler beim Löschen von /logs/demo.txt."));
  }

  // 7) Ordner löschen (nur wenn leer!)
  Serial.println(F("\nVersuche, Ordner /logs zu löschen (muss leer sein)..."));
  if (SD.rmdir("/logs")) {
    Serial.println(F("→ /logs wurde gelöscht."));
  } else {
    Serial.println(F("→ /logs konnte nicht gelöscht werden (vielleicht nicht leer?)."));
  }

  Serial.println(F("\nFinaler Inhalt der SD-Karte:"));
  listDir("/", 2);

  Serial.println(F("\n=== SD-Demo fertig. Im loop() passiert nichts mehr. ==="));
}

void loop() {
  // Hier könntest du später z.B. Logfunktionen etc. einbauen.
}

/**
 * Rekursive Verzeichnisauflistung
 * dirname: Startpfad, z.B. "/" oder "/logs"
 * levels: Wie tief rekursiv in Unterordner gegangen wird
 */
void listDir(const char *dirname, uint8_t levels) {
  Serial.print(F("Listing directory: "));
  Serial.println(dirname);

  File root = SD.open(dirname);
  if (!root) {
    Serial.println(F("→ Fehler: Verzeichnis konnte nicht geöffnet werden."));
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(F("→ Fehler: Angegebener Pfad ist kein Verzeichnis."));
    root.close();
    return;
  }

  File entry = root.openNextFile();
  while (entry) {
    Serial.print("  ");
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      if (levels > 0) {
        // Rekursiv in Unterordnern weitermachen
        char path[64];
        snprintf(path, sizeof(path), "%s/%s", dirname, entry.name());
        listDir(path, levels - 1);
      }
    } else {
      // Normale Datei: Name und Größe anzeigen
      Serial.print("\t\t");
      Serial.print(entry.size());
      Serial.println(" bytes");
    }
    entry.close();
    entry = root.openNextFile();
  }
  root.close();
}
