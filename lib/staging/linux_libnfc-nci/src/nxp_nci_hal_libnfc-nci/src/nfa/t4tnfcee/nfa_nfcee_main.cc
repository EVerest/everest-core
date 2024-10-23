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
#include "nfa_dm_int.h"
#include "nfa_ee_int.h"
#include "nfa_nfcee_int.h"
#include "nfa_rw_int.h"
#include "nfc_config.h"
#include "phNxpConfig.h"
#if (NXP_EXTNS == TRUE)
using android::base::StringPrintf;

extern bool nfc_debug_enabled;

tNFA_T4TNFCEE_CB nfa_t4tnfcee_cb;
void nfa_t4tnfcee_info_cback(tNFA_EE_EVT event, tNFA_EE_CBACK_DATA* p_data);
static void nfa_t4tnfcee_sys_enable(void);
static void nfa_t4tnfcee_sys_disable(void);

#define NFA_T4T_NFCEE_ENANLE_BIT_POS 0x01

/*****************************************************************************
** Constants and types
*****************************************************************************/
static const tNFA_SYS_REG nfa_t4tnfcee_sys_reg = {
    nfa_t4tnfcee_sys_enable, nfa_t4tnfcee_handle_event,
    nfa_t4tnfcee_sys_disable, NULL};
/* NFA_T4TNFCEE actions */
const tNFA_T4TNFCEE_ACTION nfa_t4tnfcee_action_tbl[] = {
    nfa_t4tnfcee_handle_op_req, /* NFA_T4TNFCEE_OP_REQUEST_EVT            */
};

/*******************************************************************************
**
** Function         nfa_t4tnfcee_init
**
** Description      Initialize NFA T4TNFCEE
**
** Returns          None
**
*******************************************************************************/
void nfa_t4tnfcee_init(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  unsigned long num = 0;
  if(GetNxpNumValue(NAME_NXP_T4T_NFCEE_ENABLE, &num, sizeof(num))) {
    if (NFA_T4T_NFCEE_ENANLE_BIT_POS & num) {
      /* initialize control block */
      memset(&nfa_t4tnfcee_cb, 0, sizeof(tNFA_T4TNFCEE_CB));
      nfa_t4tnfcee_cb.t4tnfcee_state = NFA_T4TNFCEE_STATE_DISABLED;
      /* register message handler on NFA SYS */
      nfa_sys_register(NFA_ID_T4TNFCEE, &nfa_t4tnfcee_sys_reg);
    }
  } 
}

/*******************************************************************************
**
** Function         nfa_t4tnfcee_deinit
**
** Description      DeInitialize NFA T4TNFCEE
**
** Returns          None
**
*******************************************************************************/
void nfa_t4tnfcee_deinit(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("nfa_t4tnfcee_deinit ()");

  /* reset state */
  nfa_t4tnfcee_cb.t4tnfcee_state = NFA_T4TNFCEE_STATE_DISABLED;

}

/*******************************************************************************
**
** Function         nfa_t4tnfcee_conn_cback
**
** Description      This function Process event from NCI
**
** Returns          None
**
*******************************************************************************/
static void nfa_t4tnfcee_conn_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                                    tNFC_CONN* p_data) {
  tNFA_CONN_EVT_DATA conn_evt_data;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s : Enter, conn_id = %d, event = 0x%x", __func__, conn_id, event);
  switch (event) {
    case NFC_CONN_CREATE_CEVT: {
      if (conn_id == NCI_DEST_TYPE_T4T_NFCEE) {
        if (p_data->status != NFA_STATUS_OK) {
          NFC_ConnClose(NCI_DEST_TYPE_T4T_NFCEE);
          nfa_t4tnfcee_cb.t4tnfcee_state = NFA_T4TNFCEE_STATE_OPEN_FAILED;
        } else {
          conn_evt_data.status = NFA_STATUS_OK;
        }
      }
      break;
    }
    case NFC_CONN_CLOSE_CEVT: {
      if (conn_id == NCI_DEST_TYPE_T4T_NFCEE) {
        if (p_data->status != NFA_STATUS_OK) {
          conn_evt_data.status = NFA_STATUS_FAILED;
        } else {
          nfa_t4tnfcee_cb.t4tnfcee_state = NFA_T4TNFCEE_STATE_DISCONNECTED;
          conn_evt_data.status = p_data->status;
        }
        /*reset callbacks*/
        RW_SetT4tNfceeInfo(NULL, 0);
      }
      break;
    }
    default:
      conn_evt_data.status = NFA_STATUS_FAILED;
      RW_SetT4tNfceeInfo(NULL, 0);
      break;
  }
  nfa_dm_act_conn_cback_notify(NFA_T4TNFCEE_EVT, &conn_evt_data);
}

/*******************************************************************************
 **
 ** Function         nfa_t4tnfcee_info_cback
 **
 ** Description      Callback function to handle EE configuration events
 **
 ** Returns          None
 **
 *******************************************************************************/
void nfa_t4tnfcee_info_cback(tNFA_EE_EVT event, tNFA_EE_CBACK_DATA* p_data) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s event: %x", __func__, event);
  switch (event) {
    case NFA_EE_DISCOVER_EVT:
      if (nfa_t4tnfcee_cb.t4tnfcee_state == NFA_T4TNFCEE_STATE_DISABLED) {
        nfa_t4tnfcee_cb.t4tnfcee_state = NFA_T4TNFCEE_STATE_TRY_ENABLE;
        if ((p_data != nullptr) &&
            (p_data->new_ee.ee_status != NFA_STATUS_OK)) {
          NFC_NfceeModeSet(T4TNFCEE_TARGET_HANDLE, NFC_MODE_ACTIVATE);
        }
      }
      break;
    case NFA_EE_MODE_SET_EVT:
      if ((p_data != nullptr) && (p_data->mode_set.status != NFA_STATUS_OK) &&
          (nfa_t4tnfcee_cb.t4tnfcee_state >= NFA_T4TNFCEE_STATE_TRY_ENABLE)) {
        nfa_t4tnfcee_cb.t4tnfcee_state = NFA_T4TNFCEE_STATE_DISABLED;
        nfa_sys_cback_notify_enable_complete(NFA_ID_T4TNFCEE);
        nfa_ee_report_disc_done(true);
      } else {
        nfa_ee_report_event(NULL,event,p_data);
      }
      break;
    case NFA_EE_DISCOVER_REQ_EVT:
      if (nfa_t4tnfcee_cb.t4tnfcee_state == NFA_T4TNFCEE_STATE_TRY_ENABLE) {
        nfa_t4tnfcee_cb.t4tnfcee_state = NFA_T4TNFCEE_STATE_INITIALIZED;
        nfa_sys_cback_notify_enable_complete(NFA_ID_T4TNFCEE);
        nfa_ee_report_disc_done(true);
      }
      break;
    case NFA_EE_CONNECT_EVT:
      if ((nfa_t4tnfcee_cb.t4tnfcee_state == NFA_T4TNFCEE_STATE_INITIALIZED) ||
          (nfa_t4tnfcee_cb.t4tnfcee_state == NFA_T4TNFCEE_STATE_DISCONNECTED)) {
        if (NFC_STATUS_OK ==
            NFC_ConnCreate(NCI_DEST_TYPE_NFCEE, T4TNFCEE_TARGET_HANDLE,
                           NFC_NFCEE_INTERFACE_APDU, nfa_t4tnfcee_conn_cback))
          nfa_t4tnfcee_cb.t4tnfcee_state = NFA_T4TNFCEE_STATE_CONNECTED;
      } else {
        tNFC_CONN p_data;
        p_data.status = NFC_STATUS_FAILED;
        nfa_t4tnfcee_conn_cback(NCI_DEST_TYPE_T4T_NFCEE, NFC_ERROR_CEVT,
                                &p_data);
      }
      break;
    default:
      nfa_ee_report_event(NULL, event, p_data);
      break;
  }
  return;
}

/*******************************************************************************
**
** Function         nfa_t4tnfcee_set_ee_cback
**
** Description      assign t4t callback to receive ee_events
**
** Returns          None
**
*******************************************************************************/
void nfa_t4tnfcee_set_ee_cback(tNFA_EE_ECB* p_ecb) {
  p_ecb->p_ee_cback = nfa_t4tnfcee_info_cback;
  return;
}

/*******************************************************************************
**
** Function         nfa_rw_evt_2_str
**
** Description      convert nfa_rw evt to string
**
*******************************************************************************/
static std::string nfa_t4tnfcee_evt_2_str(uint16_t event) {
  switch (event) {
    case NFA_RW_OP_REQUEST_EVT:
      return "NFA_T4TNFCEE_OP_REQUEST_EVT";
    default:
      break;
  }
  return "Unknown";
}

/*******************************************************************************
**
** Function         nfa_t4tnfcee_sys_enable
**
** Description      Enable NFA HCI
**
** Returns          None
**
*******************************************************************************/
void nfa_t4tnfcee_sys_enable(void) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_t4tnfcee_sys_enable ()");
}

/*******************************************************************************
**
** Function         nfa_t4tnfcee_sys_disable
**
** Description      Clean up t4tnfcee sub-system
**
**
** Returns          void
**
*******************************************************************************/
void nfa_t4tnfcee_sys_disable(void) {
  /* Free scratch buffer if any */
  nfa_t4tnfcee_free_rx_buf();

  /* Free pending command if any */
  if (nfa_t4tnfcee_cb.p_pending_msg) {
    GKI_freebuf(nfa_t4tnfcee_cb.p_pending_msg);
    nfa_t4tnfcee_cb.p_pending_msg = NULL;
  }

  nfa_sys_deregister(NFA_ID_T4TNFCEE);
}

/*******************************************************************************
**
** Function         nfa_t4tnfcee_proc_disc_evt
**
** Description      Called by nfa_dm to handle Ndef Nfcee Requests
**
** Returns          NFA_STATUS_OK if success else Failed status
**
*******************************************************************************/
tNFC_STATUS nfa_t4tnfcee_proc_disc_evt(tNFA_T4TNFCEE_OP event) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s Enter. Event = %d ", __func__, (int)event);
  tNFC_STATUS status = NFC_STATUS_FAILED;

  switch (event) {
    case NFA_T4TNFCEE_OP_OPEN_CONNECTION:
      nfa_t4tnfcee_info_cback(NFA_EE_CONNECT_EVT, nullptr);
      break;
    case NFA_T4TNFCEE_OP_CLOSE_CONNECTION:
      if (nfa_t4tnfcee_cb.t4tnfcee_state == NFA_T4TNFCEE_STATE_CONNECTED) {
        NFC_SetStaticT4tNfceeCback(nfa_t4tnfcee_conn_cback);
        if (NFC_STATUS_OK != NFC_ConnClose(NCI_DEST_TYPE_T4T_NFCEE)) {
          tNFC_CONN p_data;
          p_data.status = NFC_STATUS_FAILED;
          nfa_t4tnfcee_conn_cback(NCI_DEST_TYPE_T4T_NFCEE, NFC_ERROR_CEVT,
                                  &p_data);
        }
      }
      break;
  }
  return status;
}

/*******************************************************************************
**
** Function         nfa_t4tnfcee_handle_event
**
** Description      nfa t4tnfcee main event handling function.
**
** Returns          true if caller should free p_msg buffer
**
*******************************************************************************/
bool nfa_t4tnfcee_handle_event(NFC_HDR* p_msg) {
  uint16_t act_idx;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nfa_t4tnfcee_handle_event event: %s (0x%02x)",
      nfa_t4tnfcee_evt_2_str(p_msg->event).c_str(), p_msg->event);

  /* Get NFA_T4TNFCEE sub-event */
  if ((act_idx = (p_msg->event & 0x00FF)) < (NFA_T4TNFCEE_MAX_EVT & 0xFF)) {
    return (*nfa_t4tnfcee_action_tbl[act_idx])((tNFA_T4TNFCEE_MSG*)p_msg);
  } else {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "nfa_t4tnfcee_handle_event: unhandled event 0x%02X", p_msg->event);
    return true;
  }
}

/*******************************************************************************
**
** Function         nfa_t4tnfcee_is_enabled
**
** Description      T4T is enabled and initialized.
**
** Returns          true if T4T Nfcee is enabled initialization
**
*******************************************************************************/
bool nfa_t4tnfcee_is_enabled(void) {
  return (nfa_t4tnfcee_cb.t4tnfcee_state >= NFA_T4TNFCEE_STATE_INITIALIZED);
}

/*******************************************************************************
**
** Function         nfa_t4tnfcee_is_processing
**
** Description      Indicates if T4tNfcee Read or write under process
**
** Returns          true if under process else false
**
*******************************************************************************/
bool nfa_t4tnfcee_is_processing(void) {
 return (nfa_t4tnfcee_cb.t4tnfcee_state == NFA_T4TNFCEE_STATE_CONNECTED);
}
#endif
