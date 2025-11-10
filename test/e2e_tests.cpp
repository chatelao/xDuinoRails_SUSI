#include "gtest/gtest.h"
#include "susi_master.h"
#include "susi_slave.h"
#include "mock_hal.h"
#include "susi_commands.h"

// Test fixture for End-to-End tests
class EndToEndTest : public ::testing::Test {
protected:
    const uint8_t CLOCK_PIN = 2;
    const uint8_t DATA_PIN = 3;
    const uint8_t SLAVE_ADDRESS = 5;

    EndToEndTest() : master(CLOCK_PIN, DATA_PIN), slave(CLOCK_PIN, DATA_PIN) {}

    void SetUp() override {
        mock_hal_reset();
        master.begin();
        slave.begin(SLAVE_ADDRESS);
        // Set initial clock state to HIGH
        digitalWrite(CLOCK_PIN, HIGH);
    }

    SUSI_Master master;
    SUSI_Slave slave;
};

TEST_F(EndToEndTest, TrueEndToEnd_SendAndReceiveSetSpeedPacket) {
    // The master's sendPacket will now automatically trigger the slave's
    // interrupt handler via the enhanced mock HAL.
    master.setSpeed(SLAVE_ADDRESS, 100, true);

    EXPECT_TRUE(slave.available());
    SUSI_Packet received_packet = slave.read();
    EXPECT_EQ(received_packet.address, SLAVE_ADDRESS);
    EXPECT_EQ(received_packet.command, SUSI_CMD_SET_SPEED);
    EXPECT_EQ(received_packet.data, (100 & 0x7F) | 0x80);

    EXPECT_EQ(slave.getSpeed(), 100);
    EXPECT_TRUE(slave.getDirection());
}
