#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Displayparameter
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1          // Reset-Pin (oder -1, wenn über Arduino-Reset)
#define SCREEN_ADDRESS 0x3C    // I2C-Adresse deines Displays

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Kleine Bitmap (16x16) als Beispiel (smiley)
const unsigned char PROGMEM smiley_bitmap[] = {
  0b00000000, 0b00000000,
  0b00011111, 0b11100000,
  0b00100000, 0b00010000,
  0b01000110, 0b01100000,
  0b01000110, 0b01100000,
  0b01000000, 0b00000000,
  0b01001000, 0b00000000,
  0b01000100, 0b00000000,
  0b01000010, 0b00000000,
  0b01000001, 0b00000000,
  0b01000000, 0b10000000,
  0b01000000, 0b10000000,
  0b00100001, 0b00000000,
  0b00011110, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000
};

// ---------------------------
// Hilfsfunktionen (Demos)
// ---------------------------

void textDemo() {
  display.clearDisplay();

  display.setTextSize(1);                 // 1 = klein
  display.setTextColor(SSD1306_WHITE);    // Pixel "an"
  display.setCursor(0, 0);
  display.println(F("SSD1306 Text-Demo"));
  display.println(F("------------------"));

  display.setTextSize(1);
  display.println(F("TextSize(1)"));
  display.setTextSize(2);
  display.println(F("Size(2)"));
  display.setTextSize(1);

  display.println();
  display.println(F("Cursor, Zeilen,"));
  display.println(F("Umlaute gehen mit"));
  display.println(F("cp437(true);"));

  // Optional: CP437 aktivieren, damit z.B. Gradzeichen etc. funktionieren
  display.cp437(true);
  display.write(248);  // Gradzeichen als Beispiel (°)

  display.display();
  delay(2500);
}

void graphicsDemo() {
  display.clearDisplay();

  // Pixel
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Linien
  display.drawLine(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, SSD1306_WHITE);
  display.drawLine(SCREEN_WIDTH - 1, 0, 0, SCREEN_HEIGHT - 1, SSD1306_WHITE);

  // Rechteck (umrandet)
  display.drawRect(0, 16, 40, 20, SSD1306_WHITE);
  // Rechteck (gefüllt)
  display.fillRect(45, 16, 40, 20, SSD1306_WHITE);

  // Abgerundetes Rechteck (Outline)
  display.drawRoundRect(90, 16, 30, 20, 5, SSD1306_WHITE);

  // Kreis und gefüllter Kreis
  display.drawCircle(20, 52, 10, SSD1306_WHITE);
  display.fillCircle(50, 52, 10, SSD1306_WHITE);

  // Dreieck und gefülltes Dreieck
  display.drawTriangle(80, 42, 120, 42, 100, 60, SSD1306_WHITE);
  display.fillTriangle(82, 44, 118, 44, 100, 58, SSD1306_WHITE);

  display.display();
  delay(2500);
}

void bitmapDemo() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Bitmap-Demo"));

  // Bitmap zeichnen (16x16)
  display.drawBitmap(56, 24, smiley_bitmap, 16, 16, SSD1306_WHITE);

  display.display();
  delay(2000);

  // Invertiertes Display
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);

  // Dimmen
  display.dim(true);
  delay(1000);
  display.dim(false);

  delay(1000);
}

void scrollDemo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Scroll-Demo"));
  display.println();
  display.println(F("Dies ist ein Test-"));
  display.println(F("text, der per HW-"));
  display.println(F("Scroll-Funktion"));
  display.println(F("verschoben wird."));

  display.display();
  delay(500);

  // Nach rechts scrollen
  display.startscrollright(0x00, 0x0F);  // Start/End-Page
  delay(2000);
  display.stopscroll();

  // Nach links scrollen
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();

  // Diagonal (rechts)
  display.startscrolldiagright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();

  // Diagonal (links)
  display.startscrolldiagleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();

  delay(500);
}

// ---------------------------
// Setup & Loop
// ---------------------------

void setup() {
  // Seriell optional zum Debuggen
  Serial.begin(9600);
  while (!Serial) { /* warten, falls nötig */ }

  // I2C initialisieren
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    // Falls das Display nicht gefunden wird
    Serial.println(F("SSD1306 nicht gefunden. Check Verkabelung/Adresse!"));
    for (;;); // Hängen bleiben
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("SSD1306 Demo"));
  display.println(F("128x64 @ 0x3C"));
  display.display();
  delay(1500);
}

void loop() {
  textDemo();      // Textfunktionen
  graphicsDemo();  // Linien, Formen, Pixel
  bitmapDemo();    // Bitmap, invertDisplay, dim
  scrollDemo();    // Hardware-Scroll
}
