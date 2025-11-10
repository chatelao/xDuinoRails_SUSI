#include "SUSI_Slave.h"

#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif

// [S2] Interrupt-driven routine - static instance and ISR
SUSI_Slave* SUSI_Slave::instance = nullptr;

void IRAM_ATTR SUSI_Slave_ISR() {
    if (SUSI_Slave::instance) {
        SUSI_Slave::instance->handleClockInterrupt();
    }
}

// [S1] HAL for the slave's clock and data pins.
SUSI_Slave::SUSI_Slave(uint8_t clockPin, uint8_t dataPin, uint8_t address) {
    _clockPin = clockPin;
    _dataPin = dataPin;
    _address = address;
    instance = this;

    // [S4] Initialize bit assembly state variables
    _bitCount = 0;
    _dataBuffer = 0;
    _packetReady = false;
}

void SUSI_Slave::begin() {
    pinMode(_clockPin, INPUT); // Clock is an input for the slave
    pinMode(_dataPin, INPUT);  // Data is initially an input
    attachInterrupt(digitalPinToInterrupt(_clockPin), SUSI_Slave_ISR, FALLING);
}

void SUSI_Slave::handleClockInterrupt() {
    // [S3] Read the data bit from the data line
    bool dataBit = digitalRead(_dataPin);

    // [S4] Assemble the incoming bits into a complete SUSI packet
    if (_bitCount == 0) { // Start bit
        if (dataBit == LOW) {
            _dataBuffer = 0;
            _bitCount++;
        }
    } else if (_bitCount < 25) { // Data bits (3 bytes * 8 bits)
        if (dataBit) {
            _dataBuffer |= (1UL << (_bitCount - 1));
        }
        _bitCount++;
    } else { // Stop bit
        if (dataBit == HIGH) {
            _packetReady = true;
        }
        _bitCount = 0;
    }
}

void SUSI_Slave::loop() {
    if (_packetReady) {
        // [S5] Validate and parse the packet
        SUSI_Packet packet;
        packet.address = _dataBuffer & 0xFF;
        packet.command = (_dataBuffer >> 8) & 0xFF;
        packet.data = (_dataBuffer >> 16) & 0xFF;

        // [S6] Check if the packet's address matches
        if (packet.address == _address) {
#ifdef TESTING
            _last_received_packet = packet;
#endif
            // [S7] Packet is valid and for us, process it.
            // For now, just print it to serial.
            Serial.print("Received SUSI Packet: ");
            Serial.print("Addr: ");
            Serial.print(packet.address);
            Serial.print(", Cmd: ");
            Serial.print(packet.command);
            Serial.print(", Data: ");
            Serial.println(packet.data);
        }

        _packetReady = false; // Reset the flag
    }
}
