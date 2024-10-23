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
 *  This file contains the main LLCP entry points
 *
 ******************************************************************************/

#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "bt_types.h"
#include "gki.h"
#include "llcp_api.h"
#include "llcp_int.h"
#include "nfc_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

tLLCP_CB llcp_cb;

/*******************************************************************************
**
** Function         llcp_init
**
** Description      This function is called once at startup to initialize
**                  all the LLCP structures
**
** Returns          void
**
*******************************************************************************/
void llcp_init(void) {
  uint32_t pool_count;

  memset(&llcp_cb, 0, sizeof(tLLCP_CB));

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  llcp_cb.lcb.local_link_miu =
      (LLCP_MIU <= LLCP_MAX_MIU ? LLCP_MIU : LLCP_MAX_MIU);
  llcp_cb.lcb.local_opt = LLCP_OPT_VALUE;
  llcp_cb.lcb.local_wt = LLCP_WAITING_TIME;
  llcp_cb.lcb.local_lto = LLCP_LTO_VALUE;

  llcp_cb.lcb.inact_timeout_init = LLCP_INIT_INACTIVITY_TIMEOUT;
  llcp_cb.lcb.inact_timeout_target = LLCP_TARGET_INACTIVITY_TIMEOUT;
  llcp_cb.lcb.symm_delay = LLCP_DELAY_RESP_TIME;
  llcp_cb.lcb.data_link_timeout = LLCP_DATA_LINK_CONNECTION_TOUT;
  llcp_cb.lcb.delay_first_pdu_timeout = LLCP_DELAY_TIME_TO_SEND_FIRST_PDU;

  llcp_cb.lcb.wks = LLCP_WKS_MASK_LM;

  /* total number of buffers for LLCP */
  pool_count = GKI_poolcount(LLCP_POOL_ID);

  /* number of buffers for receiving data */
  llcp_cb.num_rx_buff = (pool_count * LLCP_RX_BUFF_RATIO) / 100;

  /* rx congestion start/end threshold */
  llcp_cb.overall_rx_congest_start =
      (uint8_t)((llcp_cb.num_rx_buff * LLCP_RX_CONGEST_START) / 100);
  llcp_cb.overall_rx_congest_end =
      (uint8_t)((llcp_cb.num_rx_buff * LLCP_RX_CONGEST_END) / 100);

  /* max number of buffers for receiving data on logical data link */
  llcp_cb.max_num_ll_rx_buff =
      (uint8_t)((llcp_cb.num_rx_buff * LLCP_LL_RX_BUFF_LIMIT) / 100);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "num_rx_buff = %d, rx_congest_start = %d, rx_congest_end = %d, "
      "max_num_ll_rx_buff = %d",
      llcp_cb.num_rx_buff, llcp_cb.overall_rx_congest_start,
      llcp_cb.overall_rx_congest_end, llcp_cb.max_num_ll_rx_buff);

  /* max number of buffers for transmitting data */
  llcp_cb.max_num_tx_buff = (uint8_t)(pool_count - llcp_cb.num_rx_buff);

  /* max number of buffers for transmitting data on logical data link */
  llcp_cb.max_num_ll_tx_buff =
      (uint8_t)((llcp_cb.max_num_tx_buff * LLCP_LL_TX_BUFF_LIMIT) / 100);

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("max_num_tx_buff = %d, max_num_ll_tx_buff = %d",
                      llcp_cb.max_num_tx_buff, llcp_cb.max_num_ll_tx_buff);

  llcp_cb.ll_tx_uncongest_ntf_start_sap = LLCP_SAP_SDP + 1;

  LLCP_RegisterServer(LLCP_SAP_SDP, LLCP_LINK_TYPE_DATA_LINK_CONNECTION,
                      "urn:nfc:sn:sdp", llcp_sdp_proc_data);
}

/*******************************************************************************
**
** Function         llcp_cleanup
**
** Description      This function is called once at closing to clean up
**
** Returns          void
**
*******************************************************************************/
void llcp_cleanup(void) {
  uint8_t sap;
  tLLCP_APP_CB* p_app_cb;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  for (sap = LLCP_SAP_SDP; sap < LLCP_NUM_SAPS; sap++) {
    p_app_cb = llcp_util_get_app_cb(sap);

    if ((p_app_cb) && (p_app_cb->p_app_cback)) {
      LLCP_Deregister(sap);
    }
  }

  nfc_stop_quick_timer(&llcp_cb.lcb.inact_timer);
  nfc_stop_quick_timer(&llcp_cb.lcb.timer);
}

/*******************************************************************************
**
** Function         llcp_process_timeout
**
** Description      This function is called when an LLCP-related timeout occurs
**
** Returns          void
**
*******************************************************************************/
void llcp_process_timeout(TIMER_LIST_ENT* p_tle) {
  uint8_t reason;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("llcp_process_timeout: event=%d", p_tle->event);

  switch (p_tle->event) {
    case NFC_TTYPE_LLCP_LINK_MANAGER:
      /* Link timeout or Symm timeout */
      llcp_link_process_link_timeout();
      break;

    case NFC_TTYPE_LLCP_LINK_INACT:
      /* inactivity timeout */
      llcp_link_deactivate(LLCP_LINK_LOCAL_INITIATED);
      break;

    case NFC_TTYPE_LLCP_DATA_LINK:
      reason = LLCP_SAP_DISCONNECT_REASON_TIMEOUT;
      llcp_dlsm_execute((tLLCP_DLCB*)(p_tle->param), LLCP_DLC_EVENT_TIMEOUT,
                        &reason);
      break;

    case NFC_TTYPE_LLCP_DELAY_FIRST_PDU:
      llcp_link_check_send_data();
      break;

    default:
      break;
  }
}
