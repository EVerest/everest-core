/******************************************************************************
 *
 *  Copyright (C) 2010-2014 Broadcom Corporation
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
 *  NFA reader/writer API functions
 *
 ******************************************************************************/
#ifndef NFA_RW_API_H
#define NFA_RW_API_H

#include "nfa_api.h"
#include "nfc_target.h"

#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         NFA_SetReconnectState
**
** Description:
**      This function enable/disable p2p prio logic if re-connect is in progress
**
** Returns:
**      void
*******************************************************************************/
NFC_API extern void NFA_SetReconnectState (BOOLEAN flag);

/*******************************************************************************
**
** Function         NFA_SetEmvCoState
**
** Description:
**      This function enable/disable p2p prio logic if emvco polling is enabled.
**
** Returns:
**      void
*******************************************************************************/
NFC_API extern void NFA_SetEmvCoState (BOOLEAN flag);

#endif

/*****************************************************************************
**  Constants and data types
*****************************************************************************/
enum {
  NFA_RW_PRES_CHK_DEFAULT,        /* The default behavior             */
  NFA_RW_PRES_CHK_I_BLOCK,        /* Empty I Block                    */
  NFA_RW_PRES_CHK_ISO_DEP_NAK = 5 /* presence check command ISO-DEP
                                 NAK as per NCI2.0                */
};
typedef uint8_t tNFA_RW_PRES_CHK_OPTION;

/*****************************************************************************
**  NFA T3T Constants and definitions
*****************************************************************************/

/* Block descriptor. (For non-NDEF read/write */
typedef struct {
  uint16_t service_code; /* Service code for the block   */
  uint16_t block_number; /* Block number.                */
} tNFA_T3T_BLOCK_DESC;

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/

/*******************************************************************************
**
** Function         NFA_RwDetectNDef
**
** Description      Perform the NDEF detection procedure  using the appropriate
**                  method for the currently activated tag.
**
**                  Upon successful completion of NDEF detection, a
**                  NFA_NDEF_DETECT_EVT will be sent, to notify the application
**                  of the NDEF attributes (NDEF total memory size, current
**                  size, etc.).
**
**                  It is not mandatory to call this function -  NFA_RwReadNDef
**                  and NFA_RwWriteNDef will perform NDEF detection internally
**                  if not performed already. This API may be called to get a
**                  tag's NDEF size before issuing a write-request.
**
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFC_STATUS_REFUSED if tag does not support NDEF
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwDetectNDef(void);

/*******************************************************************************
**
** Function         NFA_RwReadNDef
**
** Description      Read NDEF message from tag. This function will internally
**                  perform the NDEF detection procedure (if not performed
**                  previously), and read the NDEF tag data using the
**                  appropriate method for the currently activated tag.
**
**                  Upon successful completion of NDEF detection (if performed),
**                  a NFA_NDEF_DETECT_EVT will be sent, to notify the
**                  application of the NDEF attributes (NDEF total memory size,
**                  current size, etc.).
**
**                  Upon receiving the NDEF message, the message will be sent to
**                  the handler registered with NFA_RegisterNDefTypeHandler or
**                  NFA_RequestExclusiveRfControl (if exclusive RF mode is
**                  active)
**
**
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFC_STATUS_REFUSED if tag does not support NDEF
**                  NFC_STATUS_NOT_INITIALIZED if NULL NDEF was detected on the
**                  tag
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwReadNDef(void);

/*******************************************************************************
**
** Function         NFA_RwWriteNDef
**
** Description      Write NDEF data to the activated tag. This function will
**                  internally perform NDEF detection if necessary, and write
**                  the NDEF tag data using the appropriate method for the
**                  currently activated tag.
**
**                  When the entire message has been written, or if an error
**                  occurs, the app will be notified with NFA_RW_WRITE_CPLT_EVT.
**
**                  p_data needs to be persistent until NFA_RW_WRITE_CPLT_EVT
**
**
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFC_STATUS_REFUSED if tag does not support NDEF/locked
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwWriteNDef(uint8_t* p_data, uint32_t len);

/*****************************************************************************
**
** Function         NFA_RwPresenceCheck
**
** Description      Check if the tag is still in the field.
**
**                  The NFA_RW_PRESENCE_CHECK_EVT w/ status is used to
**                  indicate presence or non-presence.
**
**                  option is used only with ISO-DEP protocol
**
** Returns
**                  NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*****************************************************************************/
extern tNFA_STATUS NFA_RwPresenceCheck(tNFA_RW_PRES_CHK_OPTION option);

/*****************************************************************************
**
** Function         NFA_RwFormatTag
**
** Description      Check if the tag is NDEF Formatable. If yes Format the
**                  tag
**
**                  The NFA_RW_FORMAT_CPLT_EVT w/ status is used to
**                  indicate if tag is formated or not.
**
** Returns
**                  NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*****************************************************************************/
extern tNFA_STATUS NFA_RwFormatTag(void);

/*******************************************************************************
** LEGACY / PROPRIETARY TAG READ AND WRITE APIs
*******************************************************************************/

/*******************************************************************************
**
** Function         NFA_RwLocateTlv
**
** Description:
**      Search for the Lock/Memory contril TLV on the activated Type1/Type2 tag
**
**      Data is returned to the application using the NFA_TLV_DETECT_EVT. When
**      search operation has completed, or if an error occurs, the app will be
**      notified with NFA_TLV_DETECT_EVT.
**
** Description      Perform the TLV detection procedure  using the appropriate
**                  method for the currently activated tag.
**
**                  Upon successful completion of TLV detection in T1/T2 tag, a
**                  NFA_TLV_DETECT_EVT will be sent, to notify the application
**                  of the TLV attributes (total lock/reserved bytes etc.).
**                  However if the TLV type specified is NDEF then it is same as
**                  calling NFA_RwDetectNDef and should expect to receive
**                  NFA_NDEF_DETECT_EVT instead of NFA_TLV_DETECT_EVT
**
**                  It is not mandatory to call this function -
**                  NFA_RwDetectNDef, NFA_RwReadNDef and NFA_RwWriteNDef will
**                  perform TLV detection internally if not performed already.
**                  An application may call this API to check the a
**                  tag/card-emulator's total Reserved/ Lock bytes before
**                  issuing a write-request.
**
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFC_STATUS_REFUSED if tlv_type is NDEF & tag won't support
**                  NDEF
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwLocateTlv(uint8_t tlv_type);

/*******************************************************************************
**
** Function         NFA_RwSetTagReadOnly
**
** Description:
**      Sets tag as read only.
**
**      When tag is set as read only, or if an error occurs, the app will be
**      notified with NFA_SET_TAG_RO_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_REJECTED if protocol is not T1/T2/ISO15693
**                 (or) if hard lock is not requested for protocol ISO15693
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwSetTagReadOnly(bool b_hard_lock);

/*******************************************************************************
**
** Function         NFA_RwT1tRid
**
** Description:
**      Send a RID command to the activated Type 1 tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_READ_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_NOT_INITIALIZED: type 1 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwT1tRid(void);

/*******************************************************************************
**
** Function         NFA_RwT1tReadAll
**
** Description:
**      Send a RALL command to the activated Type 1 tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_READ_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_NOT_INITIALIZED: type 1 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwT1tReadAll(void);

/*******************************************************************************
**
** Function         NFA_RwT1tRead
**
** Description:
**      Send a READ command to the activated Type 1 tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_READ_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_NOT_INITIALIZED: type 1 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwT1tRead(uint8_t block_number, uint8_t index);

/*******************************************************************************
**
** Function         NFA_RwT1tWrite
**
** Description:
**      Send a WRITE command to the activated Type 1 tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      write operation has completed, or if an error occurs, the app will be
**      notified with NFA_WRITE_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_NOT_INITIALIZED: type 1 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwT1tWrite(uint8_t block_number, uint8_t index,
                                  uint8_t data, bool b_erase);

/*******************************************************************************
**
** Function         NFA_RwT1tReadSeg
**
** Description:
**      Send a RSEG command to the activated Type 1 tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_READ_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_NOT_INITIALIZED: type 1 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwT1tReadSeg(uint8_t segment_number);

/*******************************************************************************
**
** Function         NFA_RwT1tRead8
**
** Description:
**      Send a READ8 command to the activated Type 1 tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_READ_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_NOT_INITIALIZED: type 1 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwT1tRead8(uint8_t block_number);

/*******************************************************************************
**
** Function         NFA_RwT1tWrite8
**
** Description:
**      Send a WRITE8_E / WRITE8_NE command to the activated Type 1 tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_READ_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_NOT_INITIALIZED: type 1 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwT1tWrite8(uint8_t block_number, uint8_t* p_data,
                                   bool b_erase);

/*******************************************************************************
**
** Function         NFA_RwT2tRead
**
** Description:
**      Send a READ command to the activated Type 2 tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_READ_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_NOT_INITIALIZED: type 2 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwT2tRead(uint8_t block_number);

/*******************************************************************************
**
** Function         NFA_RwT2tWrite
**
** Description:
**      Send an WRITE command to the activated Type 2 tag.
**
**      When the write operation has completed (or if an error occurs), the
**      app will be notified with NFA_WRITE_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_NOT_INITIALIZED: type 2 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwT2tWrite(uint8_t block_number, uint8_t* p_data);

/*******************************************************************************
**
** Function         NFA_RwT2tSectorSelect
**
** Description:
**      Send SECTOR SELECT command to the activated Type 2 tag.
**
**      When the sector select operation has completed (or if an error occurs),
**      the app will be notified with NFA_SECTOR_SELECT_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_NOT_INITIALIZED: type 2 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwT2tSectorSelect(uint8_t sector_number);

/*******************************************************************************
**
** Function         NFA_RwT3tRead
**
** Description:
**      Send a CHECK (read) command to the activated Type 3 tag.
**
**      Data is returned to the application using the NFA_RW_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_READ_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_NOT_INITIALIZED: type 3 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwT3tRead(uint8_t num_blocks,
                                 tNFA_T3T_BLOCK_DESC* t3t_blocks);

/*******************************************************************************
**
** Function         NFA_RwT3tWrite
**
** Description:
**      Send an UPDATE (write) command to the activated Type 3 tag.
**
**      When the write operation has completed (or if an error occurs), the
**      app will be notified with NFA_WRITE_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_NOT_INITIALIZED: type 3 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwT3tWrite(uint8_t num_blocks,
                                  tNFA_T3T_BLOCK_DESC* t3t_blocks,
                                  uint8_t* p_data);

/*******************************************************************************
**
** Function         NFA_RwI93Inventory
**
** Description:
**      Send Inventory command to the activated ISO 15693 tag with/without AFI..
**      If UID is provided then set UID[0]:MSB, ... UID[7]:LSB
**
**      When the write operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_NOT_INITIALIZED: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93Inventory(bool afi_present, uint8_t afi,
                                      uint8_t* p_uid);

/*******************************************************************************
**
** Function         NFA_RwI93StayQuiet
**
** Description:
**      Send Stay Quiet command to the activated ISO 15693 tag.
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93StayQuiet(void);

/*******************************************************************************
**
** Function         NFA_RwI93ReadSingleBlock
**
** Description:
**      Send Read Single Block command to the activated ISO 15693 tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93ReadSingleBlock(uint8_t block_number);

/*******************************************************************************
**
** Function         NFA_RwI93WriteSingleBlock
**
** Description:
**      Send Write Single Block command to the activated ISO 15693 tag.
**
**      When the write operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93WriteSingleBlock(uint8_t block_number,
                                             uint8_t* p_data);

/*******************************************************************************
**
** Function         NFA_RwI93LockBlock
**
** Description:
**      Send Lock block command to the activated ISO 15693 tag.
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93LockBlock(uint8_t block_number);

/*******************************************************************************
**
** Function         NFA_RwI93ReadMultipleBlocks
**
** Description:
**      Send Read Multiple Block command to the activated ISO 15693 tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93ReadMultipleBlocks(uint8_t first_block_number,
                                               uint16_t number_blocks);

/*******************************************************************************
**
** Function         NFA_RwI93WriteMultipleBlocks
**
** Description:
**      Send Write Multiple Block command to the activated ISO 15693 tag.
**
**      When the write operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93WriteMultipleBlocks(uint8_t first_block_number,
                                                uint16_t number_blocks,
                                                uint8_t* p_data);

/*******************************************************************************
**
** Function         NFA_RwI93Select
**
** Description:
**      Send Select command to the activated ISO 15693 tag.
**
**      UID[0]: 0xE0, MSB
**      UID[1]: IC Mfg Code
**      ...
**      UID[7]: LSB
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93Select(uint8_t* p_uid);

/*******************************************************************************
**
** Function         NFA_RwI93ResetToReady
**
** Description:
**      Send Reset to ready command to the activated ISO 15693 tag.
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93ResetToReady(void);

/*******************************************************************************
**
** Function         NFA_RwI93WriteAFI
**
** Description:
**      Send Write AFI command to the activated ISO 15693 tag.
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93WriteAFI(uint8_t afi);

/*******************************************************************************
**
** Function         NFA_RwI93LockAFI
**
** Description:
**      Send Lock AFI command to the activated ISO 15693 tag.
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93LockAFI(void);

/*******************************************************************************
**
** Function         NFA_RwI93WriteDSFID
**
** Description:
**      Send Write DSFID command to the activated ISO 15693 tag.
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93WriteDSFID(uint8_t dsfid);

/*******************************************************************************
**
** Function         NFA_RwI93LockDSFID
**
** Description:
**      Send Lock DSFID command to the activated ISO 15693 tag.
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93LockDSFID(void);

/*******************************************************************************
**
** Function         NFA_RwI93GetSysInfo
**
** Description:
**      Send Get system information command to the activated ISO 15693 tag.
**      If UID is provided then set UID[0]:MSB, ... UID[7]:LSB
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93GetSysInfo(uint8_t* p_uid);

/*******************************************************************************
**
** Function         NFA_RwI93GetMultiBlockSecurityStatus
**
** Description:
**      Send Get Multiple block security status command to the activated ISO
**      15693 tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: ISO 15693 tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_RwI93GetMultiBlockSecurityStatus(
    uint8_t first_block_number, uint16_t number_blocks);

#endif /* NFA_RW_API_H */
