/*
 * Copyright (C) 2010-2014 NXP Semiconductors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * NFC Status Values - Function Return Codes
 */

#ifndef PHNFCSTATUS_H
#define PHNFCSTATUS_H

#include <phNfcTypes.h>

/* Internally required by PHNFCSTVAL. */
#define PHNFCSTSHL8 (8U)
/* Required by PHNFCSTVAL. */
#define PHNFCSTBLOWER ((NFCSTATUS)(0x00FFU))

/*
 *  NFC Status Composition Macro
 *
 *  This is the macro which must be used to compose status values.
 *
 *  phNfcCompID Component ID, as defined in phNfcCompId.h .
 *  phNfcStatus Status values, as defined in phNfcStatus.h .
 *
 *  The macro is not required for the NFCSTATUS_SUCCESS value.
 *  This is the only return value to be used directly.
 *  For all other values it shall be used in assignment and conditional
 * statements, e.g.:
 *     NFCSTATUS status = PHNFCSTVAL(phNfcCompID, phNfcStatus); ...
 *     if (status == PHNFCSTVAL(phNfcCompID, phNfcStatus)) ...
 */
#define PHNFCSTVAL(phNfcCompID, phNfcStatus)               \
  (((phNfcStatus) == (NFCSTATUS_SUCCESS))                  \
       ? (NFCSTATUS_SUCCESS)                               \
       : ((((NFCSTATUS)(phNfcStatus)) & (PHNFCSTBLOWER)) | \
          (((uint16_t)(phNfcCompID)) << (PHNFCSTSHL8))))

/*
 * PHNFCSTATUS
 * Get grp_retval from Status Code
 */
#define PHNFCSTATUS(phNfcStatus) ((phNfcStatus)&0x00FFU)
#define PHNFCCID(phNfcStatus) (((phNfcStatus)&0xFF00U) >> 8)

#define PHNFC_I2C_FRAGMENT_SIZE 512
/*
 *  Status Codes
 *
 *  Generic Status codes for the NFC components. Combined with the Component ID
 *  they build the value (status) returned by each function.
 *  Example:
 *      grp_comp_id "Component ID" -  e.g. 0x10, plus
 *      status code as listed in this file - e.g. 0x03
 *      result in a status value of 0x0003.
 */

/*
 * The function indicates successful completion
 */
#define NFCSTATUS_SUCCESS (0x0000)

/*
 *  The function indicates successful completion
 */
#define NFCSTATUS_OK (NFCSTATUS_SUCCESS)

/*
 * At least one parameter could not be properly interpreted
 */
#define NFCSTATUS_INVALID_PARAMETER (0x0001)

/*
 * The buffer provided by the caller is too small
 */
#define NFCSTATUS_BUFFER_TOO_SMALL (0x0003)

/*
 * Device specifier/handle value is invalid for the operation
 */
#define NFCSTATUS_INVALID_DEVICE (0x0006)

/*
 * The function executed successfully but could have returned
 * more information than space provided by the caller
 */
#define NFCSTATUS_MORE_INFORMATION (0x0008)

/*
 * No response from the remote device received: Time-out
 */
#define NFCSTATUS_RF_TIMEOUT (0x0009)

/*
 * RF Error during data transaction with the remote device
 */
#define NFCSTATUS_RF_ERROR (0x000A)

/*
 * Not enough resources Memory, Timer etc(e.g. allocation failed.)
 */
#define NFCSTATUS_INSUFFICIENT_RESOURCES (0x000C)

/*
 * A non-blocking function returns this immediately to indicate
 * that an internal operation is in progress
 */
#define NFCSTATUS_PENDING (0x000D)

/*
 * A board communication error occurred
 * (e.g. Configuration went wrong)
 */
#define NFCSTATUS_BOARD_COMMUNICATION_ERROR (0x000F)

/*
 * Invalid State of the particular state machine
 */
#define NFCSTATUS_INVALID_STATE (0x0011)

/*
 * This Layer is Not initialized, hence initialization required.
 */
#define NFCSTATUS_NOT_INITIALISED (0x0031)

/*
 * The Layer is already initialized, hence initialization repeated.
 */
#define NFCSTATUS_ALREADY_INITIALISED (0x0032)

/*
 * Feature not supported
 */
#define NFCSTATUS_FEATURE_NOT_SUPPORTED (0x0033)

/*  The Unregistration command has failed because the user wants to unregister
 * on
 * an element for which he was not registered
 */
#define NFCSTATUS_NOT_REGISTERED (0x0034)

/* The Registration command has failed because the user wants to register on
 * an element for which he is already registered
 */
#define NFCSTATUS_ALREADY_REGISTERED (0x0035)

/*  Single Tag with Multiple
    Protocol support detected */
#define NFCSTATUS_MULTIPLE_PROTOCOLS (0x0036)

/*
 * Feature not supported
 */
#define NFCSTATUS_MULTIPLE_TAGS (0x0037)

/*
 * A DESELECT event has occurred
 */
#define NFCSTATUS_DESELECTED (0x0038)

/*
 * A RELEASE event has occurred
 */
#define NFCSTATUS_RELEASED (0x0039)

/*
 * The operation is currently not possible or not allowed
 */
#define NFCSTATUS_NOT_ALLOWED (0x003A)

/*
 * FW version error while performing FW download,
 * FW major version mismatch (cannot downgrade FW major version) or FW version
 * already upto date
 * User may be trying to flash Mobile FW on top of Infra FW, which is not
 * allowed
 * Download appropriate version of FW
 */
#define NFCSTATUS_FW_VERSION_ERROR (0x003C)

/*
 *  The system is busy with the previous operation.
 */
#define NFCSTATUS_BUSY (0x006F)

/* NDEF Mapping error codes */

/* The remote device (type) is not valid for this request. */
#define NFCSTATUS_INVALID_REMOTE_DEVICE (0x001D)

/* Read operation failed */
#define NFCSTATUS_READ_FAILED (0x0014)

/*
 * Write operation failed
 */
#define NFCSTATUS_WRITE_FAILED (0x0015)

/* Non Ndef Compliant */
#define NFCSTATUS_NO_NDEF_SUPPORT (0x0016)

/* Could not proceed further with the write operation: reached card EOF*/
#define NFCSTATUS_EOF_NDEF_CONTAINER_REACHED (0x001A)

/* Incorrect number of bytes received from the card*/
#define NFCSTATUS_INVALID_RECEIVE_LENGTH (0x001B)

/* The data format/composition is not understood/correct. */
#define NFCSTATUS_INVALID_FORMAT (0x001C)

/* There is not sufficient storage available. */
#define NFCSTATUS_INSUFFICIENT_STORAGE (0x001F)

/* The Ndef Format procedure has failed. */
#define NFCSTATUS_FORMAT_ERROR (0x0023)

/* The NCI Cedit error */
#define NFCSTATUS_CREDIT_TIMEOUT (0x0024)

/*
 * Response Time out for the control message(NFCC not responded)
 */
#define NFCSTATUS_RESPONSE_TIMEOUT (0x0025)

/*
 * Device is already connected
 */
#define NFCSTATUS_ALREADY_CONNECTED (0x0026)

/*
 * Device is already connected
 */
#define NFCSTATUS_ANOTHER_DEVICE_CONNECTED (0x0027)

/*
 * Single Target Detected and Activated
 */
#define NFCSTATUS_SINGLE_TAG_ACTIVATED (0x0028)

/*
 * Single Target Detected
 */
#define NFCSTATUS_SINGLE_TAG_DISCOVERED (0x0029)

/*
 * Secure element Detected and Activated
 */
#define NFCSTATUS_SECURE_ELEMENT_ACTIVATED (0x0028)

/*
 * Unknown error Status Codes
 */
#define NFCSTATUS_UNKNOWN_ERROR (0x00FE)

/*
 * Status code for failure
 */
#define NFCSTATUS_FAILED (0x00FF)

/*
 * The function/command has been aborted
 */
#define NFCSTATUS_CMD_ABORTED (0x0002)

/*
 * No target found after poll
 */
#define NFCSTATUS_NO_TARGET_FOUND (0x000A)

/* Attempt to disconnect a not connected remote device. */
#define NFCSTATUS_NO_DEVICE_CONNECTED (0x000B)

/* External RF field detected. */
#define NFCSTATUS_EXTERNAL_RF_DETECTED (0x000E)

/* Message is not allowed by the state machine
 * (e.g. configuration went wrong)
 */
#define NFCSTATUS_MSG_NOT_ALLOWED_BY_FSM (0x0010)

/*
 * No access has been granted
 */
#define NFCSTATUS_ACCESS_DENIED (0x001E)

/* No registry node matches the specified input data. */
#define NFCSTATUS_NODE_NOT_FOUND (0x0017)

/* The current module is busy ; one might retry later */
#define NFCSTATUS_SMX_BAD_STATE (0x00F0)

/* The Abort mechanism has failed for unexpected reason: user can try again*/
#define NFCSTATUS_ABORT_FAILED (0x00F2)

/* The Registration command has failed because the user wants to register as
 * target
 * on a operating mode not supported
 */
#define NFCSTATUS_REG_OPMODE_NOT_SUPPORTED (0x00F5)

/*
 * Shutdown in progress, cannot handle the request at this time.
 */
#define NFCSTATUS_SHUTDOWN (0x0091)

/*
 * Target is no more in RF field
 */
#define NFCSTATUS_TARGET_LOST (0x0092)

/*
 * Request is rejected
 */
#define NFCSTATUS_REJECTED (0x0093)

/*
 * Target is not connected
 */
#define NFCSTATUS_TARGET_NOT_CONNECTED (0x0094)

/*
 * Invalid handle for the operation
 */
#define NFCSTATUS_INVALID_HANDLE (0x0095)

/*
 * Process aborted
 */
#define NFCSTATUS_ABORTED (0x0096)

/*
 * Requested command is not supported
 */
#define NFCSTATUS_COMMAND_NOT_SUPPORTED (0x0097)

/*
 * Tag is not NDEF compilant
 */
#define NFCSTATUS_NON_NDEF_COMPLIANT (0x0098)

/*
 * Not enough memory available to complete the requested operation
 */
#define NFCSTATUS_NOT_ENOUGH_MEMORY (0x001F)

/*
 * Indicates incoming connection
 */
#define NFCSTATUS_INCOMING_CONNECTION (0x0045)

/*
 * Indicates Connection was successful
 */
#define NFCSTATUS_CONNECTION_SUCCESS (0x0046)

/*
 * Indicates Connection failed
 */
#define NFCSTATUS_CONNECTION_FAILED (0x0047)

#endif /* PHNFCSTATUS_H */
