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
