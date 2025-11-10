#ifndef SUSI_SLAVE_H
#define SUSI_SLAVE_H

#include <Arduino.h>
#include "susi_hal.h"
#include "susi_packet.h"

class SUSI_Slave {
public:
    SUSI_Slave(uint8_t clockPin, uint8_t dataPin);
    void begin(uint8_t address);
    bool available();
    SUSI_Packet read();

public:
    uint8_t getSpeed() const { return _speed; }
    bool getDirection() const { return _forward; }

private:
    static void onClockFall();
    void handleClockFall();

    SusiHAL _hal;
    uint8_t _address;
    volatile bool _packetReady;
    volatile uint8_t _buffer[3];
    volatile uint8_t _bitCount;
    uint8_t _speed;
    bool _forward;
};

#endif // SUSI_SLAVE_H
