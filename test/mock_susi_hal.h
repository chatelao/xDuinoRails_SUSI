#ifndef MOCK_SUSI_HAL_H
#define MOCK_SUSI_HAL_H

#include "susi_hal.h"
#include "susi_packet.h"
#include "susi_response.h"
#include <functional>

class MockSusiHAL : public SusiHAL {
public:
    MockSusiHAL() : SusiHAL(0, 0) {}

    std::function<void(const SUSI_Packet&, bool)> onSendPacket;
    std::function<void()> afterSendPacket;
    SusiMasterResult ack_result = SUCCESS;
    uint8_t last_sent_byte;

    void sendByte(uint8_t byte) override { last_sent_byte = byte; }
    void begin() override {}
    void set_clock_high() override {}
    void set_clock_low() override {}
    void generate_clock_pulse() override {}
    void set_data_high() override {}
    void set_data_low() override {}
    bool read_data() override { return false; }
    bool read_bit() override { return false; }
    SusiMasterResult waitForAck() override { return ack_result; }
    void sendAck() override {}
};

#endif // MOCK_SUSI_HAL_H
