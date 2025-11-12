#ifndef SUSI_PACKET_H
#define SUSI_PACKET_H

#include <stdint.h>

/**
 * @brief This struct represents a SUSI packet.
 * @details This struct is shared between the Master and Slave modules.
 * @see RCN-600
 */
struct SUSI_Packet {
    /**
     * @brief The address of the slave.
     */
    uint8_t address;
    /**
     * @brief The command to be executed.
     */
    uint8_t command;
    /**
     * @brief The data associated with the command.
     */
    uint8_t data;
};

#endif // SUSI_PACKET_H
