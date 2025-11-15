#ifndef SUSI_SLAVE_H
#define SUSI_SLAVE_H

#include <Arduino.h>
#include "susi_hal.h"
#include "susi_packet.h"

class SUSI_Slave;
extern SUSI_Slave* _susi_slave_instance;

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
     */
    SUSI_Slave(SusiHAL& hal);

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

    /**
     * @brief Enables bidirectional communication mode.
     * @see RCN-601
     */
    void enableBidirectionalMode();

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
     * @brief Queues data to be sent in the next bidirectional response.
     * @param data A pointer to a 4-byte array containing the data to send.
     * @see RCN-601
     */
    void queueBidirectionalData(const uint8_t* data);

    /**
     * @brief Sends a position response in the next bidirectional poll.
     * @param address The position address to send.
     * @see RCN-601
     */
    void sendPositionResponse(uint16_t address);

    /**
     * @brief Sends a signal state response.
     * @param state The signal state to send.
     */
    void sendSignalState(uint8_t state);

    /**
     * @brief Sends a direct function response.
     * @param function The function to send.
     * @param action The action to send.
     */
    void sendDirectFunction(uint8_t function, uint8_t action);

    /**
     * @brief Sends a DCC function response.
     * @param function The DCC function to send.
     * @param action The action to send.
     */
    void sendDCCFunction(uint8_t function, uint8_t action);

    /**
     * @brief Sends a short binary state response.
     * @param state The state to send.
     */
    void sendShortBinaryState(uint8_t state);

    /**
     * @brief Sends an automatic speed response.
     * @param speed The speed to send.
     * @param forward The direction to send.
     */
    void sendAutoSpeed(uint8_t speed, bool forward);

    /**
     * @brief Sends an automatic operation response.
     * @param operation The operation to send.
     */
    void sendAutoOperation(uint8_t operation);

    /**
     * @brief Sends an analog value response.
     * @param channel The analog channel.
     * @param value The analog value.
     */
    void sendAnalogValue(uint8_t channel, uint8_t value);

    /**
     * @brief Sends an error response.
     * @param error The error to send.
     */
    void sendError(uint8_t error);

    /**
     * @brief Reads the value of a CV.
     * @param cv The CV to read.
     * @return uint8_t The value of the CV.
     */
    uint8_t readCV(uint16_t cv);

    /**
     * @brief Sets the manufacturer ID of the slave.
     * @param id The manufacturer ID.
     */
    void setManufacturerID(uint16_t id);

    /**
     * @brief Sets the hardware ID of the slave.
     * @param id The hardware ID.
     */
    void setHardwareID(uint16_t id);

    /**
     * @brief Sets the version number of the slave.
     * @param version The version number.
     */
    void setVersionNumber(uint16_t version);

    /**
     * @brief Sets the status bits.
     * @param bits The status bits to set.
     */
    void setStatusBits(uint8_t bits);

    /**
     * @brief Clears the status bits.
     * @param bits The status bits to clear.
     */
    void clearStatusBits(uint8_t bits);

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
    void _send_bidi_response(uint8_t header1, uint8_t data1, uint8_t header2, uint8_t data2);
    void getCVBank(uint8_t bank, uint8_t* data);
    static void onClockFall();
    void handleClockFall();

    SusiHAL& _hal;
    uint8_t _address;
    volatile bool _packetReady;
    volatile uint8_t _buffer[3];
    volatile uint8_t _bitCount;
    volatile unsigned long _last_bit_time_us;
    uint8_t _speed;
    bool _forward;
    uint32_t _functions;
    uint8_t _cv_bank;
    uint8_t _cv_bank_select;
    uint16_t _cv_address;
    bool _cv_read_mode;
    bool _cv_op_in_progress;
    uint16_t _cv_keys[MAX_CVS];
    uint8_t _cv_values[MAX_CVS];
    uint8_t _cv_count;
    bool _bidirectional_mode;
    uint8_t _bidi_response_buffer[4];
    bool _bidi_data_available;
    uint8_t _status_bits;
    FunctionCallback _function_callback;

    uint16_t _manufacturer_id;
    uint16_t _hardware_id;
    uint16_t _version_number;
};

#endif // SUSI_SLAVE_H
