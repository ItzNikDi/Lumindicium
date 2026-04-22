#pragma once

#if defined(ARDUINO_M5STACK_Core2)
#include <M5Unified.h>
#define LCD M5.Lcd

inline void initDisplay() {
    LCD.init();
}

#elif defined(ARDUINO_ESP32_DEV)
#include <TFT_eSPI.h>
TFT_eSPI tft;
#define LCD tft

inline void initDisplay() {
    LCD.init();
    LCD.setRotation(1);
}

#else
#error "Unsupported board :("
#endif
