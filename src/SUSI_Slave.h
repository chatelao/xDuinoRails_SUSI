#ifndef SUSI_SLAVE_H
#define SUSI_SLAVE_H

#include <Arduino.h>
#include "SUSI_Packet.h"

class SUSI_Slave {
public:
    friend void SUSI_Slave_ISR();
    friend class EndToEndTest; // Grant access to the test fixture
    friend class SUSISlaveTest; // Grant access to the unit test fixture

    // [S1] HAL for the slave's clock and data pins.
    SUSI_Slave(uint8_t clockPin, uint8_t dataPin, uint8_t address);

    void begin();
    void loop();

    // [S2] Interrupt-driven routine
    void handleClockInterrupt();

private:
    static SUSI_Slave* instance;

    uint8_t _clockPin;
    uint8_t _dataPin;
    uint8_t _address;

    // [S4] Bit assembly state variables
    volatile uint8_t _bitCount;
    volatile uint32_t _dataBuffer;
    volatile bool _packetReady;

#ifdef TESTING
public:
    SUSI_Packet _last_received_packet;
    bool getPacketReady() const { return _packetReady; }
    uint32_t getDataBuffer() const { return _dataBuffer; }
#endif
};

#endif // SUSI_SLAVE_H
