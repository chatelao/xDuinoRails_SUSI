#ifndef SUSI_SLAVE_H
#define SUSI_SLAVE_H

#include <Arduino.h>
#include "susi_hal.h"
#include "susi_packet.h"

/**
 * @brief The maximum number of CVs that can be stored by the slave.
 */
const uint8_t MAX_CVS = 32;

/**
 * @brief A callback function that is called when a function is changed.
 * @param function The function that was changed.
 * @param on Whether the function was turned on or off.
 */
typedef void (*FunctionCallback)(uint8_t, bool);

/**
 * @brief Represents a SUSI Slave device.
 * @details This class provides the functionality for a SUSI slave device to receive and process SUSI packets from a master.
 * @see RCN-600
 */
class SUSI_Slave {
public:
    /**
     * @brief Constructs a new SUSI_Slave object.
     * @param hal A reference to a SusiHAL object that provides the hardware abstraction.
     * @param unique_id The unique ID of the slave.
     */
    SUSI_Slave(SusiHAL& hal, uint32_t unique_id);

    /**
     * @brief Initializes the SUSI slave.
     * @param address The address of the slave.
     */
    void begin(uint8_t address);

    /**
     * @brief Checks if a SUSI packet is available to be read.
     * @return bool Whether a packet is available.
     */
    bool available();

    /**
     * @brief Reads a SUSI packet from the slave.
     * @return SUSI_Packet The packet that was read.
     */
    SUSI_Packet read();

    /**
     * @brief Sets a callback function that is called when a function is changed.
     * @param callback The callback function.
     */
    void onFunctionChange(FunctionCallback callback);

public:
    /**
     * @brief Gets the current speed of the slave.
     * @return uint8_t The current speed.
     */
    uint8_t getSpeed() const { return _speed; }

    /**
     * @brief Gets the current direction of the slave.
     * @return bool The current direction (true for forward, false for reverse).
     */
    bool getDirection() const { return _forward; }

    /**
     * @brief Gets the state of a function.
     * @param function The function to get the state of.
     * @return bool The state of the function (true for on, false for off).
     */
    bool getFunction(uint8_t function) const { return (_functions >> function) & 1; }

    /**
     * @brief Reads the value of a CV.
     * @param cv The CV to read.
     * @return uint8_t The value of the CV.
     */
    uint8_t readCV(uint16_t cv);

#ifdef TESTING
    /**
     * @brief Checks if bidirectional mode is enabled.
     * @return bool Whether bidirectional mode is enabled.
     */
    bool isBidirectionalModeEnabled() const { return _bidirectional_mode; }

    /**
     * @brief Test-only function to inject a packet into the slave.
     * @param packet The packet to inject.
     */
    void _test_receive_packet(const SUSI_Packet& packet);
#endif

private:
    static void onClockFall();
    void handleClockFall();

    SusiHAL _hal;
    uint8_t _address;
    volatile bool _packetReady;
    volatile uint8_t _buffer[3];
    volatile uint8_t _bitCount;
    volatile unsigned long _last_bit_time_us;
    uint8_t _speed;
    bool _forward;
    uint32_t _functions;
    uint8_t _cv_bank;
    uint16_t _cv_address;
    bool _cv_read_mode;
    uint16_t _cv_keys[MAX_CVS];
    uint8_t _cv_values[MAX_CVS];
    uint8_t _cv_count;
    bool _bidirectional_mode;
    uint32_t _unique_id;
    FunctionCallback _function_callback;
};

#endif // SUSI_SLAVE_H
