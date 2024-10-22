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
 *  This file contains function of the NFC unit to receive/process NCI
 *  commands.
 *
 ******************************************************************************/
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nfc_target.h"

#include "bt_types.h"
#include "gki.h"
#include "nci_defs.h"
#include "nci_hmsgs.h"
#include "nfc_api.h"
#include "nfc_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/*******************************************************************************
**
** Function         nci_proc_core_rsp
**
** Description      Process NCI responses in the CORE group
**
** Returns          TRUE-caller of this function to free the GKI buffer p_msg
**
*******************************************************************************/
bool nci_proc_core_rsp(NFC_HDR* p_msg) {
  uint8_t* p;
  uint8_t *pp, len, op_code;
  bool free = true;
  uint8_t* p_old = nfc_cb.last_cmd;

  /* find the start of the NCI message and parse the NCI header */
  p = (uint8_t*)(p_msg + 1) + p_msg->offset;
  pp = p + 1;
  NCI_MSG_PRS_HDR1(pp, op_code);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nci_proc_core_rsp opcode:0x%x", op_code);
  len = *pp++;

  /* process the message based on the opcode and message type */
  switch (op_code) {
    case NCI_MSG_CORE_RESET:
      nfc_ncif_proc_reset_rsp(pp, false);
      break;

    case NCI_MSG_CORE_INIT:
      nfc_ncif_proc_init_rsp(p_msg);
      free = false;
      break;

    case NCI_MSG_CORE_GET_CONFIG:
      nfc_ncif_proc_get_config_rsp(p_msg);
      break;

    case NCI_MSG_CORE_SET_CONFIG:
      nfc_ncif_set_config_status(pp, len);
      break;

    case NCI_MSG_CORE_CONN_CREATE:
      nfc_ncif_proc_conn_create_rsp(p, p_msg->len, *p_old);
      break;

    case NCI_MSG_CORE_CONN_CLOSE:
      nfc_ncif_report_conn_close_evt(*p_old, *pp);
      break;
    case NCI_MSG_CORE_SET_POWER_SUB_STATE:
      nfc_ncif_event_status(NFC_SET_POWER_SUB_STATE_REVT, *pp);
      break;
    default:
      LOG(ERROR) << StringPrintf("unknown opcode:0x%x", op_code);
      break;
  }

  return free;
}

/*******************************************************************************
**
** Function         nci_proc_core_ntf
**
** Description      Process NCI notifications in the CORE group
**
** Returns          void
**
*******************************************************************************/
void nci_proc_core_ntf(NFC_HDR* p_msg) {
  uint8_t* p;
  uint8_t *pp, len, op_code;
  uint8_t conn_id;

  /* find the start of the NCI message and parse the NCI header */
  p = (uint8_t*)(p_msg + 1) + p_msg->offset;
  len = p_msg->len;
  pp = p + 1;

  if (len < NCI_MSG_HDR_SIZE) {
    LOG(ERROR) << __func__ << ": Invalid packet length";
    return;
  }
  NCI_MSG_PRS_HDR1(pp, op_code);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nci_proc_core_ntf opcode:0x%x", op_code);
  pp++;
  len -= NCI_MSG_HDR_SIZE;
  /* process the message based on the opcode and message type */
  switch (op_code) {
    case NCI_MSG_CORE_RESET:
      nfc_ncif_proc_reset_rsp(pp, true);
      break;

    case NCI_MSG_CORE_GEN_ERR_STATUS:
      /* process the error ntf */
      /* in case of timeout: notify the static connection callback */
      nfc_ncif_event_status(NFC_GEN_ERROR_REVT, *pp);
      nfc_ncif_error_status(NFC_RF_CONN_ID, *pp);
      break;

    case NCI_MSG_CORE_INTF_ERR_STATUS:
      conn_id = *(pp + 1);
      nfc_ncif_error_status(conn_id, *pp);
      break;

    case NCI_MSG_CORE_CONN_CREDITS:
      nfc_ncif_proc_credits(pp, len);
      break;

    default:
      LOG(ERROR) << StringPrintf("unknown opcode:0x%x", op_code);
      break;
  }
}

/*******************************************************************************
**
** Function         nci_proc_rf_management_rsp
**
** Description      Process NCI responses in the RF Management group
**
** Returns          void
**
*******************************************************************************/
void nci_proc_rf_management_rsp(NFC_HDR* p_msg) {
  uint8_t* p;
  uint8_t *pp, len, op_code;
  uint8_t* p_old = nfc_cb.last_cmd;

  /* find the start of the NCI message and parse the NCI header */
  p = (uint8_t*)(p_msg + 1) + p_msg->offset;
  pp = p + 1;
  NCI_MSG_PRS_HDR1(pp, op_code);
  len = *pp++;

  switch (op_code) {
    case NCI_MSG_RF_DISCOVER:
      nfa_dm_p2p_prio_logic(op_code, pp, NFA_DM_P2P_PRIO_RSP);
      nfc_ncif_rf_management_status(NFC_START_DEVT, *pp);
      break;

    case NCI_MSG_RF_DISCOVER_SELECT:
      nfc_ncif_rf_management_status(NFC_SELECT_DEVT, *pp);
      break;

    case NCI_MSG_RF_T3T_POLLING:
      break;

    case NCI_MSG_RF_DISCOVER_MAP:
      nfc_ncif_rf_management_status(NFC_MAP_DEVT, *pp);
      break;

    case NCI_MSG_RF_DEACTIVATE:
      if (nfa_dm_p2p_prio_logic(op_code, pp, NFA_DM_P2P_PRIO_RSP) == false) {
        return;
      }
      nfc_ncif_proc_deactivate(*pp, *p_old, false);
      break;

#if (NFC_NFCEE_INCLUDED == TRUE)
#if (NFC_RW_ONLY == FALSE)

    case NCI_MSG_RF_SET_ROUTING:
      nfc_ncif_event_status(NFC_SET_ROUTING_REVT, *pp);
      break;

    case NCI_MSG_RF_GET_ROUTING:
      if (*pp != NFC_STATUS_OK)
        nfc_ncif_event_status(NFC_GET_ROUTING_REVT, *pp);
      break;
#endif
#endif

    case NCI_MSG_RF_PARAMETER_UPDATE:
      nfc_ncif_event_status(NFC_RF_COMM_PARAMS_UPDATE_REVT, *pp);
      break;

    case NCI_MSG_RF_ISO_DEP_NAK_PRESENCE:
      nfc_ncif_proc_isodep_nak_presence_check_status(*pp, false);
      break;
    default:
      LOG(ERROR) << StringPrintf("unknown opcode:0x%x", op_code);
      break;
  }
}

/*******************************************************************************
**
** Function         nci_proc_rf_management_ntf
**
** Description      Process NCI notifications in the RF Management group
**
** Returns          void
**
*******************************************************************************/
void nci_proc_rf_management_ntf(NFC_HDR* p_msg) {
  uint8_t* p;
  uint8_t *pp, len, op_code;

  /* find the start of the NCI message and parse the NCI header */
  p = (uint8_t*)(p_msg + 1) + p_msg->offset;
  pp = p + 1;
  NCI_MSG_PRS_HDR1(pp, op_code);
  len = *pp++;

  switch (op_code) {
    case NCI_MSG_RF_DISCOVER:
      nfc_ncif_proc_discover_ntf(p, p_msg->len);
      break;

    case NCI_MSG_RF_DEACTIVATE:
      if (nfa_dm_p2p_prio_logic(op_code, pp, NFA_DM_P2P_PRIO_NTF) == false) {
        return;
      }
      if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
        nfc_cb.deact_reason = *(pp + 1);
      }
      nfc_ncif_proc_deactivate(NFC_STATUS_OK, *pp, true);
      break;

    case NCI_MSG_RF_INTF_ACTIVATED:
      if (nfa_dm_p2p_prio_logic(op_code, pp, NFA_DM_P2P_PRIO_NTF) == false) {
        return;
      }
      nfc_ncif_proc_activate(pp, len);
      break;

    case NCI_MSG_RF_FIELD:
      nfc_ncif_proc_rf_field_ntf(*pp);
      break;

    case NCI_MSG_RF_T3T_POLLING:
      nfc_ncif_proc_t3t_polling_ntf(pp, len);
      break;

#if (NFC_NFCEE_INCLUDED == TRUE)
#if (NFC_RW_ONLY == FALSE)

    case NCI_MSG_RF_GET_ROUTING:
      nfc_ncif_proc_get_routing(pp, len);
      break;

    case NCI_MSG_RF_EE_ACTION:
      nfc_ncif_proc_ee_action(pp, len);
      break;

    case NCI_MSG_RF_EE_DISCOVERY_REQ:
      nfc_ncif_proc_ee_discover_req(pp, len);
      break;
#endif
#endif
    case NCI_MSG_RF_ISO_DEP_NAK_PRESENCE:
      nfc_ncif_proc_isodep_nak_presence_check_status(*pp, true);
      break;
    default:
      LOG(ERROR) << StringPrintf("unknown opcode:0x%x", op_code);
      break;
  }
}

#if (NFC_NFCEE_INCLUDED == TRUE)
#if (NFC_RW_ONLY == FALSE)

/*******************************************************************************
**
** Function         nci_proc_ee_management_rsp
**
** Description      Process NCI responses in the NFCEE Management group
**
** Returns          void
**
*******************************************************************************/
void nci_proc_ee_management_rsp(NFC_HDR* p_msg) {
  uint8_t* p;
  uint8_t *pp, len, op_code;
  tNFC_RESPONSE_CBACK* p_cback = nfc_cb.p_resp_cback;
  tNFC_RESPONSE nfc_response;
  tNFC_RESPONSE_EVT event = NFC_NFCEE_INFO_REVT;
  uint8_t* p_old = nfc_cb.last_cmd;

  /* find the start of the NCI message and parse the NCI header */
  p = (uint8_t*)(p_msg + 1) + p_msg->offset;
  pp = p + 1;
  NCI_MSG_PRS_HDR1(pp, op_code);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nci_proc_ee_management_rsp opcode:0x%x", op_code);
  len = p_msg->len - NCI_MSG_HDR_SIZE;
  /* Use pmsg->len in boundary checks, skip *pp */
  pp++;

  switch (op_code) {
    case NCI_MSG_NFCEE_DISCOVER:
      if (len > 1) {
        nfc_response.nfcee_discover.status = *pp++;
        nfc_response.nfcee_discover.num_nfcee = *pp++;
      } else {
        nfc_response.nfcee_discover.status = NFC_STATUS_FAILED;
      }
      if (nfc_response.nfcee_discover.status != NFC_STATUS_OK)
        nfc_response.nfcee_discover.num_nfcee = 0;

      event = NFC_NFCEE_DISCOVER_REVT;
      break;

    case NCI_MSG_NFCEE_MODE_SET:
      if (len > 0) {
        nfc_response.mode_set.status = *pp;
      } else {
        nfc_response.mode_set.status = NFC_STATUS_FAILED;
      }
      nfc_response.mode_set.nfcee_id = *p_old++;
      nfc_response.mode_set.mode = *p_old++;
      if (nfc_cb.nci_version != NCI_VERSION_2_0 || *pp != NCI_STATUS_OK) {
        nfc_cb.flags &= ~NFC_FL_WAIT_MODE_SET_NTF;
        event = NFC_NFCEE_MODE_SET_REVT;
      } else {
        /* else response reports OK status on notification */
        return;
      }
      break;

    case NCI_MSG_NFCEE_POWER_LINK_CTRL:
      if (len > 0) {
        nfc_response.pl_control.status = *pp;
      } else {
        nfc_response.pl_control.status = NFC_STATUS_FAILED;
      }
      nfc_response.pl_control.nfcee_id = *p_old++;
      nfc_response.pl_control.pl_control = *p_old++;
      event = NFC_NFCEE_PL_CONTROL_REVT;
      break;
    default:
      p_cback = nullptr;
      LOG(ERROR) << StringPrintf("unknown opcode:0x%x", op_code);
      break;
  }

  if (p_cback) (*p_cback)(event, &nfc_response);
}

/*******************************************************************************
**
** Function         nci_proc_ee_management_ntf
**
** Description      Process NCI notifications in the NFCEE Management group
**
** Returns          void
**
*******************************************************************************/
void nci_proc_ee_management_ntf(NFC_HDR* p_msg) {
  uint8_t* p;
  uint8_t *pp, len, op_code;
  tNFC_RESPONSE_CBACK* p_cback = nfc_cb.p_resp_cback;
  tNFC_RESPONSE nfc_response;
  tNFC_RESPONSE_EVT event = NFC_NFCEE_INFO_REVT;
  uint8_t* p_old = nfc_cb.last_cmd;
  uint8_t xx;
  uint8_t yy;
  tNFC_NFCEE_TLV* p_tlv;
  /* find the start of the NCI message and parse the NCI header */
  p = (uint8_t*)(p_msg + 1) + p_msg->offset;
  pp = p + 1;
  NCI_MSG_PRS_HDR1(pp, op_code);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nci_proc_ee_management_ntf opcode:0x%x", op_code);
  len = *pp++;

  if (op_code == NCI_MSG_NFCEE_DISCOVER) {
    nfc_response.nfcee_info.nfcee_id = *pp++;

    nfc_response.nfcee_info.ee_status = *pp++;
    yy = *pp;
    nfc_response.nfcee_info.num_interface = *pp++;
    p = pp;

    if (nfc_response.nfcee_info.num_interface > NFC_MAX_EE_INTERFACE)
      nfc_response.nfcee_info.num_interface = NFC_MAX_EE_INTERFACE;

    for (xx = 0; xx < nfc_response.nfcee_info.num_interface; xx++) {
      nfc_response.nfcee_info.ee_interface[xx] = *pp++;
    }

    pp = p + yy;
    nfc_response.nfcee_info.num_tlvs = *pp++;
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "nfcee_id: 0x%x num_interface:0x%x/0x%x, num_tlvs:0x%x",
        nfc_response.nfcee_info.nfcee_id, nfc_response.nfcee_info.num_interface,
        yy, nfc_response.nfcee_info.num_tlvs);

    if (nfc_response.nfcee_info.num_tlvs > NFC_MAX_EE_TLVS)
      nfc_response.nfcee_info.num_tlvs = NFC_MAX_EE_TLVS;

    p_tlv = &nfc_response.nfcee_info.ee_tlv[0];

    for (xx = 0; xx < nfc_response.nfcee_info.num_tlvs; xx++, p_tlv++) {
      p_tlv->tag = *pp++;
      p_tlv->len = yy = *pp++;
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("tag:0x%x, len:0x%x", p_tlv->tag, p_tlv->len);
      if (p_tlv->len > NFC_MAX_EE_INFO) p_tlv->len = NFC_MAX_EE_INFO;
      p = pp;
      STREAM_TO_ARRAY(p_tlv->info, pp, p_tlv->len);
      pp = p += yy;
    }
  } else if (op_code == NCI_MSG_NFCEE_MODE_SET) {
    nfc_response.mode_set.status = *pp;
    nfc_response.mode_set.nfcee_id = *p_old++;
    nfc_response.mode_set.mode = *p_old++;
    event = NFC_NFCEE_MODE_SET_REVT;
    nfc_cb.flags &= ~NFC_FL_WAIT_MODE_SET_NTF;
    nfc_stop_timer(&nfc_cb.nci_mode_set_ntf_timer);
  } else if (op_code == NCI_MSG_NFCEE_STATUS) {
    event = NFC_NFCEE_STATUS_REVT;
    nfc_response.nfcee_status.status = NCI_STATUS_OK;
    nfc_response.nfcee_status.nfcee_id = *pp++;
    nfc_response.nfcee_status.nfcee_status = *pp;
  } else {
    p_cback = nullptr;
    LOG(ERROR) << StringPrintf("unknown opcode:0x%x", op_code);
  }

  if (p_cback) (*p_cback)(event, &nfc_response);
}

#endif
#endif

/*******************************************************************************
**
** Function         nci_proc_prop_rsp
**
** Description      Process NCI responses in the Proprietary group
**
** Returns          void
**
*******************************************************************************/
void nci_proc_prop_rsp(NFC_HDR* p_msg) {
  uint8_t* p;
  uint8_t* p_evt;
  uint8_t *pp, len, op_code;
  tNFC_VS_CBACK* p_cback = (tNFC_VS_CBACK*)nfc_cb.p_vsc_cback;

  /* find the start of the NCI message and parse the NCI header */
  p = p_evt = (uint8_t*)(p_msg + 1) + p_msg->offset;
  pp = p + 1;
  NCI_MSG_PRS_HDR1(pp, op_code);
  len = *pp++;

  /*If there's a pending/stored command, restore the associated address of the
   * callback function */
  if (p_cback)
    (*p_cback)((tNFC_VS_EVT)(NCI_RSP_BIT | op_code), p_msg->len, p_evt);
}

/*******************************************************************************
**
** Function         nci_proc_prop_raw_vs_rsp
**
** Description      Process RAW VS responses
**
** Returns          void
**
*******************************************************************************/
void nci_proc_prop_raw_vs_rsp(NFC_HDR* p_msg) {
  uint8_t op_code;
  tNFC_VS_CBACK* p_cback = (tNFC_VS_CBACK*)nfc_cb.p_vsc_cback;

  /* find the start of the NCI message and parse the NCI header */
  uint8_t* p_evt = (uint8_t*)(p_msg + 1) + p_msg->offset;
  uint8_t* p = p_evt + 1;
  NCI_MSG_PRS_HDR1(p, op_code);

  /* If there's a pending/stored command, restore the associated address of the
   * callback function */
  if (p_cback) {
    (*p_cback)((tNFC_VS_EVT)(NCI_RSP_BIT | op_code), p_msg->len, p_evt);
    nfc_cb.p_vsc_cback = nullptr;
  }
  nfc_cb.rawVsCbflag = false;
  nfc_ncif_update_window();
}

/*******************************************************************************
**
** Function         nci_proc_prop_ntf
**
** Description      Process NCI notifications in the Proprietary group
**
** Returns          void
**
*******************************************************************************/
void nci_proc_prop_ntf(NFC_HDR* p_msg) {
  uint8_t* p;
  uint8_t* p_evt;
  uint8_t *pp, len, op_code;
  int i;

  /* find the start of the NCI message and parse the NCI header */
  p = p_evt = (uint8_t*)(p_msg + 1) + p_msg->offset;
  pp = p + 1;
  NCI_MSG_PRS_HDR1(pp, op_code);
  len = *pp++;

  for (i = 0; i < NFC_NUM_VS_CBACKS; i++) {
    if (nfc_cb.p_vs_cb[i]) {
      (*nfc_cb.p_vs_cb[i])((tNFC_VS_EVT)(NCI_NTF_BIT | op_code), p_msg->len,
                           p_evt);
    }
  }
}
