/******************************************************************************
 *
 *  Copyright 2019-2020 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
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
#if (NXP_EXTNS == TRUE)
#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <string.h>
#include <iomanip>
#include <unordered_map>
#include "ndef_utils.h"
#include "nfa_dm_int.h"
#include "nfa_mem_co.h"
#include "nfa_nfcee_int.h"
#include "nci_defs_extns.h"

using android::base::StringPrintf;

extern bool nfc_debug_enabled;
extern tNFC_STATUS nfa_t4tnfcee_proc_disc_evt(tNFA_T4TNFCEE_OP event);

void nfa_t4tnfcee_handle_t4t_evt(tRW_EVENT event, tRW_DATA* p_data);
void nfa_t4tnfcee_store_cc_info(NFC_HDR* p_data);
void nfa_t4tnfcee_notify_rx_evt(void);
void nfa_t4tnfcee_handle_file_operations(tRW_DATA* p_rwData);
bool isReadPermitted(void);
bool isWritePermitted(void);
bool isDataLenBelowMaxFileCapacity(void);
void nfa_t4tnfcee_store_rx_buf(NFC_HDR* p_data);
void nfa_t4tnfcee_initialize_data(tNFA_T4TNFCEE_MSG* p_data);
bool is_read_precondition_valid(tNFA_T4TNFCEE_MSG* p_data);
bool is_write_precondition_valid(tNFA_T4TNFCEE_MSG* p_data);
uint16_t nfa_t4tnfcee_get_len(tRW_DATA* p_rwData);
tNFC_STATUS getWritePreconditionStatus();
bool isError(tNFC_STATUS status);
unordered_map<uint16_t, tNFA_T4TNFCEE_FILE_INFO> ccFileInfo;

/*******************************************************************************
 **
 ** Function         nfa_t4tnfcee_free_rx_buf
 **
 ** Description      Free buffer allocated to hold incoming T4T message
 **
 ** Returns          Nothing
 **
 *******************************************************************************/
void nfa_t4tnfcee_free_rx_buf(void) {
  /*Free only if it is Read operation
  For write, buffer will be passed from JNI which will be freed by JNI*/
  if (((nfa_t4tnfcee_cb.cur_op == NFA_T4TNFCEE_OP_READ) ||
       (nfa_t4tnfcee_cb.cur_op == NFA_T4TNFCEE_OP_CLEAR)) &&
      nfa_t4tnfcee_cb.p_dataBuf) {
    nfa_mem_co_free(nfa_t4tnfcee_cb.p_dataBuf);
    nfa_t4tnfcee_cb.p_dataBuf = NULL;
  }
  nfa_t4tnfcee_cb.rd_offset = 0x00;
  nfa_t4tnfcee_cb.dataLen = 0x00;
}

/*******************************************************************************
 **
 ** Function         nfa_t4tnfcee_exec_file_operation
 **
 ** Description      Handles read sequence for Ndef and proprietary
 **
 ** Returns          tNFA_STATUS
 **
 *******************************************************************************/
tNFA_STATUS nfa_t4tnfcee_exec_file_operation() {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s Enter", __func__);
  status = RW_SetT4tNfceeInfo((tRW_CBACK*)nfa_t4tnfcee_handle_t4t_evt,
                              NCI_DEST_TYPE_T4T_NFCEE);
  if (status != NFA_STATUS_OK) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s T4T info not able to set. Return", __func__);
    return status;
  }
  status = RW_T4tNfceeSelectApplication();
  if (status != NFA_STATUS_OK) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s T4T Select application failed", __func__);
    return status;
  } else {
    nfa_t4tnfcee_cb.prop_rw_state = WAIT_SELECT_APPLICATION;
    return NFA_STATUS_OK;
  }
}

/*******************************************************************************
 **
 ** Function         nfa_t4tnfcee_handle_op_req
 **
 ** Description      Handler for NFA_T4TNFCEE_OP_REQUEST_EVT, operation request
 **
 ** Returns          true if caller should free p_data
 **                  false if caller does not need to free p_data
 **
 *******************************************************************************/
bool nfa_t4tnfcee_handle_op_req(tNFA_T4TNFCEE_MSG* p_data) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nfa_t4tnfcee_handle_op_req: op=0x%02x", p_data->op_req.op);
  nfa_t4tnfcee_cb.cur_op = p_data->op_req.op;

  /* Call appropriate handler for requested operation */
  switch (p_data->op_req.op) {
    case NFA_T4TNFCEE_OP_OPEN_CONNECTION: {
      nfa_t4tnfcee_proc_disc_evt(NFA_T4TNFCEE_OP_OPEN_CONNECTION);
    } break;
    case NFA_T4TNFCEE_OP_READ: {
      if (!is_read_precondition_valid(p_data)) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s Failed", __func__);
        nfa_t4tnfcee_cb.status = NFA_STATUS_INVALID_PARAM;
        nfa_t4tnfcee_notify_rx_evt();
        break;
      }
      nfa_t4tnfcee_initialize_data(p_data);
      tNFA_STATUS status = nfa_t4tnfcee_exec_file_operation();
      if (status != NFA_STATUS_OK) {
        nfa_t4tnfcee_cb.status = NFA_STATUS_FAILED;
        nfa_t4tnfcee_notify_rx_evt();
      }
    } break;
    case NFA_T4TNFCEE_OP_WRITE: {
      if (!is_write_precondition_valid(p_data)) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s Failed", __func__);
        nfa_t4tnfcee_cb.status = NFA_STATUS_INVALID_PARAM;
        nfa_t4tnfcee_notify_rx_evt();
        break;
      }
      nfa_t4tnfcee_initialize_data(p_data);
      if ((p_data->op_req.write.p_data != nullptr) &&
          (p_data->op_req.write.len > 0)) {
        nfa_t4tnfcee_cb.p_dataBuf = p_data->op_req.write.p_data;
        nfa_t4tnfcee_cb.dataLen = p_data->op_req.write.len;
      }
      tNFA_STATUS status = nfa_t4tnfcee_exec_file_operation();
      if (status != NFA_STATUS_OK) {
        nfa_t4tnfcee_cb.status = NFA_STATUS_FAILED;
        nfa_t4tnfcee_notify_rx_evt();
      }
    } break;
    case NFA_T4TNFCEE_OP_CLEAR: {
      nfa_t4tnfcee_initialize_data(p_data);
      tNFA_STATUS status = nfa_t4tnfcee_exec_file_operation();
      if (status != NFA_STATUS_OK) {
        nfa_t4tnfcee_cb.status = NFA_STATUS_FAILED;
        nfa_t4tnfcee_notify_rx_evt();
      }
      break;
    }
    case NFA_T4TNFCEE_OP_CLOSE_CONNECTION: {
      nfa_t4tnfcee_proc_disc_evt(NFA_T4TNFCEE_OP_CLOSE_CONNECTION);
    } break;
    default:
      break;
  }
  return true;
}

/*******************************************************************************
 **
 ** Function     nfa_t4tnfcee_check_sw
 **
 ** Description  Updates the status if R-APDU has been received with failure status
 **
 ** Returns      Nothing
 **
 *******************************************************************************/
static void nfa_t4tnfcee_check_sw(tRW_DATA* p_rwData) {
  uint8_t *p; uint16_t status_words;
  NFC_HDR* p_r_apdu=  p_rwData->raw_frame.p_data;
  p = (uint8_t*)(p_r_apdu + 1) + p_r_apdu->offset;
  p += (p_r_apdu->len - T4T_RSP_STATUS_WORDS_SIZE);
  BE_STREAM_TO_UINT16(status_words, p);
  if ((status_words != T4T_RSP_CMD_CMPLTED) &&
      (!T4T_RSP_WARNING_PARAMS_CHECK(status_words >> 8))) {
    p_rwData->raw_frame.status = NFC_STATUS_FAILED;
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("status 0x%X",status_words);
  }
}
/*******************************************************************************
 **
 ** Function         nfa_t4tnfcee_handle_t4t_evt
 **
 ** Description      Handler for Type-4 NFCEE reader/writer events
 **
 ** Returns          Nothing
 **
 *******************************************************************************/
void nfa_t4tnfcee_handle_t4t_evt(tRW_EVENT event, tRW_DATA* p_rwData) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: Enter event=0x%02x 0x%02x", __func__, event, p_rwData->status);
  switch (event) {
    case RW_T4T_RAW_FRAME_EVT:
      nfa_t4tnfcee_check_sw(p_rwData);
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s RW_T4T_RAW_FRAME_EVT", __func__);
      nfa_t4tnfcee_handle_file_operations(p_rwData);
      break;
    case RW_T4T_INTF_ERROR_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s RW_T4T_INTF_ERROR_EVT", __func__);
      nfa_t4tnfcee_handle_file_operations(p_rwData);
      break;
    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s UNKNOWN EVENT", __func__);
      break;
  }
  return;
}

/*******************************************************************************
 **
 ** Function         nfa_t4tnfcee_store_cc_info
 **
 ** Description      stores CC info into local data structure
 **
 ** Returns          Nothing
 **
 *******************************************************************************/
void nfa_t4tnfcee_store_cc_info(NFC_HDR* p_data) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s Enter", __func__);

  uint16_t keyFileId;
  string valueFileLength;
  const uint8_t skipTL = 0x02, tlvLen = 0x08;
  uint8_t jumpToFirstTLV = 0x03; /*Le index*/
  uint16_t RemainingDataLen = 0;
  uint8_t* ccInfo;

  if (NULL != p_data) {
    ccInfo = (uint8_t*)(p_data + 1) + p_data->offset + jumpToFirstTLV;
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s empty cc info", __func__);
    return;
  }
  RW_T4tNfceeUpdateCC(ccInfo);
  jumpToFirstTLV = 0x07;
  ccInfo = (uint8_t*)(p_data + 1) + p_data->offset + jumpToFirstTLV;

  ccFileInfo.clear();
  RemainingDataLen =
      (p_data->len - jumpToFirstTLV - T4TNFCEE_SIZEOF_STATUS_BYTES);
  while (RemainingDataLen >= 0x08) {
    tNFA_T4TNFCEE_FILE_INFO fileInfo;
    ccInfo += skipTL;
    BE_STREAM_TO_UINT16(keyFileId, ccInfo);
    BE_STREAM_TO_UINT16(fileInfo.capacity, ccInfo);
    BE_STREAM_TO_UINT8(fileInfo.read_access, ccInfo);
    BE_STREAM_TO_UINT8(fileInfo.write_access, ccInfo);
    ccFileInfo.insert(
        pair<uint16_t, tNFA_T4TNFCEE_FILE_INFO>(keyFileId, fileInfo));
    keyFileId = 0x00;
    RemainingDataLen -= tlvLen;
  }
}

/*******************************************************************************
 **
 ** Function         nfa_t4tnfcee_store_rx_buf
 **
 ** Description      Stores read data.
 **
 ** Returns          Nothing
 **
 *******************************************************************************/
void nfa_t4tnfcee_store_rx_buf(NFC_HDR* p_data) {
  uint8_t* p;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s copying data len %d  rd_offset: %d", __func__,
                      p_data->len, nfa_t4tnfcee_cb.rd_offset);
  if (NULL != p_data) {
    p = (uint8_t*)(p_data + 1) + p_data->offset;
    memcpy(&nfa_t4tnfcee_cb.p_dataBuf[nfa_t4tnfcee_cb.rd_offset], p,
           p_data->len);
    nfa_t4tnfcee_cb.rd_offset += p_data->len;
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s Data is NULL", __func__);
  }
}

/*******************************************************************************
 **
 ** Function         nfa_t4tnfcee_initialize_data
 **
 ** Description      Initializes control block
 **
 ** Returns          none
 **
 *******************************************************************************/
void nfa_t4tnfcee_initialize_data(tNFA_T4TNFCEE_MSG* p_data) {
  nfa_t4tnfcee_cb.prop_rw_state = PROP_DISABLED;
  nfa_t4tnfcee_cb.rd_offset = 0;
  nfa_t4tnfcee_cb.p_dataBuf = nullptr;
  nfa_t4tnfcee_cb.dataLen = 0x00;
  BE_STREAM_TO_UINT16(nfa_t4tnfcee_cb.cur_fileId, p_data->op_req.p_fileId);
}
/*******************************************************************************
 **
 ** Function         nfa_t4tnfcee_handle_file_operations
 **
 ** Description      Handles proprietary file operations
 **
 ** Returns          none
 **
 *******************************************************************************/
void nfa_t4tnfcee_handle_file_operations(tRW_DATA* p_rwData) {
  if (p_rwData == nullptr) {
    nfa_t4tnfcee_cb.status = NFC_STATUS_FAILED;
    nfa_t4tnfcee_notify_rx_evt();
    return;
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s currState : 0x%02x", __func__, nfa_t4tnfcee_cb.prop_rw_state);
  switch (nfa_t4tnfcee_cb.prop_rw_state) {
    case WAIT_SELECT_APPLICATION:
      if (isError(p_rwData->raw_frame.status)) break;
      RW_T4tNfceeSelectFile(CC_FILE_ID);
      nfa_t4tnfcee_cb.prop_rw_state = WAIT_SELECT_CC;
      break;

    case WAIT_SELECT_CC:
      if (isError(p_rwData->raw_frame.status)) break;
      RW_T4tNfceeReadDataLen();
      nfa_t4tnfcee_cb.prop_rw_state = WAIT_READ_CC_DATA_LEN;
      break;

    case WAIT_READ_CC_DATA_LEN: {
      if (isError(p_rwData->raw_frame.status)) break;
      uint16_t lenDataToBeRead = nfa_t4tnfcee_get_len(p_rwData);
      if (lenDataToBeRead <= 0x00) {
        nfa_t4tnfcee_cb.status = NFC_STATUS_NO_BUFFERS;
        nfa_t4tnfcee_notify_rx_evt();
        break;
      }
      RW_T4tNfceeReadFile(0x00, lenDataToBeRead);
      nfa_t4tnfcee_cb.prop_rw_state = WAIT_READ_CC_FILE;
      break;
    }

    case WAIT_READ_CC_FILE: {
      if (isError(p_rwData->raw_frame.status)) break;
      nfa_t4tnfcee_store_cc_info(p_rwData->raw_frame.p_data);
      if (ccFileInfo.find(nfa_t4tnfcee_cb.cur_fileId) == ccFileInfo.end()) {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s FileId Not found in CC", __func__);
        nfa_t4tnfcee_cb.status = NFA_T4T_STATUS_INVALID_FILE_ID;
        nfa_t4tnfcee_notify_rx_evt();
        break;
      }

      RW_T4tNfceeSelectFile(nfa_t4tnfcee_cb.cur_fileId);
      nfa_t4tnfcee_cb.prop_rw_state = WAIT_SELECT_FILE;
      break;
    }

    case WAIT_SELECT_FILE: {
      if (isError(p_rwData->raw_frame.status)) break;
      if ((nfa_t4tnfcee_cb.cur_op == NFA_T4TNFCEE_OP_READ) &&
          isReadPermitted()) {
        RW_T4tNfceeReadDataLen();
        nfa_t4tnfcee_cb.prop_rw_state = WAIT_READ_DATA_LEN;
      } else if (nfa_t4tnfcee_cb.cur_op == NFA_T4TNFCEE_OP_WRITE) {
        tNFA_STATUS preCondStatus = getWritePreconditionStatus();
        if (preCondStatus == NFA_STATUS_OK) {
          RW_T4tNfceeUpdateNlen(0x0000);
          nfa_t4tnfcee_cb.prop_rw_state = WAIT_RESET_NLEN;
        } else {
          nfa_t4tnfcee_cb.status = preCondStatus;
          nfa_t4tnfcee_notify_rx_evt();
        }
      } else if (nfa_t4tnfcee_cb.cur_op == NFA_T4TNFCEE_OP_CLEAR) {
        RW_T4tNfceeReadDataLen();
        nfa_t4tnfcee_cb.prop_rw_state = WAIT_CLEAR_NDEF_DATA;
      }
      break;
    }

    case WAIT_CLEAR_NDEF_DATA: {
      if (isError(p_rwData->raw_frame.status)) break;
      uint16_t lenDataToBeClear = nfa_t4tnfcee_get_len(p_rwData);
      if (lenDataToBeClear == 0x00) {
        nfa_t4tnfcee_cb.status = p_rwData->raw_frame.status;;
        nfa_t4tnfcee_notify_rx_evt();
        break;
      }
      RW_T4tNfceeUpdateNlen(0x0000);
      nfa_t4tnfcee_cb.p_dataBuf = (uint8_t*)nfa_mem_co_alloc(lenDataToBeClear);
      if(!nfa_t4tnfcee_cb.p_dataBuf) {
        nfa_t4tnfcee_cb.status = NFC_STATUS_FAILED;
        nfa_t4tnfcee_notify_rx_evt();
        break;
      }
      memset(nfa_t4tnfcee_cb.p_dataBuf, 0, lenDataToBeClear);
      nfa_t4tnfcee_cb.dataLen = lenDataToBeClear;
      nfa_t4tnfcee_cb.prop_rw_state = WAIT_RESET_NLEN;
      break;
    }

    case WAIT_READ_DATA_LEN: {
      if (isError(p_rwData->raw_frame.status)) break;
      uint16_t lenDataToBeRead = nfa_t4tnfcee_get_len(p_rwData);
      if (lenDataToBeRead <= 0x00) {
        nfa_t4tnfcee_cb.status = NFC_STATUS_NO_BUFFERS;
        nfa_t4tnfcee_notify_rx_evt();
        break;
      }

      nfa_t4tnfcee_cb.p_dataBuf = (uint8_t*)nfa_mem_co_alloc(lenDataToBeRead);
      RW_T4tNfceeReadFile(T4T_FILE_LENGTH_SIZE, lenDataToBeRead);
      nfa_t4tnfcee_cb.prop_rw_state = WAIT_READ_FILE;
      break;
    }

    case WAIT_READ_FILE: {
      if (isError(p_rwData->raw_frame.status)) break;
      /*updating length field to discard status while processing read data
      For RAW data, T4T module returns length including status length*/
      if (p_rwData->raw_frame.p_data->len >= 0x02)
        p_rwData->raw_frame.p_data->len -= 0x02;
      nfa_t4tnfcee_store_rx_buf(p_rwData->raw_frame.p_data);
      if (RW_T4tIsReadComplete()) {
        nfa_t4tnfcee_cb.dataLen = nfa_t4tnfcee_cb.rd_offset;
        nfa_t4tnfcee_cb.status = p_rwData->raw_frame.status;
        nfa_t4tnfcee_notify_rx_evt();
      } else {
        RW_T4tNfceeReadPendingData();
      }
      break;
    }

    case WAIT_RESET_NLEN: {
      if (isError(p_rwData->raw_frame.status)) break;
      RW_T4tNfceeStartUpdateFile(nfa_t4tnfcee_cb.dataLen,
                                 nfa_t4tnfcee_cb.p_dataBuf);
      if (RW_T4tIsUpdateComplete())
        nfa_t4tnfcee_cb.prop_rw_state = WAIT_WRITE_COMPLETE;
      else
        nfa_t4tnfcee_cb.prop_rw_state = WAIT_WRITE;
      break;
    }

    case WAIT_WRITE: {
      RW_T4tNfceeUpdateFile();
      if (RW_T4tIsUpdateComplete())
        nfa_t4tnfcee_cb.prop_rw_state = WAIT_WRITE_COMPLETE;
      break;
    }

    case WAIT_WRITE_COMPLETE: {
      if (isError(p_rwData->raw_frame.status)) break;
      if (nfa_t4tnfcee_cb.cur_op == NFA_T4TNFCEE_OP_CLEAR) {
        nfa_t4tnfcee_cb.status = p_rwData->raw_frame.status;
        /*Length is already zero returning from here.*/
        nfa_t4tnfcee_notify_rx_evt();
      } else {
        RW_T4tNfceeUpdateNlen(nfa_t4tnfcee_cb.dataLen);
        nfa_t4tnfcee_cb.prop_rw_state = WAIT_UPDATE_NLEN;
      }
      break;
    }

    case WAIT_UPDATE_NLEN: {
      if (isError(p_rwData->raw_frame.status)) break;
      nfa_t4tnfcee_cb.status = p_rwData->raw_frame.status;
      nfa_t4tnfcee_notify_rx_evt();
      break;
    }

    default:
      break;
  }
  GKI_freebuf(p_rwData->raw_frame.p_data);
}
/*******************************************************************************
 **
 ** Function         nfa_t4tnfcee_notify_rx_evt
 **
 ** Description      Notifies to upper layer with data
 **
 ** Returns          None
 **
 *******************************************************************************/
void nfa_t4tnfcee_notify_rx_evt(void) {
  tNFA_CONN_EVT_DATA conn_evt_data;
  conn_evt_data.status = nfa_t4tnfcee_cb.status;
  nfa_t4tnfcee_cb.prop_rw_state = OP_COMPLETE;
  if (nfa_t4tnfcee_cb.cur_op == NFA_T4TNFCEE_OP_READ) {
    if (conn_evt_data.status == NFA_STATUS_OK) {
      conn_evt_data.data.p_data = nfa_t4tnfcee_cb.p_dataBuf;
      conn_evt_data.data.len = nfa_t4tnfcee_cb.dataLen;
    }
    nfa_dm_act_conn_cback_notify(NFA_T4TNFCEE_READ_CPLT_EVT, &conn_evt_data);
  } else if (nfa_t4tnfcee_cb.cur_op == NFA_T4TNFCEE_OP_WRITE) {
    if (conn_evt_data.status == NFA_STATUS_OK) {
      conn_evt_data.data.len = nfa_t4tnfcee_cb.dataLen;
    }
    nfa_dm_act_conn_cback_notify(NFA_T4TNFCEE_WRITE_CPLT_EVT, &conn_evt_data);
  } else if (nfa_t4tnfcee_cb.cur_op == NFA_T4TNFCEE_OP_CLEAR) {
    nfa_dm_act_conn_cback_notify(NFA_T4TNFCEE_CLEAR_CPLT_EVT, &conn_evt_data);
  }
  nfa_t4tnfcee_free_rx_buf();
}

/*******************************************************************************
 **
 ** Function         is_read_precondition_valid
 **
 ** Description      validates precondition for read
 **
 ** Returns          true/false
 **
 *******************************************************************************/
bool is_read_precondition_valid(tNFA_T4TNFCEE_MSG* p_data) {
  if ((p_data->op_req.p_fileId == nullptr) ||
      (nfa_t4tnfcee_cb.t4tnfcee_state != NFA_T4TNFCEE_STATE_CONNECTED)) {
    return false;
  }
  return true;
}

/*******************************************************************************
 **
 ** Function         is_write_precondition_valid
 **
 ** Description      validates precondition for write
 **
 ** Returns          true/false
 **
 *******************************************************************************/
bool is_write_precondition_valid(tNFA_T4TNFCEE_MSG* p_data) {
  if ((p_data->op_req.p_fileId == nullptr) ||
      (nfa_t4tnfcee_cb.t4tnfcee_state != NFA_T4TNFCEE_STATE_CONNECTED) ||
      (p_data->op_req.write.p_data == nullptr) ||
      (p_data->op_req.write.len == 0)) {
    return false;
  }
  return true;
}

/*******************************************************************************
 **
 ** Function         isReadPermitted
 **
 ** Description      Checks if read permitted for current file
 **
 ** Returns          true/false
 **
 *******************************************************************************/
bool isReadPermitted(void) {
  return (ccFileInfo.find(nfa_t4tnfcee_cb.cur_fileId)->second.read_access ==
          T4T_NFCEE_READ_ALLOWED);
}

/*******************************************************************************
 **
 ** Function         isWritePermitted
 **
 ** Description      Checks if write permitted for current file
 **
 ** Returns          true/false
 **
 *******************************************************************************/
bool isWritePermitted(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s : 0x%2x", __func__,
      ccFileInfo.find(nfa_t4tnfcee_cb.cur_fileId)->second.write_access);
  return ((ccFileInfo.find(nfa_t4tnfcee_cb.cur_fileId)->second.write_access !=
           T4T_NFCEE_WRITE_NOT_ALLOWED));
}

/*******************************************************************************
 **
 ** Function         isDataLenBelowMaxFileCapacity
 **
 ** Description      Checks if current data length is less not exceeding file
 **                  capacity
 **
 ** Returns          true/false
 **
 *******************************************************************************/
bool isDataLenBelowMaxFileCapacity(void) {
  return (nfa_t4tnfcee_cb.dataLen <=
          (ccFileInfo.find(nfa_t4tnfcee_cb.cur_fileId)->second.capacity -
           T4TNFCEE_SIZEOF_LEN_BYTES));
}

/*******************************************************************************
 **
 ** Function         getWritePreconditionStatus
 **
 ** Description      Checks if write preconditions are satisfied
 **
 ** Returns          NFA_STATUS_OK if success else ERROR status
 **
 *******************************************************************************/
tNFC_STATUS getWritePreconditionStatus() {
  if (!isWritePermitted()) return NFA_STATUS_READ_ONLY;
  if (!isDataLenBelowMaxFileCapacity()) {
    DLOG_IF(ERROR, nfc_debug_enabled) << StringPrintf("Data Len exceeds max file size");
    return NFA_STATUS_FAILED;
  }
  if (nfa_t4tnfcee_cb.cur_fileId == NDEF_FILE_ID) {
    tNDEF_STATUS ndef_status;
    if ((ndef_status = NDEF_MsgValidate(nfa_t4tnfcee_cb.p_dataBuf,
                                        nfa_t4tnfcee_cb.dataLen, true)) !=
        NDEF_OK) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "Invalid NDEF message. NDEF_MsgValidate returned %i", ndef_status);
      return NFA_STATUS_REJECTED;
    }
    /*NDEF Msg validation SUCCESS*/
    return NFA_STATUS_OK;
  }
  /*Proprietary file id*/
  return NFA_STATUS_OK;
}

/*******************************************************************************
 **
 ** Function         nfa_t4tnfcee_get_len
 **
 ** Description      get the length of data available in current selected file
 **
 ** Returns          data len
 **
 *******************************************************************************/
uint16_t nfa_t4tnfcee_get_len(tRW_DATA* p_rwData) {
  uint8_t* p = nullptr;
  uint16_t readLen = 0x00;
  if (p_rwData->raw_frame.p_data->len > 0x00) {
    p = (uint8_t*)(p_rwData->raw_frame.p_data + 1) +
        p_rwData->raw_frame.p_data->offset;
  }
  if (p != nullptr) BE_STREAM_TO_UINT16(readLen, p);
  if (readLen > 0x00) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s readLen  0x%x", __func__, readLen);
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s No Data to Read", __func__);
  }
  return readLen;
}

/*******************************************************************************
 **
 ** Function         isError
 **
 ** Description      Checks and notifies upper layer in case of error
 **
 ** Returns          true if error else false
 **
 *******************************************************************************/
bool isError(tNFC_STATUS status) {
  if (status != NFA_STATUS_OK) {
    nfa_t4tnfcee_cb.status = NFC_STATUS_FAILED;
    nfa_t4tnfcee_notify_rx_evt();
    return true;
  } else
    return false;
}
#endif
