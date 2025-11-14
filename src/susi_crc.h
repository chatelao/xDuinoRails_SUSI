#ifndef SUSI_CRC_H
#define SUSI_CRC_H

#include <stdint.h>

/**
 * @brief Calculates the CRC-16-CCITT checksum for a block of data.
 * @param data A pointer to the data.
 * @param length The length of the data in bytes.
 * @return uint16_t The calculated CRC-16 checksum.
 */
uint16_t crc16_ccitt(const uint8_t* data, int length);

#endif // SUSI_CRC_H
