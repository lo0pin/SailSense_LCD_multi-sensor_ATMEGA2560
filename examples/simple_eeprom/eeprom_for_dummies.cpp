/*
   Beispiel: EEPROM auf Arduino Mega

   - Am Anfang des EEPROM wird ein Block fuer Konfiguration/Settings reserviert.
   - Dahinter wird der restliche EEPROM als Ringpuffer fuer Messdaten genutzt.
   - Es gibt 3 Arrays mit je 24 Werten:
        1) Temperaturen      -> int16_t (Skalierung: 0,1 Grad Celsius)
        2) Luftdruecke       -> uint16_t (Skalierung: 0,1 hPa)
        3) Luftfeuchtigkeiten-> uint16_t (Skalierung: 0,1 %)

   Ziel:
   - Die Konfiguration liegt an einer festen Position.
   - Die Messwerte werden immer an wechselnden Stellen im EEPROM geschrieben
     (Ringpuffer), um die Lebensdauer des EEPROM zu erhoehen (Wear-Leveling).

   Der Code ist maximal ausfuehrlich kommentiert, damit auch jemand ohne
   EEPROM-Vorkenntnisse verstehen kann, was passiert.
*/

#include <Arduino.h>
#include <EEPROM.h>      // Bibliothek fuer Zugriff auf den internen EEPROM
#include <stdint.h>      // fuer int16_t, uint16_t, uint32_t usw.


// ---------------------------------------------------------------------------
// 1) Konfiguration: fester Block am Anfang des EEPROM
// ---------------------------------------------------------------------------

/*
   Wir definieren einen "Magic"-Wert fuer die Konfiguration.
   Damit koennen wir spaeter pruefen, ob in diesem Bereich des EEPROM
   ueberhaupt bereits eine gueltige Konfiguration steht.

   Falls Magic != CONFIG_MAGIC, nehmen wir an: "Konfiguration existiert noch nicht"
   und setzen Default-Werte.
*/
const uint16_t CONFIG_MAGIC = 0x42A7;  // beliebiger, konstanter Erkennungswert

/*
   Konfigurationsstruktur.
   Hier kannst du alle Einstellungen eintragen, die du dauerhaft speichern willst.
   Zum Beispiel:
   - Version der Konfiguration
   - Kalibrierfaktoren
   - Flags fuer Anzeigeeinstellungen
   usw.
*/
struct ConfigBlock {
  uint16_t magic;          // muss CONFIG_MAGIC entsprechen, sonst Defaults laden
  uint8_t  version;        // Versionsnummer (z.B. 1, spaeter 2, 3, ...)
  uint8_t  reserved;       // auffuellen auf 4 Byte (optional, fuer Alignment)

  // Beispielhafte Konfigurationswerte:
  float    tempOffset;     // Temperatur-Offset fuer Kalibrierung
  float    pressureOffset; // Druck-Offset fuer Kalibrierung
  float    humidityOffset; // Feuchte-Offset fuer Kalibrierung
};

// Globale Variable im RAM, die die aktuelle Konfiguration haelt.
ConfigBlock g_config;


// ---------------------------------------------------------------------------
// 2) Messdaten-Snapshots (Ringpuffer) im EEPROM
// ---------------------------------------------------------------------------

/*
   Magic fuer jeden Snapshot-Eintrag im Ringpuffer.
   Damit erkennen wir, ob an einer bestimmten Stelle im EEPROM bereits
   ein gueltiger Eintrag liegt.
*/
const uint32_t SNAPSHOT_MAGIC = 0x1234ABCDUL;

/*
   Struktur fuer einen "Snapshot" aller Messarrays.
   Jeder Eintrag enthaelt:
   - einen Magic-Wert (zur Gueltigkeitspruefung)
   - eine fortlaufende Sequence-Nummer (damit wir den neuesten Eintrag finden)
   - 3 Arrays mit jeweils 24 Werten
*/
struct DataSnapshot {
  uint32_t magic;                      // muss SNAPSHOT_MAGIC entsprechen
  uint32_t sequence;                   // steigt bei jedem neuen Eintrag

  int16_t   temperatures[24];          // gespeicherte Temperaturen (0,1 °C)
  uint16_t  pressures[24];             // gespeicherte Druecke (0,1 hPa)
  uint16_t  humidities[24];           // gespeicherte Feuchten (0,1 %)
};

/*
   Wichtig:
   Wir speichern keine floats direkt, sondern skalierte Ganzzahlen.

   Beispiel:

   Temperatur:
   - scala_temp = 10.0
   - gespeicherter Wert = round(temp_in_grad * 10)
   - 23,4 °C -> 234
   - Beim Auslesen: temp_in_grad = gespeicherter_wert / 10.0
*/
const float TEMP_SCALE      = 10.0f;    // 0,1 °C
const float PRESSURE_SCALE  = 10.0f;    // 0,1 hPa
const float HUMIDITY_SCALE  = 10.0f;    // 0,1 %


// ---------------------------------------------------------------------------
// 3) EEPROM-Speicherlayout
// ---------------------------------------------------------------------------

/*
   Wir moechten:

   - Zuerst den Konfigurationsblock speichern: ab Adresse 0
   - Danach den Ringpuffer fuer die DataSnapshots

   Der Arduino Mega hat 4096 Bytes EEPROM.
   EEPROM.length() gibt uns die Groesse zur Laufzeit.
*/
int EEPROM_SIZE = 0;  // wird in setup() gesetzt

// Startadresse des Konfigurationsblocks im EEPROM
const int CONFIG_START_ADDR = 0;

// Groesse des Konfigurationsblocks (in Bytes)
const int CONFIG_SIZE = sizeof(ConfigBlock);

// Startadresse des Ringpuffers fuer DataSnapshots
const int SNAPSHOT_AREA_START_ADDR = CONFIG_START_ADDR + CONFIG_SIZE;

// Groesse eines Snapshots
const int SNAPSHOT_SIZE = sizeof(DataSnapshot);

// Anzahl der Snapshots, die in den restlichen EEPROM passen
int SNAPSHOT_CAPACITY = 0;  // wird in setup() berechnet


// ---------------------------------------------------------------------------
// 4) Verwaltungsvariablen fuer den Ringpuffer
// ---------------------------------------------------------------------------

/*
   Wir merken uns:
   - currentSnapshotIndex: Index des zuletzt geschriebenen Snapshots
   - currentSequence:      hoechste bisher verwendete Sequence-Nummer

   Index-Bereich: 0 .. SNAPSHOT_CAPACITY-1
*/
int      currentSnapshotIndex = -1;   // -1 bedeutet: noch keine gueltigen Eintraege
uint32_t currentSequence      = 0;    // Sequence-Zaehler fuer neue Eintraege


// ---------------------------------------------------------------------------
// 5) Hilfsfunktionen: Skalierung float <-> int16_t / uint16_t
// ---------------------------------------------------------------------------

/*
   Temperatur als float (z.B. 23.4 °C) -> int16_t mit 0,1 °C Aufloesung.
   Wir runden und begrenzen auf den moeglichen Wertebereich von int16_t.
*/
int16_t encodeTemperature(float tempC) {
  float scaled = round(tempC * TEMP_SCALE);

  // Begrenzung (Clamping) auf int16_t-Bereich:
  if (scaled < -32768.0f) scaled = -32768.0f;
  if (scaled >  32767.0f) scaled =  32767.0f;

  return (int16_t)scaled;
}

/*
   Rueckumwandlung: int16_t -> float (Grad Celsius).
*/
float decodeTemperature(int16_t raw) {
  return ((float)raw) / TEMP_SCALE;
}

/*
   Druck als float (z.B. 1013.2 hPa) -> uint16_t mit 0,1 hPa Aufloesung.
   Hier nehmen wir an:
   - Werte sind sinnvoll (z.B. 300..1200 hPa)
   Wir begrenzen auf den Bereich 0..65535.
*/
uint16_t encodePressure(float pressureHpa) {
  float scaled = round(pressureHpa * PRESSURE_SCALE);

  if (scaled < 0.0f)      scaled = 0.0f;
  if (scaled > 65535.0f)  scaled = 65535.0f;

  return (uint16_t)scaled;
}

float decodePressure(uint16_t raw) {
  return ((float)raw) / PRESSURE_SCALE;
}

/*
   Feuchte als float (z.B. 56.7 %) -> uint16_t mit 0,1 % Aufloesung.
   Sinnvoller Bereich: 0..100 %
*/
uint16_t encodeHumidity(float humidityPercent) {
  float scaled = round(humidityPercent * HUMIDITY_SCALE);

  if (scaled < 0.0f)      scaled = 0.0f;
  if (scaled > 65535.0f)  scaled = 65535.0f;

  return (uint16_t)scaled;
}

float decodeHumidity(uint16_t raw) {
  return ((float)raw) / HUMIDITY_SCALE;
}


// ---------------------------------------------------------------------------
// 6) Konfiguration: Default-Werte, Laden, Speichern
// ---------------------------------------------------------------------------

/*
   Default-Konfiguration setzen.
   Diese Funktion fuellt die globale Variable g_config mit sinnvollen
   Standardwerten, falls noch nichts im EEPROM steht oder die Daten ungueltig sind.
*/
void loadDefaultConfig() {
  g_config.magic          = CONFIG_MAGIC;
  g_config.version        = 1;
  g_config.reserved       = 0;

  g_config.tempOffset     = 0.0f;
  g_config.pressureOffset = 0.0f;
  g_config.humidityOffset = 0.0f;
}

/*
   Konfiguration aus dem EEPROM laden.
   - Wir lesen den Block ab CONFIG_START_ADDR.
   - Wenn magic != CONFIG_MAGIC, laden wir Default-Werte.
*/
void loadConfigFromEEPROM() {
  EEPROM.get(CONFIG_START_ADDR, g_config);

  if (g_config.magic != CONFIG_MAGIC) {
    // Es liegt offenbar keine gueltige Konfiguration im EEPROM.
    // -> Default setzen und in EEPROM schreiben.
    Serial.println(F("Keine gueltige Konfiguration gefunden. Lade Defaults."));
    loadDefaultConfig();
    EEPROM.put(CONFIG_START_ADDR, g_config);
  } else {
    Serial.println(F("Gueltige Konfiguration aus EEPROM geladen."));
  }
}

/*
   Konfiguration explizit in den EEPROM schreiben.
   (z.B. wenn der Benutzer ueber ein Menue "Speichern" waehlt)
*/
void saveConfigToEEPROM() {
  EEPROM.put(CONFIG_START_ADDR, g_config);
  Serial.println(F("Konfiguration wurde in den EEPROM geschrieben."));
}


// ---------------------------------------------------------------------------
// 7) Ringpuffer-Verwaltung: Initialisierung, Schreiben, Lesen
// ---------------------------------------------------------------------------

/*
   Hilfsfunktion:
   Berechnet die Startadresse im EEPROM fuer einen Snapshot mit Index i.
   Index i ist 0-basierend (0..SNAPSHOT_CAPACITY-1).
*/
int snapshotAddress(int index) {
  return SNAPSHOT_AREA_START_ADDR + index * SNAPSHOT_SIZE;
}

/*
   Beim Start muessen wir herausfinden:
   - Gibt es bereits gueltige Snapshots?
   - Wenn ja: welcher ist der neueste (hoechtse Sequence)?
   - Daraus ergeben sich currentSnapshotIndex und currentSequence.

   Vorgehen:
   - Wir laufen einmal ueber alle moeglichen Snapshot-Slots.
   - Wir lesen jeweils den DataSnapshot und pruefen:
       - Ist magic == SNAPSHOT_MAGIC?
         -> Dann ist dieser Slot gueltig.
         -> Wir vergleichen die sequence mit der bisher besten.
   - Der Slot mit der groessten sequence ist der neueste Eintrag.
*/
void initSnapshotRing() {
  if (SNAPSHOT_CAPACITY <= 0) {
    Serial.println(F("WARNUNG: Kein Platz fuer Snapshots im EEPROM!"));
    return;
  }

  int bestIndex       = -1;       // Index des neuesten Snapshots
  uint32_t bestSeq    = 0;        // hoechste gefundene Sequence

  for (int i = 0; i < SNAPSHOT_CAPACITY; i++) {
    DataSnapshot snap;
    EEPROM.get(snapshotAddress(i), snap);

    if (snap.magic == SNAPSHOT_MAGIC) {
      // Gueltiger Eintrag
      if (bestIndex == -1 || snap.sequence > bestSeq) {
        bestIndex = i;
        bestSeq   = snap.sequence;
      }
    }
  }

  if (bestIndex == -1) {
    // Es gibt noch keine gueltigen Snapshots.
    Serial.println(F("Noch keine gueltigen Snapshots im EEPROM gefunden."));
    currentSnapshotIndex = -1;
    currentSequence      = 0;
  } else {
    Serial.print(F("Neuester Snapshot an Index "));
    Serial.print(bestIndex);
    Serial.print(F(" mit Sequence "));
    Serial.println(bestSeq);

    currentSnapshotIndex = bestIndex;
    currentSequence      = bestSeq;
  }
}

/*
   Schreiben eines neuen Snapshots in den Ringpuffer.

   Eingabe:
   - 3 float-Arrays mit je 24 Elementen:
       temps[24], pressures[24], humidities[24]

   Ablauf:
   - wir bestimmen den naechsten Index im Ring (mit Wrap-around)
   - erhoehen die sequence
   - konvertieren die floats in int16_t / uint16_t
   - schreiben alles mit EEPROM.put() an die berechnete Adresse
*/
void saveMeasurementArraysToEEPROM(const float temps[24],
                                   const float pressures[24],
                                   const float humidities[24]) {
  if (SNAPSHOT_CAPACITY <= 0) {
    Serial.println(F("Kein Platz fuer Snapshots im EEPROM! Abbruch."));
    return;
  }

  // Naechsten Index im Ring ermitteln:
  // - Wenn bisher kein Eintrag existierte -> bei 0 anfangen.
  // - Sonst: (current + 1) modulo CAPACITY.
  if (currentSnapshotIndex < 0) {
    currentSnapshotIndex = 0;
  } else {
    currentSnapshotIndex = (currentSnapshotIndex + 1) % SNAPSHOT_CAPACITY;
  }

  // Sequence erhoehen (damit jeder Snapshot eine groessere Nummer bekommt).
  currentSequence++;

  // Snapshot-Struktur im RAM fuellen.
  DataSnapshot snap;
  snap.magic    = SNAPSHOT_MAGIC;
  snap.sequence = currentSequence;

  // 24 Temperaturwerte umwandeln und in das Snapshot-Array schreiben.
  for (int i = 0; i < 24; i++) {
    snap.temperatures[i] = encodeTemperature(temps[i]);
  }

  // 24 Druckwerte umwandeln.
  for (int i = 0; i < 24; i++) {
    snap.pressures[i] = encodePressure(pressures[i]);
  }

  // 24 Feuchtewerte umwandeln.
  for (int i = 0; i < 24; i++) {
    snap.humidities[i] = encodeHumidity(humidities[i]);
  }

  // Snapshot in den EEPROM schreiben.
  // EEPROM.put() schreibt die komplette Struktur in einem Rutsch.
  EEPROM.put(snapshotAddress(currentSnapshotIndex), snap);

  Serial.print(F("Snapshot an Index "));
  Serial.print(currentSnapshotIndex);
  Serial.print(F(" gespeichert. Sequence = "));
  Serial.println(currentSequence);
}

/*
   Laden des zuletzt gespeicherten Snapshots.

   Ausgabe:
   - 3 float-Arrays, in die die Werte decodiert werden.

   Rueckgabewert:
   - true  -> es wurde ein gueltiger Snapshot geladen
   - false -> es existiert noch kein Snapshot
*/
bool loadLastMeasurementArraysFromEEPROM(float temps[24],
                                         float pressures[24],
                                         float humidities[24]) {
  if (currentSnapshotIndex < 0) {
    // Noch keine gueltigen Snapshots vorhanden
    Serial.println(F("Es wurde noch kein Snapshot im EEPROM gespeichert."));
    return false;
  }

  DataSnapshot snap;
  EEPROM.get(snapshotAddress(currentSnapshotIndex), snap);

  if (snap.magic != SNAPSHOT_MAGIC) {
    Serial.println(F("Fehler: Letzter Snapshot hat ungueltige Magic. Daten evtl. defekt."));
    return false;
  }

  // Werte decodieren
  for (int i = 0; i < 24; i++) {
    temps[i]      = decodeTemperature(snap.temperatures[i]);
    pressures[i]  = decodePressure(snap.pressures[i]);
    humidities[i] = decodeHumidity(snap.humidities[i]);
  }

  Serial.print(F("Snapshot mit Sequence "));
  Serial.print(snap.sequence);
  Serial.println(F(" wurde aus EEPROM geladen."));

  return true;
}

/*
   Hilfsfunktion zur Ausgabe des zuletzt geladenen Snapshots auf den seriellen Monitor.
   (Nur zu Debug-/Lernzwecken)
*/
void printMeasurementArrays(const float temps[24],
                            const float pressures[24],
                            const float humidities[24]) {
  Serial.println(F("Inhalt der Messarrays (Index: Temp / Druck / Feuchte):"));

  for (int i = 0; i < 24; i++) {
    Serial.print(F("Index "));
    Serial.print(i);
    Serial.print(F(": T="));
    Serial.print(temps[i]);
    Serial.print(F(" °C,  P="));
    Serial.print(pressures[i]);
    Serial.print(F(" hPa,  H="));
    Serial.print(humidities[i]);
    Serial.println(F(" %"));
  }
}


// ---------------------------------------------------------------------------
// 8) Beispiel: Dummy-Messwerte erzeugen (nur zum Testen)
// ---------------------------------------------------------------------------

/*
   Diese Funktion erzeugt einfach Beispielwerte fuer die 3 Arrays,
   damit wir etwas zum Speichern haben.

   In der echten Anwendung wuerdest du hier deine aktuellen Messwerte
   (z.B. von Sensoren) eintragen.
*/
void fillExampleMeasurementArrays(float temps[24],
                                  float pressures[24],
                                  float humidities[24]) {
  for (int i = 0; i < 24; i++) {
    // Beispiel: Temperatur zwischen 20 und 30 °C
    temps[i] = 20.0f + i * 0.5f;

    // Beispiel: Druck um 1013 hPa mit leichtem Verlauf
    pressures[i] = 1010.0f + i * 0.2f;

    // Beispiel: Feuchte zwischen 40 und 60 %
    humidities[i] = 40.0f + i * 0.8f;
  }
}


// ---------------------------------------------------------------------------
// 9) setup() und loop()
// ---------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    // Warten, bis die serielle Schnittstelle bereit ist (bei manchen Boards noetig)
  }

  Serial.println(F("EEPROM-Demo mit Konfiguration + Ringpuffer startet."));

  // EEPROM-Groesse ermitteln (auf dem Mega normalerweise 4096)
  EEPROM_SIZE = EEPROM.length();
  Serial.print(F("EEPROM-Groesse: "));
  Serial.print(EEPROM_SIZE);
  Serial.println(F(" Bytes"));

  // Kapazitaet des Ringpuffers berechnen:
  // Groesse des Bereichs fuer Snapshots = gesamt - Config
