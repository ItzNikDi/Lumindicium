#include "leds.h"

static constexpr uint8_t GREEN_LED_PIN = 32;
static constexpr uint8_t YELLOW_LED_PIN = 27;
static constexpr uint8_t RED_LED_PIN = 19;
static constexpr uint8_t POTENTIOMETER_PIN = 35;

static constexpr uint8_t GREEN_CHANNEL = 0;
static constexpr uint8_t YELLOW_CHANNEL = 1;
static constexpr uint8_t RED_CHANNEL = 2;

static constexpr uint8_t PWM_RESOLUTION_BITS = 8;
static constexpr uint32_t PWM_FREQUENCY_HZ = 5000;

static auto gCurrentZone = Zone::GREEN;
static uint8_t gCurrentDuty = 255;

void ledsBegin() {
    ledcSetup(GREEN_CHANNEL, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);
    ledcSetup(YELLOW_CHANNEL, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);
    ledcSetup(RED_CHANNEL, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);

    ledcAttachPin(GREEN_LED_PIN, GREEN_CHANNEL);
    ledcAttachPin(YELLOW_LED_PIN, YELLOW_CHANNEL);
    ledcAttachPin(RED_LED_PIN, RED_CHANNEL);
}

void tickPotentiometer() {
    const auto raw = analogRead(POTENTIOMETER_PIN);
    const auto duty = static_cast<uint8_t>(raw >> 4);
    gCurrentDuty = duty;

    setLedZone(gCurrentZone);
}

void setLedZone(const Zone zone) {
    gCurrentZone = zone;

    ledcWrite(GREEN_CHANNEL, 0);
    ledcWrite(YELLOW_CHANNEL, 0);
    ledcWrite(RED_CHANNEL, 0);

    switch (zone) {
        case Zone::GREEN:
            ledcWrite(GREEN_CHANNEL, gCurrentDuty);
            break;
        case Zone::YELLOW:
            ledcWrite(YELLOW_CHANNEL, gCurrentDuty);
            break;
        case Zone::RED:
            ledcWrite(RED_CHANNEL, gCurrentDuty);
            break;
    }
}
