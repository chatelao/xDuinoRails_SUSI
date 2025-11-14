#include "susi_slave.h"
#include "susi_commands.h"
#include "susi_crc.h"

SUSI_Slave* _susi_slave_instance = nullptr;

SUSI_Slave::SUSI_Slave(SusiHAL& hal) : _hal(hal) {
    _packetReady = false;
    _bitCount = 0;
    _last_bit_time_us = 0;
    _buffer[0] = 0;
    _buffer[1] = 0;
    _buffer[2] = 0;
    _speed = 0;
    _forward = false;
    _functions = 0;
    _cv_bank = 0;
    _cv_bank_select = 0;
    _cv_count = 0;
    _cv_address = 0;
    _cv_read_mode = false;
    _bidirectional_mode = false;
    _bidi_data_available = false;
    _status_bits = 0;
    _function_callback = nullptr;
    _manufacturer_id = 0;
    _hardware_id = 0;
    _version_number = 0;
    for (int i = 0; i < 4; i++) {
        _bidi_response_buffer[i] = 0;
    }
}

void SUSI_Slave::begin(uint8_t address) {
    _address = address;
    _hal.begin();
    attachInterrupt(digitalPinToInterrupt(_hal.get_clock_pin()), onClockFall, FALLING);
}

void SUSI_Slave::enableBidirectionalMode() {
    _bidirectional_mode = true;
}

void SUSI_Slave::onFunctionChange(FunctionCallback callback) {
    _function_callback = callback;
}

void SUSI_Slave::queueBidirectionalData(const uint8_t* data) {
    if (data != nullptr) {
        for (int i = 0; i < 4; i++) {
            _bidi_response_buffer[i] = data[i];
        }
        _bidi_data_available = true;
    }
}

void SUSI_Slave::_send_bidi_response(uint8_t header1, uint8_t data1, uint8_t header2, uint8_t data2) {
    _hal.sendByte(header1);
    _hal.sendByte(data1);
    _hal.sendByte(header2);
    _hal.sendByte(data2);
}

bool SUSI_Slave::available() {
    return _packetReady;
}

void SUSI_Slave::setManufacturerID(uint16_t id) {
    _manufacturer_id = id;
}

void SUSI_Slave::setHardwareID(uint16_t id) {
    _hardware_id = id;
}

void SUSI_Slave::setVersionNumber(uint16_t version) {
    _version_number = version;
}

void SUSI_Slave::setStatusBits(uint8_t bits) {
    _status_bits |= bits;
}

void SUSI_Slave::clearStatusBits(uint8_t bits) {
    _status_bits &= ~bits;
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

        switch (packet.command) {
            case SUSI_CMD_READ_CV_BANK_0:
            case SUSI_CMD_READ_CV_BANK_1:
            case SUSI_CMD_READ_CV_BANK_2:
                {
                    _hal.sendAck();
                    uint8_t bank = packet.command - SUSI_CMD_READ_CV_BANK_0;
                    uint8_t bank_data[40];
                    getCVBank(bank, bank_data);

                    for (int i = 0; i < 40; i++) {
                        _hal.sendByte(bank_data[i]);
                    }

                    uint16_t crc = crc16_ccitt(bank_data, 40);
                    _hal.sendByte((crc >> 8) & 0xFF);
                    _hal.sendByte(crc & 0xFF);
                }
                break;
            case SUSI_CMD_SET_SPEED:
                _speed = packet.data & 0x7F;
                _forward = (packet.data & 0x80) != 0;
                _hal.sendAck();
                break;
            case SUSI_CMD_SET_FUNCTION:
                {
                    uint8_t function = packet.data & 0x1F;
                    bool on = (packet.data & 0x80) != 0;
                    if (on) {
                        _functions |= (1L << function);
                    } else {
                        _functions &= ~(1L << function);
                    }
                    if (_function_callback != nullptr) {
                        _function_callback(function, on);
                    }
                    _hal.sendAck();
                }
                break;
            case SUSI_CMD_WRITE_CV:
                _cv_bank = packet.data;
                _cv_read_mode = false;
                _hal.sendAck();
                break;
            case SUSI_CMD_READ_CV:
                _cv_bank = packet.data;
                _cv_read_mode = true;
                _hal.sendAck();
                break;
            case SUSI_CMD_BIDI_HOST_CALL:
                {
                    uint8_t module_number = packet.data & 0x03;
                    bool forced_response = (packet.data & 0x04) != 0;

                    if (forced_response && module_number == _address) {
                        _bidirectional_mode = true; // Enable BiDi on handshake
                        _hal.sendAck();
                        // Respond with two IDLE messages as per RCN-601
                        _send_bidi_response(SUSI_MSG_BIDI_IDLE, 0, SUSI_MSG_BIDI_IDLE, 0);
                    } else if (_bidirectional_mode && module_number == _address) {
                        // This is a regular poll, not a handshake
                        _hal.sendAck();

                        if (_bidi_data_available) {
                            _send_bidi_response(
                                _bidi_response_buffer[0],
                                _bidi_response_buffer[1],
                                _bidi_response_buffer[2],
                                _bidi_response_buffer[3]
                            );
                            _bidi_data_available = false; // Reset after sending
                        } else if (_status_bits != 0) {
                            _send_bidi_response(SUSI_MSG_BIDI_STATE_RESPONSE, _status_bits, SUSI_MSG_BIDI_STATE_RESPONSE, _status_bits);
                            _status_bits = 0; // Reset after sending
                        }
                        else {
                            // Send IDLE message if no data is queued
                            _send_bidi_response(SUSI_MSG_BIDI_IDLE, 0, SUSI_MSG_BIDI_IDLE, 0);
                        }
                    }
                }
                break;
            default:
                if (_cv_bank != 0) {
                    _cv_address = ((_cv_bank) << 8) | packet.command;
                    if (_cv_read_mode) {
                        uint8_t value1 = readCV(_cv_address);
                        uint8_t value2 = readCV(_cv_address + 1);
                        _send_bidi_response(SUSI_MSG_BIDI_CV_RESPONSE, value1, SUSI_MSG_BIDI_CV_RESPONSE, value2);
                    } else {
                        if (_cv_address == CV_SUSI_CV_BANKING) {
                            _cv_bank_select = packet.data;
                        } else {
                            for (int i = 0; i < _cv_count; i++) {
                                if (_cv_keys[i] == _cv_address) {
                                    _cv_values[i] = packet.data;
                                    _cv_bank = 0;
                                    return packet;
                                }
                            }
                            if (_cv_count < MAX_CVS) {
                                _cv_keys[_cv_count] = _cv_address;
                                _cv_values[_cv_count] = packet.data;
                                _cv_count++;
                            }
                        }
                    }
                    _cv_bank = 0;
                }
        }
    }
    return packet;
}

uint8_t SUSI_Slave::readCV(uint16_t cv) {
    switch (cv) {
        case CV_SUSI_MODULE_NUM:
            return _address;
        case CV_MANUFACTURER_ID:
            if (_cv_bank_select == 0) return (_manufacturer_id >> 8) & 0xFF;
            if (_cv_bank_select == 1) return (_hardware_id >> 8) & 0xFF;
            return 0;
        case CV_MANUFACTURER_ID + 1:
            if (_cv_bank_select == 0) return _manufacturer_id & 0xFF;
            if (_cv_bank_select == 1) return _hardware_id & 0xFF;
            return 0;
        case CV_VERSION_NUM:
            return (_version_number >> 8) & 0xFF;
        case CV_VERSION_NUM + 1:
            return _version_number & 0xFF;
        case CV_STATUS_BITS:
            return _status_bits;
        case CV_SUSI_CV_BANKING:
            return _cv_bank_select;
        default:
            for (int i = 0; i < _cv_count; i++) {
                if (_cv_keys[i] == cv) {
                    return _cv_values[i];
                }
            }
            return 0;
    }
}

void SUSI_Slave::getCVBank(uint8_t bank, uint8_t* data) {
    for (int i = 0; i < 40; i++) {
        data[i] = 0;
    }

    uint16_t start_cv = bank * 40;
    for (int i = 0; i < _cv_count; i++) {
        if (_cv_keys[i] >= start_cv && _cv_keys[i] < start_cv + 40) {
            data[_cv_keys[i] - start_cv] = _cv_values[i];
        }
    }
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
            if (_buffer[0] == _address || _buffer[0] == 0) {
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

#ifdef TESTING
void SUSI_Slave::_test_receive_packet(const SUSI_Packet& packet) {
    if (packet.address == _address || packet.address == 0) {
        _buffer[0] = packet.address;
        _buffer[1] = packet.command;
        _buffer[2] = packet.data;
        _packetReady = true;
    }
}
#endif