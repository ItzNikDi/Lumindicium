#include <Arduino.h>
#include <M5Unified.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WiFi.h>

#include "audio.h"
#include "globals.h"
#include "leds.h"
#include "sensor.h"
#include "ui.h"
#include "web_handlers.h"

static constexpr uint32_t POT_INTERVAL_MS = 50;
static constexpr uint32_t SENSOR_INTERVAL_MS = 200;
static constexpr uint32_t DISPLAY_INTERVAL_MS = 500;

static uint32_t lastPotMs = 0;
static uint32_t lastSensorMs = 0;
static uint32_t lastDisplayMs = 0;

void setup() {
    auto m5config = m5::M5Unified::config();
    m5config.serial_baudrate = 115200;
    M5.begin(m5config);
    M5.Log.setLogLevel(m5::log_target_serial, ESP_LOG_DEBUG);
    Serial2.begin(9600, SERIAL_8N1, 13, 14);
    M5.Speaker.begin();

    ledsBegin();
    LittleFS.begin();
    loadWav("/sounds/beep1.wav", gWavYellow);
    loadWav("/sounds/beep2.wav", gWavRed);

    WiFi.softAP(AP_SSID, AP_PASS);
    setupRoutes();
    server.begin();

    M5.Display.begin();
    M5.Display.setRotation(1);
}

void loop() {
    M5.update();

    const auto now = millis();
    if (now - lastPotMs >= POT_INTERVAL_MS) {
        lastPotMs = now;
        tickPotentiometer();
    }
    if (now - lastSensorMs >= SENSOR_INTERVAL_MS) {
        lastSensorMs = now;
        tickSensor();
    }
    if (now - lastDisplayMs >= DISPLAY_INTERVAL_MS) {
        lastDisplayMs = now;
        tickDisplay();
    }
}
