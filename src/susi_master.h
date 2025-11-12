#ifndef SUSI_MASTER_H
#define SUSI_MASTER_H

#include <Arduino.h>
#include "susi_hal.h"
#include "susi_packet.h"
#include "susi_response.h"

/**
 * @brief Represents a SUSI Master device.
 * @details This class provides the low-level functionality for sending SUSI packets to slave devices.
 * It handles the timing and synchronization of the SUSI protocol.
 * @see RCN-600
 */
class SUSI_Master {
public:
    /**
     * @brief Constructs a new SUSI_Master object.
     * @param hal A reference to a SusiHAL object that provides the hardware abstraction.
     */
    SUSI_Master(SusiHAL& hal);

    /**
     * @brief Initializes the SUSI master.
     */
    void begin();

    /**
     * @brief Sends a SUSI packet to a slave device.
     * @param packet The packet to send.
     * @param expectAck Whether to expect an acknowledge from the slave.
     * @return SusiMasterResult A result code indicating the status of the operation.
     */
    SusiMasterResult sendPacket(const SUSI_Packet& packet, bool expectAck = false);

    /**
     * @brief Reads a byte from the SUSI bus after sending a request.
     * @return uint8_t The byte read from the bus.
     */
    uint8_t readByteAfterRequest();

private:
    SusiHAL& _hal;
    unsigned long _last_packet_time_ms;
    uint8_t _packets_since_sync;

    void sendByte(uint8_t byte);
};

/**
 * @brief The maximum number of slaves that can be managed by the SUSI_Master_API.
 */
const uint8_t MAX_SLAVES = 16;

/**
 * @brief Represents the state of a SUSI slave device.
 */
struct SUSI_Slave_State {
    uint8_t address;
    uint32_t functions;
};

/**
 * @brief Represents a SUSI slave device that supports bidirectional communication.
 * @see RCN-601
 */
struct SUSI_Bidi_Slave {
    uint8_t address;
};

/**
 * @brief Provides a high-level API for interacting with SUSI slave devices.
 * @details This class provides a more user-friendly interface for sending commands to SUSI slaves.
 */
class SUSI_Master_API {
public:
    /**
     * @brief Constructs a new SUSI_Master_API object.
     * @param master A reference to a SUSI_Master object.
     */
    SUSI_Master_API(SUSI_Master& master);

    /**
     * @brief Initializes the SUSI master API.
     */
    void begin();

    /**
     * @brief Resets the state of the SUSI master API.
     */
    void reset();

    /**
     * @brief Sets a function on a SUSI slave device.
     * @param address The address of the slave.
     * @param function The function to set (0-31).
     * @param on Whether to turn the function on or off.
     * @return SusiMasterResult A result code indicating the status of the operation.
     * @see RCN-600
     */
    SusiMasterResult setFunction(uint8_t address, uint8_t function, bool on);

    /**
     * @brief Gets the state of a function on a SUSI slave device.
     * @param address The address of the slave.
     * @param function The function to get (0-31).
     * @return bool The state of the function (true for on, false for off).
     * @see RCN-600
     */
    bool getFunction(uint8_t address, uint8_t function);

    /**
     * @brief Sets the speed of a SUSI slave device.
     * @param address The address of the slave.
     * @param speed The speed to set (0-127).
     * @param forward The direction of travel (true for forward, false for reverse).
     * @return SusiMasterResult A result code indicating the status of the operation.
     * @see RCN-600
     */
    SusiMasterResult setSpeed(uint8_t address, uint8_t speed, bool forward);

    /**
     * @brief Writes a value to a Configuration Variable (CV) on a SUSI slave device.
     * @param address The address of the slave.
     * @param cv The CV to write to (1-1024).
     * @param value The value to write (0-255).
     * @return SusiMasterResult A result code indicating the status of the operation.
     * @see RCN-602
     */
    SusiMasterResult writeCV(uint8_t address, uint16_t cv, uint8_t value);

    /**
     * @brief Reads a value from a Configuration Variable (CV) on a SUSI slave device.
     * @param address The address of the slave.
     * @param cv The CV to read from (1-1024).
     * @param value A reference to a byte to store the value in.
     * @return SusiMasterResult A result code indicating the status of the operation.
     * @see RCN-602
     */
    SusiMasterResult readCV(uint8_t address, uint16_t cv, uint8_t& value);

    /**
     * @brief Performs the handshake to detect and register bidirectional slaves.
     * @return SusiMasterResult A result code indicating the status of the operation.
     * @see RCN-601
     */
    SusiMasterResult performHandshake();

    /**
     * @brief Polls all registered bidirectional slaves.
     * @see RCN-601
     */
    void pollSlaves();

    /**
     * @brief A callback function that is called when a bidirectional response is received.
     * @param address The address of the slave that sent the response.
     * @param data The 4-byte response data.
     */
    typedef void (*BidiResponseCallback)(uint8_t address, uint8_t* data);

    /**
     * @brief Sets the callback function for bidirectional responses.
     * @param callback The callback function.
     */
    void onBidiResponse(BidiResponseCallback callback);

private:
    SUSI_Master& _master;
    SUSI_Slave_State _slave_states[MAX_SLAVES];
    uint8_t _slave_count;
    SUSI_Bidi_Slave _bidi_slaves[MAX_SLAVES];
    uint8_t _bidi_slave_count;
    BidiResponseCallback _bidi_callback;
};

#endif // SUSI_MASTER_H
