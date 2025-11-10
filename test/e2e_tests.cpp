#include "gtest/gtest.h"
#include "SUSI_Master.h"
#include "SUSI_Slave.h"
#include "mock_hal.h"

// Test fixture for End-to-End tests
class EndToEndTest : public ::testing::Test {
protected:
    const uint8_t CLOCK_PIN = 2;
    const uint8_t DATA_PIN = 3;
    const uint8_t SLAVE_ADDRESS = 5;

    EndToEndTest() : master(CLOCK_PIN, DATA_PIN), slave(CLOCK_PIN, DATA_PIN, SLAVE_ADDRESS) {}

    void SetUp() override {
        mock_hal_reset();
        master.begin();
        slave.begin();
        // Set initial clock state to HIGH
        digitalWrite(CLOCK_PIN, HIGH);
    }

    SUSI_Master master;
    SUSI_Slave slave;
};

TEST_F(EndToEndTest, TrueEndToEnd_SendAndReceivePacket) {
    SUSI_Packet packet_to_send = {SLAVE_ADDRESS, 0xAB, 0xCD};

    // The master's sendPacket will now automatically trigger the slave's
    // interrupt handler via the enhanced mock HAL.
    master.sendPacket(packet_to_send, false); // Send without ACK for this test

    // Manually trigger the slave's loop to process the packet
    slave.loop();

    EXPECT_EQ(slave._last_received_packet.address, packet_to_send.address);
    EXPECT_EQ(slave._last_received_packet.command, packet_to_send.command);
    EXPECT_EQ(slave._last_received_packet.data, packet_to_send.data);
}
