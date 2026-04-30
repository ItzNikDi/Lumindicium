#include "audio.h"
#include <M5Unified.h>
#include <LittleFS.h>

static constexpr uint32_t BEEP_INTERVAL_MIN_MS = 150;
static constexpr uint32_t BEEP_INTERVAL_MAX_MS = 1200;

static uint32_t lastBeepMs = 0;
static auto lastZone = Zone::GREEN;

bool loadWav(const char *path, WavBuffer &out) {
    auto f = LittleFS.open(path, "r");
    if (!f) {
        M5.Log(ESP_LOG_ERROR, "[WAV] Failed to open %s!\n", path);
        return false;
    }

    out.len = f.size();
    out.data = static_cast<uint8_t *>(ps_malloc(out.len));
    if (!out.data) {
        f.close();
        return false;
    }

    f.read(out.data, out.len);
    f.close();
    out.ok = true;
    M5.Log(ESP_LOG_DEBUG, "[WAV] Loaded %s (%u bytes)\n", path, out.len);
    return true;
}

static uint32_t beepIntervalFor(const float distanceCm, const float nearEdgeCm, const float farEdgeCm) {
    auto t = (distanceCm - nearEdgeCm) / (farEdgeCm - nearEdgeCm);
    t = constrain(t, 0.f, 1.f);
    return static_cast<uint32_t>(BEEP_INTERVAL_MIN_MS + t * (BEEP_INTERVAL_MAX_MS - BEEP_INTERVAL_MIN_MS));
}

void updateProximitySound(const float distanceCm, const Zone zone) {
    if (zone == Zone::GREEN) {
        M5.Speaker.stop();
        lastBeepMs = 0;
        lastZone = zone;
        return;
    }

    portENTER_CRITICAL(&gMux);
    const auto config = gConfig;
    portEXIT_CRITICAL(&gMux);

    WavBuffer *wav;
    uint8_t volume;
    uint32_t interval;

    if (zone == Zone::RED) {
        wav = &gWavRed;
        volume = config.redVolume;
        interval = beepIntervalFor(distanceCm, config.redThresholdCm, config.yellowThresholdCm);
    } else {
        wav = &gWavYellow;
        volume = config.yellowVolume;
        interval = beepIntervalFor(distanceCm, config.redThresholdCm, config.yellowThresholdCm);
    }

    if (!wav->ok) return;

    const auto now = millis();

    const auto zoneChanged = (zone != lastZone);
    lastZone = zone;

    M5.Speaker.setVolume(volume);

    if (zoneChanged || (now - lastBeepMs >= interval)) {
        lastBeepMs = now;
        M5.Speaker.playWav(wav->data, wav->len, 1, 0, true);
    }
}

void stopProximitySound() {
    M5.Speaker.stop();
    lastBeepMs = 0;
}
