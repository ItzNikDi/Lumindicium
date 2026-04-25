#include <Arduino.h>
#include "display.h"

#include <US100.h>

US100 sonar(Serial2);

void setup() {
#if defined(ARDUINO_M5STACK_Core2)
    auto config = m5::M5Unified::config();
    config.serial_baudrate = 115200;
    M5.begin(config);
#elif defined(ARDUINO_ESP32_DEV)
    Serial.begin(115200);
#endif
    //TODO abstract this logic away :)
    Serial2.begin(9600, SERIAL_8N1, 13, 14);
    initDisplay();

    if (sonar.begin()) {
        Serial.printf("Sonar initialized!\n");
        LCD.printf("Sonar initialized!\n");
    }
}

void loop() {
    delay(200);

    const auto measurement = sonar.readDistanceCm();
    Serial.printf("Read value: %.2f\n", measurement);
    if (measurement != US100::ERROR_VALUE) {
        LCD.fillScreen(TFT_BLACK);
        LCD.setCursor(20, 30);
        LCD.printf("Distance: %f cm", measurement);
    }

    delay(300);
}
