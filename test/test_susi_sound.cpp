#include "gtest/gtest.h"
#include "susi_master.h"
#include "susi_slave.h"
#include "mock_susi_hal.h"
#include <sstream>

namespace {
    std::stringstream sound_output;
    void sound_event_callback(uint8_t function, bool on) {
        sound_output << "Sound event: function " << (int)function << (on ? " on" : " off") << std::endl;
    }
}

// Test fixture for Sound SUSI End-to-End tests
class SoundSusiE2ETest : public ::testing::Test {
protected:
    const uint8_t SLAVE_ADDRESS = 1;

    MockSusiHAL hal;
    SUSI_Master master;
    SUSI_Master_API api;
    SUSI_Slave slave;

    SoundSusiE2ETest() : master(hal), api(master), slave(hal) {}

    void SetUp() override {
        sound_output.str(""); // Clear the stream
        api.begin();
        slave.begin(SLAVE_ADDRESS);

        // Forward packets from master to slave
        hal.onSendPacket = [&](const SUSI_Packet& p, bool expectAck) {
            slave._test_receive_packet(p);
            if (slave.available()) {
                slave.read();
            }
        };
    }
};

TEST_F(SoundSusiE2ETest, SoundEventTrigger) {
    slave.onFunctionChange(sound_event_callback);

    api.setFunction(SLAVE_ADDRESS, 5, true);

    std::string expected_output = "Sound event: function 5 on\n";
    EXPECT_EQ(sound_output.str(), expected_output);
}
