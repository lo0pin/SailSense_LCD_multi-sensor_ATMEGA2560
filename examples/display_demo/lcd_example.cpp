^#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

// Auto-Scan für PCF8574/PCF8574A/AT Boards
hd44780_I2Cexp lcd;

void setup() {
  // LCD starten (20 Spalten, 4 Zeilen)
  int status = lcd.begin(20, 4);
  if (status) {
    // Optional: Fehlermeldung am Serial-Monitor
    // Bedeutet: I2C oder Display ist nicht erreichbar
  }

  lcd.clear();
  lcd.print("hd44780 Feature Demo");
  delay(2000);

  // ==========================
  //  BASIC FUNCTIONS
  // ==========================

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hello World!");
  lcd.setCursor(0, 1);
  lcd.print("Numbers: ");
  lcd.print(1234);

  lcd.setCursor(0, 2);
  lcd.print("Hex: 0x");
  lcd.print(255, HEX);

  lcd.setCursor(0, 3);
  lcd.print("Float: ");
  lcd.print(3.1415, 3);  // 3 Nachkommastellen
  delay(3000);

  // ==========================
  //  CURSOR UND BLINKEN
  // ==========================

  lcd.clear();
  lcd.print("Cursor on...");
  lcd.cursor();
  delay(2000);

  lcd.setCursor(0, 1);
  lcd.print("Blink...");
  lcd.blink();
  delay(2000);

  lcd.noCursor();
  lcd.noBlink();
  delay(1000);

  // ==========================
  //  DISPLAY AN/AUS
  // ==========================

  lcd.clear();
  lcd.print("Display off...");
  delay(1500);
  lcd.noDisplay();

  delay(1500);
  lcd.display();
  lcd.setCursor(0, 1);
  lcd.print("Display on");
  delay(2000);

  // ==========================
  //  SCROLLEN
  // ==========================

  lcd.clear();
  lcd.print("Scroll-Test");
  delay(1000);

  for (int i = 0; i < 10; i++) {
    lcd.scrollDisplayLeft();
    delay(300);
  }

  for (int i = 0; i < 10; i++) {
    lcd.scrollDisplayRight();
    delay(300);
  }

  delay(1500);

  // ==========================
  //  AUTOSCROLL
  // ==========================

  lcd.clear();
  lcd.setCursor(19, 0);  // Am rechten Rand starten
  lcd.autoscroll();

  for (char c = 'A'; c <= 'Z'; c++) {
    lcd.print(c);
    delay(200);
  }

  lcd.noAutoscroll();
  delay(2000);

  // ==========================
  //  CUSTOM CHARACTERS
  // ==========================

  uint8_t smiley[8] = {
    0b00000,
    0b00000,
    0b01010,
    0b00000,
    0b00000,
    0b10001,
    0b01110,
    0b00000
  };

  lcd.createChar(0, smiley);

  lcd.clear();
  lcd.print("Custom char: ");
  lcd.write((uint8_t)0);
  delay(3000);

  // ==========================
  //  HOME & CLEAR
  // ==========================

  lcd.clear();
  lcd.print("Home test...");
  delay(1000);

  lcd.setCursor(10, 2);
  lcd.print("Here");
  delay(1500);

  lcd.home();  // Cursor zurück auf (0,0)
  lcd.print("Fin!");
}

void loop() {
  // Nichts – der Demo-Sketch läuft einmal durch
}
