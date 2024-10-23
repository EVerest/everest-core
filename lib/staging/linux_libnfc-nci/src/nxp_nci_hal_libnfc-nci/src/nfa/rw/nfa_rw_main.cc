/******************************************************************************
 *
 *  Copyright (C) 2003-2014 Broadcom Corporation
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
 *  This is the main implementation file for the NFA_RW
 *
 ******************************************************************************/
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>
//#define INFO 1
#include "nfa_dm_int.h"
#include "nfa_rw_api.h"
#include "nfa_rw_int.h"
#include "bt_trace.h""
using android::base::StringPrintf;
extern bool nfc_debug_enabled;

/* NFA_RW control block */
tNFA_RW_CB nfa_rw_cb;

/*****************************************************************************
** Constants and types
*****************************************************************************/
static const tNFA_SYS_REG nfa_rw_sys_reg = {nullptr, nfa_rw_handle_event,
                                            nfa_rw_sys_disable, nullptr};

/* NFA_RW actions */
const tNFA_RW_ACTION nfa_rw_action_tbl[] = {
    nfa_rw_handle_op_req,         /* NFA_RW_OP_REQUEST_EVT            */
    nfa_rw_activate_ntf,          /* NFA_RW_ACTIVATE_NTF_EVT          */
    nfa_rw_deactivate_ntf,        /* NFA_RW_DEACTIVATE_NTF_EVT        */
    nfa_rw_presence_check_tick,   /* NFA_RW_PRESENCE_CHECK_TICK_EVT   */
    nfa_rw_presence_check_timeout /* NFA_RW_PRESENCE_CHECK_TIMEOUT_EVT*/
};

/*****************************************************************************
** Local function prototypes
*****************************************************************************/
static std::string nfa_rw_evt_2_str(uint16_t event);

/*******************************************************************************
**
** Function         nfa_rw_init
**
** Description      Initialize NFA RW
**
** Returns          None
**
*******************************************************************************/
void nfa_rw_init(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* initialize control block */
  memset(&nfa_rw_cb, 0, sizeof(tNFA_RW_CB));

  /* register message handler on NFA SYS */
  nfa_sys_register(NFA_ID_RW, &nfa_rw_sys_reg);
}

/*******************************************************************************
**
** Function         nfa_rw_sys_disable
**
** Description      Clean up rw sub-system
**
**
** Returns          void
**
*******************************************************************************/
void nfa_rw_sys_disable(void) {
  /* Return to idle */
  NFC_SetStaticRfCback(nullptr);

  /* Stop presence check timer (if started) */
  nfa_rw_stop_presence_check_timer();

  /* Free scratch buffer if any */
  nfa_rw_free_ndef_rx_buf();

  /* Free pending command if any */
  if (nfa_rw_cb.p_pending_msg) {
    GKI_freebuf(nfa_rw_cb.p_pending_msg);
    nfa_rw_cb.p_pending_msg = nullptr;
  }

  nfa_sys_deregister(NFA_ID_RW);
}

/*******************************************************************************
**
** Function         nfa_rw_proc_disc_evt
**
** Description      Called by nfa_dm to handle ACTIVATED/DEACTIVATED  events
**
** Returns          void
**
*******************************************************************************/
void nfa_rw_proc_disc_evt(tNFA_DM_RF_DISC_EVT event, tNFC_DISCOVER* p_data,
                          bool excl_rf_not_active) {
  tNFA_RW_MSG msg;

  switch (event) {
    case NFA_DM_RF_DISC_ACTIVATED_EVT:
      msg.hdr.event = NFA_RW_ACTIVATE_NTF_EVT;
      msg.activate_ntf.p_activate_params = &p_data->activate;
      msg.activate_ntf.excl_rf_not_active = excl_rf_not_active;

      nfa_rw_handle_event((NFC_HDR*)&msg);
      break;

    case NFA_DM_RF_DISC_DEACTIVATED_EVT:
      msg.hdr.event = NFA_RW_DEACTIVATE_NTF_EVT;

      nfa_rw_handle_event((NFC_HDR*)&msg);
      break;

    default:
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_rw_send_raw_frame
**
** Description      Called by nfa_dm to send raw frame
**
** Returns          tNFA_STATUS
**
*******************************************************************************/
tNFA_STATUS nfa_rw_send_raw_frame(NFC_HDR* p_data) {
  tNFA_RW_MSG* p_msg;

  p_msg = (tNFA_RW_MSG*)GKI_getbuf((uint16_t)sizeof(tNFA_RW_MSG));
  if (p_msg != nullptr) {
    p_msg->hdr.event = NFA_RW_OP_REQUEST_EVT;
    p_msg->op_req.op = NFA_RW_OP_SEND_RAW_FRAME;

    p_msg->op_req.params.send_raw_frame.p_data = p_data;

    if (nfa_rw_handle_event((NFC_HDR*)p_msg)) GKI_freebuf(p_msg);

    return (NFA_STATUS_OK);
  }
  return NFA_STATUS_FAILED;
}

/*******************************************************************************
**
** Function         nfa_rw_handle_event
**
** Description      nfa rw main event handling function.
**
** Returns          TRUE if caller should free p_msg buffer
**
*******************************************************************************/
bool nfa_rw_handle_event(NFC_HDR* p_msg) {
  uint16_t act_idx;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nfa_rw_handle_event event: %s (0x%02x), flags: %08x",
      nfa_rw_evt_2_str(p_msg->event).c_str(), p_msg->event, nfa_rw_cb.flags);

  /* Get NFA_RW sub-event */
  act_idx = (p_msg->event & 0x00FF);
  if (act_idx < (NFA_RW_MAX_EVT & 0xFF)) {
    return (*nfa_rw_action_tbl[act_idx])((tNFA_RW_MSG*)p_msg);
  } else {
    LOG(ERROR) << StringPrintf("nfa_rw_handle_event: unhandled event 0x%02X",
                               p_msg->event);
    return true;
  }
}

/*******************************************************************************
**
** Function         nfa_rw_evt_2_str
**
** Description      convert nfa_rw evt to string
**
*******************************************************************************/
static std::string nfa_rw_evt_2_str(uint16_t event) {
  switch (event) {
    case NFA_RW_OP_REQUEST_EVT:
      return "NFA_RW_OP_REQUEST_EVT";
    case NFA_RW_ACTIVATE_NTF_EVT:
      return "NFA_RW_ACTIVATE_NTF_EVT";
    case NFA_RW_DEACTIVATE_NTF_EVT:
      return "NFA_RW_DEACTIVATE_NTF_EVT";
    case NFA_RW_PRESENCE_CHECK_TICK_EVT:
      return "NFA_RW_PRESENCE_CHECK_TICK_EVT";
    case NFA_RW_PRESENCE_CHECK_TIMEOUT_EVT:
      return "NFA_RW_PRESENCE_CHECK_TIMEOUT_EVT";
    default:
      return "Unknown";
  }
}
