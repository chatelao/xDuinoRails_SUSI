#include "susi_hal.h"

SusiHAL::SusiHAL(uint8_t clock_pin, uint8_t data_pin) {
    _clock_pin = clock_pin;
    _data_pin = data_pin;
}

void SusiHAL::begin() {
    pinMode(_clock_pin, OUTPUT);
    pinMode(_data_pin, OUTPUT);
}

void SusiHAL::set_clock_high() {
    digitalWrite(_clock_pin, HIGH);
}

void SusiHAL::set_clock_low() {
    digitalWrite(_clock_pin, LOW);
}

void SusiHAL::generate_clock_pulse() {
    set_clock_low();
    delayMicroseconds(10);
    set_clock_high();
    delayMicroseconds(10);
}

void SusiHAL::set_data_high() {
    digitalWrite(_data_pin, HIGH);
}

void SusiHAL::set_data_low() {
    digitalWrite(_data_pin, LOW);
}

bool SusiHAL::read_data() {
    pinMode(_data_pin, INPUT);
    bool value = digitalRead(_data_pin);
    pinMode(_data_pin, OUTPUT);
    return value;
}

bool SusiHAL::read_bit() {
    set_clock_low();
    delayMicroseconds(10);
    bool value = read_data();
    set_clock_high();
    delayMicroseconds(10);
    return value;
}

bool SusiHAL::waitForAck() {
    unsigned long start_time = millis();

    // Wait for the data line to go LOW (start of ACK)
    while (read_data()) {
        if (millis() - start_time > 20) {
            return false; // Timeout
        }
    }

    unsigned long pulse_start_time = micros();

    // Wait for the data line to go HIGH again (end of ACK)
    while (!read_data()) {
        if (millis() - start_time > 20) {
            return false; // Timeout
        }
    }

    unsigned long pulse_duration = micros() - pulse_start_time;

    // Check if the pulse duration is within the valid range (0.5ms to 7ms)
    if (pulse_duration >= 500 && pulse_duration <= 7000) {
        return true;
    }

    return false;
}

void SusiHAL::sendAckPulse() {
    pinMode(_data_pin, OUTPUT);
    digitalWrite(_data_pin, LOW);
    delay(1); // 1-2ms ACK pulse
    digitalWrite(_data_pin, HIGH);
    pinMode(_data_pin, INPUT);
}
