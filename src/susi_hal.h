#ifndef SUSI_HAL_H
#define SUSI_HAL_H

#include <Arduino.h>

class SusiHAL {
public:
    SusiHAL(uint8_t clock_pin, uint8_t data_pin);
    void begin();
    void set_clock_high();
    void set_clock_low();
    void generate_clock_pulse();
    void set_data_high();
    void set_data_low();
    bool read_data();
    bool read_bit();
    bool waitForAck();

    uint8_t get_clock_pin() const { return _clock_pin; }

private:
    uint8_t _clock_pin;
    uint8_t _data_pin;
};

#endif // SUSI_HAL_H
