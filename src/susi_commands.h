#ifndef SUSI_COMMANDS_H
#define SUSI_COMMANDS_H

/**
 * @file susi_commands.h
 * @brief This file contains the definitions of the SUSI commands.
 * @see RCN-600
 */

/**
 * @brief The SUSI command to set a function.
 */
const uint8_t SUSI_CMD_SET_FUNCTION = 0x01;

/**
 * @brief The SUSI command to set the speed.
 */
const uint8_t SUSI_CMD_SET_SPEED = 0x02;

/**
 * @brief The SUSI command to write a CV.
 */
const uint8_t SUSI_CMD_WRITE_CV = 0x04;

/**
 * @brief The SUSI command to read a CV.
 */
const uint8_t SUSI_CMD_READ_CV = 0x03;

/**
 * @brief The SUSI command to request bidirectional communication.
 */
const uint8_t SUSI_CMD_BIDIRECTIONAL_REQUEST = 0x05;

/**
 * @brief The SUSI command to poll a bidirectional slave.
 */
const uint8_t SUSI_CMD_BIDIRECTIONAL_POLL = 0x06;

#endif // SUSI_COMMANDS_H
