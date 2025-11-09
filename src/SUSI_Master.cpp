#include "SUSI_Master.h"

// SUSI Command Constants
const uint8_t SUSI_CMD_SET_FUNCTION = 0x01;
const uint8_t SUSI_CMD_WRITE_CV = 0x02;
const uint8_t SUSI_CMD_READ_CV = 0x03;

// [M1] Hardware Abstraction Layer (HAL) for pin control
SUSI_Master::SUSI_Master(uint8_t clockPin, uint8_t dataPin) : _hal(clockPin, dataPin) {
}

void SUSI_Master::begin() {
    _hal.begin();
    _hal.set_clock_high(); // Clock idle is HIGH
    _hal.set_data_high();  // Data idle is HIGH
}

// [M2] Precise microsecond-level timing functions
void SUSI_Master::clockPulse() {
    _hal.set_clock_low();
    delay_us(SUSI_CLOCK_HALF_PERIOD_US);
    _hal.set_clock_high();
    delay_us(SUSI_CLOCK_HALF_PERIOD_US);
}

void SUSI_Master::delay_us(unsigned long us) {
    delayMicroseconds(us);
}

// [M4] Low-level function to transmit a single SUSI packet
void SUSI_Master::sendPacket(const SUSI_Packet& packet) {
    // Start bit
    _hal.set_data_low();
    clockPulse();

    // Send the 3 bytes of the packet
    sendByte(packet.address);
    sendByte(packet.command);
    sendByte(packet.data);

    // Stop bit
    _hal.set_data_high();
    clockPulse();
}

void SUSI_Master::sendByte(uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        if ((byte >> i) & 0x01) {
            _hal.set_data_high();
        } else {
            _hal.set_data_low();
        }
        clockPulse();
    }
}

// [M5] Helper functions to encode standard SUSI commands
void SUSI_Master::setFunction(uint8_t address, uint8_t function, bool on) {
    SUSI_Packet packet;
    packet.address = address;
    packet.command = SUSI_CMD_SET_FUNCTION;
    packet.data = (function & 0x1F) | (on ? 0x80 : 0x00);
    sendPacket(packet);
}

void SUSI_Master::writeCV(uint8_t address, uint16_t cv, uint8_t value) {
    // SUSI requires two packets to write a CV
    SUSI_Packet packet1;
    packet1.address = address;
    packet1.command = SUSI_CMD_WRITE_CV;
    packet1.data = (cv >> 8) & 0x03; // High 2 bits of CV
    sendPacket(packet1);

    SUSI_Packet packet2;
    packet2.address = address;
    packet2.command = cv & 0xFF; // Low 8 bits of CV
    packet2.data = value;
    sendPacket(packet2);
}

bool SUSI_Master::readData() {
    return _hal.read_data();
}

uint8_t SUSI_Master::readCV(uint8_t address, uint16_t cv) {
    // SUSI requires two packets to read a CV
    SUSI_Packet packet1;
    packet1.address = address;
    packet1.command = SUSI_CMD_READ_CV;
    packet1.data = (cv >> 8) & 0x03; // High 2 bits of CV
    sendPacket(packet1);

    SUSI_Packet packet2;
    packet2.address = address;
    packet2.command = cv & 0xFF; // Low 8 bits of CV
    packet2.data = 0; // Data is ignored for read requests
    sendPacket(packet2);

    // After sending the request, we need to read the 8 bits of the CV value
    uint8_t value = 0;
    for (int i = 0; i < 8; i++) {
        // The slave will present a bit on the data line, and we clock it in
        _hal.set_clock_low();
        delay_us(SUSI_CLOCK_HALF_PERIOD_US);
        if (readData()) {
            value |= (1 << i);
        }
        _hal.set_clock_high();
        delay_us(SUSI_CLOCK_HALF_PERIOD_US);
    }

    // A final clock pulse to signal the end of the read
    clockPulse();

    return value;
}
