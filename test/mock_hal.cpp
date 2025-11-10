#include "mock_hal.h"
#include <iostream>
#include <map>

// Store pin states
std::map<uint8_t, uint8_t> pin_modes;
std::map<uint8_t, uint8_t> pin_states;
std::vector<Call> digitalWrite_calls;
std::map<uint8_t, ISR> isr_map;
std::map<uint8_t, int> isr_mode_map;
unsigned long mock_micros_time = 0;
unsigned long ack_pulse_start_time = 0;
unsigned long ack_pulse_duration = 0;
std::vector<int> digitalRead_return_sequence;

void noInterrupts() {
    // Not implemented for mock
}

void interrupts() {
    // Not implemented for mock
}

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
    if (!digitalRead_return_sequence.empty()) {
        int val = digitalRead_return_sequence.front();
        digitalRead_return_sequence.erase(digitalRead_return_sequence.begin());
        return val;
    }

    // Simulate the passage of time for polling loops
    mock_micros_time += 10; // Advance time by 10us on each read

    // Simulate an ACK pulse
    if (pin == 3 && ack_pulse_duration > 0) {
        if (mock_micros_time >= ack_pulse_start_time &&
            mock_micros_time < ack_pulse_start_time + ack_pulse_duration) {
            return LOW;
        }
    }
    return pin_states.count(pin) ? pin_states[pin] : HIGH; // Default to HIGH for ACK test
}

void delayMicroseconds(unsigned int us) {
    mock_micros_time += us;
}

void delay(unsigned long ms) {
    // No-op for now
}

unsigned long millis() {
    return mock_micros_time / 1000;
}

unsigned long micros() {
    return mock_micros_time;
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
    mock_micros_time = 0;
    ack_pulse_start_time = 0;
    ack_pulse_duration = 0;
    digitalRead_return_sequence.clear();
}

void mock_hal_advance_time(unsigned long ms) {
    mock_micros_time += ms * 1000;
}
