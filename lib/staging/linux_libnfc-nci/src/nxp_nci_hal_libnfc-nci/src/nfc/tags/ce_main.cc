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
 *  This file contains functions that interface with the NFC NCI transport.
 *  On the receive side, it routes events to the appropriate handler
 *  (callback). On the transmit side, it manages the command transmission.
 *
 ******************************************************************************/
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nfc_target.h"

#include "bt_types.h"
#include "ce_api.h"
#include "ce_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

tCE_CB ce_cb;

/*******************************************************************************
*******************************************************************************/
void ce_init(void) {
  memset(&ce_cb, 0, sizeof(tCE_CB));

  /* Initialize tag-specific fields of ce control block */
  ce_t3t_init();
}

/*******************************************************************************
**
** Function         CE_SendRawFrame
**
** Description      This function sends a raw frame to the peer device.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS CE_SendRawFrame(uint8_t* p_raw_data, uint16_t data_len) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  NFC_HDR* p_data;
  uint8_t* p;

  if (ce_cb.p_cback) {
    /* a valid opcode for RW */
    p_data = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
    if (p_data) {
      p_data->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
      p = (uint8_t*)(p_data + 1) + p_data->offset;
      memcpy(p, p_raw_data, data_len);
      p_data->len = data_len;
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("CE SENT raw frame (0x%x)", data_len);
      status = NFC_SendData(NFC_RF_CONN_ID, p_data);
    }
  }
  return status;
}

/*******************************************************************************
**
** Function         CE_SetActivatedTagType
**
** Description      This function selects the tag type for CE mode.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS CE_SetActivatedTagType(tNFC_ACTIVATE_DEVT* p_activate_params,
                                   uint16_t t3t_system_code,
                                   tCE_CBACK* p_cback) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tNFC_PROTOCOL protocol = p_activate_params->protocol;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("CE_SetActivatedTagType protocol:%d", protocol);

  switch (protocol) {
    case NFC_PROTOCOL_T1T:
    case NFC_PROTOCOL_T2T:
      return NFC_STATUS_FAILED;

    case NFC_PROTOCOL_T3T: /* Type3Tag    - NFC-F */
      /* store callback function before NFC_SetStaticRfCback () */
      ce_cb.p_cback = p_cback;
      status = ce_select_t3t(t3t_system_code,
                             p_activate_params->rf_tech_param.param.lf.nfcid2);
      break;

    case NFC_PROTOCOL_ISO_DEP: /* ISODEP/4A,4B- NFC-A or NFC-B */
      /* store callback function before NFC_SetStaticRfCback () */
      ce_cb.p_cback = p_cback;
      status = ce_select_t4t();
      break;

    default:
      LOG(ERROR) << StringPrintf("CE_SetActivatedTagType Invalid protocol");
      return NFC_STATUS_FAILED;
  }

  if (status != NFC_STATUS_OK) {
    NFC_SetStaticRfCback(nullptr);
    ce_cb.p_cback = nullptr;
  }
  return status;
}

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
UINT8 CE_SetTraceLevel (UINT8 new_level)
{
    if (new_level != 0xFF)
        ce_cb.trace_level = new_level;

    return (ce_cb.trace_level);
}

