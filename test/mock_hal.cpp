#include "mock_hal.h"
#include <iostream>
#include <map>

// Store pin states
std::map<uint8_t, uint8_t> pin_modes;
std::map<uint8_t, uint8_t> pin_states;
std::vector<Call> digitalWrite_calls;
std::map<uint8_t, ISR> isr_map;
std::map<uint8_t, int> isr_mode_map;

void pinMode(uint8_t pin, uint8_t mode) {
    pin_modes[pin] = mode;
}

void digitalWrite(uint8_t pin, uint8_t val) {
    // Log the call
    digitalWrite_calls.push_back({pin, val});

    // Check for a falling edge to trigger an ISR
    if (pin_states.count(pin) && pin_states[pin] == HIGH && val == LOW) {
        if (isr_map.count(pin) && isr_mode_map[pin] == FALLING) {
            isr_map[pin]();
        }
    }

    pin_states[pin] = val;
}

int digitalRead(uint8_t pin) {
    return pin_states.count(pin) ? pin_states[pin] : LOW;
}

void delayMicroseconds(unsigned int us) {
    // In a real testing scenario, this could be a no-op,
    // or it could interact with a time-mocking library.
}

void delay(unsigned long ms) {
    // No-op for now
}

unsigned long millis() {
    // Return a fixed value for now
    return 1000;
}

unsigned long micros() {
    // Return a fixed value for now
    return 1000000;
}

uint8_t digitalPinToInterrupt(uint8_t pin) {
    return pin;
}

void attachInterrupt(uint8_t interrupt, void (*isr)(), int mode) {
    // The interrupt number is the same as the pin number in our mock
    isr_map[interrupt] = isr;
    isr_mode_map[interrupt] = mode;
}

void MockSerial::print(const char* s) {
    std::cout << s;
}

void MockSerial::println(const char* s) {
    std::cout << s << std::endl;
}

void MockSerial::print(uint8_t n) {
    std::cout << n;
}

void MockSerial::println(uint8_t n) {
    std::cout << n << std::endl;
}

MockSerial Serial;

void mock_hal_reset() {
    pin_modes.clear();
    pin_states.clear();
    digitalWrite_calls.clear();
    isr_map.clear();
    isr_mode_map.clear();
}
