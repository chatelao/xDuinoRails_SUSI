#ifndef SUSI_MASTER_H
#define SUSI_MASTER_H

#include <Arduino.h>

// [M3] Data structure to represent a SUSI packet
struct SUSI_Packet {
    uint8_t address;
    uint8_t command;
    uint8_t data;
};

class SUSI_Master {
public:
    // [M1] Hardware Abstraction Layer (HAL) for pin control
    SUSI_Master(uint8_t clockPin, uint8_t dataPin);

    void begin();

    // [M5] Helper functions to encode standard SUSI commands
    void setFunction(uint8_t address, uint8_t function, bool on);
    void writeCV(uint8_t address, uint16_t cv, uint8_t value);

    // [M4] Low-level function to transmit a single SUSI packet
    void sendPacket(const SUSI_Packet& packet);


private:
    uint8_t _clockPin;
    uint8_t _dataPin;

    // [M2] Precise microsecond-level timing functions
    void clockPulse();
    void delay_us(unsigned long us);

    void sendByte(uint8_t byte);

    void setClock(bool high);
    void setData(bool high);
    bool readData();
};

#endif // SUSI_MASTER_H
