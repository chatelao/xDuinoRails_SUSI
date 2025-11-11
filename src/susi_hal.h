#ifndef SUSI_HAL_H
#define SUSI_HAL_H

#include <Arduino.h>
#include "susi_response.h"

class SusiHAL {
public:
    SusiHAL(uint8_t clock_pin, uint8_t data_pin);
    virtual ~SusiHAL() = default;
    virtual void begin();
    virtual void set_clock_high();
    virtual void set_clock_low();
    virtual void generate_clock_pulse();
    virtual void set_data_high();
    virtual void set_data_low();
    virtual bool read_data();
    virtual bool read_bit();
    virtual SusiMasterResult waitForAck();
    virtual void sendAckPulse();
    virtual void sendByte(uint8_t byte);

    uint8_t get_clock_pin() const { return _clock_pin; }

private:
    uint8_t _clock_pin;
    uint8_t _data_pin;
};

#endif // SUSI_HAL_H
