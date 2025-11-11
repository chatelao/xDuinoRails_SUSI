#include "susi_master.h"
#include "susi_commands.h"

// Timing constants from the SUSI specification
const unsigned long SUSI_INTER_BYTE_TIMEOUT_MS = 7;
const unsigned long SUSI_SYNC_GAP_MS = 9;
const uint8_t SUSI_PACKETS_PER_SYNC = 20;

// SUSI_Master implementation
SUSI_Master::SUSI_Master(SusiHAL& hal) : _hal(hal) {
    _last_packet_time_ms = 0;
    _packets_since_sync = 0;
}

void SUSI_Master::begin() {
    _hal.begin();
    _hal.set_clock_high(); // Clock idle is HIGH
    _hal.set_data_high();  // Data idle is HIGH
}

#ifdef TESTING
#include "mock_susi_hal.h"
#endif

SusiMasterResult SUSI_Master::sendPacket(const SUSI_Packet& packet, bool expectAck) {
#ifdef TESTING
    MockSusiHAL* mock_hal = dynamic_cast<MockSusiHAL*>(&_hal);
    if (mock_hal) {
        if (mock_hal->onSendPacket) {
            mock_hal->onSendPacket(packet, expectAck);
            if (mock_hal->afterSendPacket) {
                mock_hal->afterSendPacket();
            }
            return mock_hal->ack_result;
        }
    }
#endif
    unsigned long current_time_ms = millis();
    if (current_time_ms - _last_packet_time_ms > SUSI_INTER_BYTE_TIMEOUT_MS) {
        delay(SUSI_SYNC_GAP_MS);
    }

    if (_packets_since_sync >= SUSI_PACKETS_PER_SYNC) {
        delay(SUSI_SYNC_GAP_MS);
        _packets_since_sync = 0;
    }

    _hal.set_data_low();
    _hal.generate_clock_pulse();

    sendByte(packet.address);
    sendByte(packet.command);
    sendByte(packet.data);

    _hal.set_data_high();
    _hal.generate_clock_pulse();

    _last_packet_time_ms = millis();
    _packets_since_sync++;

    if (expectAck) {
        return _hal.waitForAck();
    }

    return SUCCESS;
}

void SUSI_Master::sendByte(uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        if ((byte >> i) & 0x01) {
            _hal.set_data_high();
        } else {
            _hal.set_data_low();
        }
        _hal.generate_clock_pulse();
    }
}

uint8_t SUSI_Master::readByteAfterRequest() {
    uint8_t value = 0;
    for (int i = 0; i < 8; i++) {
        if (_hal.read_bit()) {
            value |= (1 << i);
        }
    }

    _hal.generate_clock_pulse();

    return value;
}

// SUSI_Master_API implementation
SUSI_Master_API::SUSI_Master_API(SUSI_Master& master) : _master(master) {
    _slave_count = 0;
}

void SUSI_Master_API::begin() {
    _master.begin();
}

void SUSI_Master_API::reset() {
    _slave_count = 0;
}

bool SUSI_Master_API::getFunction(uint8_t address, uint8_t function) {
    for (int i = 0; i < _slave_count; i++) {
        if (_slave_states[i].address == address) {
            return (_slave_states[i].functions >> function) & 1;
        }
    }
    return false;
}

bool SUSI_Master_API::setFunction(uint8_t address, uint8_t function, bool on) {
    SUSI_Packet packet;
    packet.address = address;
    packet.command = SUSI_CMD_SET_FUNCTION;
    packet.data = (function & 0x1F) | (on ? 0x80 : 0x00);
    SusiMasterResult result = _master.sendPacket(packet, true);

    if (result != SUCCESS) {
        return false;
    }

    // Find existing slave state
    SUSI_Slave_State* state = nullptr;
    for (int i = 0; i < _slave_count; i++) {
        if (_slave_states[i].address == address) {
            state = &_slave_states[i];
            break;
        }
    }

    // Or create a new one
    if (state == nullptr) {
        if (_slave_count < MAX_SLAVES) {
            state = &_slave_states[_slave_count];
            state->address = address;
            state->functions = 0; // Initialize functions
            _slave_count++;
        } else {
            // Cannot store state for new slave, but command was successful
            return true;
        }
    }

    // Update the state
    if (on) {
        state->functions |= (1L << function);
    } else {
        state->functions &= ~(1L << function);
    }

    return true;
}

bool SUSI_Master_API::setSpeed(uint8_t address, uint8_t speed, bool forward) {
    SUSI_Packet packet;
    packet.address = address;
    packet.command = SUSI_CMD_SET_SPEED;
    packet.data = (speed & 0x7F) | (forward ? 0x80 : 0x00);
    return _master.sendPacket(packet, true) == SUCCESS;
}

bool SUSI_Master_API::writeCV(uint8_t address, uint16_t cv, uint8_t value) {
    uint16_t cv_addr = cv - 1;
    SUSI_Packet packet1;
    packet1.address = address;
    packet1.command = SUSI_CMD_WRITE_CV;
    packet1.data = (cv_addr >> 8) & 0x03;
    if (_master.sendPacket(packet1, true) != SUCCESS) {
        return false;
    }

    SUSI_Packet packet2;
    packet2.address = address;
    packet2.command = cv_addr & 0xFF;
    packet2.data = value;
    return _master.sendPacket(packet2, true) == SUCCESS;
}

uint8_t SUSI_Master_API::readCV(uint8_t address, uint16_t cv) {
    uint16_t cv_addr = cv - 1;
    SUSI_Packet packet1;
    packet1.address = address;
    packet1.command = SUSI_CMD_READ_CV;
    packet1.data = (cv_addr >> 8) & 0x03;
    if (_master.sendPacket(packet1, true) != SUCCESS) {
        return 0;
    }

    SUSI_Packet packet2;
    packet2.address = address;
    packet2.command = cv_addr & 0xFF;
    packet2.data = 0;
    if (_master.sendPacket(packet2, true) != SUCCESS) {
        return 0;
    }

    return _master.readByteAfterRequest();
}
