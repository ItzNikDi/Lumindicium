#pragma once

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <freertos/portmacro.h>

#include "US100.h"

static constexpr auto AP_SSID = "Lumindicium";
static constexpr auto AP_PASS = "Lumin!c1A";

struct Config {
    float yellowThresholdCm = 200.f;
    float redThresholdCm = 30.f;
    uint8_t yellowVolume = 80;
    uint8_t redVolume = 100;
};


inline void convertToJson(const Config &src, const JsonVariant dst) {
    dst["yellowThreshold"] = src.yellowThresholdCm;
    dst["redThreshold"] = src.redThresholdCm;
    dst["yellowVolume"] = src.yellowVolume;
    dst["redVolume"] = src.redVolume;
}

inline void convertFromJson(const JsonVariantConst src, Config &dst) {
    dst.yellowThresholdCm = src["yellowThreshold"] | dst.yellowThresholdCm;
    dst.redThresholdCm = src["redThreshold"] | dst.redThresholdCm;
    dst.yellowVolume = src["yellowVolume"] | dst.yellowVolume;
    dst.redVolume = src["redVolume"] | dst.redVolume;
}

struct WavBuffer {
    uint8_t *data = nullptr;
    size_t len = 0;
    bool ok = false;
};

struct State {
    float distanceCm = -1.f;
};

enum class Zone {
    GREEN,
    YELLOW,
    RED
};

extern US100 sonar;
extern Config gConfig;
extern State gState;
extern WavBuffer gWavYellow;
extern WavBuffer gWavRed;
extern portMUX_TYPE gMux;
extern AsyncWebServer server;
extern AsyncEventSource events;
