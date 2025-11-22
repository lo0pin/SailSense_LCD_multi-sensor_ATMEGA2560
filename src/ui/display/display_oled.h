#pragma once

namespace DisplayOLED {
    void begin();
    void clear();
    void renderMain(const EnvData&, const IMUData&, const BatteryStatus&);
    void renderCompass(float headingDeg);
    void showSplash();
}

