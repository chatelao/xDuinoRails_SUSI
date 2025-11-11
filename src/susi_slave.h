#ifndef SUSI_SLAVE_H
#define SUSI_SLAVE_H

#include <Arduino.h>
#include "susi_hal.h"
#include "susi_packet.h"

const uint8_t MAX_CVS = 32;

typedef void (*FunctionCallback)(uint8_t, bool);

class SUSI_Slave {
public:
    SUSI_Slave(uint8_t clockPin, uint8_t dataPin, uint32_t unique_id);
    void begin(uint8_t address);
    bool available();
    SUSI_Packet read();

    void onFunctionChange(FunctionCallback callback);

public:
    uint8_t getSpeed() const { return _speed; }
    bool getDirection() const { return _forward; }
    bool getFunction(uint8_t function) const { return (_functions >> function) & 1; }
    uint8_t readCV(uint16_t cv);

#ifdef TESTING
    bool isBidirectionalModeEnabled() const { return _bidirectional_mode; }
#endif

private:
    static void onClockFall();
    void handleClockFall();

    SusiHAL _hal;
    uint8_t _address;
    volatile bool _packetReady;
    volatile uint8_t _buffer[3];
    volatile uint8_t _bitCount;
    volatile unsigned long _last_bit_time_us;
    uint8_t _speed;
    bool _forward;
    uint32_t _functions;
    uint8_t _cv_bank;
    uint16_t _cv_address;
    bool _cv_read_mode;
    uint16_t _cv_keys[MAX_CVS];
    uint8_t _cv_values[MAX_CVS];
    uint8_t _cv_count;
    bool _bidirectional_mode;
    uint32_t _unique_id;
    FunctionCallback _function_callback;
};

#endif // SUSI_SLAVE_H
