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

/**
 * @brief The SUSI response message for a BiDi state response.
 * @see RCN-601
 */
const uint8_t SUSI_MSG_BIDI_SIGNAL_STATE = 0x80;
const uint8_t SUSI_MSG_BIDI_DIRECT_FUNCTION = 0x81;
const uint8_t SUSI_MSG_BIDI_FUNCTION_VALUE_DCC = 0x82;
const uint8_t SUSI_MSG_BIDI_SHORT_BINARY_STATES = 0x83;
const uint8_t SUSI_MSG_BIDI_AUTO_SPEED = 0x84;
const uint8_t SUSI_MSG_BIDI_AUTO_OPERATION = 0x85;
const uint8_t SUSI_MSG_BIDI_POSITION_HIGH = 0x88;
const uint8_t SUSI_MSG_BIDI_POSITION_LOW = 0x89;
const uint8_t SUSI_MSG_BIDI_ANALOG_A = 0x8C;
const uint8_t SUSI_MSG_BIDI_ANALOG_B = 0x8D;
const uint8_t SUSI_MSG_BIDI_STATE_RESPONSE = 0x8B;

/**
 * @brief The SUSI response message for a BiDi CV response.
 * @see RCN-601
 */
const uint8_t SUSI_MSG_BIDI_CV_RESPONSE = 0x8D;

/**
 * @brief The SUSI response message for a BiDi error.
 * @see RCN-601
 */
const uint8_t SUSI_MSG_BIDI_ERROR = 0x8E;

/**
 * @brief The SUSI response message for a BiDi bank response.
 * @see RCN-601
 */
const uint8_t SUSI_MSG_BIDI_BANK_RESPONSE = 0x8F;

/**
 * @brief The SUSI command to read CV bank 0.
 * @see RCN-601
 */
const uint8_t SUSI_CMD_READ_CV_BANK_0 = 0x0C;

/**
 * @brief The SUSI command to read CV bank 1.
 * @see RCN-601
 */
const uint8_t SUSI_CMD_READ_CV_BANK_1 = 0x0D;

/**
 * @brief The SUSI command to read CV bank 2.
 * @see RCN-601
 */
const uint8_t SUSI_CMD_READ_CV_BANK_2 = 0x0E;

// RCN-602 Specific CVs
const uint16_t CV_SUSI_MODULE_NUM = 897;
const uint16_t CV_MANUFACTURER_ID = 900;
const uint16_t CV_MANUFACTURER_ID_BANK_1 = 940;
const uint16_t CV_MANUFACTURER_ID_BANK_2 = 980;
const uint16_t CV_VERSION_NUM = 902;
const uint16_t CV_VERSION_NUM_BANK_1 = 942;
const uint16_t CV_VERSION_NUM_BANK_2 = 982;
const uint16_t CV_STATUS_BITS = 1020;
const uint16_t CV_SUSI_CV_BANKING = 1021;

// RCN-602 Status Bits
const uint8_t STATUS_BIT_WAIT = 0;
const uint8_t STATUS_BIT_SLOW = 1;
const uint8_t STATUS_BIT_HOLD = 2;
const uint8_t STATUS_BIT_STOP = 3;


#endif // SUSI_COMMANDS_H
