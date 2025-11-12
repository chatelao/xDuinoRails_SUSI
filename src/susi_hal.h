#ifndef SUSI_HAL_H
#define SUSI_HAL_H

#include <Arduino.h>
#include "susi_response.h"

/**
 * @brief This class provides a hardware abstraction layer for the SUSI protocol.
 * @details This class is responsible for all direct communication with the hardware pins.
 * It is designed to be extensible, so that it can be used with different hardware environments.
 */
class SusiHAL {
public:
    /**
     * @brief Construct a new SusiHAL object
     * @param clock_pin The pin number for the clock signal.
     * @param data_pin The pin number for the data signal.
     */
    SusiHAL(uint8_t clock_pin, uint8_t data_pin);

    /**
     * @brief Destroy the SusiHAL object
     */
    virtual ~SusiHAL() = default;

    /**
     * @brief Initialize the HAL.
     */
    virtual void begin();

    /**
     * @brief Set the clock pin to high.
     */
    virtual void set_clock_high();

    /**
     * @brief Set the clock pin to low.
     */
    virtual void set_clock_low();

    /**
     * @brief Generate a clock pulse.
     */
    virtual void generate_clock_pulse();

    /**
     * @brief Set the data pin to high.
     */
    virtual void set_data_high();

    /**
     * @brief Set the data pin to low.
     */
    virtual void set_data_low();

    /**
     * @brief Read the data pin.
     * @return true if the pin is high, false otherwise.
     */
    virtual bool read_data();

    /**
     * @brief Read a bit from the data pin.
     * @return true if the bit is 1, false otherwise.
     */
    virtual bool read_bit();

    /**
     * @brief Wait for an acknowledgement from the slave.
     * @return SusiMasterResult The result of the operation.
     */
    virtual SusiMasterResult waitForAck();

    /**
     * @brief Send an acknowledgement to the master.
     */
    virtual void sendAck();

    /**
     * @brief Send a byte to the master.
     * @param byte The byte to send.
     */
    virtual void sendByte(uint8_t byte);

    /**
     * @brief Get the clock pin number.
     * @return uint8_t The clock pin number.
     */
    uint8_t get_clock_pin() const { return _clock_pin; }

private:
    uint8_t _clock_pin;
    uint8_t _data_pin;
};

#endif // SUSI_HAL_H
