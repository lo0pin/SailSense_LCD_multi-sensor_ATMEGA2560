/*
  I2C-Scanner (robust) – zeigt alle 7-Bit-Adressen (0x03..0x77)
  Arduino Uno: SDA=A4, SCL=A5
*/

#include <Wire.h>

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  Serial.println(F("\nI2C-Scanner startet..."));

  // Uno nutzt A4/A5 automatisch. Für andere Boards:
  // Wire.setClock(100000); // Standard 100 kHz (bei langen Kabeln ggf. niedriger)

  Wire.begin();
}

void loop() {
  byte count = 0;

  Serial.println(F("\nScanne Bus..."));
  Serial.println(F("Gefundene Geraete (HEX):"));

  for (uint8_t address = 0x03; address <= 0x77; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
      Serial.print(F("  0x"));
      if (address < 16) Serial.print('0');
      Serial.print(address, HEX);
      Serial.println(F("  (ACK)"));
      count++;
    } else if (error == 4) {
      Serial.print(F("  0x"));
      if (address < 16) Serial.print('0');
      Serial.print(address, HEX);
      Serial.println(F("  (Unbekannter Fehler)"));
    }
    // error==2/3 -> NACK/Busy -> kein Gerät
  }

  if (count == 0) {
    Serial.println(F("Keine I2C-Geraete gefunden."));
  } else {
    Serial.print(F("Summe: "));
    Serial.println(count);
  }

  // Alle 3 Sekunden erneut scannen
  delay(3000);
}
