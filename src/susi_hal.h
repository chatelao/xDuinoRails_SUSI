#ifndef SUSI_HAL_H
#define SUSI_HAL_H

#include <Arduino.h>
#include "susi_defines.h"

class SusiHAL {
public:
    SusiHAL(uint8_t clock_pin, uint8_t data_pin);
    void begin();
    void set_clock_high();
    void set_clock_low();
    void set_data_high();
    void set_data_low();
    bool read_data();
    void wait_t_clk();
    void wait_t_setup();

private:
    uint8_t _clock_pin;
    uint8_t _data_pin;
};

#endif // SUSI_HAL_H
