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
 * Firmware Download Status Values
 */
#ifndef PHDNLDNFC_STATUS_H
#define PHDNLDNFC_STATUS_H

#include <phNfcStatus.h>

/* reusing LibNfcStatus.h value below as a placeholder for now, need to find
   the right value */
#define NFCSTATUS_ABORTED (0x0096) /* Command aborted */

/*
 * Enum definition contains Firmware Download Status codes
 */
typedef enum phDnldNfc_Status {
  PH_DL_STATUS_PLL_ERROR = 0x0D,
  PH_DL_STATUS_LC_WRONG = 0x13,
  PH_DL_STATUS_LC_TERMINATION_NOT_SUPPORTED = 0x14,
  PH_DL_STATUS_LC_CREATION_NOT_SUPPORTED = 0x15,
  PH_DL_STATUS_LC_UNKNOWN = 0x16,
  PH_DL_STATUS_AUTHENTICATION_ERROR = 0x19,
  PH_DL_STATUS_NOT_AUTHENTICATED = 0x1A,
  PH_DL_STATUS_AUTHENTICATION_LOST = 0x1B,
  PH_DL_STATUS_WRITE_PROTECTED = 0x1C,
  PH_DL_STATUS_READ_PROTECTED = 0x1D,
  PH_DL_STATUS_ADDR_RANGE_OFL_ERROR = 0x1E,
  PH_DL_STATUS_BUFFER_OFL_ERROR = 0x1F,
  PH_DL_STATUS_MEM_BSY = 0x20,
  PH_DL_STATUS_SIGNATURE_ERROR = 0x21,
  PH_DL_STATUS_SESSION_WAS_OPEN = 0x22,
  PH_DL_STATUS_SESSION_WAS_CLOSED = 0x23,
  /* the Firmware version passed to CommitSession is not greater than
      the EEPROM resident stored Firmware version number */
  PH_DL_STATUS_FIRMWARE_VERSION_ERROR = 0x24,
  PH_DL_STATUS_LOOPBACK_DATA_MISSMATCH_ERROR = 0x25,
  /*****************************/
  PH_DL_STATUS_HOST_PAYLOAD_UFL_ERROR = 0x26,
  PH_DL_STATUS_HOST_PAYLOAD_OFL_ERROR = 0x27,
  PH_DL_STATUS_PROTOCOL_ERROR = 0x28,
  /* Download codes re-mapped to generic entries */
  PH_DL_STATUS_INVALID_ADDR = NFCSTATUS_INVALID_PARAMETER,
  PH_DL_STATUS_GENERIC_ERROR = NFCSTATUS_FAILED,
  PH_DL_STATUS_ABORTED_CMD = NFCSTATUS_ABORTED,
  PH_DL_STATUS_FLASH_WRITE_PROTECTED = PH_DL_STATUS_WRITE_PROTECTED,
  PH_DL_STATUS_FLASH_READ_PROTECTED = PH_DL_STATUS_READ_PROTECTED,
  PH_DL_STATUS_USERDATA_WRITE_PROTECTED = PH_DL_STATUS_WRITE_PROTECTED,
  PH_DL_STATUS_USERDATA_READ_PROTECTED = PH_DL_STATUS_READ_PROTECTED,
  PH_DL_STATUS_OK = NFCSTATUS_SUCCESS
} phDnldNfc_Status_t;

#endif /* PHDNLDNFC_STATUS_H */
