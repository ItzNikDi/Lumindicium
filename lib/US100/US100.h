#pragma once
#include <Arduino.h>

#define US100_RX 13
#define US100_TX 14

class US100 {
public:
    static constexpr auto ERROR_VALUE = -1.f;

    static constexpr auto MIN_DISTANCE_CM = 2.f;
    static constexpr auto MAX_DISTANCE_CM = 450.f;

    explicit US100(Stream &serial);

    bool begin() const;

    float readDistanceCm(uint32_t timeoutMs = 100) const;

    float readTemperatureC(uint32_t timeoutMs = 100) const;

    float readTemperatureF(uint32_t timeoutMs = 100) const;

    bool isReady() const;

private:
    Stream *_serial = nullptr;
    bool _isInitialized = false;

    void flush() const;

    bool readByte(uint8_t &out, uint32_t timeoutMs) const;
};
