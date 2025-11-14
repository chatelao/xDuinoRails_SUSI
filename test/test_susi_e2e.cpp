#include "gtest/gtest.h"
#include "susi_master.h"
#include "susi_slave.h"
#include "mock_hal.h"
#include "mock_susi_hal.h"
#include "susi_commands.h"
#include "susi_crc.h"

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

    for (int i = 0; i < 8; i++) {
        hal.read_bits.push((255 >> i) & 0x01);
    }
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
        if (p.command == SUSI_CMD_BIDI_HOST_CALL && (p.data & 0x04) != 0) {
            if ((p.data & 0x03) == SLAVE_ADDRESS) {
                slave._test_receive_packet(p);
                if (slave.available()) {
                    slave.read(); // This will trigger the sendAck and sendByte calls in the slave
                    hal.ack_result = SUCCESS;
                } else {
                    hal.ack_result = TIMEOUT;
                }
            } else {
                hal.ack_result = TIMEOUT;
            }
        }
    };

    EXPECT_EQ(api.performHandshake(), SUCCESS);
    EXPECT_EQ(api.getBidiSlaveCount(), 1);

    _callback_fired_e2e = false;
    api.onBidiResponse(bidi_callback_e2e);

    hal.onSendPacket = [&](const SUSI_Packet& p, bool expectAck) {
        if (p.command == SUSI_CMD_BIDI_HOST_CALL && p.data == SLAVE_ADDRESS) {
            slave._test_receive_packet(p);
            if (slave.available()) {
                slave.read();
                hal.ack_result = SUCCESS;
            }
        }
    };

    api.pollSlaves();

    EXPECT_TRUE(_callback_fired_e2e);
    EXPECT_EQ(_callback_address_e2e, SLAVE_ADDRESS);
    EXPECT_EQ(_callback_data_e2e[0], SUSI_MSG_BIDI_IDLE);
}

TEST_F(LegacySusiE2ETest, Poll) {
    // Manually register a bidi slave for this test
    api.registerBiDiSlave(SLAVE_ADDRESS);
    slave.enableBidirectionalMode();

    // Setup callback
    _callback_fired_e2e = false;
    memset(_callback_data_e2e, 0, sizeof(_callback_data_e2e));
    api.onBidiResponse(bidi_callback_e2e);

    // Setup mock for packet sending.
    hal.onSendPacket = [&](const SUSI_Packet& p, bool expectAck) {
        if (p.command == SUSI_CMD_BIDI_HOST_CALL && p.data == SLAVE_ADDRESS) {
            slave._test_receive_packet(p);
            if (slave.available()) {
                slave.read(); // This should trigger ACK and response from slave
                hal.ack_result = SUCCESS;
            } else {
                hal.ack_result = TIMEOUT;
            }
        }
    };

    // Action
    api.pollSlaves();

    // Assertions
    EXPECT_TRUE(_callback_fired_e2e);
    EXPECT_EQ(_callback_address_e2e, SLAVE_ADDRESS);
    // Queue some data on the slave
    uint8_t slave_data[] = {0xAA, 0xBB, 0xCC, 0xDD};
    slave.queueBidirectionalData(slave_data);

    // The mock HAL needs to know what the slave will send back.
    // This is a bit of a hack, but necessary for the mock to work.
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            hal.read_bits.push((slave_data[i] >> j) & 1);
        }
    }

    // Action
    api.pollSlaves();

    // Assertions
    EXPECT_TRUE(_callback_fired_e2e);
    EXPECT_EQ(_callback_address_e2e, SLAVE_ADDRESS);
    EXPECT_EQ(_callback_data_e2e[0], 0xAA);
    EXPECT_EQ(_callback_data_e2e[1], 0xBB);
    EXPECT_EQ(_callback_data_e2e[2], 0xCC);
    EXPECT_EQ(_callback_data_e2e[3], 0xDD);
}

TEST_F(LegacySusiE2ETest, readCVBank) {
    hal.ack_result = SUCCESS;

    // Write some CVs to the slave
    api.writeCV(SLAVE_ADDRESS, 1, 10);
    api.writeCV(SLAVE_ADDRESS, 20, 20);
    api.writeCV(SLAVE_ADDRESS, 40, 30);

    hal.onSendPacket = [&](const SUSI_Packet& p, bool expectAck) {
        slave._test_receive_packet(p);
    };

    hal.afterSendPacket = [&]() {
        EXPECT_TRUE(slave.available());
        slave.read();
    };

    // Prepare the data for the mock HAL to send back
    uint8_t bank_data[40] = {0};
    bank_data[0] = 10;
    bank_data[19] = 20;
    bank_data[39] = 30;
    uint16_t crc = crc16_ccitt(bank_data, 40);

    for (int i = 0; i < 40; i++) {
        for (int j = 0; j < 8; j++) {
            hal.read_bits.push((bank_data[i] >> j) & 0x01);
        }
    }
    for (int j = 0; j < 8; j++) {
        hal.read_bits.push((crc >> (j + 8)) & 0x01);
    }
    for (int j = 0; j < 8; j++) {
        hal.read_bits.push((crc >> j) & 0x01);
    }

    uint8_t received_data[40];
    EXPECT_EQ(api.readCVBank(SLAVE_ADDRESS, 0, received_data), SUCCESS);

    EXPECT_EQ(received_data[0], 10);
    EXPECT_EQ(received_data[19], 20);
    EXPECT_EQ(received_data[39], 30);
}
