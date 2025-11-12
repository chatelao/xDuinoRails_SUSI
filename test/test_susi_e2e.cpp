#include "gtest/gtest.h"
#include "susi_master.h"
#include "susi_slave.h"
#include "mock_hal.h"
#include "mock_susi_hal.h"
#include "susi_commands.h"

// Test fixture for End-to-End tests
class LegacySusiE2ETest : public ::testing::Test {
protected:
    const uint8_t CLOCK_PIN = 2;
    const uint8_t DATA_PIN = 3;
    const uint8_t SLAVE_ADDRESS = 1;

    MockSusiHAL hal;
    SUSI_Master master;
    SUSI_Master_API api;
    SUSI_Slave slave;

    LegacySusiE2ETest() : master(hal), api(master), slave(hal) {}

    void SetUp() override {
        mock_hal_reset();
        _susi_slave_instance = &slave;
        api.begin();
        slave.begin(SLAVE_ADDRESS);
        // Set initial clock state to HIGH
        digitalWrite(CLOCK_PIN, HIGH);
    }
};

TEST_F(LegacySusiE2ETest, setFunction) {
    hal.ack_result = SUCCESS;
    hal.onSendPacket = [&](const SUSI_Packet& p, bool expectAck) {
        slave._test_receive_packet(p);
    };
    hal.afterSendPacket = [&]() {
        EXPECT_TRUE(slave.available());
        slave.read();
        EXPECT_TRUE(slave.getFunction(5));
    };

    EXPECT_EQ(api.setFunction(SLAVE_ADDRESS, 5, true), SUCCESS);
}

TEST_F(LegacySusiE2ETest, setSpeed) {
    hal.ack_result = SUCCESS;
    hal.onSendPacket = [&](const SUSI_Packet& p, bool expectAck) {
        slave._test_receive_packet(p);
    };
    hal.afterSendPacket = [&]() {
        EXPECT_TRUE(slave.available());
        slave.read();
        EXPECT_EQ(slave.getSpeed(), 100);
        EXPECT_FALSE(slave.getDirection());
    };

    EXPECT_EQ(api.setSpeed(SLAVE_ADDRESS, 100, false), SUCCESS);
}

TEST_F(LegacySusiE2ETest, writeCV) {
    hal.ack_result = SUCCESS;
    hal.onSendPacket = [&](const SUSI_Packet& p, bool expectAck) {
        slave._test_receive_packet(p);
    };
    hal.afterSendPacket = [&]() {
        EXPECT_TRUE(slave.available());
        slave.read();
    };

    EXPECT_EQ(api.writeCV(SLAVE_ADDRESS, 1, 255), SUCCESS);
    EXPECT_EQ(slave.readCV(1), 255);
}

TEST_F(LegacySusiE2ETest, readCV) {
    hal.ack_result = SUCCESS;
    hal.onSendPacket = [&](const SUSI_Packet& p, bool expectAck) {
        slave._test_receive_packet(p);
    };
    hal.afterSendPacket = [&]() {
        EXPECT_TRUE(slave.available());
        slave.read();
    };

    api.writeCV(SLAVE_ADDRESS, 1, 255);

    hal.read_bytes.push(255);
    uint8_t value;
    EXPECT_EQ(api.readCV(SLAVE_ADDRESS, 1, value), SUCCESS);
    EXPECT_EQ(value, 255);
}

namespace {
    bool _callback_fired_e2e = false;
    uint8_t _callback_address_e2e = 0;
    uint8_t _callback_data_e2e[4] = {0};

    void bidi_callback_e2e(uint8_t address, uint8_t* data) {
        _callback_fired_e2e = true;
        _callback_address_e2e = address;
        for (int i = 0; i < 4; i++) {
            _callback_data_e2e[i] = data[i];
        }
    }
}

TEST_F(LegacySusiE2ETest, Handshake) {
    hal.onSendPacket = [&](const SUSI_Packet& p, bool expectAck) {
        if (p.command == SUSI_CMD_BIDI_HOST_CALL && (p.data & 0x04) != 0) { // Handshake call
            if ((p.data & 0x03) == SLAVE_ADDRESS) {
                hal.ack_result = SUCCESS;
                hal.read_bytes.push(SUSI_MSG_BIDI_IDLE);
                hal.read_bytes.push(0x00);
                hal.read_bytes.push(SUSI_MSG_BIDI_IDLE);
                hal.read_bytes.push(0x00);
            } else {
                hal.ack_result = TIMEOUT;
            }
        } else {
            hal.ack_result = SUCCESS;
        }

        slave._test_receive_packet(p);
        if(slave.available()) {
            slave.read();
        }
    };

    EXPECT_EQ(api.performHandshake(), SUCCESS);

    // After the handshake, the polling should work.
    _callback_fired_e2e = false;
    api.onBidiResponse(bidi_callback_e2e);

    hal.onSendPacket = [&](const SUSI_Packet& p, bool expectAck) {
        std::cout << "Polling slave " << (int)p.data << std::endl;
        hal.ack_result = SUCCESS;
        hal.read_bytes.push(0xDE);
        hal.read_bytes.push(0xAD);
        hal.read_bytes.push(0xBE);
        hal.read_bytes.push(0xEF);
    };

    while(!hal.read_bytes.empty()) {
        hal.read_bytes.pop();
    }

    api.pollSlaves();

    EXPECT_TRUE(_callback_fired_e2e);
    EXPECT_EQ(_callback_address_e2e, SLAVE_ADDRESS);
    EXPECT_EQ(_callback_data_e2e[0], 0xDE);
}
