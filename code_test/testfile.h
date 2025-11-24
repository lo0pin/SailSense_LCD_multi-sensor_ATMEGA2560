constexpr uint8_t  SCREEN_WIDTH    128
constexpr uint8_t  SCREEN_HEIGHT   64
constexpr int8_t   OLED_RESET      -1      // Reset-Pin (oder -1, wenn über Arduino-Reset)
constexpr uint8_t  SCREEN_ADDRESS  0x3C    // I2C-Adresse deines Displays

constexpr uint8_t MPU9250_ADDR      0x68
//constexpr uint8_t INT_PIN           2          // optional, falls INT verbunden ist

class Adafruit_BME280;
class RTC_DS3231;
class Adafruit_SSD1306;
class MPU9250_WE;

void systemInit(Adafruit_BME280& bme_var, RTC_DS3231& rtc_var, Adafruit_SSD1306& oled_var, MPU9250_WE& imu_var);
bool initBME280(Adafruit_BME280& bme_var);
bool initRTC(RTC_DS3231& rtc_var);
bool initDISPLAY(Adafruit_SSD1306& display_var);
bool initIMU(MPU9250_WE& imu_var);

void updateButtons();
void updateSensors();
void updateNavigation();
void updateMenuSystem();
void renderDisplay();
void handleAlarms();
