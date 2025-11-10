#ifndef SUSI_PACKET_H
#define SUSI_PACKET_H

#include <stdint.h>

// This struct is shared between the Master and Slave modules.
struct SUSI_Packet {
    uint8_t address;
    uint8_t command;
    uint8_t data;
};

#endif // SUSI_PACKET_H
