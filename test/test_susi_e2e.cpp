#include "gtest/gtest.h"
#include "susi_master.h"
#include "susi_slave.h"
#include "mock_hal.h"
#include "mock_susi_hal.h"
#include "susi_commands.h"

// Test fixture for End-to-End tests
class SusiE2ETest : public ::testing::Test {
protected:
    const uint8_t CLOCK_PIN = 2;
    const uint8_t DATA_PIN = 3;
    const uint8_t SLAVE_ADDRESS = 5;

    MockSusiHAL hal;
    SUSI_Master master;
    SUSI_Master_API master_api;
    SUSI_Slave slave;

    SusiE2ETest() : hal(), master(hal), master_api(master), slave(CLOCK_PIN, DATA_PIN) {}

    void SetUp() override {
        mock_hal_reset();
        master_api.begin();
        slave.begin(SLAVE_ADDRESS);
        // Set initial clock state to HIGH
        digitalWrite(CLOCK_PIN, HIGH);
    }
};

TEST_F(SusiE2ETest, setFunction) {
    hal.ack_result = SUCCESS;
    hal.afterSendPacket = [&]() {
        EXPECT_TRUE(slave.available());
        SUSI_Packet received_packet = slave.read();
        EXPECT_EQ(received_packet.address, SLAVE_ADDRESS);
        EXPECT_EQ(received_packet.command, SUSI_CMD_SET_FUNCTION);
        EXPECT_EQ(received_packet.data, 0x85); // F5 on
        EXPECT_TRUE(slave.getFunction(5));
    };

    EXPECT_EQ(master_api.setFunction(SLAVE_ADDRESS, 5, true), SUCCESS);
}

TEST_F(SusiE2ETest, setSpeed) {
    hal.ack_result = SUCCESS;
    hal.afterSendPacket = [&]() {
        EXPECT_TRUE(slave.available());
        SUSI_Packet received_packet = slave.read();
        EXPECT_EQ(received_packet.address, SLAVE_ADDRESS);
        EXPECT_EQ(received_packet.command, SUSI_CMD_SET_SPEED);
        EXPECT_EQ(received_packet.data, 100); // Speed 100, backwards

        EXPECT_EQ(slave.getSpeed(), 100);
        EXPECT_FALSE(slave.getDirection());
    };

    EXPECT_EQ(master_api.setSpeed(SLAVE_ADDRESS, 100, false), SUCCESS);
}

TEST_F(SusiE2ETest, writeCV) {
    hal.ack_result = SUCCESS;
    int packet_count = 0;
    hal.afterSendPacket = [&]() {
        packet_count++;
        switch (packet_count) {
            case 1: {
                EXPECT_TRUE(slave.available());
                SUSI_Packet received_packet = slave.read();
                EXPECT_EQ(received_packet.address, SLAVE_ADDRESS);
                EXPECT_EQ(received_packet.command, SUSI_CMD_WRITE_CV);
                EXPECT_EQ(received_packet.data, 0); // CV bank 0
                break;
            }
            case 2: {
                EXPECT_TRUE(slave.available());
                SUSI_Packet received_packet = slave.read();
                EXPECT_EQ(received_packet.address, SLAVE_ADDRESS);
                EXPECT_EQ(received_packet.command, 0); // CV 1 is 0 in bank 0
                EXPECT_EQ(received_packet.data, 255);
                EXPECT_EQ(slave.readCV(1), 255);
                break;
            }
        }
    };

    EXPECT_EQ(master_api.writeCV(SLAVE_ADDRESS, 1, 255), SUCCESS);
}

TEST_F(SusiE2ETest, readCV) {
    hal.ack_result = SUCCESS;
    int packet_count = 0;
    hal.afterSendPacket = [&]() {
        packet_count++;
        switch (packet_count) {
            case 1: {
                EXPECT_TRUE(slave.available());
                SUSI_Packet received_packet = slave.read();
                EXPECT_EQ(received_packet.address, SLAVE_ADDRESS);
                EXPECT_EQ(received_packet.command, SUSI_CMD_READ_CV);
                EXPECT_EQ(received_packet.data, 0); // CV bank 0
                break;
            }
            case 2: {
                EXPECT_TRUE(slave.available());
                SUSI_Packet received_packet = slave.read();
                EXPECT_EQ(received_packet.address, SLAVE_ADDRESS);
                EXPECT_EQ(received_packet.command, 0); // CV 1 is 0 in bank 0
                EXPECT_EQ(received_packet.data, 0);
                EXPECT_EQ(hal.last_sent_byte, 0);
                break;
            }
        }
    };

    uint8_t value;
    EXPECT_EQ(master_api.readCV(SLAVE_ADDRESS, 1, value), SUCCESS);
}
