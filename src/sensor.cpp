#include "sensor.h"
#include "audio.h"
#include "leds.h"
#include "web_handlers.h"

static auto gLastAcceptedCm = -1.f;
static auto gLastZone = Zone::GREEN;

static constexpr auto HYSTERESIS_PCT = 0.02f;
static constexpr uint8_t MEDIAN_WINDOW = 5;

static float readings[MEDIAN_WINDOW] = {};
static uint8_t readIdx = 0;
static auto bufferFull = false;

Zone zoneFor(const float distanceCm, const Config &config) {
    if (distanceCm < 0) return Zone::GREEN;
    if (distanceCm <= config.redThresholdCm) return Zone::RED;
    if (distanceCm <= config.yellowThresholdCm) return Zone::YELLOW;
    return Zone::GREEN;
}

static float smoothedRead() {
    const auto rawReading = sonar.readDistanceCm(165);
    if (rawReading < 0) return rawReading;

    readings[readIdx] = rawReading;
    readIdx = (readIdx + 1) % MEDIAN_WINDOW;
    if (readIdx == 0) bufferFull = true;

    const auto count = bufferFull ? MEDIAN_WINDOW : readIdx;
    float tmp[MEDIAN_WINDOW];
    memcpy(tmp, readings, count * sizeof(float));
    std::sort(tmp, tmp + count);

    return tmp[count / 2];
}

void tickSensor() {
    const auto rawReading = smoothedRead();

    portENTER_CRITICAL(&gMux);
    const auto config = gConfig;
    portEXIT_CRITICAL(&gMux);

    const auto rawZone = zoneFor(rawReading, config);

    const auto zoneChanged = (rawZone != gLastZone);
    const auto movedEnough = (gLastAcceptedCm > 0) && (
                                 fabsf(rawReading - gLastAcceptedCm) / gLastAcceptedCm >= HYSTERESIS_PCT);
    const auto isError = (rawReading < 0);
    const auto isFirst = (gLastAcceptedCm < 0);

    if (!isFirst && !isError && !zoneChanged && !movedEnough) return;

    gLastAcceptedCm = rawReading;
    gLastZone = rawZone;

    portENTER_CRITICAL(&gMux);
    gState.distanceCm = rawReading;
    portEXIT_CRITICAL(&gMux);

    setLedZone(rawZone);
    updateProximitySound(rawReading, rawZone);
    broadcastState(rawReading);
}
