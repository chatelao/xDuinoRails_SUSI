#include "gtest/gtest.h"
#include "susi_master.h"
#include "susi_commands.h"
#include "mock_susi_hal.h"

TEST(SUSI_Master_API, setFunction) {
    MockSusiHAL hal;
    SUSI_Master master(hal);
    SUSI_Master_API api(master);
    hal.ack_result = SUCCESS;

    // Capture the packet that is sent
    SUSI_Packet sentPacket;
    hal.onSendPacket = [&](const SUSI_Packet& p, bool a) {
        sentPacket = p;
    };

    EXPECT_EQ(api.setFunction(10, 5, true), SUCCESS);

    EXPECT_EQ(sentPacket.address, 10);
    EXPECT_EQ(sentPacket.command, SUSI_CMD_SET_FUNCTION);
    EXPECT_EQ(sentPacket.data, 0x85); // F5 on
}

TEST(SUSI_Master_API, setSpeed) {
    MockSusiHAL hal;
    SUSI_Master master(hal);
    SUSI_Master_API api(master);
    hal.ack_result = SUCCESS;

    // Capture the packet that is sent
    SUSI_Packet sentPacket;
    hal.onSendPacket = [&](const SUSI_Packet& p, bool a) {
        sentPacket = p;
    };

    EXPECT_EQ(api.setSpeed(10, 100, false), SUCCESS);

    EXPECT_EQ(sentPacket.address, 10);
    EXPECT_EQ(sentPacket.command, SUSI_CMD_SET_SPEED);
    EXPECT_EQ(sentPacket.data, 100); // Speed 100, backwards
}

TEST(SUSI_Master_API, writeCV) {
    MockSusiHAL hal;
    SUSI_Master master(hal);
    SUSI_Master_API api(master);
    hal.ack_result = SUCCESS;

    // Capture the packets that are sent
    std::vector<SUSI_Packet> sentPackets;
    hal.onSendPacket = [&](const SUSI_Packet& p, bool a) {
        sentPackets.push_back(p);
    };

    EXPECT_EQ(api.writeCV(10, 1024, 255), SUCCESS);

    EXPECT_EQ(sentPackets.size(), 2);
    EXPECT_EQ(sentPackets[0].address, 10);
    EXPECT_EQ(sentPackets[0].command, SUSI_CMD_WRITE_CV);
    EXPECT_EQ(sentPackets[0].data, 0x03); // CV bank 3
    EXPECT_EQ(sentPackets[1].address, 10);
    EXPECT_EQ(sentPackets[1].command, 0xFF); // CV 1024 is 255 in bank 3
    EXPECT_EQ(sentPackets[1].data, 255);
}

TEST(SUSI_Master_API, readCV) {
    MockSusiHAL hal;
    SUSI_Master master(hal);
    SUSI_Master_API api(master);
    hal.ack_result = SUCCESS;

    // Capture the packets that are sent
    std::vector<SUSI_Packet> sentPackets;
    hal.onSendPacket = [&](const SUSI_Packet& p, bool a) {
        sentPackets.push_back(p);
    };

    uint8_t value;
    EXPECT_EQ(api.readCV(10, 1024, value), SUCCESS);

    EXPECT_EQ(sentPackets.size(), 2);
    EXPECT_EQ(sentPackets[0].address, 10);
    EXPECT_EQ(sentPackets[0].command, SUSI_CMD_READ_CV);
    EXPECT_EQ(sentPackets[0].data, 0x03); // CV bank 3
    EXPECT_EQ(sentPackets[1].address, 10);
    EXPECT_EQ(sentPackets[1].command, 0xFF); // CV 1024 is 255 in bank 3
    EXPECT_EQ(sentPackets[1].data, 0);
}

TEST(SUSI_Master_API, getUniqueId) {
    MockSusiHAL hal;
    SUSI_Master master(hal);
    SUSI_Master_API api(master);
    hal.ack_result = SUCCESS;

    // Simulate a successful handshake
    EXPECT_EQ(api.enableBidirectionalMode(10), SUCCESS);

    uint32_t unique_id;
    EXPECT_TRUE(api.getUniqueId(10, unique_id));
    EXPECT_FALSE(api.getUniqueId(11, unique_id));
}

TEST(SUSI_Master_API, enableBidirectionalMode_alreadyExists) {
    MockSusiHAL hal;
    SUSI_Master master(hal);
    SUSI_Master_API api(master);
    hal.ack_result = SUCCESS;

    EXPECT_EQ(api.enableBidirectionalMode(10), SUCCESS);
    EXPECT_EQ(api.enableBidirectionalMode(10), SLAVE_ALREADY_EXISTS);
}

TEST(SUSI_Master_API, enableBidirectionalMode_listFull) {
    MockSusiHAL hal;
    SUSI_Master master(hal);
    SUSI_Master_API api(master);
    hal.ack_result = SUCCESS;

    for (int i = 0; i < MAX_SLAVES; i++) {
        EXPECT_EQ(api.enableBidirectionalMode(i), SUCCESS);
    }

    EXPECT_EQ(api.enableBidirectionalMode(MAX_SLAVES), SLAVE_LIST_FULL);
}
