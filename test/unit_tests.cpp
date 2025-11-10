#include "gtest/gtest.h"
#include "susi_hal.h"
#include "mock_hal.h"
#include "SUSI_Master.h"
#include "SUSI_Slave.h"

// Test fixture for SusiHAL tests
class SusiHALTest : public ::testing::Test {
protected:
    const uint8_t CLOCK_PIN = 2;
    const uint8_t DATA_PIN = 3;

    SusiHALTest() : hal(CLOCK_PIN, DATA_PIN) {}

    void SetUp() override {
        mock_hal_reset();
    }

    SusiHAL hal;
};

TEST_F(SusiHALTest, PinInitialization) {
    hal.begin();
    EXPECT_EQ(pin_modes[CLOCK_PIN], OUTPUT);
    EXPECT_EQ(pin_modes[DATA_PIN], OUTPUT);
}

TEST_F(SusiHALTest, ClockControl) {
    hal.begin();
    hal.set_clock_high();
    EXPECT_EQ(pin_states[CLOCK_PIN], HIGH);
    hal.set_clock_low();
    EXPECT_EQ(pin_states[CLOCK_PIN], LOW);
}

TEST_F(SusiHALTest, DataControl) {
    hal.begin();
    hal.set_data_high();
    EXPECT_EQ(pin_states[DATA_PIN], HIGH);
    hal.set_data_low();
    EXPECT_EQ(pin_states[DATA_PIN], LOW);
}

class SUSIMasterTest : public ::testing::Test {
protected:
    const uint8_t CLOCK_PIN = 2;
    const uint8_t DATA_PIN = 3;

    SUSIMasterTest() : master(CLOCK_PIN, DATA_PIN) {}

    void SetUp() override {
        mock_hal_reset();
        master.begin();
    }

    SUSI_Master master;
};

TEST_F(SUSIMasterTest, Initialization) {
    // master.begin() is called in SetUp()
    SUCCEED();
}

TEST_F(SUSIMasterTest, SendPacket) {
    SUSI_Packet packet_to_send = {0x05, 0xAB, 0xCD};
    master.sendPacket(packet_to_send, false);

    // Total writes from sendPacket: 26 bits * (1 data write + 2 clock writes) = 78
    // Additional writes from master.begin() in SetUp(): 2 (initial state)
    // Total expected writes: 80
    EXPECT_EQ(digitalWrite_calls.size(), 80);
}

class SUSISlaveTest : public ::testing::Test {
protected:
    const uint8_t CLOCK_PIN = 2;
    const uint8_t DATA_PIN = 3;
    const uint8_t SLAVE_ADDRESS = 5;

    SUSISlaveTest() : slave(CLOCK_PIN, DATA_PIN, SLAVE_ADDRESS) {}

    void SetUp() override {
        mock_hal_reset();
        slave.begin();
        digitalWrite(CLOCK_PIN, HIGH); // Set initial clock state
    }

    SUSI_Slave slave;
};

TEST_F(SUSISlaveTest, Initialization) {
    // slave.begin() is called in SetUp()
    SUCCEED();
}

TEST_F(SUSISlaveTest, PacketReception) {
    // Manually send a packet to the slave by manipulating the data pin
    // and toggling the clock. The mock HAL will automatically call the
    // slave's interrupt handler on each falling edge of the clock.

    // Start bit
    digitalWrite(DATA_PIN, LOW);
    digitalWrite(CLOCK_PIN, LOW);
    digitalWrite(CLOCK_PIN, HIGH);

    uint32_t packet_data = 0;
    packet_data |= (uint32_t)SLAVE_ADDRESS;
    packet_data |= (uint32_t)0xAB << 8;
    packet_data |= (uint32_t)0xCD << 16;

    for (int i = 0; i < 24; ++i) {
        digitalWrite(DATA_PIN, (packet_data >> i) & 0x01);
        digitalWrite(CLOCK_PIN, LOW);
        digitalWrite(CLOCK_PIN, HIGH);
    }

    // Stop bit
    digitalWrite(DATA_PIN, HIGH);
    digitalWrite(CLOCK_PIN, LOW);
    digitalWrite(CLOCK_PIN, HIGH);

    EXPECT_TRUE(slave.getPacketReady());
    EXPECT_EQ(slave.getDataBuffer(), packet_data);
}
