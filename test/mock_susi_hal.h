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
    std::function<void(uint8_t)> onSendByte;
    SusiMasterResult ack_result = SUCCESS;
    std::queue<bool> read_bits;

    void sendByte(uint8_t byte) override {
        if (onSendByte) {
            onSendByte(byte);
        }
        for (int i = 0; i < 8; i++) {
            read_bits.push((byte >> i) & 0x01);
        }
    }
    void begin() override {}
    void set_clock_high() override {}
    void set_clock_low() override {}
    void generate_clock_pulse() override {}
    void set_data_high() override {}
    void set_data_low() override {}
    bool read_data() override { return false; }
    bool read_bit() override {
        if (read_bits.empty()) {
            return false;
        }
        bool bit = read_bits.front();
        read_bits.pop();
        return bit;
    }
    SusiMasterResult waitForAck() override { return ack_result; }
    void sendAck() override {}
};

#endif // MOCK_SUSI_HAL_H
