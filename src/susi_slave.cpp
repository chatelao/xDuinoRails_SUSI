#include "susi_slave.h"
#include "susi_commands.h"

static SUSI_Slave*_susi_slave_instance = nullptr;

SUSI_Slave::SUSI_Slave(uint8_t clockPin, uint8_t dataPin) : _hal(clockPin, dataPin) {
    _packetReady = false;
    _bitCount = 0;
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
    _hal.set_data_low(); // Data line is low by default
    attachInterrupt(digitalPinToInterrupt(_hal.get_clock_pin()), onClockFall, FALLING);
}

bool SUSI_Slave::available() {
    return _packetReady;
}

SUSI_Packet SUSI_Slave::read() {
    SUSI_Packet packet;
    packet.address = _buffer[0];
    packet.command = _buffer[1];
    packet.data = _buffer[2];

    if (packet.command == SUSI_CMD_SET_SPEED) {
        _speed = packet.data & 0x7F;
        _forward = (packet.data & 0x80) != 0;
    }

    _packetReady = false;
    _buffer[0] = 0;
    _buffer[1] = 0;
    _buffer[2] = 0;
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

    // Ignore start bit
    if (_bitCount == 0) {
        _bitCount++;
        return;
    }

    // Ignore stop bit
    if (_bitCount == 25) {
        if (_buffer[0] == _address) {
            _packetReady = true;
        }
        _bitCount = 0;
        return;
    }

    uint8_t byteIndex = (_bitCount - 1) / 8;
    uint8_t bitIndex = (_bitCount - 1) % 8;

    if (byteIndex < 3) {
        if (_hal.read_data()) {
            _buffer[byteIndex] |= (1 << bitIndex);
        } else {
            _buffer[byteIndex] &= ~(1 << bitIndex);
        }
    }

    _bitCount++;
}
