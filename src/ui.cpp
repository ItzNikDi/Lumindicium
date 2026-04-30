#include "ui.h"
#include <M5Unified.h>
#include "globals.h"

void tickDisplay() {
    portENTER_CRITICAL(&gMux);
    const auto distanceCm = gState.distanceCm;
    const auto config = gConfig;
    portEXIT_CRITICAL(&gMux);

    auto zone = Zone::GREEN;
    if (distanceCm >= 0) {
        if (distanceCm <= config.redThresholdCm) zone = Zone::RED;
        else if (distanceCm <= config.yellowThresholdCm) zone = Zone::YELLOW;
    }

    const uint16_t zoneColor = zone == Zone::RED ? ILI9341_RED : zone == Zone::YELLOW ? ILI9341_YELLOW : ILI9341_GREEN;

    M5.Display.fillScreen(ILI9341_BLACK);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 8);
    M5.Display.print("LUMINDICIUM");

    M5.Display.setTextColor(zoneColor, TFT_BLACK);
    M5.Display.setTextSize(5);
    M5.Display.setCursor(10, 40);
    if (distanceCm < 0) M5.Display.print("  ---");
    else M5.Display.printf("%5.1f", distanceCm);

    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 100);
    M5.Display.print("cm");

    M5.Display.setTextColor(zoneColor, ILI9341_BLACK);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 130);
    switch (zone) {
        case Zone::RED: M5.Display.print("!! DANGER  !!");
            break;
        case Zone::YELLOW: M5.Display.print("   WARNING   ");
            break;
        default: M5.Display.print("     SAFE    ");
            break;
    }

    M5.Display.setTextColor(ILI9341_DARKGREY, ILI9341_BLACK);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 165);
    M5.Display.printf("YLW < %.0fcm  RED < %.0fcm",
                      config.yellowThresholdCm, config.redThresholdCm);
    M5.Display.setCursor(10, 178);
    M5.Display.printf("Vol Y:%d%%  R:%d%%", config.yellowVolume, config.redVolume);
}
