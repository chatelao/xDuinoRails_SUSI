#include "SUSI_Master.h"

// SUSI Command Constants
const uint8_t SUSI_CMD_SET_FUNCTION = 0x01;
const uint8_t SUSI_CMD_WRITE_CV = 0x02;

// Timing constants from the SUSI specification (in microseconds)
const unsigned long T_CLK_HALF = 50; // Half period of the clock signal

// [M1] Hardware Abstraction Layer (HAL) for pin control
SUSI_Master::SUSI_Master(uint8_t clockPin, uint8_t dataPin) {
    _clockPin = clockPin;
    _dataPin = dataPin;
}

void SUSI_Master::begin() {
    pinMode(_clockPin, OUTPUT);
    pinMode(_dataPin, OUTPUT);
    digitalWrite(_clockPin, HIGH); // Clock idle is HIGH
    digitalWrite(_dataPin, HIGH);  // Data idle is HIGH
}

void SUSI_Master::setClock(bool high) {
    digitalWrite(_clockPin, high ? HIGH : LOW);
}

void SUSI_Master::setData(bool high) {
    digitalWrite(_dataPin, high ? HIGH : LOW);
}

bool SUSI_Master::readData() {
    // To be implemented for bidirectional communication
    return digitalRead(_dataPin);
}

// [M2] Precise microsecond-level timing functions
void SUSI_Master::clockPulse() {
    setClock(LOW);
    delay_us(T_CLK_HALF);
    setClock(HIGH);
    delay_us(T_CLK_HALF);
}

void SUSI_Master::delay_us(unsigned long us) {
    delayMicroseconds(us);
}

// [M4] Low-level function to transmit a single SUSI packet
void SUSI_Master::sendPacket(const SUSI_Packet& packet) {
    // Start bit
    setData(LOW);
    clockPulse();

    // Send the 3 bytes of the packet
    sendByte(packet.address);
    sendByte(packet.command);
    sendByte(packet.data);

    // Stop bit
    setData(HIGH);
    clockPulse();
}

void SUSI_Master::sendByte(uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        setData((byte >> i) & 0x01);
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
