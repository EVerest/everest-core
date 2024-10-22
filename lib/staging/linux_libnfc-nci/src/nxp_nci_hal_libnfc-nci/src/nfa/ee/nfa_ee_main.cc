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
 *  This is the main implementation file for the NFA EE.
 *
 ******************************************************************************/
#include <string>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nfa_dm_int.h"
#include "nfa_ee_int.h"
#include "nfc_config.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/*****************************************************************************
**  Global Variables
*****************************************************************************/

/* system manager control block definition */
tNFA_EE_CB nfa_ee_cb;

/*****************************************************************************
**  Constants
*****************************************************************************/
static const tNFA_SYS_REG nfa_ee_sys_reg = {nfa_ee_sys_enable, nfa_ee_evt_hdlr,
                                            nfa_ee_sys_disable,
                                            nfa_ee_proc_nfcc_power_mode};

#define NFA_EE_NUM_ACTIONS (NFA_EE_MAX_EVT & 0x00ff)

const tNFA_EE_SM_ACT nfa_ee_actions[] = {
    /* NFA-EE action function/ internal events */
    nfa_ee_api_discover,        /* NFA_EE_API_DISCOVER_EVT      */
    nfa_ee_api_register,        /* NFA_EE_API_REGISTER_EVT      */
    nfa_ee_api_deregister,      /* NFA_EE_API_DEREGISTER_EVT    */
    nfa_ee_api_mode_set,        /* NFA_EE_API_MODE_SET_EVT      */
    nfa_ee_api_set_tech_cfg,    /* NFA_EE_API_SET_TECH_CFG_EVT  */
    nfa_ee_api_clear_tech_cfg,  /*NFA_EE_API_CLEAR_TECH_CFG_EVT */
    nfa_ee_api_set_proto_cfg,   /* NFA_EE_API_SET_PROTO_CFG_EVT */
    nfa_ee_api_clear_proto_cfg, /*NFA_EE_API_CLEAR_PROTO_CFG_EVT*/
    nfa_ee_api_add_aid,         /* NFA_EE_API_ADD_AID_EVT       */
    nfa_ee_api_remove_aid,      /* NFA_EE_API_REMOVE_AID_EVT    */
    nfa_ee_api_add_sys_code,    /* NFA_EE_API_ADD_SYSCODE_EVT   */
    nfa_ee_api_remove_sys_code, /* NFA_EE_API_REMOVE_SYSCODE_EVT*/
    nfa_ee_api_lmrt_size,       /* NFA_EE_API_LMRT_SIZE_EVT     */
    nfa_ee_api_update_now,      /* NFA_EE_API_UPDATE_NOW_EVT    */
    nfa_ee_api_connect,         /* NFA_EE_API_CONNECT_EVT       */
    nfa_ee_api_send_data,       /* NFA_EE_API_SEND_DATA_EVT     */
    nfa_ee_api_disconnect,      /* NFA_EE_API_DISCONNECT_EVT    */
    nfa_ee_nci_disc_rsp,        /* NFA_EE_NCI_DISC_RSP_EVT      */
    nfa_ee_nci_disc_ntf,        /* NFA_EE_NCI_DISC_NTF_EVT      */
    nfa_ee_nci_mode_set_rsp,    /* NFA_EE_NCI_MODE_SET_RSP_EVT  */
    nfa_ee_nci_conn,            /* NFA_EE_NCI_CONN_EVT          */
    nfa_ee_nci_conn,            /* NFA_EE_NCI_DATA_EVT          */
    nfa_ee_nci_action_ntf,      /* NFA_EE_NCI_ACTION_NTF_EVT    */
    nfa_ee_nci_disc_req_ntf,    /* NFA_EE_NCI_DISC_REQ_NTF_EVT  */
    nfa_ee_nci_wait_rsp,        /* NFA_EE_NCI_WAIT_RSP_EVT      */
    nfa_ee_rout_timeout,        /* NFA_EE_ROUT_TIMEOUT_EVT      */
    nfa_ee_discv_timeout,       /* NFA_EE_DISCV_TIMEOUT_EVT     */
    nfa_ee_lmrt_to_nfcc,        /* NFA_EE_CFG_TO_NFCC_EVT       */
    nfa_ee_nci_nfcee_status_ntf /*NFA_EE_NCI_NFCEE_STATUS_NTF_EVT*/
};

/*******************************************************************************
**
** Function         nfa_ee_init
**
** Description      Initialize NFA EE control block
**                  register to NFA SYS
**
** Returns          None
**
*******************************************************************************/
void nfa_ee_init(void) {
  int xx;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* initialize control block */
  memset(&nfa_ee_cb, 0, sizeof(tNFA_EE_CB));
  for (xx = 0; xx < NFA_EE_MAX_EE_SUPPORTED; xx++) {
    nfa_ee_cb.ecb[xx].nfcee_id = NFA_EE_INVALID;
    nfa_ee_cb.ecb[xx].ee_status = NFC_NFCEE_STATUS_INACTIVE;
  }

  nfa_ee_cb.ecb[NFA_EE_CB_4_DH].ee_status = NFC_NFCEE_STATUS_ACTIVE;
  nfa_ee_cb.ecb[NFA_EE_CB_4_DH].nfcee_id = NFC_DH_ID;

  /* register message handler on NFA SYS */
  nfa_sys_register(NFA_ID_EE, &nfa_ee_sys_reg);
}

/*******************************************************************************
**
** Function         nfa_ee_sys_enable
**
** Description      Enable NFA EE
**
** Returns          None
**
*******************************************************************************/
void nfa_ee_sys_enable(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
#ifdef ANDROID
  nfa_ee_cb.route_block_control = 0x00;
  if (NfcConfig::hasKey(NAME_NFA_AID_BLOCK_ROUTE)) {
    unsigned retlen = NfcConfig::getUnsigned(NAME_NFA_AID_BLOCK_ROUTE);
    if ((retlen == 0x01) && (NFC_GetNCIVersion() == NCI_VERSION_2_0)) {
      nfa_ee_cb.route_block_control = NCI_ROUTE_QUAL_BLOCK_ROUTE;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "nfa_ee_cb.route_block_control=0x%x", nfa_ee_cb.route_block_control);
    }
  }

  if (nfa_ee_max_ee_cfg) {
    /* collect NFCEE information */
    NFC_NfceeDiscover(true);
    nfa_sys_start_timer(&nfa_ee_cb.discv_timer, NFA_EE_DISCV_TIMEOUT_EVT,
                        NFA_EE_DISCV_TIMEOUT_VAL);
  } else {
    nfa_ee_cb.em_state = NFA_EE_EM_STATE_INIT_DONE;
    nfa_sys_cback_notify_enable_complete(NFA_ID_EE);
  }

     unsigned retlen = NfcConfig::getUnsigned(NAME_NFA_AID_BLOCK_ROUTE);
    if ((retlen == 0x01) && (NFC_GetNCIVersion() == NCI_VERSION_2_0)) {
      nfa_ee_cb.route_block_control = NCI_ROUTE_QUAL_BLOCK_ROUTE;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "nfa_ee_cb.route_block_control=0x%x", nfa_ee_cb.route_block_control);
    }
#else
    if (nfa_ee_max_ee_cfg)
    {
        /* collect NFCEE information */
        NFC_NfceeDiscover (TRUE);
        nfa_sys_start_timer (&nfa_ee_cb.discv_timer, NFA_EE_DISCV_TIMEOUT_EVT, NFA_EE_DISCV_TIMEOUT_VAL);
    }
    else
    {
        nfa_ee_cb.em_state = NFA_EE_EM_STATE_INIT_DONE;
        nfa_sys_cback_notify_enable_complete (NFA_ID_EE);
    }
#endif
}

/*******************************************************************************
**
** Function         nfa_ee_restore_one_ecb
**
** Description      activate the NFCEE and restore the routing when
**                  changing power state from low power mode to full power mode
**
** Returns          None
**
*******************************************************************************/
void nfa_ee_restore_one_ecb(tNFA_EE_ECB* p_cb) {
  uint8_t mask;
  tNFC_NFCEE_MODE_SET_REVT rsp;
  tNFA_EE_NCI_MODE_SET ee_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nfcee_id:0x%x, ecb_flags:0x%x ee_status:0x%x "
      "ee_old_status: 0x%x",
      p_cb->nfcee_id, p_cb->ecb_flags, p_cb->ee_status, p_cb->ee_old_status);
  if ((p_cb->nfcee_id != NFA_EE_INVALID) &&
      (p_cb->ee_status & NFA_EE_STATUS_RESTORING) == 0 &&
      (p_cb->ee_old_status & NFA_EE_STATUS_RESTORING) != 0) {
    p_cb->ee_old_status &= ~NFA_EE_STATUS_RESTORING;
    mask = nfa_ee_ecb_to_mask(p_cb);
    if (p_cb->ee_status != p_cb->ee_old_status) {
      p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_RESTORE;
      if (p_cb->ee_old_status == NFC_NFCEE_STATUS_ACTIVE) {
        NFC_NfceeModeSet(p_cb->nfcee_id, NFC_MODE_ACTIVATE);

        if (nfa_ee_cb.ee_cfged & mask) {
          /* if any routing is configured on this NFCEE. need to mark this NFCEE
           * as changed
           * to cause the configuration to be sent to NFCC again */
          p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_ROUTING;
          p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_VS;
        }
      } else {
        NFC_NfceeModeSet(p_cb->nfcee_id, NFC_MODE_DEACTIVATE);
      }
    } else if (p_cb->ee_status == NFC_NFCEE_STATUS_ACTIVE) {
      /* the initial NFCEE status after start up is the same as the current
       * status and it's active:
       * process the same as the host gets activate rsp */
      p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_RESTORE;
      if (nfa_ee_cb.ee_cfged & mask) {
        /* if any routing is configured on this NFCEE. need to mark this NFCEE
         * as changed
         * to cause the configuration to be sent to NFCC again */
        p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_ROUTING;
        p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_VS;
      }
      rsp.mode = NFA_EE_MD_ACTIVATE;
      rsp.nfcee_id = p_cb->nfcee_id;
      rsp.status = NFA_STATUS_OK;
      ee_msg.p_data = &rsp;
      tNFA_EE_MSG nfa_ee_msg;
      nfa_ee_msg.mode_set_rsp = ee_msg;
      nfa_ee_nci_mode_set_rsp(&nfa_ee_msg);
    }
  }
}

/*******************************************************************************
**
** Function         nfa_ee_proc_nfcc_power_mode
**
** Description      Restore NFA EE sub-module
**
** Returns          None
**
*******************************************************************************/
void nfa_ee_proc_nfcc_power_mode(uint8_t nfcc_power_mode) {
  uint32_t xx;
  tNFA_EE_ECB* p_cb;
  bool proc_complete = true;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfcc_power_mode=%d", nfcc_power_mode);
  /* if NFCC power state is change to full power */
  if (nfcc_power_mode == NFA_DM_PWR_MODE_FULL) {
    if (nfa_ee_max_ee_cfg) {
      p_cb = nfa_ee_cb.ecb;
      for (xx = 0; xx < NFA_EE_MAX_EE_SUPPORTED; xx++, p_cb++) {
        p_cb->ee_old_status = 0;
        if (xx >= nfa_ee_cb.cur_ee) p_cb->nfcee_id = NFA_EE_INVALID;

        if ((p_cb->nfcee_id != NFA_EE_INVALID) &&
            (p_cb->ee_interface[0] != NFC_NFCEE_INTERFACE_HCI_ACCESS) &&
            (p_cb->ee_status != NFA_EE_STATUS_REMOVED)) {
          proc_complete = false;
          /* NFA_EE_STATUS_RESTORING bit makes sure the ee_status restore to
           * ee_old_status
           * NFA_EE_STATUS_RESTORING bit is cleared in ee_status at
           * NFCEE_DISCOVER NTF.
           * NFA_EE_STATUS_RESTORING bit is cleared in ee_old_status at
           * restoring the activate/inactive status after NFCEE_DISCOVER NTF */
          p_cb->ee_status |= NFA_EE_STATUS_RESTORING;
          p_cb->ee_old_status = p_cb->ee_status;
          /* NFA_EE_FLAGS_RESTORE bit makes sure the routing/nci logical
           * connection is restore to prior to entering low power mode */
          p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_RESTORE;
        }
      }
      nfa_ee_cb.em_state = NFA_EE_EM_STATE_RESTORING;
      nfa_ee_cb.num_ee_expecting = 0;
      if (nfa_sys_is_register(NFA_ID_HCI)) {
        nfa_ee_cb.ee_flags |= NFA_EE_FLAG_WAIT_HCI;
        nfa_ee_cb.ee_flags |= NFA_EE_FLAG_NOTIFY_HCI;
      }
      NFC_NfceeDiscover(true);
      nfa_sys_start_timer(&nfa_ee_cb.discv_timer, NFA_EE_DISCV_TIMEOUT_EVT,
                          NFA_EE_DISCV_TIMEOUT_VAL);
    }
  } else {
    nfa_sys_stop_timer(&nfa_ee_cb.timer);
    nfa_sys_stop_timer(&nfa_ee_cb.discv_timer);
    nfa_ee_cb.num_ee_expecting = 0;
  }

  if (proc_complete)
    nfa_sys_cback_notify_nfcc_power_mode_proc_complete(NFA_ID_EE);
}

/*******************************************************************************
**
** Function         nfa_ee_proc_hci_info_cback
**
** Description      HCI initialization complete from power off sleep mode
**
** Returns          None
**
*******************************************************************************/
void nfa_ee_proc_hci_info_cback(void) {
  uint32_t xx;
  tNFA_EE_ECB* p_cb;
  tNFA_EE_MSG data;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;
  /* if NFCC power state is change to full power */
  nfa_ee_cb.ee_flags &= ~NFA_EE_FLAG_WAIT_HCI;

  p_cb = nfa_ee_cb.ecb;
  for (xx = 0; xx < NFA_EE_MAX_EE_SUPPORTED; xx++, p_cb++) {
    /* NCI spec says: An NFCEE_DISCOVER_NTF that contains a Protocol type of
     * "HCI Access"
     * SHALL NOT contain any other additional Protocol
     * i.e. check only first supported NFCEE interface is HCI access */
    /* NFA_HCI module handles restoring configurations for HCI access */
    if (p_cb->ee_interface[0] != NFC_NFCEE_INTERFACE_HCI_ACCESS) {
      nfa_ee_restore_one_ecb(p_cb);
    }
  }

  if (nfa_ee_restore_ntf_done()) {
    nfa_ee_check_restore_complete();
    if (nfa_ee_cb.em_state == NFA_EE_EM_STATE_INIT_DONE) {
      if (nfa_ee_cb.discv_timer.in_use) {
        nfa_sys_stop_timer(&nfa_ee_cb.discv_timer);
        data.hdr.event = NFA_EE_DISCV_TIMEOUT_EVT;
        nfa_ee_evt_hdlr(&data.hdr);
      }
    }
  }
}

/*******************************************************************************
**
** Function         nfa_ee_proc_evt
**
** Description      Process NFCEE related events from NFC stack
**
**
** Returns          None
**
*******************************************************************************/
void nfa_ee_proc_evt(tNFC_RESPONSE_EVT event, void* p_data) {
  tNFA_EE_INT_EVT int_event = 0;
  tNFA_EE_NCI_WAIT_RSP cbk = tNFA_EE_NCI_WAIT_RSP();

  switch (event) {
    case NFC_NFCEE_DISCOVER_REVT: /* 4  NFCEE Discover response */
      int_event = NFA_EE_NCI_DISC_RSP_EVT;
      break;

    case NFC_NFCEE_INFO_REVT: /* 5  NFCEE Discover Notification */
      int_event = NFA_EE_NCI_DISC_NTF_EVT;
      break;

    case NFC_NFCEE_MODE_SET_REVT: /* 6  NFCEE Mode Set response */
      int_event = NFA_EE_NCI_MODE_SET_RSP_EVT;
      break;

    case NFC_EE_ACTION_REVT:
      int_event = NFA_EE_NCI_ACTION_NTF_EVT;
      break;

    case NFC_EE_DISCOVER_REQ_REVT: /* 10 EE Discover Req notification */
      int_event = NFA_EE_NCI_DISC_REQ_NTF_EVT;
      break;

    case NFC_SET_ROUTING_REVT:
      int_event = NFA_EE_NCI_WAIT_RSP_EVT;
      cbk.opcode = NCI_MSG_RF_SET_ROUTING;
      break;

    case NFC_NFCEE_STATUS_REVT:
      int_event = NFA_EE_NCI_NFCEE_STATUS_NTF_EVT;
      break;
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nfa_ee_proc_evt: event=0x%02x int_event:0x%x", event, int_event);
  if (int_event) {
    cbk.hdr.event = int_event;
    cbk.p_data = p_data;
    tNFA_EE_MSG nfa_ee_msg;
    nfa_ee_msg.wait_rsp = cbk;

    nfa_ee_evt_hdlr(&nfa_ee_msg.hdr);
  }
}

/*******************************************************************************
**
** Function         nfa_ee_ecb_to_mask
**
** Description      Given a ecb, return the bit mask to be used in
**                  nfa_ee_cb.ee_cfged
**
** Returns          the bitmask for the given ecb.
**
*******************************************************************************/
uint8_t nfa_ee_ecb_to_mask(tNFA_EE_ECB* p_cb) {
  uint8_t mask;
  uint8_t index;

  index = (uint8_t)(p_cb - nfa_ee_cb.ecb);
  mask = 1 << index;

  return mask;
}

/*******************************************************************************
**
** Function         nfa_ee_find_ecb
**
** Description      Return the ecb associated with the given nfcee_id
**
** Returns          tNFA_EE_ECB
**
*******************************************************************************/
tNFA_EE_ECB* nfa_ee_find_ecb(uint8_t nfcee_id) {
  uint32_t xx;
  tNFA_EE_ECB *p_ret = nullptr, *p_cb;
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (nfcee_id == NFC_DH_ID) {
    p_ret = &nfa_ee_cb.ecb[NFA_EE_CB_4_DH];
  } else {
    p_cb = nfa_ee_cb.ecb;
    for (xx = 0; xx < NFA_EE_MAX_EE_SUPPORTED; xx++, p_cb++) {
      if (nfcee_id == p_cb->nfcee_id) {
        p_ret = p_cb;
        break;
      }
    }
  }

  return p_ret;
}

/*******************************************************************************
**
** Function         nfa_ee_find_ecb_by_conn_id
**
** Description      Return the ecb associated with the given connection id
**
** Returns          tNFA_EE_ECB
**
*******************************************************************************/
tNFA_EE_ECB* nfa_ee_find_ecb_by_conn_id(uint8_t conn_id) {
  uint32_t xx;
  tNFA_EE_ECB *p_ret = nullptr, *p_cb;
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  p_cb = nfa_ee_cb.ecb;
  for (xx = 0; xx < nfa_ee_cb.cur_ee; xx++, p_cb++) {
    if (conn_id == p_cb->conn_id) {
      p_ret = p_cb;
      break;
    }
  }

  return p_ret;
}

/*******************************************************************************
**
** Function         nfa_ee_sys_disable
**
** Description      Deregister NFA EE from NFA SYS/DM
**
**
** Returns          None
**
*******************************************************************************/
void nfa_ee_sys_disable(void) {
  uint32_t xx;
  tNFA_EE_ECB* p_cb;
  tNFA_EE_MSG msg;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  nfa_ee_cb.em_state = NFA_EE_EM_STATE_DISABLED;
  /* report NFA_EE_DEREGISTER_EVT to all registered to EE */
  for (xx = 0; xx < NFA_EE_MAX_CBACKS; xx++) {
    if (nfa_ee_cb.p_ee_cback[xx]) {
      msg.deregister.index = xx;
      nfa_ee_api_deregister(&msg);
    }
  }

  nfa_ee_cb.num_ee_expecting = 0;
  p_cb = nfa_ee_cb.ecb;
  for (xx = 0; xx < nfa_ee_cb.cur_ee; xx++, p_cb++) {
    if (p_cb->conn_st == NFA_EE_CONN_ST_CONN) {
      if (nfa_sys_is_graceful_disable()) {
        /* Disconnect NCI connection on graceful shutdown */
        msg.disconnect.p_cb = p_cb;
        nfa_ee_api_disconnect(&msg);
        nfa_ee_cb.num_ee_expecting++;
      } else {
        /* fake NFA_EE_DISCONNECT_EVT on ungraceful shutdown */
        msg.conn.conn_id = p_cb->conn_id;
        msg.conn.event = NFC_CONN_CLOSE_CEVT;
        nfa_ee_nci_conn(&msg);
      }
    }
  }

  if (nfa_ee_cb.num_ee_expecting) {
    nfa_ee_cb.ee_flags |= NFA_EE_FLAG_WAIT_DISCONN;
    nfa_ee_cb.em_state = NFA_EE_EM_STATE_DISABLING;
  }

  nfa_sys_stop_timer(&nfa_ee_cb.timer);
  nfa_sys_stop_timer(&nfa_ee_cb.discv_timer);

  /* If Application initiated NFCEE discovery, fake/report the event */
  nfa_ee_report_disc_done(false);

  /* deregister message handler on NFA SYS */
  if (nfa_ee_cb.em_state == NFA_EE_EM_STATE_DISABLED)
    nfa_sys_deregister(NFA_ID_EE);
}

/*******************************************************************************
**
** Function         nfa_ee_check_disable
**
** Description      Check if it is safe to move to disabled state
**
** Returns          None
**
*******************************************************************************/
void nfa_ee_check_disable(void) {
  if (!(nfa_ee_cb.ee_flags & NFA_EE_FLAG_WAIT_DISCONN)) {
    nfa_ee_cb.em_state = NFA_EE_EM_STATE_DISABLED;
    nfa_sys_deregister(NFA_ID_EE);
  }
}
/*******************************************************************************
**
** Function         nfa_ee_reg_cback_enable_done
**
** Description      Allow a module to register to EE to be notified when NFA-EE
**                  finishes enable process
**
** Returns          None
**
*******************************************************************************/
void nfa_ee_reg_cback_enable_done(tNFA_EE_ENABLE_DONE_CBACK* p_cback) {
  nfa_ee_cb.p_enable_cback = p_cback;
}

/*******************************************************************************
**
** Function         nfa_ee_sm_st_2_str
**
** Description      convert nfa-ee state to string
**
*******************************************************************************/
static std::string nfa_ee_sm_st_2_str(uint8_t state) {
  switch (state) {
    case NFA_EE_EM_STATE_INIT:
      return "INIT";
    case NFA_EE_EM_STATE_INIT_DONE:
      return "INIT_DONE";
    case NFA_EE_EM_STATE_RESTORING:
      return "RESTORING";
    case NFA_EE_EM_STATE_DISABLING:
      return "DISABLING";
    case NFA_EE_EM_STATE_DISABLED:
      return "DISABLED";
    default:
      return "Unknown";
  }
}

/*******************************************************************************
**
** Function         nfa_ee_sm_evt_2_str
**
** Description      convert nfa-ee evt to string
**
*******************************************************************************/
static std::string nfa_ee_sm_evt_2_str(uint16_t event) {
  switch (event) {
    case NFA_EE_API_DISCOVER_EVT:
      return "API_DISCOVER";
    case NFA_EE_API_REGISTER_EVT:
      return "API_REGISTER";
    case NFA_EE_API_DEREGISTER_EVT:
      return "API_DEREGISTER";
    case NFA_EE_API_MODE_SET_EVT:
      return "API_MODE_SET";
    case NFA_EE_API_SET_TECH_CFG_EVT:
      return "API_SET_TECH_CFG";
    case NFA_EE_API_CLEAR_TECH_CFG_EVT:
      return "API_CLEAR_TECH_CFG";
    case NFA_EE_API_SET_PROTO_CFG_EVT:
      return "API_SET_PROTO_CFG";
    case NFA_EE_API_CLEAR_PROTO_CFG_EVT:
      return "API_CLEAR_PROTO_CFG";
    case NFA_EE_API_ADD_AID_EVT:
      return "API_ADD_AID";
    case NFA_EE_API_REMOVE_AID_EVT:
      return "API_REMOVE_AID";
    case NFA_EE_API_ADD_SYSCODE_EVT:
      return "NFA_EE_API_ADD_SYSCODE_EVT";
    case NFA_EE_API_REMOVE_SYSCODE_EVT:
      return "NFA_EE_API_REMOVE_SYSCODE_EVT";
    case NFA_EE_API_LMRT_SIZE_EVT:
      return "API_LMRT_SIZE";
    case NFA_EE_API_UPDATE_NOW_EVT:
      return "API_UPDATE_NOW";
    case NFA_EE_API_CONNECT_EVT:
      return "API_CONNECT";
    case NFA_EE_API_SEND_DATA_EVT:
      return "API_SEND_DATA";
    case NFA_EE_API_DISCONNECT_EVT:
      return "API_DISCONNECT";
    case NFA_EE_NCI_DISC_RSP_EVT:
      return "NCI_DISC_RSP";
    case NFA_EE_NCI_DISC_NTF_EVT:
      return "NCI_DISC_NTF";
    case NFA_EE_NCI_MODE_SET_RSP_EVT:
      return "NCI_MODE_SET";
    case NFA_EE_NCI_CONN_EVT:
      return "NCI_CONN";
    case NFA_EE_NCI_DATA_EVT:
      return "NCI_DATA";
    case NFA_EE_NCI_ACTION_NTF_EVT:
      return "NCI_ACTION";
    case NFA_EE_NCI_DISC_REQ_NTF_EVT:
      return "NCI_DISC_REQ";
    case NFA_EE_NCI_WAIT_RSP_EVT:
      return "NCI_WAIT_RSP";
    case NFA_EE_ROUT_TIMEOUT_EVT:
      return "ROUT_TIMEOUT";
    case NFA_EE_DISCV_TIMEOUT_EVT:
      return "NFA_EE_DISCV_TIMEOUT_EVT";
    case NFA_EE_CFG_TO_NFCC_EVT:
      return "CFG_TO_NFCC";
    default:
      return "Unknown";
  }
}

/*******************************************************************************
**
** Function         nfa_ee_evt_hdlr
**
** Description      Processing event for NFA EE
**
**
** Returns          TRUE if p_msg needs to be deallocated
**
*******************************************************************************/
bool nfa_ee_evt_hdlr(NFC_HDR* p_msg) {
  bool act = false;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "Event %s(0x%02x), State: %s(%d)",
      nfa_ee_sm_evt_2_str(p_msg->event).c_str(), p_msg->event,
      nfa_ee_sm_st_2_str(nfa_ee_cb.em_state).c_str(), nfa_ee_cb.em_state);

  switch (nfa_ee_cb.em_state) {
    case NFA_EE_EM_STATE_INIT_DONE:
    case NFA_EE_EM_STATE_RESTORING:
      act = true;
      break;
    case NFA_EE_EM_STATE_INIT:
      if ((p_msg->event == NFA_EE_NCI_DISC_NTF_EVT) ||
          (p_msg->event == NFA_EE_NCI_DISC_RSP_EVT))
        act = true;
      break;
    case NFA_EE_EM_STATE_DISABLING:
      if (p_msg->event == NFA_EE_NCI_CONN_EVT) act = true;
      break;
  }

  tNFA_EE_MSG* p_evt_data = (tNFA_EE_MSG*)p_msg;
  if (act) {
    uint16_t event = p_msg->event & 0x00ff;
    if (event < NFA_EE_NUM_ACTIONS) {
      (*nfa_ee_actions[event])(p_evt_data);
    }
  } else {
    /* If the event is not handled, free the data packet. */
    /* FIXME: Is it really always tNFA_EE_NCI_CONN? */
    if (p_msg->event == NFA_EE_NCI_DATA_EVT)
      GKI_freebuf(p_evt_data->conn.p_data);
  }

  return true;
}
