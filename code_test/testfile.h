class Adafruit_BME280;
class RTC_DS3231;

void systemInit(Adafruit_BME280& bme_var, RTC_DS3231& rtc_var);
bool initBME280(Adafruit_BME280& bme_var);
bool initRTC(RTC_DS3231& rtc_var);

void updateButtons();
void updateSensors();
void updateNavigation();
void updateMenuSystem();
void renderDisplay();
void handleAlarms();
