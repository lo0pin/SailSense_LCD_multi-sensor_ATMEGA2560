class Adafruit_BME280;

void systemInit(Adafruit_BME280& bme_var);
bool initBME280(Adafruit_BME280& bme_var);
void updateButtons();
void updateSensors();
void updateNavigation();
void updateMenuSystem();
void renderDisplay();
void handleAlarms();
