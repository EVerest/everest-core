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
 *  This file contains functions that NCI vendor specific interface with the
 *  NFCC. On the receive side, it routes events to the appropriate handler
 *  (callback). On the transmit side, it manages the command transmission.
 *
 ******************************************************************************/
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nfc_target.h"

#include "gki.h"
#include "nfc_int.h"

//using android::base::StringPrintf;

/****************************************************************************
** Declarations
****************************************************************************/

/*******************************************************************************
**
** Function         NFC_RegVSCback
**
** Description      This function is called to register or de-register a
**                  callback function to receive Proprietary NCI response and
**                  notification events. The maximum number of callback
**                  functions allowed is NFC_NUM_VS_CBACKS
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_RegVSCback(bool is_register, tNFC_VS_CBACK* p_cback) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  int i;

  if (is_register) {
    for (i = 0; i < NFC_NUM_VS_CBACKS; i++) {
      /* find an empty spot to hold the callback function */
      if (nfc_cb.p_vs_cb[i] == nullptr) {
        nfc_cb.p_vs_cb[i] = p_cback;
        status = NFC_STATUS_OK;
        break;
      }
    }
  } else {
    for (i = 0; i < NFC_NUM_VS_CBACKS; i++) {
      /* find the callback to de-register */
      if (nfc_cb.p_vs_cb[i] == p_cback) {
        nfc_cb.p_vs_cb[i] = nullptr;
        status = NFC_STATUS_OK;
        break;
      }
    }
  }
  return status;
}

/*******************************************************************************
**
** Function         NFC_SendRawVsCommand
**
** Description      This function is called to send the raw vendor specific
**                  command to NFCC. The response from NFCC is reported to the
**                  given tNFC_VS_CBACK.
**
** Parameters       p_data - The command buffer
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_SendRawVsCommand(NFC_HDR* p_data, tNFC_VS_CBACK* p_cback) {
  /* Validate parameters */
  if (p_data == nullptr || (p_data->len > NCI_MAX_VSC_SIZE)) {
    LOG(ERROR) << StringPrintf("buffer offset must be >= %d",
                               NCI_VSC_MSG_HDR_SIZE);
    if (p_data) GKI_freebuf(p_data);
    return NFC_STATUS_INVALID_PARAM;
  }

  p_data->event = BT_EVT_TO_NFC_NCI;
  p_data->layer_specific = NFC_WAIT_RSP_RAW_VS;
  /* save the callback function in the BT_HDR, to receive the response */
  ((tNFC_NCI_VS_MSG*)p_data)->p_cback = p_cback;

  nfc_ncif_check_cmd_queue(p_data);
  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         NFC_SendVsCommand
**
** Description      This function is called to send the given vendor specific
**                  command to NFCC. The response from NFCC is reported to the
**                  given tNFC_VS_CBACK as (oid).
**
** Parameters       oid - The opcode of the VS command.
**                  p_data - The parameters for the VS command
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_SendVsCommand(uint8_t oid, NFC_HDR* p_data,
                              tNFC_VS_CBACK* p_cback) {
  tNFC_STATUS status = NFC_STATUS_OK;
  uint8_t* pp;

  /* Allow VSC with 0-length payload */
  if (p_data == nullptr) {
    p_data = NCI_GET_CMD_BUF(0);
    if (p_data) {
      p_data->offset = NCI_VSC_MSG_HDR_SIZE;
      p_data->len = 0;
    }
  }

  /* Validate parameters */
  if ((p_data == nullptr) || (p_data->offset < NCI_VSC_MSG_HDR_SIZE) ||
      (p_data->len > NCI_MAX_VSC_SIZE)) {
    LOG(ERROR) << StringPrintf("buffer offset must be >= %d",
                               NCI_VSC_MSG_HDR_SIZE);
    if (p_data) GKI_freebuf(p_data);
    return NFC_STATUS_INVALID_PARAM;
  }

  p_data->event = BT_EVT_TO_NFC_NCI;
  p_data->layer_specific = NFC_WAIT_RSP_VSC;
  /* save the callback function in the NFC_HDR, to receive the response */
  ((tNFC_NCI_VS_MSG*)p_data)->p_cback = p_cback;

  p_data->offset -= NCI_MSG_HDR_SIZE;
  pp = (uint8_t*)(p_data + 1) + p_data->offset;
  NCI_MSG_BLD_HDR0(pp, NCI_MT_CMD, NCI_GID_PROP);
  NCI_MSG_BLD_HDR1(pp, oid);
  *pp = (uint8_t)p_data->len;
  p_data->len += NCI_MSG_HDR_SIZE;
  nfc_ncif_check_cmd_queue(p_data);
  return status;
}
