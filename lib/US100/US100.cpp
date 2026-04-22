#include "US100.h"

US100::US100(Stream &serial) : _serial(&serial), _isInitialized(true) {
}

bool US100::begin() const {
    return isReady();
}

bool US100::isReady() const {
    return _isInitialized && _serial != nullptr;
}


float US100::readDistanceCm(const uint32_t timeoutMs) const {
    if (!isReady()) return ERROR_VALUE;

    flush();

    _serial->write(0x55);

    uint8_t high, low;
    if (!readByte(high, timeoutMs) || !readByte(low, timeoutMs)) return ERROR_VALUE;

    const auto mm = (static_cast<uint16_t>(high) << 8) | low;

    const auto cm = mm / 10.0f;

    if (cm < MIN_DISTANCE_CM || cm > MAX_DISTANCE_CM) return ERROR_VALUE;

    return cm;
}

float US100::readTemperatureC(const uint32_t timeoutMs) const {
    if (!isReady()) return ERROR_VALUE;

    flush();

    _serial->write(0x50);

    uint8_t raw;
    if (!readByte(raw, timeoutMs)) return ERROR_VALUE;

    return static_cast<float>(raw) - 45.0f;
}

float US100::readTemperatureF(const uint32_t timeoutMs) const {
    const auto c = readTemperatureC(timeoutMs);

    return (c == ERROR_VALUE) ? ERROR_VALUE : c * 9.0f / 5.0f + 32.0f;
}


void US100::flush() const {
    while (_serial->available()) _serial->read();
}

bool US100::readByte(uint8_t &out, const uint32_t timeoutMs) const {
    const auto start = millis();

    while (!_serial->available()) if (millis() - start >= timeoutMs) return false;

    out = static_cast<uint8_t>(_serial->read());
    return true;
}
