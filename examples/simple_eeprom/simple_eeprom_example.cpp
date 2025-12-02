#include <EEPROM.h>

/*
  Dieses Programm demonstriert die wichtigsten Funktionen der EEPROM Bibliothek.

  Es zeigt:
  1) Wie man die Größe des EEPROM im Arduino ermittelt.
  2) Wie man einzelne Bytes mit read und write liest und schreibt.
  3) Wie man mit update schreibt, ohne unnötige Schreibzugriffe zu erzeugen.
  4) Wie man komplexere Datenstrukturen mit get und put speichert und lädt.
  5) Wie man den gesamten EEPROM in einer Schleife löscht.

  Wichtige Hinweise:
  - Der EEPROM hat nur eine begrenzte Anzahl von Schreibzyklen pro Speicherzelle.
  - Man sollte daher nicht ständig in einer schnellen Schleife schreiben.
  - Dieses Beispiel ist zum Lernen gedacht, nicht für Dauerbetrieb in dieser Form.
*/

// Struktur für ein Beispielobjekt, das im EEPROM abgelegt wird
struct ConfigurationData {
  int versionNumber;
  float calibrationFactor;
  char deviceName[16];  // Fester Zeichenspeicher für den Namen
};

/*
  Hilfsfunktion, um eine Überschrift im seriellen Monitor auszugeben.
  Diese Funktion dient nur der Übersicht, damit die Ausgaben besser lesbar sind.
*/
void printSeparator(const char *title) {
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(title);
  Serial.println(F("========================================"));
}

void setup() {
  Serial.begin(9600);
  // Auf manchen Boards ist eine kurze Wartezeit sinnvoll,
  // damit der serielle Monitor bereit ist.
  while (!Serial) {
    ; // Warten, bis die serielle Verbindung hergestellt ist
  }

  printSeparator("EEPROM Demo startet");

  // 1) Größe des EEPROM ermitteln
  printSeparator("1) Groesse des EEPROM");
  int eepromSize = EEPROM.length();
  Serial.print(F("Die Groesse des EEPROM betraegt: "));
  Serial.print(eepromSize);
  Serial.println(F(" Bytes."));

  // 2) Einfaches Schreiben und Lesen eines Bytes mit write und read
  printSeparator("2) EEPROM.write und EEPROM.read");

  int exampleAddress = 0;       // Adresse 0 im EEPROM
  byte valueToWrite = 123;      // Beispielwert, der im EEPROM gespeichert werden soll

  Serial.print(F("Schreibe den Wert "));
  Serial.print(valueToWrite);
  Serial.print(F(" an Adresse "));
  Serial.println(exampleAddress);

  EEPROM.write(exampleAddress, valueToWrite);

  Serial.println(F("Lese nun den Wert an der gleichen Adresse aus."));
  byte readValue = EEPROM.read(exampleAddress);

  Serial.print(F("Gelesener Wert: "));
  Serial.println(readValue);

  // 3) Schreiben mit EEPROM.update
  printSeparator("3) EEPROM.update");

  /*
    EEPROM.write schreibt immer, auch wenn der neue Wert gleich dem alten ist.
    Jeder Schreibvorgang belastet die Speicherzelle.

    EEPROM.update hingegen schreibt nur dann, wenn sich der Wert wirklich aendert.
    Dadurch werden ueberfluessige Schreibzugriffe vermieden.
  */

  int updateAddress = 1;
  byte firstUpdateValue = 42;
  byte secondUpdateValue = 42;
  byte thirdUpdateValue = 99;

  Serial.print(F("Schreibe mit EEPROM.update den Wert "));
  Serial.print(firstUpdateValue);
  Serial.print(F(" an Adresse "));
  Serial.println(updateAddress);

  EEPROM.update(updateAddress, firstUpdateValue);

  Serial.println(F("Schreibe noch einmal den gleichen Wert mit EEPROM.update."));
  Serial.println(F("In diesem Fall findet intern kein echter Schreibzugriff statt,"));
  Serial.println(F("da der Wert unveraendert ist."));
  EEPROM.update(updateAddress, secondUpdateValue);

  Serial.println(F("Schreibe nun einen anderen Wert mit EEPROM.update, sodass ein Schreibzugriff erfolgt."));
  EEPROM.update(updateAddress, thirdUpdateValue);

  Serial.print(F("Gelesener Wert an der Adresse "));
  Serial.print(updateAddress);
  Serial.print(F(": "));
  Serial.println(EEPROM.read(updateAddress));

  // 4) Komplexere Datenstrukturen mit EEPROM.put und EEPROM.get
  printSeparator("4) EEPROM.put und EEPROM.get mit einer Struktur");

  /*
    EEPROM.put schreibt ein ganzes Objekt (zum Beispiel eine Struktur) in den EEPROM.
    EEPROM.get liest das Objekt wieder heraus.

    Dabei werden die Daten exakt so gespeichert, wie sie im Speicher des Mikrocontrollers liegen.
    Man muss nur darauf achten, dass man beim Lesen den gleichen Datentyp verwendet wie beim Schreiben.
  */

  int structStartAddress = 10; // Adresse, ab der die Struktur gespeichert wird

  ConfigurationData configToSave;
  configToSave.versionNumber    = 1;
  configToSave.calibrationFactor = 1.2345;
  // Der Name muss als C-String mit Abschluss durch ein Nullzeichen gespeichert werden.
  // Die Funktion strncpy sorgt dafuer, dass der Puffer nicht ueberlaeuft.
  strncpy(configToSave.deviceName, "SensorNode01", sizeof(configToSave.deviceName));
  configToSave.deviceName[sizeof(configToSave.deviceName) - 1] = '\0';

  Serial.print(F("Speichere ConfigurationData Struktur ab Adresse "));
  Serial.println(structStartAddress);

  EEPROM.put(structStartAddress, configToSave);

  Serial.println(F("Lese die Struktur wieder aus dem EEPROM und gebe die Werte aus."));

  ConfigurationData configLoaded;
  EEPROM.get(structStartAddress, configLoaded);

  Serial.println(F("Geladene Konfiguration:"));
  Serial.print(F("Versionsnummer: "));
  Serial.println(configLoaded.versionNumber);

  Serial.print(F("Kalibrierungsfaktor: "));
  Serial.println(configLoaded.calibrationFactor, 4);

  Serial.print(F("Geraetename: "));
  Serial.println(configLoaded.deviceName);

  // 5) Gesamten EEPROM loeschen (nur Demonstration)
  printSeparator("5) Gesamten EEPROM loeschen (nur Demonstration)");

  Serial.println(F("Achtung: Dieser Schritt beschreibt jede Speicherzelle."));
  Serial.println(F("Dies reduziert die verbleibende Lebensdauer des EEPROM."));
  Serial.println(F("Dieser Vorgang ist nur zum Lernen gedacht."));
  Serial.println(F("Loesche nun alle Zellen und setze sie auf 0."));

  for (int address = 0; address < eepromSize; address++) {
    EEPROM.update(address, 0); // update statt write, um unnoetige Schreibvorgaenge zu vermeiden
  }

  Serial.println(F("Alle Speicherzellen wurden auf 0 gesetzt."));

  // Kontrolle: Einige Zellen probeweise lesen
  Serial.println(F("Pruefe einige Speicherzellen:"));
  for (int address = 0; address < 8 && address < eepromSize; address++) {
    Serial.print(F("Adresse "));
    Serial.print(address);
    Serial.print(F(": "));
    Serial.println(EEPROM.read(address));
  }

  printSeparator("EEPROM Demo beendet");
}

void loop() {
  // In dieser Demonstration wird im loop nichts mehr ausgefuehrt.
  // Alle wichtigen Aktionen finden im setup statt.
}
