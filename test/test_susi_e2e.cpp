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

    void runPollTest(const uint8_t* response_data);
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

    // The slave will send back a 4-byte BiDi message.
    // We need to push the bits for the mock HAL to read.
    uint8_t response[] = {SUSI_MSG_BIDI_CV_RESPONSE, 255, SUSI_MSG_BIDI_CV_RESPONSE, 0};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            hal.read_bits.push((response[i] >> j) & 0x01);
        }
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
    EXPECT_EQ(_callback_data_e2e[0], SUSI_MSG_BIDI_EMPTY);
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

TEST_F(LegacySusiE2ETest, readCV_BiDi) {
    hal.ack_result = SUCCESS;

    hal.onSendPacket = [&](const SUSI_Packet& p, bool expectAck) {
        slave._test_receive_packet(p);
    };
    hal.afterSendPacket = [&]() {
        EXPECT_TRUE(slave.available());
        slave.read();
    };

    api.writeCV(SLAVE_ADDRESS, 1, 255);

    // The slave will send back a 4-byte BiDi message.
    // We need to push the bits for the mock HAL to read.
    uint8_t response[] = {SUSI_MSG_BIDI_CV_RESPONSE, 255, SUSI_MSG_BIDI_CV_RESPONSE, 0};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            hal.read_bits.push((response[i] >> j) & 0x01);
        }
    }

    uint8_t value;
    EXPECT_EQ(api.readCV(SLAVE_ADDRESS, 1, value), SUCCESS);

    // The slave should have sent back a BiDi message with the CV value.
    // The master should parse this and return the correct value.
    EXPECT_EQ(value, 255);
}

TEST_F(LegacySusiE2ETest, readSpecialCVs) {
    hal.ack_result = SUCCESS;

    slave.setManufacturerID(0x1234);
    slave.setHardwareID(0x5678);
    slave.setVersionNumber(0x9ABC);

    hal.onSendPacket = [&](const SUSI_Packet& p, bool expectAck) {
        slave._test_receive_packet(p);
    };
    hal.afterSendPacket = [&]() {
        EXPECT_TRUE(slave.available());
        slave.read();
    };

    uint8_t value;

    auto check_cv = [&](uint16_t cv, uint8_t expected_value) {
        hal.read_bits = {};
        uint8_t response[] = {SUSI_MSG_BIDI_CV_RESPONSE, expected_value, SUSI_MSG_BIDI_CV_RESPONSE, 0};
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 8; j++) {
                hal.read_bits.push((response[i] >> j) & 0x01);
            }
        }
        EXPECT_EQ(api.readCV(SLAVE_ADDRESS, cv, value), SUCCESS);
        EXPECT_EQ(value, expected_value);
    };

    check_cv(CV_SUSI_MODULE_NUM, SLAVE_ADDRESS);
    check_cv(CV_MANUFACTURER_ID, 0x12);
    check_cv(CV_MANUFACTURER_ID + 1, 0x34);
    check_cv(CV_VERSION_NUM, 0x9A);
    check_cv(CV_VERSION_NUM + 1, 0xBC);
    check_cv(CV_STATUS_BITS, 0);
}

TEST_F(LegacySusiE2ETest, PollPositionResponse) {
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

    // Queue a position response on the slave
    uint16_t position = 0xABCD;
    slave.sendPositionResponse(position);

    // The mock HAL needs to know what the slave will send back.
    uint8_t response_data[] = {SUSI_MSG_BIDI_POSITION_HIGH, 0xAB, SUSI_MSG_BIDI_POSITION_LOW, 0xCD};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            hal.read_bits.push((response_data[i] >> j) & 1);
        }
    }

    // Action
    api.pollSlaves();

    // Assertions
    EXPECT_TRUE(_callback_fired_e2e);
    EXPECT_EQ(_callback_address_e2e, SLAVE_ADDRESS);
    EXPECT_EQ(_callback_data_e2e[0], SUSI_MSG_BIDI_POSITION_HIGH);
    EXPECT_EQ(_callback_data_e2e[1], 0xAB);
    EXPECT_EQ(_callback_data_e2e[2], SUSI_MSG_BIDI_POSITION_LOW);
    EXPECT_EQ(_callback_data_e2e[3], 0xCD);
}

void LegacySusiE2ETest::runPollTest(const uint8_t* response_data) {
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

    // The mock HAL needs to know what the slave will send back.
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            hal.read_bits.push((response_data[i] >> j) & 1);
        }
    }

    // Action
    api.pollSlaves();
}

TEST_F(LegacySusiE2ETest, PollSignalState) {
    // Queue a signal state response on the slave
    uint8_t state = 0x04; // Langsamfahrt (HP2)
    slave.sendSignalState(state);

    // The mock HAL needs to know what the slave will send back.
    uint8_t response_data[] = {SUSI_MSG_BIDI_SIGNAL_STATE, state, SUSI_MSG_BIDI_EMPTY, 0};
    runPollTest(response_data);

    // Assertions
    EXPECT_TRUE(_callback_fired_e2e);
    EXPECT_EQ(_callback_address_e2e, SLAVE_ADDRESS);
    EXPECT_EQ(_callback_data_e2e[0], SUSI_MSG_BIDI_SIGNAL_STATE);
    EXPECT_EQ(_callback_data_e2e[1], state);
    EXPECT_EQ(_callback_data_e2e[2], SUSI_MSG_BIDI_EMPTY);
    EXPECT_EQ(_callback_data_e2e[3], 0);
}

TEST_F(LegacySusiE2ETest, PollDirectFunction) {
    uint8_t function = 1; // Pfeife/Hupe
    uint8_t action = 32; // Einschalten
    slave.sendDirectFunction(function, action);
    uint8_t response_data[] = {SUSI_MSG_BIDI_DIRECT_FUNCTION, (uint8_t)(function | action), SUSI_MSG_BIDI_EMPTY, 0};
    runPollTest(response_data);
    EXPECT_TRUE(_callback_fired_e2e);
    EXPECT_EQ(_callback_address_e2e, SLAVE_ADDRESS);
    EXPECT_EQ(_callback_data_e2e[0], SUSI_MSG_BIDI_DIRECT_FUNCTION);
    EXPECT_EQ(_callback_data_e2e[1], (function | action));
    EXPECT_EQ(_callback_data_e2e[2], SUSI_MSG_BIDI_EMPTY);
    EXPECT_EQ(_callback_data_e2e[3], 0);
}

TEST_F(LegacySusiE2ETest, PollDCCFunction) {
    uint8_t function = 5; // DCC Function 5
    uint8_t action = 64; // Ausschalten
    slave.sendDCCFunction(function, action);
    uint8_t response_data[] = {SUSI_MSG_BIDI_FUNCTION_VALUE_DCC, (uint8_t)(function | action), SUSI_MSG_BIDI_EMPTY, 0};
    runPollTest(response_data);
    EXPECT_TRUE(_callback_fired_e2e);
    EXPECT_EQ(_callback_address_e2e, SLAVE_ADDRESS);
    EXPECT_EQ(_callback_data_e2e[0], SUSI_MSG_BIDI_FUNCTION_VALUE_DCC);
    EXPECT_EQ(_callback_data_e2e[1], (function | action));
    EXPECT_EQ(_callback_data_e2e[2], SUSI_MSG_BIDI_EMPTY);
    EXPECT_EQ(_callback_data_e2e[3], 0);
}

TEST_F(LegacySusiE2ETest, PollShortBinaryState) {
    uint8_t state = 0xAB;
    slave.sendShortBinaryState(state);
    uint8_t response_data[] = {SUSI_MSG_BIDI_SHORT_BINARY_STATES, state, SUSI_MSG_BIDI_EMPTY, 0};
    runPollTest(response_data);
    EXPECT_TRUE(_callback_fired_e2e);
    EXPECT_EQ(_callback_address_e2e, SLAVE_ADDRESS);
    EXPECT_EQ(_callback_data_e2e[0], SUSI_MSG_BIDI_SHORT_BINARY_STATES);
    EXPECT_EQ(_callback_data_e2e[1], state);
    EXPECT_EQ(_callback_data_e2e[2], SUSI_MSG_BIDI_EMPTY);
    EXPECT_EQ(_callback_data_e2e[3], 0);
}

TEST_F(LegacySusiE2ETest, PollAutoSpeed) {
    uint8_t speed = 100;
    bool forward = true;
    slave.sendAutoSpeed(speed, forward);
    uint8_t data_byte = speed & 0x7F;
    if (forward) {
        data_byte |= 0x80;
    }
    uint8_t response_data[] = {SUSI_MSG_BIDI_AUTO_SPEED, data_byte, SUSI_MSG_BIDI_EMPTY, 0};
    runPollTest(response_data);
    EXPECT_TRUE(_callback_fired_e2e);
    EXPECT_EQ(_callback_address_e2e, SLAVE_ADDRESS);
    EXPECT_EQ(_callback_data_e2e[0], SUSI_MSG_BIDI_AUTO_SPEED);
    EXPECT_EQ(_callback_data_e2e[1], data_byte);
    EXPECT_EQ(_callback_data_e2e[2], SUSI_MSG_BIDI_EMPTY);
    EXPECT_EQ(_callback_data_e2e[3], 0);
}

TEST_F(LegacySusiE2ETest, PollAutoOperation) {
    uint8_t operation = 4; // Vorwärtsfahrt (Pendelfahrt)
    slave.sendAutoOperation(operation);
    uint8_t response_data[] = {SUSI_MSG_BIDI_AUTO_OPERATION, operation, SUSI_MSG_BIDI_EMPTY, 0};
    runPollTest(response_data);
    EXPECT_TRUE(_callback_fired_e2e);
    EXPECT_EQ(_callback_address_e2e, SLAVE_ADDRESS);
    EXPECT_EQ(_callback_data_e2e[0], SUSI_MSG_BIDI_AUTO_OPERATION);
    EXPECT_EQ(_callback_data_e2e[1], operation);
    EXPECT_EQ(_callback_data_e2e[2], SUSI_MSG_BIDI_EMPTY);
    EXPECT_EQ(_callback_data_e2e[3], 0);
}

TEST_F(LegacySusiE2ETest, PollAnalogValue) {
    uint8_t channel = 1;
    uint8_t value = 0xFE;
    slave.sendAnalogValue(channel, value);
    uint8_t header = SUSI_MSG_BIDI_ANALOG_A;
    uint8_t response_data[] = {header, value, SUSI_MSG_BIDI_EMPTY, 0};
    runPollTest(response_data);
    EXPECT_TRUE(_callback_fired_e2e);
    EXPECT_EQ(_callback_address_e2e, SLAVE_ADDRESS);
    EXPECT_EQ(_callback_data_e2e[0], header);
    EXPECT_EQ(_callback_data_e2e[1], value);
    EXPECT_EQ(_callback_data_e2e[2], SUSI_MSG_BIDI_EMPTY);
    EXPECT_EQ(_callback_data_e2e[3], 0);
}

TEST_F(LegacySusiE2ETest, PollError) {
    uint8_t error = 0x01; // CV nicht unterstützt
    slave.sendError(error);
    uint8_t response_data[] = {SUSI_MSG_BIDI_ERROR, error, SUSI_MSG_BIDI_EMPTY, 0};
    runPollTest(response_data);
    EXPECT_TRUE(_callback_fired_e2e);
    EXPECT_EQ(_callback_address_e2e, SLAVE_ADDRESS);
    EXPECT_EQ(_callback_data_e2e[0], SUSI_MSG_BIDI_ERROR);
    EXPECT_EQ(_callback_data_e2e[1], error);
    EXPECT_EQ(_callback_data_e2e[2], SUSI_MSG_BIDI_EMPTY);
    EXPECT_EQ(_callback_data_e2e[3], 0);
}
