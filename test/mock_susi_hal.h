#ifndef MOCK_SUSI_HAL_H
#define MOCK_SUSI_HAL_H

#include "susi_hal.h"
#include "susi_packet.h"
#include "susi_response.h"
#include <functional>
#include <queue>

class MockSusiHAL : public SusiHAL {
public:
    MockSusiHAL() : SusiHAL(0, 0) {}

    std::function<void(const SUSI_Packet&, bool)> onSendPacket;
    std::function<void()> afterSendPacket;
    SusiMasterResult ack_result = SUCCESS;
    uint8_t last_sent_byte;
    std::queue<uint8_t> read_bytes;
    int bit_read_count = 0;

    void sendByte(uint8_t byte) override { read_bytes.push(byte); }
    void begin() override {}
    void set_clock_high() override {}
    void set_clock_low() override {}
    void generate_clock_pulse() override {}
    void set_data_high() override {}
    void set_data_low() override {}
    bool read_data() override { return false; }
    bool read_bit() override {
        if (read_bytes.empty()) {
            return false;
        }

        uint8_t current_byte = read_bytes.front();
        bool bit = (current_byte >> bit_read_count) & 0x01;
        bit_read_count++;
        if (bit_read_count == 8) {
            bit_read_count = 0;
            read_bytes.pop();
        }
        return bit;
    }
    SusiMasterResult waitForAck() override { return ack_result; }
    void sendAck() override {}
};

#endif // MOCK_SUSI_HAL_H
