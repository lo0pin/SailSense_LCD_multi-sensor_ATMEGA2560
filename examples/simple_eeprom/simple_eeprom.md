Gerne – hier bekommst du **ergänzenden Beispielcode**, der perfekt **unter deinen bestehenden EEPROM-Demo-Sketch passt** und dir zeigt:

* wie man **3 Arrays mit je bis zu 24 float-Werten** vorbereitet,
* wie man sie **in einer einzigen Struktur bündelt**,
* wie man sie **mit `EEPROM.put()` speichert**
* und **mit `EEPROM.get()` wieder lädt**.

Alles schön übersichtlich, direkt anwendbar und vollständig kompatibel zu deinem bisherigen Sketch.

---

# 📌 Ergänzender Beispielcode: 3 Arrays speichern & laden

Füge das unter deinen bisherigen Code oder in eine eigene Sektion ein.

---

## 📦 1. Struktur zum Speichern der Arrays

```cpp
// Struktur zum Speichern von 3 Arrays mit je 24 float-Werten
struct ArrayStorage {
  float arrayA[24];
  float arrayB[24];
  float arrayC[24];
};
```

---

## 🗄 2. Globale Instanz (RAM)

```cpp
ArrayStorage storedArrays;  // Diese Struktur wird im EEPROM gesichert
```

---

## 💾 3. Beispielwerte eintragen (Simulation)

```cpp
void fillExampleArrays() {
  for (int i = 0; i < 24; i++) {
    storedArrays.arrayA[i] = i * 1.1f;       // Beispielwerte
    storedArrays.arrayB[i] = 100 + i * 0.5;  // Beispielwerte
    storedArrays.arrayC[i] = sin(i * 0.2);   // Beispielwerte
  }
}
```

---

## 💾 4. Arrays im EEPROM speichern

```cpp
void saveArraysToEEPROM(int startAddress) {
  Serial.println(F("Speichere 3 Arrays in den EEPROM..."));
  EEPROM.put(startAddress, storedArrays);
  Serial.println(F("Arrays wurden gespeichert."));
}
```

---

## 📥 5. Arrays aus dem EEPROM laden

```cpp
void loadArraysFromEEPROM(int startAddress) {
  Serial.println(F("Lade Arrays vom EEPROM..."));
  EEPROM.get(startAddress, storedArrays);
  Serial.println(F("Arrays wurden geladen."));
}
```

---

## 🔍 6. Ausgabe zur Kontrolle

```cpp
void printArrays() {
  Serial.println(F("Inhalt der gespeicherten Arrays:"));
  for (int i = 0; i < 24; i++) {
    Serial.print(F("Index "));
    Serial.print(i);
    Serial.print(F(": A="));
    Serial.print(storedArrays.arrayA[i]);
    Serial.print(F("  B="));
    Serial.print(storedArrays.arrayB[i]);
    Serial.print(F("  C="));
    Serial.println(storedArrays.arrayC[i]);
  }
}
```

---

# 🚀 7. Beispiel-Aufruf im Setup

Füge diesen Abschnitt nach deinen bisherigen EEPROM-Demo-Teilen ein:

```cpp
int arrayStorageAddress = 100;  // Beispiel-Startadresse (abhängig von deiner Datenstruktur!)

printSeparator("6) Arrays speichern und laden");

// Beispielwerte erzeugen
fillExampleArrays();

// Arrays speichern
saveArraysToEEPROM(arrayStorageAddress);

// Arrays laden
loadArraysFromEEPROM(arrayStorageAddress);

// Ausgabe zur Kontrolle
printArrays();
```

---

# 🔧 Hinweis zur Adresse

Du musst nur eine Startadresse wählen, die **hinter deinem bisherigen Speicherteil liegt**.
Beispiel:

* Dein erster Teil (Konfiguration) startet ab Adresse 10
* Seine Strukturgröße beträgt z. B. 32 Byte
* Dann kannst du ab Adresse **50 oder 64** weitermachen

