const int BUZZER_PIN = 9;

void setup() {
  // nichts nötig
}

void loop() {
  // 1 kHz-Ton für 500 ms
  tone(BUZZER_PIN, 1000); // 1000 Hz
  delay(100);

  noTone(BUZZER_PIN);
  delay(900);
}
