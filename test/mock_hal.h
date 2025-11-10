#ifndef MOCK_HAL_H
#define MOCK_HAL_H

#include <cstdint>

// Mock Arduino data types
using uint8_t = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;

// Mock Arduino constants
#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#define FALLING 2

#define IRAM_ATTR

// Mock Arduino functions
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);
void delayMicroseconds(unsigned int us);
void delay(unsigned long ms);
unsigned long millis();
unsigned long micros();
uint8_t digitalPinToInterrupt(uint8_t pin);
void attachInterrupt(uint8_t interrupt, void (*isr)(), int mode);

// Mock Serial object
class MockSerial {
public:
    void print(const char*);
    void print(uint8_t);
    void println(const char*);
    void println(uint8_t);
};

extern MockSerial Serial;

#include <map>

// Make pin state maps available to tests
extern std::map<uint8_t, uint8_t> pin_modes;
extern std::map<uint8_t, uint8_t> pin_states;

// --- Call Logging ---
#include <vector>
#include <tuple>

struct Call {
    uint8_t pin;
    uint8_t value;
};

extern std::vector<Call> digitalWrite_calls;

// --- ISR Simulation ---
typedef void (*ISR)();
extern std::map<uint8_t, ISR> isr_map;
extern std::map<uint8_t, int> isr_mode_map;

void mock_hal_reset();
void mock_hal_advance_time(unsigned long ms);

// --- Time mocking ---
extern unsigned long mock_micros_time;
extern unsigned long ack_pulse_start_time;
extern unsigned long ack_pulse_duration;

// --- Value sequencing ---
extern std::vector<int> digitalRead_return_sequence;


#endif // MOCK_HAL_H
