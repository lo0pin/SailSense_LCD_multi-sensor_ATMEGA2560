#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Animations-Variablen
int16_t boatX = -30;              // Start links außerhalb des Bildschirms
const int16_t boatY = 40;         // Vertikale Position des Bootes
unsigned long lastFrame = 0;
const uint16_t frameDelay = 80;   // ms zwischen Frames (Geschwindigkeit)

void setup() {
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    // Falls kein Display gefunden wird: Endlosschleife
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println(F("SailSense Demo"));
  display.display();
  delay(1000);
}

void loop() {
  unsigned long now = millis();
  if (now - lastFrame > frameDelay) {
    lastFrame = now;

    // Boot-Position updaten
    boatX++;
    if (boatX > SCREEN_WIDTH + 10) {
      boatX = -30;   // wieder von links reinfahren
    }

    drawScene();
  }
}

// Gesamtszene zeichnen (Himmel, Wasser, Boot)
void drawScene() {
  display.clearDisplay();

  // Wasserlinie / kleine Wellen
  drawWaves(0, 50, SCREEN_WIDTH, 4);

  // Boot
  drawBoat(boatX, boatY);

  display.display();
}

// Minimalistische Wellen: einfache Zickzack-Linie
void drawWaves(int16_t x, int16_t y, int16_t width, int16_t amplitude) {
  for (int16_t i = x; i < x + width; i += 4) {
    display.drawLine(i, y, i + 2, y + amplitude, SSD1306_WHITE);
    display.drawLine(i + 2, y + amplitude, i + 4, y, SSD1306_WHITE);
  }
}

// Minimalistisches Segelboot
void drawBoat(int16_t x, int16_t y) {
  // Rumpf (kleines Rechteck oder Linie)
  int16_t hullWidth = 20;
  int16_t hullHeight = 3;

  // Rumpf als gefülltes Rechteck
  display.fillRect(x, y, hullWidth, hullHeight, SSD1306_WHITE);

  // Unterkante Rumpf leicht schräg (optional, bisschen "Bootsform")
  display.drawLine(x, y + hullHeight, x + hullWidth, y + hullHeight, SSD1306_WHITE);

  // Mast
  int16_t mastHeight = 15;
  int16_t mastX = x + hullWidth / 2;
  int16_t mastYTop = y - mastHeight;
  display.drawLine(mastX, y, mastX, mastYTop, SSD1306_WHITE);

  // Segel (Dreieck)
  // Eine Seite zum Mast, eine zum Bug
  int16_t sailFrontX = x + hullWidth;   // Bug-Seite
  int16_t sailBaseY = y;                // Basis am Mast
  display.fillTriangle(
    mastX, mastYTop,     // Mastspitze
    mastX, sailBaseY,    // Mastfuß
    sailFrontX, sailBaseY - 2, // vorne leicht angehoben
    SSD1306_WHITE
  );

  // Optional: kleines Heck-Segel (Sturmfock-Feeling 😄)
  int16_t sailBackX = x;  // Heck-Seite
  display.drawTriangle(
    mastX, mastYTop + 3,
    mastX, sailBaseY - 1,
    sailBackX, sailBaseY - 2
  );
}
