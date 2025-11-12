#include "gtest/gtest.h"
#include "susi_master.h"
#include "susi_slave.h"
#include "mock_hal.h"
#include "mock_susi_hal.h"
#include "susi_commands.h"

// Test fixture for End-to-End tests
class EndToEndTest : public ::testing::Test {
protected:
    const uint8_t CLOCK_PIN = 2;
    const uint8_t DATA_PIN = 3;
    const uint8_t SLAVE_ADDRESS = 5;

    SusiHAL hal;
    SUSI_Master master;
    SUSI_Master_API api;
    SUSI_Slave slave;

    EndToEndTest() : hal(CLOCK_PIN, DATA_PIN), master(hal), api(master), slave(hal) {}

    void SetUp() override {
        mock_hal_reset();
        _susi_slave_instance = &slave;
        api.begin();
        slave.begin(SLAVE_ADDRESS);
        // Set initial clock state to HIGH
        digitalWrite(CLOCK_PIN, HIGH);
    }
};

TEST_F(EndToEndTest, TrueEndToEnd_SendAndReceiveSetSpeedPacket) {
    // The master's sendPacket will now automatically trigger the slave's
    // interrupt handler via the enhanced mock HAL.
    api.setSpeed(SLAVE_ADDRESS, 100, true);

    EXPECT_TRUE(slave.available());
    SUSI_Packet received_packet = slave.read();
    EXPECT_EQ(received_packet.address, SLAVE_ADDRESS);
    EXPECT_EQ(received_packet.command, SUSI_CMD_SET_SPEED);
    EXPECT_EQ(received_packet.data, (100 & 0x7F) | 0x80);

    EXPECT_EQ(slave.getSpeed(), 100);
    EXPECT_TRUE(slave.getDirection());
}

// Test fixture for mocked End-to-End tests
class HandshakeE2ETest : public ::testing::Test {
protected:
    MockSusiHAL hal;
    SUSI_Master master;
    SUSI_Master_API api;
    SUSI_Slave slave;

    HandshakeE2ETest() : master(hal), api(master), slave(hal) {}

    void SetUp() override {
        _susi_slave_instance = &slave;
        api.begin();
        slave.begin(1);
        api.reset();
    }
};

TEST_F(HandshakeE2ETest, MockedEndToEnd_PerformHandshake) {
    hal.onSendPacket = [&](const SUSI_Packet& packet, bool expectAck) {
        slave._test_receive_packet(packet);
    };

    hal.afterSendPacket = [&]() {
        if (slave.available()) {
            slave.read();
        }
        hal.ack_result = SUCCESS;
    };

    api.performHandshake();

    ASSERT_TRUE(slave.isBidirectionalModeEnabled());
    ASSERT_EQ(api.getBidiSlaveCount(), 1);
}
