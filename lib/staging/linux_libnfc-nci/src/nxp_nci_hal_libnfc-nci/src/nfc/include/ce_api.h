/******************************************************************************
 *
 *  Copyright (C) 2009-2014 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  This file contains the Near Field Communication (NFC) Card Emulation
 *  mode related API function external definitions.
 *
 ******************************************************************************/

#ifndef CE_API_H
#define CE_API_H

#include <stdbool.h>
#include "nfc_api.h"
#include "nfc_types.h"
#include "tags_defs.h"

#define CE_T3T_FIRST_EVT 0x60
#define CE_T4T_FIRST_EVT 0x80

enum {
  CE_T3T_NDEF_UPDATE_START_EVT = CE_T3T_FIRST_EVT,
  CE_T3T_NDEF_UPDATE_CPLT_EVT,
  CE_T3T_UPDATE_EVT,
  CE_T3T_RAW_FRAME_EVT,

  CE_T4T_NDEF_UPDATE_START_EVT = CE_T4T_FIRST_EVT,
  CE_T4T_NDEF_UPDATE_CPLT_EVT,
  CE_T4T_NDEF_UPDATE_ABORT_EVT,
  CE_T4T_RAW_FRAME_EVT
};

typedef uint8_t tCE_EVENT;

typedef struct {
  tNFC_STATUS status;
  uint8_t* p_data;
  bool b_updated;
  uint32_t length;
} tCE_UPDATE_INFO;

typedef struct {
  tNFC_STATUS status;
  uint8_t aid_handle;
  NFC_HDR* p_data;
} tCE_RAW_FRAME;

typedef union {
  tNFC_STATUS status;
  tCE_UPDATE_INFO update_info;
  tCE_RAW_FRAME raw_frame;
} tCE_DATA;

typedef void(tCE_CBACK)(tCE_EVENT event, tCE_DATA* p_data);

/* T4T definitions */
typedef uint8_t tCE_T4T_AID_HANDLE; /* Handle for AID registration  */
/* Invalid tCE_T4T_AID_HANDLE               */
#define CE_T4T_AID_HANDLE_INVALID 0xFF
/* reserved handle for wildcard aid */
#define CE_T4T_WILDCARD_AID_HANDLE (CE_T4T_MAX_REG_AID)

/*******************************************************************************
**
** Function         CE_T3tSetLocalNDEFMsg
**
** Description      Initialise CE Type 3 Tag with mandatory NDEF message
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
extern tNFC_STATUS CE_T3tSetLocalNDEFMsg(bool read_only, uint32_t size_max,
                                         uint32_t size_current, uint8_t* p_buf,
                                         uint8_t* p_scratch_buf);

/*******************************************************************************
**
** Function         CE_T3tSetLocalNDefParams
**
** Description      Sets T3T-specific NDEF parameters. (Optional - if not
**                  called, then CE will use default parameters)
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
extern tNFC_STATUS CE_T3tSetLocalNDefParams(uint8_t nbr, uint8_t nbw);

/*******************************************************************************
**
** Function         CE_T3tSendCheckRsp
**
** Description      Send CHECK response message
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
extern tNFC_STATUS CE_T3tSendCheckRsp(uint8_t status1, uint8_t status2,
                                      uint8_t num_blocks,
                                      uint8_t* p_block_data);

/*******************************************************************************
**
** Function         CE_T3tSendUpdateRsp
**
** Description      Send UPDATE response message
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
extern tNFC_STATUS CE_T3tSendUpdateRsp(uint8_t status1, uint8_t status2);

/*******************************************************************************
**
** Function         CE_T4tSetLocalNDEFMsg
**
** Description      Initialise CE Type 4 Tag with mandatory NDEF message
**
**                  The following event may be returned
**                      CE_T4T_UPDATE_START_EVT for starting update
**                      CE_T4T_UPDATE_CPLT_EVT for complete update
**                      CE_T4T_UPDATE_ABORT_EVT for failure of update
**                      CE_T4T_RAW_FRAME_EVT for raw frame
**
**                  read_only:      TRUE if read only
**                  ndef_msg_max:   Max NDEF message size
**                  ndef_msg_len:   NDEF message size
**                  p_ndef_msg:     NDEF message (excluding NLEN)
**                  p_scratch_buf:  temp storage for update
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
extern tNFC_STATUS CE_T4tSetLocalNDEFMsg(bool read_only, uint16_t ndef_msg_max,
                                         uint16_t ndef_msg_len,
                                         uint8_t* p_ndef_msg,
                                         uint8_t* p_scratch_buf);

/*******************************************************************************
**
** Function         CE_T4tRegisterAID
**
** Description      Register AID in CE T4T
**
**                  aid_len: length of AID (up to NFC_MAX_AID_LEN)
**                  p_aid:   AID
**                  p_cback: Raw frame will be forwarded with CE_RAW_FRAME_EVT
**
** Returns          tCE_T4T_AID_HANDLE if successful,
**                  CE_T4T_AID_HANDLE_INVALID otherwisse
**
*******************************************************************************/
extern tCE_T4T_AID_HANDLE CE_T4tRegisterAID(uint8_t aid_len, uint8_t* p_aid,
                                            tCE_CBACK* p_cback);

/*******************************************************************************
**
** Function         CE_T4tDeregisterAID
**
** Description      Deregister AID in CE T4T
**
**                  aid_len: length of AID (up to NFC_MAX_AID_LEN)
**                  p_aid:   AID
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
extern void CE_T4tDeregisterAID(tCE_T4T_AID_HANDLE aid_handle);

/*******************************************************************************
**
** Function         CE_SendRawFrame
**
** Description      This function sends a raw frame to the peer device.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS CE_SendRawFrame(uint8_t* p_raw_data, uint16_t data_len);

/*******************************************************************************
**
** Function         CE_SetActivatedTagType
**
** Description      This function selects the tag type for Reader/Writer mode.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
extern tNFC_STATUS CE_SetActivatedTagType(tNFC_ACTIVATE_DEVT* p_activate_params,
                                          uint16_t t3t_system_code,
                                          tCE_CBACK* p_cback);
/*******************************************************************************
**
** Function         CE_SetTraceLevel
**
** Description      This function sets the trace level for Card Emulation mode.
**                  If called with a value of 0xFF,
**                  it simply returns the current trace level.
**
** Returns          The new or current trace level
**
*******************************************************************************/
NFC_API extern UINT8 CE_SetTraceLevel (UINT8 new_level);


#endif /* CE_API_H */
