#ifndef SUSI_COMMANDS_H
#define SUSI_COMMANDS_H

/**
 * @file susi_commands.h
 * @brief This file contains the definitions of the SUSI commands.
 * @see RCN-600
 */

/**
 * @brief The SUSI command to set a function (Function Group 1).
 * @see RCN-600
 */
const uint8_t SUSI_CMD_SET_FUNCTION = 0x60;

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
 * @brief The SUSI command for a BiDi Host Call.
 * @see RCN-601
 */
const uint8_t SUSI_CMD_BIDI_HOST_CALL = 0x01;

/**
 * @brief The SUSI response message for a BiDi idle state.
 * @see RCN-601
 */
const uint8_t SUSI_MSG_BIDI_IDLE = 0x8A;

#endif // SUSI_COMMANDS_H
