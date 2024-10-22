/******************************************************************************
 *
 *  Copyright 2019 NXP
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
#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <string.h>
#include "nfa_nfcee_int.h"
#if(NXP_EXTNS == TRUE)
using android::base::StringPrintf;

extern bool nfc_debug_enabled;
/*******************************************************************************
**
** Function         NFA_T4tNfcEeOpenConnection
**
** Description      Creates logical connection with T4T Nfcee
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_T4tNfcEeOpenConnection() {
  tNFA_T4TNFCEE_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s : Enter", __func__);

  if ((p_msg = (tNFA_T4TNFCEE_OPERATION*)GKI_getbuf(
           (uint16_t)(sizeof(tNFA_T4TNFCEE_OPERATION)))) != NULL) {
    p_msg->hdr.event = NFA_T4TNFCEE_OP_REQUEST_EVT;
    p_msg->op = NFA_T4TNFCEE_OP_OPEN_CONNECTION;
    nfa_sys_sendmsg(p_msg);
    LOG(ERROR) << StringPrintf("Return Success");
    return (NFA_STATUS_OK);
  }

  LOG(ERROR) << StringPrintf("Return Fail");
  return (NFA_STATUS_FAILED);
}
/*******************************************************************************
**
** Function         NFA_T4tNfcEeClear
**
** Description      Clear Ndef data to T4T NFC EE.
**                  For file ID NDEF, perform the NDEF detection procedure
**                  and set the NDEF tag data to zero.
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_T4tNfcEeClear(uint8_t* p_fileId) {
  tNFA_T4TNFCEE_OPERATION* p_msg;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s : Enter ", __func__);

  if ((p_msg = (tNFA_T4TNFCEE_OPERATION*)GKI_getbuf(
           (uint16_t)(sizeof(tNFA_T4TNFCEE_OPERATION)))) != NULL) {
    p_msg->hdr.event = NFA_T4TNFCEE_OP_REQUEST_EVT;
    p_msg->op = NFA_T4TNFCEE_OP_CLEAR;
    p_msg->p_fileId = p_fileId;
    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }
  return (NFA_STATUS_FAILED);
}
/*******************************************************************************
**
** Function         NFA_T4tNfcEeWrite
**
** Description      Write data to the T4T NFC EE of given file id.
**                  If file ID is of NDEF, perform the NDEF detection procedure
**                  and write the NDEF tag data using the appropriate method for
**                  NDEF EE.
**                  If File ID is Not NDEF then reads proprietary way
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_T4tNfcEeWrite(uint8_t* p_fileId, uint8_t* p_data,
                              uint32_t len) {
  tNFA_T4TNFCEE_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s : Enter p_data=%s, len: %i", __func__, p_data, len);

  if ((p_msg = (tNFA_T4TNFCEE_OPERATION*)GKI_getbuf(
           (uint16_t)(sizeof(tNFA_T4TNFCEE_OPERATION)))) != NULL) {
    p_msg->hdr.event = NFA_T4TNFCEE_OP_REQUEST_EVT;
    p_msg->op = NFA_T4TNFCEE_OP_WRITE;
    p_msg->p_fileId = p_fileId;
    p_msg->write.len = len;
    p_msg->write.p_data = p_data;
    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_T4tNfcEeRead
**
** Description      Read T4T message from NFCC area.of given file id
**                  If file ID is of NDEF, perform the NDEF detection procedure
**                  and read the NDEF tag data using the appropriate method for
**                  NDEF EE.
**                  If File ID is Not NDEF then reads proprietary way
**
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_T4tNfcEeRead(uint8_t* p_fileId) {
  tNFA_T4TNFCEE_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s : Enter ", __func__);

  if ((p_msg = (tNFA_T4TNFCEE_OPERATION*)GKI_getbuf(
           (uint16_t)(sizeof(tNFA_T4TNFCEE_OPERATION)))) != NULL) {
    p_msg->hdr.event = NFA_T4TNFCEE_OP_REQUEST_EVT;
    p_msg->op = NFA_T4TNFCEE_OP_READ;
    p_msg->p_fileId = p_fileId;

    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}

/*******************************************************************************
**
** Function         NFA_T4tNfcEeCloseConnection
**
** Description      Closes logical connection with T4T Nfcee
** Returns:
**                  NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_T4tNfcEeCloseConnection() {
  tNFA_T4TNFCEE_OPERATION* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s : Enter", __func__);

  if ((p_msg = (tNFA_T4TNFCEE_OPERATION*)GKI_getbuf(
           (uint16_t)(sizeof(tNFA_T4TNFCEE_OPERATION)))) != NULL) {
    p_msg->hdr.event = NFA_T4TNFCEE_OP_REQUEST_EVT;
    p_msg->op = NFA_T4TNFCEE_OP_CLOSE_CONNECTION;
    nfa_sys_sendmsg(p_msg);

    return (NFA_STATUS_OK);
  }

  return (NFA_STATUS_FAILED);
}
#endif
