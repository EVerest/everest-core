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
 *  NFA interface for tag Reader/Writer
 *
 ******************************************************************************/
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nfa_api.h"
#include "nfa_rw_int.h"
#include "bt_trace.h"
using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/*****************************************************************************
**  Constants
*****************************************************************************/

/*****************************************************************************
**  APIs
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
tNFA_STATUS NFA_RwDetectNDef(void) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_DETECT_NDEF;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

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
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFC_STATUS_REFUSED if tag does not support NDEF
**                  NFC_STATUS_NOT_INITIALIZED if NULL NDEF was detected on the
**                  tag
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwReadNDef(void) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_READ_NDEF;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

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
**                  occurs, the app will be notified with NFA_WRITE_CPLT_EVT.
**
**                  p_data needs to be persistent until NFA_WRITE_CPLT_EVT
**
**
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFC_STATUS_REFUSED if tag does not support NDEF/locked
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwWriteNDef(uint8_t* p_data, uint32_t len) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("ndef len: %i", len);

  /* Validate parameters */
  if (p_data == nullptr) return (NFA_STATUS_INVALID_PARAM);

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_WRITE_NDEF;
    p_msg->params.write_ndef.len = len;
    p_msg->params.write_ndef.p_data = p_data;
    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

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
tNFA_STATUS NFA_RwPresenceCheck(tNFA_RW_PRES_CHK_OPTION option) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_PRESENCE_CHECK;
    p_msg->params.option = option;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*****************************************************************************
**
** Function         NFA_RwFormatTag
**
** Description      Check if the tag is NDEF Formatable. If yes Format the tag
**
**                  The NFA_RW_FORMAT_CPLT_EVT w/ status is used to
**                  indicate if tag is successfully formated or not
**
** Returns
**                  NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*****************************************************************************/
tNFA_STATUS NFA_RwFormatTag(void) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_FORMAT_TAG;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

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
**      NFA_STATUS_REJECTED if protocol is not T1/T2/T5T
**                 (or) if hard lock is not requested for protocol T5T
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwSetTagReadOnly(bool b_hard_lock) {
  tNFA_RW_OPERATION* p_msg;
  tNFC_PROTOCOL protocol = nfa_rw_cb.protocol;

  if ((protocol != NFC_PROTOCOL_T1T) && (protocol != NFC_PROTOCOL_T2T) &&
      (protocol != NFC_PROTOCOL_T5T) && (protocol != NFC_PROTOCOL_ISO_DEP) &&
      (protocol != NFC_PROTOCOL_T3T)) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "Cannot Configure as read only for Protocol: "
        "%d",
        protocol);
    return (NFA_STATUS_REJECTED);
  }

  if ((!b_hard_lock && (protocol == NFC_PROTOCOL_T5T)) ||
      (b_hard_lock && (protocol == NFC_PROTOCOL_ISO_DEP))) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("Cannot %s for Protocol: %d",
                        b_hard_lock ? "Hard lock" : "Soft lock", protocol);
    return (NFA_STATUS_REJECTED);
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s", b_hard_lock ? "Hard lock" : "Soft lock");

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_SET_TAG_RO;
    p_msg->params.set_readonly.b_hard_lock = b_hard_lock;

    nfa_sys_sendmsg(p_msg);
    return (NFA_STATUS_OK);
  }
  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
** Tag specific APIs
** (note: for Type-4 tags, use NFA_SendRawFrame to exchange APDUs)
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
**                  tag/card-emulator's total Reserved/
**                  Lock bytes before issuing a write-request.
**
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFC_STATUS_REFUSED if tlv_type is NDEF & tag won't support
**                  NDEF
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwLocateTlv(uint8_t tlv_type) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;

    if (tlv_type == TAG_LOCK_CTRL_TLV) {
      p_msg->op = NFA_RW_OP_DETECT_LOCK_TLV;
    } else if (tlv_type == TAG_MEM_CTRL_TLV) {
      p_msg->op = NFA_RW_OP_DETECT_MEM_TLV;
    } else if (tlv_type == TAG_NDEF_TLV) {
      p_msg->op = NFA_RW_OP_DETECT_NDEF;
    } else
      return (NFA_STATUS_FAILED);

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

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
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwT1tRid(void) {
  tNFA_RW_OPERATION* p_msg;

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_T1T_RID;

    nfa_sys_sendmsg(p_msg);
    return (NFA_STATUS_OK);
  }
  return (NFA_STATUS_FAILED);
}

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
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwT1tReadAll(void) {
  tNFA_RW_OPERATION* p_msg;

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_T1T_RALL;

    nfa_sys_sendmsg(p_msg);
    return (NFA_STATUS_OK);
  }
  return (NFA_STATUS_FAILED);
}

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
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwT1tRead(uint8_t block_number, uint8_t index) {
  tNFA_RW_OPERATION* p_msg;

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_T1T_READ;
    p_msg->params.t1t_read.block_number = block_number;
    p_msg->params.t1t_read.index = index;

    nfa_sys_sendmsg(p_msg);
    return (NFA_STATUS_OK);
  }
  return (NFA_STATUS_FAILED);
}

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
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwT1tWrite(uint8_t block_number, uint8_t index, uint8_t data,
                           bool b_erase) {
  tNFA_RW_OPERATION* p_msg;

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->params.t1t_write.b_erase = b_erase;
    p_msg->op = NFA_RW_OP_T1T_WRITE;
    p_msg->params.t1t_write.block_number = block_number;
    p_msg->params.t1t_write.index = index;
    p_msg->params.t1t_write.p_block_data[0] = data;

    nfa_sys_sendmsg(p_msg);
    return (NFA_STATUS_OK);
  }
  return (NFA_STATUS_FAILED);
}

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
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwT1tReadSeg(uint8_t segment_number) {
  tNFA_RW_OPERATION* p_msg;

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_T1T_RSEG;
    p_msg->params.t1t_read.segment_number = segment_number;

    nfa_sys_sendmsg(p_msg);
    return (NFA_STATUS_OK);
  }
  return (NFA_STATUS_FAILED);
}

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
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwT1tRead8(uint8_t block_number) {
  tNFA_RW_OPERATION* p_msg;

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_T1T_READ8;
    p_msg->params.t1t_write.block_number = block_number;

    nfa_sys_sendmsg(p_msg);
    return (NFA_STATUS_OK);
  }
  return (NFA_STATUS_FAILED);
}

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
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwT1tWrite8(uint8_t block_number, uint8_t* p_data,
                            bool b_erase) {
  tNFA_RW_OPERATION* p_msg;

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->params.t1t_write.b_erase = b_erase;
    p_msg->op = NFA_RW_OP_T1T_WRITE8;
    p_msg->params.t1t_write.block_number = block_number;

    memcpy(p_msg->params.t1t_write.p_block_data, p_data, 8);

    nfa_sys_sendmsg(p_msg);
    return (NFA_STATUS_OK);
  }
  return (NFA_STATUS_FAILED);
}

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
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwT2tRead(uint8_t block_number) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("Block to read: %d", block_number);

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_T2T_READ;
    p_msg->params.t2t_read.block_number = block_number;

    nfa_sys_sendmsg(p_msg);
    return (NFA_STATUS_OK);
  }
  return (NFA_STATUS_FAILED);
}

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
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwT2tWrite(uint8_t block_number, uint8_t* p_data) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("Block to write: %d", block_number);

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_T2T_WRITE;

    p_msg->params.t2t_write.block_number = block_number;

    memcpy(p_msg->params.t2t_write.p_block_data, p_data, 4);

    nfa_sys_sendmsg(p_msg);
    return (NFA_STATUS_OK);
  }
  return (NFA_STATUS_FAILED);
}

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
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwT2tSectorSelect(uint8_t sector_number) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("sector to select: %d", sector_number);

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_T2T_SECTOR_SELECT;

    p_msg->params.t2t_sector_select.sector_number = sector_number;

    nfa_sys_sendmsg(p_msg);
    return (NFA_STATUS_OK);
  }
  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwT3tRead
**
** Description:
**      Send a CHECK (read) command to the activated Type 3 tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_READ_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwT3tRead(uint8_t num_blocks, tNFA_T3T_BLOCK_DESC* t3t_blocks) {
  tNFA_RW_OPERATION* p_msg;
  uint8_t* p_block_desc;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("num_blocks to read: %i", num_blocks);

  /* Validate parameters */
  if ((num_blocks == 0) || (t3t_blocks == nullptr))
    return (NFA_STATUS_INVALID_PARAM);

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(
      sizeof(tNFA_RW_OPERATION) + (num_blocks * sizeof(tNFA_T3T_BLOCK_DESC))));
  if (p_msg != nullptr) {
    /* point to area after tNFA_RW_OPERATION */
    p_block_desc = (uint8_t*)(p_msg + 1);

    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_T3T_READ;

    p_msg->params.t3t_read.num_blocks = num_blocks;
    p_msg->params.t3t_read.p_block_desc = (tNFA_T3T_BLOCK_DESC*)p_block_desc;

    /* Copy block descriptor list */
    memcpy(p_block_desc, t3t_blocks,
           (num_blocks * sizeof(tNFA_T3T_BLOCK_DESC)));

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

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
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwT3tWrite(uint8_t num_blocks, tNFA_T3T_BLOCK_DESC* t3t_blocks,
                           uint8_t* p_data) {
  tNFA_RW_OPERATION* p_msg;
  uint8_t *p_block_desc, *p_data_area;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("num_blocks to write: %i", num_blocks);

  /* Validate parameters */
  if ((num_blocks == 0) || (t3t_blocks == nullptr) | (p_data == nullptr))
    return (NFA_STATUS_INVALID_PARAM);

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf(
      (uint16_t)(sizeof(tNFA_RW_OPERATION) +
                 (num_blocks * (sizeof(tNFA_T3T_BLOCK_DESC) + 16))));
  if (p_msg != nullptr) {
    /* point to block descriptor and data areas after tNFA_RW_OPERATION */
    p_block_desc = (uint8_t*)(p_msg + 1);
    p_data_area = p_block_desc + (num_blocks * (sizeof(tNFA_T3T_BLOCK_DESC)));

    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_T3T_WRITE;

    p_msg->params.t3t_write.num_blocks = num_blocks;
    p_msg->params.t3t_write.p_block_desc = (tNFA_T3T_BLOCK_DESC*)p_block_desc;
    p_msg->params.t3t_write.p_block_data = p_data_area;

    /* Copy block descriptor list */
    memcpy(p_block_desc, t3t_blocks,
           (num_blocks * sizeof(tNFA_T3T_BLOCK_DESC)));

    /* Copy data */
    memcpy(p_data_area, p_data, (num_blocks * 16));

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93Inventory
**
** Description:
**      Send Inventory command to the activated ISO T5T tag with/without AFI
**      If UID is provided then set UID[0]:MSB, ... UID[7]:LSB
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93Inventory(bool afi_present, uint8_t afi, uint8_t* p_uid) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("afi_present:%d, AFI: 0x%02X", afi_present, afi);

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_INVENTORY;

    p_msg->params.i93_cmd.afi_present = afi_present;
    p_msg->params.i93_cmd.afi = afi;

    if (p_uid) {
      p_msg->params.i93_cmd.uid_present = true;
      memcpy(p_msg->params.i93_cmd.uid, p_uid, I93_UID_BYTE_LEN);
    } else {
      p_msg->params.i93_cmd.uid_present = false;
    }

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93StayQuiet
**
** Description:
**      Send Stay Quiet command to the activated T5T tag.
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93StayQuiet(void) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_STAY_QUIET;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93ReadSingleBlock
**
** Description:
**      Send Read Single Block command to the activated T5T tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93ReadSingleBlock(uint8_t block_number) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("block_number: 0x%02X", block_number);

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_READ_SINGLE_BLOCK;

    p_msg->params.i93_cmd.first_block_number = block_number;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93WriteSingleBlock
**
** Description:
**      Send Write Single Block command to the activated T5T tag.
**
**      When the write operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93WriteSingleBlock(uint8_t block_number, uint8_t* p_data) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("block_number: 0x%02X", block_number);

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  /* we don't know block size of tag */
  if ((nfa_rw_cb.i93_block_size == 0) || (nfa_rw_cb.i93_num_block == 0)) {
    return (NFA_STATUS_FAILED);
  }

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf(
      (uint16_t)(sizeof(tNFA_RW_OPERATION) + nfa_rw_cb.i93_block_size));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_WRITE_SINGLE_BLOCK;

    p_msg->params.i93_cmd.first_block_number = block_number;
    p_msg->params.i93_cmd.p_data = (uint8_t*)(p_msg + 1);

    memcpy(p_msg->params.i93_cmd.p_data, p_data, nfa_rw_cb.i93_block_size);

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93LockBlock
**
** Description:
**      Send Lock block command to the activated T5T tag.
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93LockBlock(uint8_t block_number) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("block_number: 0x%02X", block_number);

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_LOCK_BLOCK;

    p_msg->params.i93_cmd.first_block_number = block_number;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93ReadMultipleBlocks
**
** Description:
**      Send Read Multiple Block command to the activated T5T tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93ReadMultipleBlocks(uint8_t first_block_number,
                                        uint16_t number_blocks) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%d, %d", first_block_number, number_blocks);

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_READ_MULTI_BLOCK;

    p_msg->params.i93_cmd.first_block_number = first_block_number;
    p_msg->params.i93_cmd.number_blocks = number_blocks;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93WriteMultipleBlocks
**
** Description:
**      Send Write Multiple Block command to the activated T5T tag.
**
**      When the write operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93WriteMultipleBlocks(uint8_t first_block_number,
                                         uint16_t number_blocks,
                                         uint8_t* p_data) {
  tNFA_RW_OPERATION* p_msg;
  uint16_t data_length;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%d, %d", first_block_number, number_blocks);

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  /* we don't know block size of tag */
  if ((nfa_rw_cb.i93_block_size == 0) || (nfa_rw_cb.i93_num_block == 0)) {
    return (NFA_STATUS_FAILED);
  }

  data_length = nfa_rw_cb.i93_block_size * number_blocks;

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf(
      (uint16_t)(sizeof(tNFA_RW_OPERATION) + data_length));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_WRITE_MULTI_BLOCK;

    p_msg->params.i93_cmd.first_block_number = first_block_number;
    p_msg->params.i93_cmd.number_blocks = number_blocks;
    p_msg->params.i93_cmd.p_data = (uint8_t*)(p_msg + 1);

    memcpy(p_msg->params.i93_cmd.p_data, p_data, data_length);

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93Select
**
** Description:
**      Send Select command to the activated T5T tag.
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
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93Select(uint8_t* p_uid) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "UID: [%02X%02X%02X...]", *(p_uid), *(p_uid + 1), *(p_uid + 2));

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf(
      (uint16_t)(sizeof(tNFA_RW_OPERATION) + I93_UID_BYTE_LEN));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_SELECT;

    p_msg->params.i93_cmd.p_data = (uint8_t*)(p_msg + 1);
    memcpy(p_msg->params.i93_cmd.p_data, p_uid, I93_UID_BYTE_LEN);

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93ResetToReady
**
** Description:
**      Send Reset to ready command to the activated T5T tag.
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93ResetToReady(void) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_RESET_TO_READY;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93WriteAFI
**
** Description:
**      Send Write AFI command to the activated T5T tag.
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93WriteAFI(uint8_t afi) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("AFI: 0x%02X", afi);

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_WRITE_AFI;

    p_msg->params.i93_cmd.afi = afi;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93LockAFI
**
** Description:
**      Send Lock AFI command to the activated T5T tag.
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93LockAFI(void) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_LOCK_AFI;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93WriteDSFID
**
** Description:
**      Send Write DSFID command to the activated T5T tag.
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93WriteDSFID(uint8_t dsfid) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("DSFID: 0x%02X", dsfid);

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_WRITE_DSFID;

    p_msg->params.i93_cmd.dsfid = dsfid;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93LockDSFID
**
** Description:
**      Send Lock DSFID command to the activated T5T tag.
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93LockDSFID(void) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_LOCK_DSFID;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93GetSysInfo
**
** Description:
**      Send Get system information command to the activated T5T tag.
**      If UID is provided then set UID[0]:MSB, ... UID[7]:LSB
**
**      When the operation has completed (or if an error occurs), the
**      app will be notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93GetSysInfo(uint8_t* p_uid) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_GET_SYS_INFO;

    if (p_uid) {
      p_msg->params.i93_cmd.uid_present = true;
      memcpy(p_msg->params.i93_cmd.uid, p_uid, I93_UID_BYTE_LEN);
    } else {
      p_msg->params.i93_cmd.uid_present = false;
    }

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_RwI93GetMultiBlockSecurityStatus
**
** Description:
**      Send Get Multiple block security status command to the activated
**      T5T tag.
**
**      Data is returned to the application using the NFA_DATA_EVT. When the
**      read operation has completed, or if an error occurs, the app will be
**      notified with NFA_I93_CMD_CPLT_EVT.
**
** Returns:
**      NFA_STATUS_OK if successfully initiated
**      NFA_STATUS_WRONG_PROTOCOL: T5T tag not activated
**      NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_RwI93GetMultiBlockSecurityStatus(uint8_t first_block_number,
                                                 uint16_t number_blocks) {
  tNFA_RW_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%d, %d", first_block_number, number_blocks);

  if (nfa_rw_cb.protocol != NFC_PROTOCOL_T5T) {
    return (NFA_STATUS_WRONG_PROTOCOL);
  }

  p_msg = (tNFA_RW_OPERATION*)GKI_getbuf((uint16_t)(sizeof(tNFA_RW_OPERATION)));
  if (p_msg != nullptr) {
    /* Fill in tNFA_RW_OPERATION struct */
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op = NFA_RW_OP_I93_GET_MULTI_BLOCK_STATUS;

    p_msg->params.i93_cmd.first_block_number = first_block_number;
    p_msg->params.i93_cmd.number_blocks = number_blocks;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}
