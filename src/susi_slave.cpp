#include "susi_slave.h"
#include "susi_commands.h"

static SUSI_Slave*_susi_slave_instance = nullptr;

SUSI_Slave::SUSI_Slave(uint8_t clockPin, uint8_t dataPin) : _hal(clockPin, dataPin) {
    _packetReady = false;
    _bitCount = 0;
    _last_bit_time_us = 0;
    _buffer[0] = 0;
    _buffer[1] = 0;
    _buffer[2] = 0;
    _susi_slave_instance = this;
    _speed = 0;
    _forward = false;
}

void SUSI_Slave::begin(uint8_t address) {
    _address = address;
    _hal.begin();
    attachInterrupt(digitalPinToInterrupt(_hal.get_clock_pin()), onClockFall, FALLING);
}

bool SUSI_Slave::available() {
    return _packetReady;
}

SUSI_Packet SUSI_Slave::read() {
    SUSI_Packet packet;
    if (_packetReady) {
        noInterrupts();
        packet.address = _buffer[0];
        packet.command = _buffer[1];
        packet.data = _buffer[2];
        _buffer[0] = 0;
        _buffer[1] = 0;
        _buffer[2] = 0;
        _packetReady = false;
        interrupts();

        if (packet.command == SUSI_CMD_SET_SPEED) {
            _speed = packet.data & 0x7F;
            _forward = (packet.data & 0x80) != 0;
        }
    }
    return packet;
}

void SUSI_Slave::onClockFall() {
    if (_susi_slave_instance) {
        _susi_slave_instance->handleClockFall();
    }
}

void SUSI_Slave::handleClockFall() {
    if (_packetReady) {
        return;
    }

    unsigned long current_time_us = micros();

    // RCN600-S1: 8ms timeout to reset buffer
    if (_bitCount > 0 && (current_time_us - _last_bit_time_us > 8000)) {
        _bitCount = 0;
        _buffer[0] = 0;
        _buffer[1] = 0;
        _buffer[2] = 0;
    }

    _last_bit_time_us = current_time_us;

    bool data = _hal.read_data();

    // The first bit must be a LOW start bit
    if (_bitCount == 0) {
        if (!data) { // Start bit is LOW
            _bitCount++;
        }
        return;
    }

    // The 25th bit must be a HIGH stop bit
    if (_bitCount == 25) {
        if (data) { // Stop bit is HIGH
            if (_buffer[0] == _address) {
                _packetReady = true;
            }
        }
        // Reset for next packet
        _bitCount = 0;
        return;
    }

    // Read the 24 data bits (3 bytes)
    uint8_t byteIndex = (_bitCount - 1) / 8;
    uint8_t bitIndex = (_bitCount - 1) % 8;

    if (byteIndex < 3) {
        if (data) {
            _buffer[byteIndex] |= (1 << bitIndex);
        } else {
            _buffer[byteIndex] &= ~(1 << bitIndex);
        }
    }

    _bitCount++;
}
