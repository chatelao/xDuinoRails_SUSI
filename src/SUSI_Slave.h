#ifndef SUSI_SLAVE_H
#define SUSI_SLAVE_H

#include <Arduino.h>

// Re-defining SUSI_Packet here for the slave.
// In a real library, this would be in a shared header.
struct SUSI_Packet {
    uint8_t address;
    uint8_t command;
    uint8_t data;
};

class SUSI_Slave {
public:
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
};

#endif // SUSI_SLAVE_H
