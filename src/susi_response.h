#ifndef SUSI_RESPONSE_H
#define SUSI_RESPONSE_H

/**
 * @brief This enum represents the possible results of a SUSI master operation.
 */
enum SusiMasterResult {
    /**
     * @brief The operation was successful.
     */
    SUCCESS,
    /**
     * @brief The operation timed out.
     */
    TIMEOUT,
    /**
     * @brief An invalid acknowledgement was received.
     */
    INVALID_ACK,
    /**
     * @brief The slave already exists.
     */
    SLAVE_ALREADY_EXISTS,
    /**
     * @brief The slave list is full.
     */
    SLAVE_LIST_FULL,
    /**
     * @brief An invalid CRC was received.
     */
    INVALID_CRC
};

#endif // SUSI_RESPONSE_H