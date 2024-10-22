/******************************************************************************
 *
 *  Copyright (C) 2011-2014 Broadcom Corporation
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
 *  The original Work has been changed by NXP Semiconductors.
 *
 *  Copyright (C) 2020 NXP Semiconductors
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

/******************************************************************************
 *
 *  This file contains the action functions the NFA_CE state machine.
 *
 ******************************************************************************/
#include <log/log.h>
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "ce_api.h"
#include "ndef_utils.h"
#include "nfa_ce_int.h"
#include "nfa_mem_co.h"

#if (NFC_NFCEE_INCLUDED == TRUE)
#include "nfa_ee_int.h"
#endif

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/*****************************************************************************
* Protocol-specific event handlers
*****************************************************************************/

/*******************************************************************************
**
** Function         nfa_ce_handle_t3t_evt
**
** Description      Handler for Type-3 tag card emulation events
**
** Returns          Nothing
**
*******************************************************************************/
void nfa_ce_handle_t3t_evt(tCE_EVENT event, tCE_DATA* p_ce_data) {
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  tNFA_CONN_EVT_DATA conn_evt;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_ce_handle_t3t_evt: event 0x%x", event);
  /* For the felica on host for nfcFcallback */
  for (uint8_t idx = 0; idx < NFA_CE_LISTEN_INFO_IDX_INVALID; idx++) {
    if ((p_cb->listen_info[idx].flags & NFA_CE_LISTEN_INFO_IN_USE) &&
        (p_cb->listen_info[idx].flags & NFA_CE_LISTEN_INFO_FELICA) &&
        (p_cb->listen_info[idx].flags & NFA_CE_LISTEN_INFO_T3T_ACTIVATE_PND)) {
      p_cb->idx_cur_active = idx;
      p_cb->p_active_conn_cback =
          p_cb->listen_info[p_cb->idx_cur_active].p_conn_cback;
      break;
    }
  }
  switch (event) {
    case CE_T3T_NDEF_UPDATE_START_EVT:
      /* Notify app using callback associated with the active ndef */
      if (p_cb->idx_cur_active == NFA_CE_LISTEN_INFO_IDX_NDEF) {
        conn_evt.status = NFA_STATUS_OK;
        (*p_cb->p_active_conn_cback)(NFA_CE_NDEF_WRITE_START_EVT, &conn_evt);
      } else {
        LOG(ERROR) << StringPrintf(
            "nfa_ce_handle_t3t_evt: got CE_T3T_UPDATE_START_EVT, but no active "
            "NDEF");
      }
      break;

    case CE_T3T_NDEF_UPDATE_CPLT_EVT:
      /* Notify app using callback associated with the active ndef */
      if (p_cb->idx_cur_active == NFA_CE_LISTEN_INFO_IDX_NDEF) {
        conn_evt.ndef_write_cplt.status = NFA_STATUS_OK;
        conn_evt.ndef_write_cplt.len = p_ce_data->update_info.length;
        conn_evt.ndef_write_cplt.p_data = p_ce_data->update_info.p_data;
        (*p_cb->p_active_conn_cback)(NFA_CE_NDEF_WRITE_CPLT_EVT, &conn_evt);
      } else {
        LOG(ERROR) << StringPrintf(
            "nfa_ce_handle_t3t_evt: got CE_T3T_UPDATE_CPLT_EVT, but no active "
            "NDEF");
      }
      break;

    case CE_T3T_RAW_FRAME_EVT:
      if (p_cb->idx_cur_active == NFA_CE_LISTEN_INFO_IDX_NDEF) {
        conn_evt.data.status = p_ce_data->raw_frame.status;
        conn_evt.data.p_data = (uint8_t*)(p_ce_data->raw_frame.p_data + 1) +
                               p_ce_data->raw_frame.p_data->offset;
        conn_evt.data.len = p_ce_data->raw_frame.p_data->len;
        (*p_cb->p_active_conn_cback)(NFA_DATA_EVT, &conn_evt);
      } else {
        /* If we have not notified the app of activation, do so now */
        if (p_cb->listen_info[p_cb->idx_cur_active].flags &
            NFA_CE_LISTEN_INFO_T3T_ACTIVATE_PND) {
          p_cb->listen_info[p_cb->idx_cur_active].flags &=
              ~NFA_CE_LISTEN_INFO_T3T_ACTIVATE_PND;

          conn_evt.ce_activated.handle =
              NFA_HANDLE_GROUP_CE | ((tNFA_HANDLE)p_cb->idx_cur_active);
          memcpy(&(conn_evt.ce_activated.activate_ntf),
                 &p_cb->activation_params, sizeof(tNFC_ACTIVATE_DEVT));
          conn_evt.ce_activated.status = NFA_STATUS_OK;

          (*p_cb->p_active_conn_cback)(NFA_CE_ACTIVATED_EVT, &conn_evt);
        }
        /* Notify app of t3t raw data */
        conn_evt.ce_data.status = p_ce_data->raw_frame.status;
        conn_evt.ce_data.handle =
            (NFA_HANDLE_GROUP_CE | ((tNFA_HANDLE)p_cb->idx_cur_active));
        conn_evt.ce_data.p_data = (uint8_t*)(p_ce_data->raw_frame.p_data + 1) +
                                  p_ce_data->raw_frame.p_data->offset;
        conn_evt.ce_data.len = p_ce_data->raw_frame.p_data->len;
        (*p_cb->p_active_conn_cback)(NFA_CE_DATA_EVT, &conn_evt);
      }
      GKI_freebuf(p_ce_data->raw_frame.p_data);
      break;

    default:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "nfa_ce_handle_t3t_evt unhandled event=0x%02x", event);
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_ce_handle_t4t_evt
**
** Description      Handler for Type-4 tag card emulation events (for NDEF case)
**
** Returns          Nothing
**
*******************************************************************************/
void nfa_ce_handle_t4t_evt(tCE_EVENT event, tCE_DATA* p_ce_data) {
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  tNFA_CONN_EVT_DATA conn_evt;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_ce_handle_t4t_evt: event 0x%x", event);

  /* AID for NDEF selected. we had notified the app of activation. */
  p_cb->idx_cur_active = NFA_CE_LISTEN_INFO_IDX_NDEF;
  if (p_cb->listen_info[p_cb->idx_cur_active].flags &
      NFA_CE_LISTEN_INFO_T4T_ACTIVATE_PND) {
    p_cb->p_active_conn_cback =
        p_cb->listen_info[p_cb->idx_cur_active].p_conn_cback;
  }

  switch (event) {
    case CE_T4T_NDEF_UPDATE_START_EVT:
      conn_evt.status = NFA_STATUS_OK;
      (*p_cb->p_active_conn_cback)(NFA_CE_NDEF_WRITE_START_EVT, &conn_evt);
      break;

    case CE_T4T_NDEF_UPDATE_CPLT_EVT:
      conn_evt.ndef_write_cplt.len = p_ce_data->update_info.length;
      conn_evt.ndef_write_cplt.p_data = p_ce_data->update_info.p_data;

      if (NDEF_MsgValidate(p_ce_data->update_info.p_data,
                           p_ce_data->update_info.length, true) != NDEF_OK)
        conn_evt.ndef_write_cplt.status = NFA_STATUS_FAILED;
      else
        conn_evt.ndef_write_cplt.status = NFA_STATUS_OK;

      (*p_cb->p_active_conn_cback)(NFA_CE_NDEF_WRITE_CPLT_EVT, &conn_evt);
      break;

    case CE_T4T_NDEF_UPDATE_ABORT_EVT:
      conn_evt.ndef_write_cplt.len = 0;
      conn_evt.ndef_write_cplt.status = NFA_STATUS_FAILED;
      conn_evt.ndef_write_cplt.p_data = nullptr;
      (*p_cb->p_active_conn_cback)(NFA_CE_NDEF_WRITE_CPLT_EVT, &conn_evt);
      break;

    default:
      /* CE_T4T_RAW_FRAME_EVT is not used in NFA CE */
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "nfa_ce_handle_t4t_evt unhandled event=0x%02x", event);
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_ce_handle_t4t_aid_evt
**
** Description      Handler for Type-4 tag AID events (for AIDs registered using
**                  NFA_CeRegisterT4tAidOnDH)
**
** Returns          Nothing
**
*******************************************************************************/
void nfa_ce_handle_t4t_aid_evt(tCE_EVENT event, tCE_DATA* p_ce_data) {
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  uint8_t listen_info_idx;
  tNFA_CONN_EVT_DATA conn_evt;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_ce_handle_t4t_aid_evt: event 0x%x", event);

  /* Get listen_info for this aid callback */
  for (listen_info_idx = 0; listen_info_idx < NFA_CE_LISTEN_INFO_IDX_INVALID;
       listen_info_idx++) {
    if ((p_cb->listen_info[listen_info_idx].flags &
         NFA_CE_LISTEN_INFO_IN_USE) &&
        (p_cb->listen_info[listen_info_idx].flags &
         NFA_CE_LISTEN_INFO_T4T_AID) &&
        (p_cb->listen_info[listen_info_idx].t4t_aid_handle ==
         p_ce_data->raw_frame.aid_handle)) {
      p_cb->idx_cur_active = listen_info_idx;
      p_cb->p_active_conn_cback =
          p_cb->listen_info[p_cb->idx_cur_active].p_conn_cback;
      break;
    }
  }

  if (event == CE_T4T_RAW_FRAME_EVT) {
    if (listen_info_idx != NFA_CE_LISTEN_INFO_IDX_INVALID) {
      /* Found listen_info entry */
      conn_evt.ce_activated.handle =
          NFA_HANDLE_GROUP_CE | ((tNFA_HANDLE)p_cb->idx_cur_active);

      /* If we have not notified the app of activation, do so now */
      if (p_cb->listen_info[p_cb->idx_cur_active].flags &
          NFA_CE_LISTEN_INFO_T4T_ACTIVATE_PND) {
        p_cb->listen_info[p_cb->idx_cur_active].flags &=
            ~NFA_CE_LISTEN_INFO_T4T_ACTIVATE_PND;

        memcpy(&(conn_evt.ce_activated.activate_ntf), &p_cb->activation_params,
               sizeof(tNFC_ACTIVATE_DEVT));
        conn_evt.ce_activated.status = NFA_STATUS_OK;
        (*p_cb->p_active_conn_cback)(NFA_CE_ACTIVATED_EVT, &conn_evt);
      }

      /* Notify app of AID data */
      conn_evt.ce_data.status = p_ce_data->raw_frame.status;
      conn_evt.ce_data.handle =
          NFA_HANDLE_GROUP_CE | ((tNFA_HANDLE)p_cb->idx_cur_active);
      conn_evt.ce_data.p_data = (uint8_t*)(p_ce_data->raw_frame.p_data + 1) +
                                p_ce_data->raw_frame.p_data->offset;
      conn_evt.ce_data.len = p_ce_data->raw_frame.p_data->len;
      (*p_cb->p_active_conn_cback)(NFA_CE_DATA_EVT, &conn_evt);
    } else {
      LOG(ERROR) << StringPrintf(
          "nfa_ce_handle_t4t_aid_evt: unable to find listen_info for aid hdl "
          "%i",
          p_ce_data->raw_frame.aid_handle);
    }

    GKI_freebuf(p_ce_data->raw_frame.p_data);
  }
}

/*****************************************************************************
* Discovery configuration and discovery event handlers
*****************************************************************************/

/*******************************************************************************
**
** Function         nfa_ce_discovery_cback
**
** Description      Processing event from discovery callback
**
** Returns          None
**
*******************************************************************************/
void nfa_ce_discovery_cback(tNFA_DM_RF_DISC_EVT event, tNFC_DISCOVER* p_data) {
  tNFA_CE_MSG ce_msg;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("event:0x%02X", event);

  switch (event) {
    case NFA_DM_RF_DISC_START_EVT:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "nfa_ce_handle_disc_start (status=0x%x)", p_data->start);
      break;

    case NFA_DM_RF_DISC_ACTIVATED_EVT:
      ce_msg.activate_ntf.hdr.event = NFA_CE_ACTIVATE_NTF_EVT;
      ce_msg.activate_ntf.p_activation_params = &p_data->activate;
      nfa_ce_hdl_event((NFC_HDR*)&ce_msg);
      break;

    case NFA_DM_RF_DISC_DEACTIVATED_EVT:
      /* DM broadcasts deactivaiton event in listen sleep state, so check before
       * processing */
      if (nfa_ce_cb.flags & NFA_CE_FLAGS_LISTEN_ACTIVE_SLEEP) {
        ce_msg.hdr.event = NFA_CE_DEACTIVATE_NTF_EVT;
        ce_msg.hdr.layer_specific = p_data->deactivate.type;
        nfa_ce_hdl_event((NFC_HDR*)&ce_msg);
      }
      break;

    default:
      LOG(ERROR) << StringPrintf("Unexpected event");
      break;
  }
}

/*******************************************************************************
**
** Function         nfc_ce_t3t_set_listen_params
**
** Description      Set t3t listening parameters
**
** Returns          Nothing
**
*******************************************************************************/
void nfc_ce_t3t_set_listen_params(void) {
  uint8_t i;
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  uint8_t tlv[128], *p_params;
  uint8_t tlv_size;
  uint16_t t3t_flags2_mask = 0xFFFF; /* Mask of which T3T_IDs are disabled */
  uint8_t t3t_idx = 0;
  uint8_t adv_Feat = 1;
  uint8_t t3tPMM[NCI_T3T_PMM_LEN] = {0xFF, 0xFF, 0xFF, 0xFF,
                                     0xFF, 0xFF, 0xFF, 0xFF};

  /* Point to start of tlv buffer */
  p_params = tlv;

  /* Set system code and NFCID2 */
  for (i = 0; i < NFA_CE_LISTEN_INFO_MAX; i++) {
    if ((p_cb->listen_info[i].flags & NFA_CE_LISTEN_INFO_IN_USE) &&
        (p_cb->listen_info[i].protocol_mask & NFA_PROTOCOL_MASK_T3T)) {
      /* Set tag's system code and NFCID2 */
      UINT8_TO_STREAM(p_params, NFC_PMID_LF_T3T_ID1 + t3t_idx); /* type */
      /* length */
      UINT8_TO_STREAM(p_params, NCI_PARAM_LEN_LF_T3T_ID(NFC_GetNCIVersion()));
      /* System Code */
      UINT16_TO_BE_STREAM(p_params, p_cb->listen_info[i].t3t_system_code);
      ARRAY_TO_BE_STREAM(p_params, p_cb->listen_info[i].t3t_nfcid2,
                         NCI_RF_F_UID_LEN);
      if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
        ARRAY_TO_BE_STREAM(p_params, p_cb->listen_info[i].t3t_pmm,
                           NCI_T3T_PMM_LEN);
      }
      /* Set mask for this ID */
      t3t_flags2_mask &= ~((uint16_t)(1 << t3t_idx));
      t3t_idx++;
    }
  }

  /* For NCI draft 22+, the polarity of NFC_PMID_LF_T3T_FLAGS2 is flipped */
  t3t_flags2_mask = ~t3t_flags2_mask;
#if (NXP_EXTNS == TRUE)
  if (NFC_GetNCIVersion() == NCI_VERSION_1_0) {
    t3t_flags2_mask = ((t3t_flags2_mask & 0x00FF) << 8 | (t3t_flags2_mask & 0xFF00) >> 8);
  }
#endif

  UINT8_TO_STREAM(p_params, NFC_PMID_LF_T3T_FLAGS2);      /* type */
  UINT8_TO_STREAM(p_params, NCI_PARAM_LEN_LF_T3T_FLAGS2); /* length */
  /* Mask of IDs to disable listening */
  UINT16_TO_STREAM(p_params, t3t_flags2_mask);

  if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
    /*Name changed in NCI2.0*/
    UINT8_TO_STREAM(p_params, NCI_PARAM_ID_LF_T3T_RD_ALLOWED);  /* type */
    UINT8_TO_STREAM(p_params, NCI_PARAM_LEN_LF_T3T_RD_ALLOWED); /* length */
  } else {
    UINT8_TO_STREAM(p_params, NCI_PARAM_ID_LF_CON_ADV_FEAT);  /* type */
    UINT8_TO_STREAM(p_params, NCI_PARAM_LEN_LF_CON_ADV_FEAT); /* length */
  }
  UINT8_TO_STREAM(p_params, adv_Feat);

  if (NFC_GetNCIVersion() != NCI_VERSION_2_0) {
    UINT8_TO_STREAM(p_params, NCI_PARAM_ID_LF_T3T_PMM);  /* type */
    UINT8_TO_STREAM(p_params, NCI_PARAM_LEN_LF_T3T_PMM); /* length */
    ARRAY_TO_BE_STREAM(p_params, t3tPMM, NCI_T3T_PMM_LEN);
  }
  tlv_size = (uint8_t)(p_params - tlv);
  if (appl_dta_mode_flag == 0x01) {
    nfa_dm_cb.eDtaMode |= NFA_DTA_HCEF_MODE;
  }
  nfa_dm_check_set_config(tlv_size, (uint8_t*)tlv, false);
}

/*******************************************************************************
**
** Function         nfa_ce_t3t_generate_rand_nfcid
**
** Description      Generate a random NFCID2 for Type-3 tag
**
** Returns          Nothing
**
*******************************************************************************/
void nfa_ce_t3t_generate_rand_nfcid(uint8_t nfcid2[NCI_RF_F_UID_LEN]) {
  uint32_t rand_seed = GKI_get_tick_count();

  /* For Type-3 tag, nfcid2 starts witn 02:fe */
  nfcid2[0] = 0x02;
  nfcid2[1] = 0xFE;

  /* The remaining 6 bytes are random */
  nfcid2[2] = (uint8_t)(rand_seed & 0xFF);
  nfcid2[3] = (uint8_t)(rand_seed >> 8 & 0xFF);
  rand_seed >>= (rand_seed & 3);
  nfcid2[4] = (uint8_t)(rand_seed & 0xFF);
  nfcid2[5] = (uint8_t)(rand_seed >> 8 & 0xFF);
  rand_seed >>= (rand_seed & 3);
  nfcid2[6] = (uint8_t)(rand_seed & 0xFF);
  nfcid2[7] = (uint8_t)(rand_seed >> 8 & 0xFF);
}

/*******************************************************************************
**
** Function         nfa_ce_start_listening
**
** Description      Start listening
**
** Returns          NFA_STATUS_OK if successful
**
*******************************************************************************/
tNFA_STATUS nfa_ce_start_listening(void) {
  tNFA_DM_DISC_TECH_PROTO_MASK listen_mask;
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  tNFA_HANDLE disc_handle;
  uint8_t listen_info_idx;

  /*************************************************************************/
  /* Construct protocol preference list to listen for */

  /* First, get protocol preference for active NDEF (if any) */
  if ((p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].flags &
       NFA_CE_LISTEN_INFO_IN_USE) &&
      (p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].rf_disc_handle ==
       NFA_HANDLE_INVALID)) {
    listen_mask = 0;

    if (p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].protocol_mask &
        NFA_PROTOCOL_MASK_T3T) {
      /* set T3T config params */
      nfc_ce_t3t_set_listen_params();

      listen_mask |= NFA_DM_DISC_MASK_LF_T3T;
    }

    if (p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].protocol_mask &
        NFA_PROTOCOL_MASK_ISO_DEP) {
      listen_mask |= nfa_ce_cb.isodep_disc_mask;
    }

    disc_handle = nfa_dm_add_rf_discover(listen_mask, NFA_DM_DISC_HOST_ID_DH,
                                         nfa_ce_discovery_cback);

    if (disc_handle == NFA_HANDLE_INVALID)
      return (NFA_STATUS_FAILED);
    else
      p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].rf_disc_handle =
          disc_handle;
  }

  /* Next, add protocols from non-NDEF, if any */
  for (listen_info_idx = 0; listen_info_idx < NFA_CE_LISTEN_INFO_IDX_INVALID;
       listen_info_idx++) {
    /* add RF discovery to DM only if it is not added yet */
    if ((p_cb->listen_info[listen_info_idx].flags &
         NFA_CE_LISTEN_INFO_IN_USE) &&
        (p_cb->listen_info[listen_info_idx].rf_disc_handle ==
         NFA_HANDLE_INVALID)) {
      if (p_cb->listen_info[listen_info_idx].flags &
          NFA_CE_LISTEN_INFO_FELICA) {
        /* set T3T config params */
        nfc_ce_t3t_set_listen_params();

        disc_handle = nfa_dm_add_rf_discover(NFA_DM_DISC_MASK_LF_T3T,
                                             NFA_DM_DISC_HOST_ID_DH,
                                             nfa_ce_discovery_cback);

        if (disc_handle == NFA_HANDLE_INVALID)
          return (NFA_STATUS_FAILED);
        else
          p_cb->listen_info[listen_info_idx].rf_disc_handle = disc_handle;
      } else if (p_cb->listen_info[listen_info_idx].flags &
                 NFA_CE_LISTEN_INFO_T4T_AID) {
        disc_handle = nfa_dm_add_rf_discover(nfa_ce_cb.isodep_disc_mask,
                                             NFA_DM_DISC_HOST_ID_DH,
                                             nfa_ce_discovery_cback);

        if (disc_handle == NFA_HANDLE_INVALID)
          return (NFA_STATUS_FAILED);
        else
          p_cb->listen_info[listen_info_idx].rf_disc_handle = disc_handle;
      }
#if (NFC_NFCEE_INCLUDED == TRUE)
      else if (p_cb->listen_info[listen_info_idx].flags &
               NFA_CE_LISTEN_INFO_UICC) {
        listen_mask = 0;
        if (nfa_ee_is_active(p_cb->listen_info[listen_info_idx].ee_handle)) {
          if (p_cb->listen_info[listen_info_idx].tech_mask &
              NFA_TECHNOLOGY_MASK_A) {
            listen_mask |= NFA_DM_DISC_MASK_LA_ISO_DEP;
          }
          if (p_cb->listen_info[listen_info_idx].tech_mask &
              NFA_TECHNOLOGY_MASK_B) {
            listen_mask |= NFA_DM_DISC_MASK_LB_ISO_DEP;
          }
          if (p_cb->listen_info[listen_info_idx].tech_mask &
              NFA_TECHNOLOGY_MASK_F) {
            listen_mask |= NFA_DM_DISC_MASK_LF_T3T;
          }
          if (p_cb->listen_info[listen_info_idx].tech_mask &
              NFA_TECHNOLOGY_MASK_B_PRIME) {
            listen_mask |= NFA_DM_DISC_MASK_L_B_PRIME;
          }
        }

        if (listen_mask) {
          /* Start listening for requested technologies */
          /* register discovery callback to NFA DM */
          disc_handle = nfa_dm_add_rf_discover(
              listen_mask,
              (tNFA_DM_DISC_HOST_ID)(
                  p_cb->listen_info[listen_info_idx].ee_handle & 0x00FF),
              nfa_ce_discovery_cback);

          if (disc_handle == NFA_HANDLE_INVALID)
            return (NFA_STATUS_FAILED);
          else {
            p_cb->listen_info[listen_info_idx].rf_disc_handle = disc_handle;
            p_cb->listen_info[listen_info_idx].tech_proto_mask = listen_mask;
          }
        } else {
          LOG(ERROR) << StringPrintf(
              "UICC[0x%x] is not activated",
              p_cb->listen_info[listen_info_idx].ee_handle);
        }
      }
#endif
    }
  }

  return NFA_STATUS_OK;
}

/*******************************************************************************
**
** Function         nfa_ce_restart_listen_check
**
** Description      Called on deactivation. Check if any active listen_info
**                  entries to listen for
**
** Returns          TRUE if listening is restarted.
**                  FALSE if listening not restarted
**
*******************************************************************************/
bool nfa_ce_restart_listen_check(void) {
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  uint8_t listen_info_idx;

  /* Check if any active entries in listen_info table */
  for (listen_info_idx = 0; listen_info_idx < NFA_CE_LISTEN_INFO_MAX;
       listen_info_idx++) {
    if (p_cb->listen_info[listen_info_idx].flags & NFA_CE_LISTEN_INFO_IN_USE)
      break;
  }

  /* Restart listening if there are any active listen_info entries */
  if (listen_info_idx != NFA_CE_LISTEN_INFO_IDX_INVALID) {
    /* restart listening */
    nfa_ce_start_listening();
  } else {
    /* No active listen_info entries */
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         nfa_ce_remove_listen_info_entry
**
** Description      Remove entry from listen_info table. (when API deregister is
**                  called or listen_start failed)
**
**
** Returns          Nothing
**
*******************************************************************************/
void nfa_ce_remove_listen_info_entry(uint8_t listen_info_idx, bool notify_app) {
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  tNFA_CONN_EVT_DATA conn_evt;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("NFA_CE: removing listen_info entry %i", listen_info_idx);

  /* Notify app that listening has stopped  if requested (for API deregister) */
  /* For LISTEN_START failures, app has already notified of NFA_LISTEN_START_EVT
   * failure */
  if (notify_app) {
    if (listen_info_idx == NFA_CE_LISTEN_INFO_IDX_NDEF) {
      conn_evt.status = NFA_STATUS_OK;
      (*p_cb->listen_info[listen_info_idx].p_conn_cback)(
          NFA_CE_LOCAL_TAG_CONFIGURED_EVT, &conn_evt);
    }
#if (NFC_NFCEE_INCLUDED == TRUE)
    else if (p_cb->listen_info[listen_info_idx].flags &
             NFA_CE_LISTEN_INFO_UICC) {
      conn_evt.status = NFA_STATUS_OK;
      (*p_cb->listen_info[listen_info_idx].p_conn_cback)(
          NFA_CE_UICC_LISTEN_CONFIGURED_EVT, &conn_evt);
    }
#endif
    else {
      conn_evt.ce_deregistered.handle = NFA_HANDLE_GROUP_CE | listen_info_idx;
      (*p_cb->listen_info[listen_info_idx].p_conn_cback)(
          NFA_CE_DEREGISTERED_EVT, &conn_evt);
    }
  }

  /* Handle NDEF stopping */
  if (listen_info_idx == NFA_CE_LISTEN_INFO_IDX_NDEF) {
    /* clear NDEF contents */
    CE_T3tSetLocalNDEFMsg(true, 0, 0, nullptr, nullptr);
    CE_T4tSetLocalNDEFMsg(true, 0, 0, nullptr, nullptr);

    if (p_cb->listen_info[listen_info_idx].protocol_mask &
        NFA_PROTOCOL_MASK_T3T) {
      p_cb->listen_info[listen_info_idx].protocol_mask = 0;

      /* clear T3T Flags for NDEF */
      nfc_ce_t3t_set_listen_params();
    }

    /* Free scratch buffer for this NDEF, if one was allocated */
    nfa_ce_free_scratch_buf();
  }
  /* If stopping listening Felica system code, then clear T3T Flags for this */
  else if (p_cb->listen_info[listen_info_idx].flags &
           NFA_CE_LISTEN_INFO_FELICA) {
    p_cb->listen_info[listen_info_idx].protocol_mask = 0;

    /* clear T3T Flags for registered Felica system code */
    nfc_ce_t3t_set_listen_params();
  }
  /* If stopping listening T4T AID, then deregister this AID from CE_T4T */
  else if (p_cb->listen_info[listen_info_idx].flags &
           NFA_CE_LISTEN_INFO_T4T_AID) {
    /* Free t4t_aid_cback used by this AID */
    CE_T4tDeregisterAID(p_cb->listen_info[listen_info_idx].t4t_aid_handle);
  }

  if (p_cb->listen_info[listen_info_idx].rf_disc_handle != NFA_HANDLE_INVALID) {
    nfa_dm_delete_rf_discover(
        p_cb->listen_info[listen_info_idx].rf_disc_handle);
    p_cb->listen_info[listen_info_idx].rf_disc_handle = NFA_HANDLE_INVALID;
  }

  /* Remove entry from listen_info table */
  p_cb->listen_info[listen_info_idx].flags = 0;
}

/*******************************************************************************
**
** Function         nfa_ce_free_scratch_buf
**
** Description      free scratch buffer (if one is allocated)
**
** Returns          nothing
**
*******************************************************************************/
void nfa_ce_free_scratch_buf(void) {
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  if (p_cb->p_scratch_buf) {
    nfa_mem_co_free(p_cb->p_scratch_buf);
    p_cb->p_scratch_buf = nullptr;
  }
}

/*******************************************************************************
**
** Function         nfa_ce_realloc_scratch_buffer
**
** Description      Set scratch buffer if necessary (for writable NDEF messages)
**
** Returns          NFA_STATUS_OK if successful
**
*******************************************************************************/
tNFA_STATUS nfa_ce_realloc_scratch_buffer(void) {
  tNFA_STATUS result = NFA_STATUS_OK;

  /* If current NDEF message is read-only, then we do not need a scratch buffer
   */
  if (nfa_ce_cb.listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].flags &
      NFC_CE_LISTEN_INFO_READONLY_NDEF) {
    /* Free existing scratch buffer, if one was allocated */
    nfa_ce_free_scratch_buf();
  } else {
    /* If no scratch buffer allocated yet, or if current scratch buffer size is
     * different from current ndef size, */
    /* then allocate a new scratch buffer. */
    if ((nfa_ce_cb.p_scratch_buf == nullptr) ||
        (nfa_ce_cb.scratch_buf_size != nfa_ce_cb.ndef_max_size)) {
      /* Free existing scratch buffer, if one was allocated */
      nfa_ce_free_scratch_buf();

      nfa_ce_cb.p_scratch_buf =
          (uint8_t*)nfa_mem_co_alloc(nfa_ce_cb.ndef_max_size);
      if (nfa_ce_cb.p_scratch_buf != nullptr) {
        nfa_ce_cb.scratch_buf_size = nfa_ce_cb.ndef_max_size;
      } else {
        LOG(ERROR) << StringPrintf(
            "Unable to allocate scratch buffer for writable NDEF message (%i "
            "bytes)",
            nfa_ce_cb.ndef_max_size);
        result = NFA_STATUS_FAILED;
      }
    }
  }

  return (result);
}

/*******************************************************************************
**
** Function         nfa_ce_set_content
**
** Description      Set NDEF contents
**
** Returns          void
**
*******************************************************************************/
tNFC_STATUS nfa_ce_set_content(void) {
  tNFC_STATUS status;
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  tNFA_PROTOCOL_MASK ndef_protocol_mask;
  bool readonly;

  /* Check if listening for NDEF */
  if (!(p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].flags &
        NFA_CE_LISTEN_INFO_IN_USE)) {
    /* Not listening for NDEF */
    return (NFA_STATUS_OK);
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("Setting NDEF contents");

  readonly = (p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].flags &
              NFC_CE_LISTEN_INFO_READONLY_NDEF)
                 ? true
                 : false;
  ndef_protocol_mask =
      p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].protocol_mask;

  /* Allocate a scratch buffer if needed (for handling write-requests) */
  status = nfa_ce_realloc_scratch_buffer();
  if (status == NFA_STATUS_OK) {
    if ((ndef_protocol_mask & NFA_PROTOCOL_MASK_T3T) &&
        (status == NFA_STATUS_OK)) {
      /* Type3Tag    - NFC-F */
      status = CE_T3tSetLocalNDEFMsg(readonly, p_cb->ndef_max_size,
                                     p_cb->ndef_cur_size, p_cb->p_ndef_data,
                                     p_cb->p_scratch_buf);
    }

    if ((ndef_protocol_mask & NFA_PROTOCOL_MASK_ISO_DEP) &&
        (status == NFA_STATUS_OK)) {
      /* ISODEP/4A,4B- NFC-A or NFC-B */
      status = CE_T4tSetLocalNDEFMsg(readonly, p_cb->ndef_max_size,
                                     p_cb->ndef_cur_size, p_cb->p_ndef_data,
                                     p_cb->p_scratch_buf);
    }
  }

  if (status != NFA_STATUS_OK) {
    /* clear NDEF contents */
    CE_T3tSetLocalNDEFMsg(true, 0, 0, nullptr, nullptr);
    CE_T4tSetLocalNDEFMsg(true, 0, 0, nullptr, nullptr);

    LOG(ERROR) << StringPrintf("Unable to set contents (error %02x)", status);
  }

  return (status);
}

/*******************************************************************************
**
** Function         nfa_ce_activate_ntf
**
** Description      Action when activation has occured (NFA_CE_ACTIVATE_NTF_EVT)
**
**                  - Find the listen_info entry assocated with this activation
**                      - get the app callback that registered for this listen
**                      - call CE_SetActivatedTagType with activation parameters
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
bool nfa_ce_activate_ntf(tNFA_CE_MSG* p_ce_msg) {
  tNFC_ACTIVATE_DEVT* p_activation_params =
      p_ce_msg->activate_ntf.p_activation_params;
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  tNFA_CONN_EVT_DATA conn_evt;
  tCE_CBACK* p_ce_cback = nullptr;
  uint16_t t3t_system_code = 0xFFFF;
  uint8_t listen_info_idx = NFA_CE_LISTEN_INFO_IDX_INVALID;
  uint8_t* p_nfcid2 = nullptr;
  uint8_t i;
  bool t4t_activate_pending = false;

  bool t3t_activate_pending = false;
  bool t3t_offhost_entry_found = false;
  uint8_t t3t_activate_idx = 0;
  uint8_t t3t_offhost_idx = 0;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "protocol=%d", p_ce_msg->activate_ntf.p_activation_params->protocol);

  /* Tag is in listen active state */
  p_cb->flags |= NFA_CE_FLAGS_LISTEN_ACTIVE_SLEEP;

  /* Store activation parameters */
  memcpy(&p_cb->activation_params, p_activation_params,
         sizeof(tNFC_ACTIVATE_DEVT));

  /* Find the listen_info entry corresponding to this activation */
  if (p_cb->activation_params.protocol == NFA_PROTOCOL_T3T) {
    /* Look for T3T entries in listen_info table that match activated system
     * code and NFCID2 */
    for (listen_info_idx = 0; listen_info_idx < NFA_CE_LISTEN_INFO_IDX_INVALID;
         listen_info_idx++) {
      /* Look for entries with NFA_PROTOCOL_MASK_T3T */
      if (p_cb->listen_info[listen_info_idx].flags &
          NFA_CE_LISTEN_INFO_IN_USE) {
        if (p_cb->listen_info[listen_info_idx].protocol_mask &
            NFA_PROTOCOL_MASK_T3T) {
          /* Check if system_code and nfcid2 that matches activation params */
          p_nfcid2 = p_cb->listen_info[listen_info_idx].t3t_nfcid2;
          t3t_system_code = p_cb->listen_info[listen_info_idx].t3t_system_code;

          /* Compare NFCID2 (note: NFCC currently does not return system code in
           * activation parameters) */
          if ((memcmp(p_nfcid2,
                      p_cb->activation_params.rf_tech_param.param.lf.nfcid2,
                      NCI_RF_F_UID_LEN) == 0)
              /* && (t3t_system_code == p_ce_msg->activation.p_activate_info->rf_tech_param.param.lf.system_code) */) {
            p_cb->listen_info[listen_info_idx].flags |=
                NFA_CE_LISTEN_INFO_T3T_ACTIVATE_PND;
            t3t_activate_pending = true;
            t3t_activate_idx = listen_info_idx;
          }
        }

        /* Check if entry is for T3T UICC */
        if ((p_cb->listen_info[listen_info_idx].flags &
             NFA_CE_LISTEN_INFO_UICC) &&
            (p_cb->listen_info[listen_info_idx].tech_mask &
             NFA_TECHNOLOGY_MASK_F)) {
          t3t_offhost_entry_found = true;
          t3t_offhost_idx = listen_info_idx;
        }
      }
    }

    p_ce_cback = nfa_ce_handle_t3t_evt;
    /* If listening for PROTO_T3T on DH and eSE/UICC, then notify CE module
     * now and wait for reader/writer to SELECT a target */
    if (t3t_activate_pending && t3t_offhost_entry_found) {
      CE_SetActivatedTagType(&p_cb->activation_params, t3t_system_code,
                             p_ce_cback);
      return true;
    } else if (t3t_activate_pending) {
      listen_info_idx = t3t_activate_idx;
    } else if (t3t_offhost_entry_found) {
      listen_info_idx = t3t_offhost_idx;
    }
  } else if (p_cb->activation_params.protocol == NFA_PROTOCOL_ISO_DEP) {
    p_ce_cback = nfa_ce_handle_t4t_evt;

    /* For T4T, we do not know which AID will be selected yet */

    /* For all T4T entries in listen_info, set T4T_ACTIVATE_NOTIFY_PENDING flag
     */
    for (i = 0; i < NFA_CE_LISTEN_INFO_IDX_INVALID; i++) {
      if (p_cb->listen_info[i].flags & NFA_CE_LISTEN_INFO_IN_USE) {
        if (p_cb->listen_info[i].protocol_mask & NFA_PROTOCOL_MASK_ISO_DEP) {
          /* Found listen_info table entry for T4T raw listen */
          p_cb->listen_info[i].flags |= NFA_CE_LISTEN_INFO_T4T_ACTIVATE_PND;

          /* If entry if for NDEF, select it, so application gets nofitifed of
           * ACTIVATE_EVT now */
          if (i == NFA_CE_LISTEN_INFO_IDX_NDEF) {
            listen_info_idx = NFA_CE_LISTEN_INFO_IDX_NDEF;
          }

          t4t_activate_pending = true;
        }

#if (NFC_NFCEE_INCLUDED == TRUE)
        /* Check if entry is for ISO_DEP UICC */
        if (p_cb->listen_info[i].flags & NFA_CE_LISTEN_INFO_UICC) {
          if (((p_cb->activation_params.rf_tech_param.mode ==
                NFC_DISCOVERY_TYPE_LISTEN_A) &&
               (p_cb->listen_info[i].tech_proto_mask &
                NFA_DM_DISC_MASK_LA_ISO_DEP)) ||
              ((p_cb->activation_params.rf_tech_param.mode ==
                NFC_DISCOVERY_TYPE_LISTEN_B) &&
               (p_cb->listen_info[i].tech_proto_mask &
                NFA_DM_DISC_MASK_LB_ISO_DEP))) {
            listen_info_idx = i;
          }
        }
#endif
      }
    }

    /* If listening for ISO_DEP, but not NDEF nor UICC, then notify CE module
     * now and wait for reader/writer to SELECT an AID */
    if (t4t_activate_pending &&
        (listen_info_idx == NFA_CE_LISTEN_INFO_IDX_INVALID)) {
      CE_SetActivatedTagType(&p_cb->activation_params, 0, p_ce_cback);
      return true;
    }
  } else if (p_cb->activation_params.intf_param.type ==
             NFC_INTERFACE_EE_DIRECT_RF) {
    /* search any entry listening UICC */
    for (i = 0; i < NFA_CE_LISTEN_INFO_IDX_INVALID; i++) {
      if ((p_cb->listen_info[i].flags & NFA_CE_LISTEN_INFO_IN_USE) &&
          (p_cb->listen_info[i].flags & NFA_CE_LISTEN_INFO_UICC)) {
        listen_info_idx = i;
        break;
      }
    }
  }

  /* Check if valid listen_info entry was found */
  if ((listen_info_idx == NFA_CE_LISTEN_INFO_IDX_INVALID) ||
      ((listen_info_idx == NFA_CE_LISTEN_INFO_IDX_NDEF) &&
       !(p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].flags &
         NFA_CE_LISTEN_INFO_IN_USE))) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "No listen_info found for this activation. listen_info_idx=%d",
        listen_info_idx);
    return true;
  }

  p_cb->listen_info[listen_info_idx].flags &=
      ~NFA_CE_LISTEN_INFO_T4T_ACTIVATE_PND;
  p_cb->listen_info[listen_info_idx].flags &=
      ~NFA_CE_LISTEN_INFO_T3T_ACTIVATE_PND;

  /* Get CONN_CBACK for this activation */
  p_cb->p_active_conn_cback = p_cb->listen_info[listen_info_idx].p_conn_cback;
  p_cb->idx_cur_active = listen_info_idx;

  if ((p_cb->idx_cur_active == NFA_CE_LISTEN_INFO_IDX_NDEF) ||
      (p_cb->listen_info[p_cb->idx_cur_active].flags &
       NFA_CE_LISTEN_INFO_UICC)) {
    memcpy(&(conn_evt.activated.activate_ntf), &p_cb->activation_params,
           sizeof(tNFC_ACTIVATE_DEVT));

    (*p_cb->p_active_conn_cback)(NFA_ACTIVATED_EVT, &conn_evt);
  } else {
    conn_evt.ce_activated.handle =
        NFA_HANDLE_GROUP_CE | ((tNFA_HANDLE)p_cb->idx_cur_active);
    memcpy(&(conn_evt.ce_activated.activate_ntf), &p_cb->activation_params,
           sizeof(tNFC_ACTIVATE_DEVT));
    conn_evt.ce_activated.status = NFA_STATUS_OK;

    (*p_cb->p_active_conn_cback)(NFA_CE_ACTIVATED_EVT, &conn_evt);
  }

  /* we don't need any CE subsystem in case of NFCEE direct RF interface */
  if (p_ce_cback) {
    /* Notify CE subsystem */
    CE_SetActivatedTagType(&p_cb->activation_params, t3t_system_code,
                           p_ce_cback);
  }
  return true;
}

/*******************************************************************************
**
** Function         nfa_ce_deactivate_ntf
**
** Description      Action when deactivate occurs. (NFA_CE_DEACTIVATE_NTF_EVT)
**
**                  - If deactivate due to API deregister, then remove its entry
**                    from listen_info table
**
**                  - If NDEF was modified while activated, then restore
**                    original NDEF contents
**
**                  - Restart listening (if any active entries in listen table)
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
bool nfa_ce_deactivate_ntf(tNFA_CE_MSG* p_ce_msg) {
  tNFC_DEACT_TYPE deact_type = (tNFC_DEACT_TYPE)p_ce_msg->hdr.layer_specific;
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  tNFA_CONN_EVT_DATA conn_evt;
  uint8_t i;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("deact_type=%d", deact_type);

  /* Check if deactivating to SLEEP mode */
  if ((deact_type == NFC_DEACTIVATE_TYPE_SLEEP) ||
      (deact_type == NFC_DEACTIVATE_TYPE_SLEEP_AF)) {
    if (nfa_ce_cb.idx_wild_card == NFA_CE_LISTEN_INFO_IDX_INVALID) {
      /* notify deactivated as sleep and wait for reactivation or deactivation
       * to idle */
      conn_evt.deactivated.type = deact_type;

      /* if T4T AID application has not been selected then p_active_conn_cback
       * could be NULL */
      if (p_cb->p_active_conn_cback)
        (*p_cb->p_active_conn_cback)(NFA_DEACTIVATED_EVT, &conn_evt);
    } else {
      conn_evt.ce_deactivated.handle =
          NFA_HANDLE_GROUP_CE | ((tNFA_HANDLE)nfa_ce_cb.idx_wild_card);
      conn_evt.ce_deactivated.type = deact_type;
      if (p_cb->p_active_conn_cback)
        (*p_cb->p_active_conn_cback)(NFA_CE_DEACTIVATED_EVT, &conn_evt);
    }

    return true;
  } else {
#if (NXP_EXTNS != TRUE)
    deact_type = NFC_DEACTIVATE_TYPE_IDLE;
#endif
  }

  /* Tag is in idle state */
  p_cb->flags &= ~NFA_CE_FLAGS_LISTEN_ACTIVE_SLEEP;

  /* First, notify app of deactivation */
  for (i = 0; i < NFA_CE_LISTEN_INFO_IDX_INVALID; i++) {
    if (p_cb->listen_info[i].flags & NFA_CE_LISTEN_INFO_IN_USE) {
      if ((p_cb->listen_info[i].flags & NFA_CE_LISTEN_INFO_UICC) &&
          (i == p_cb->idx_cur_active)) {
        conn_evt.deactivated.type = deact_type;
        if (p_cb->p_active_conn_cback)
          (*p_cb->p_active_conn_cback)(NFA_DEACTIVATED_EVT, &conn_evt);
      } else if ((p_cb->activation_params.protocol == NFA_PROTOCOL_ISO_DEP) &&
                 (p_cb->listen_info[i].protocol_mask &
                  NFA_PROTOCOL_MASK_ISO_DEP)) {
        /* Don't send NFA_DEACTIVATED_EVT if NFA_ACTIVATED_EVT wasn't sent */
        if (!(p_cb->listen_info[i].flags &
              NFA_CE_LISTEN_INFO_T4T_ACTIVATE_PND)) {
          if (i == NFA_CE_LISTEN_INFO_IDX_NDEF) {
            conn_evt.deactivated.type = deact_type;
            if (p_cb->p_active_conn_cback)
              (*p_cb->p_active_conn_cback)(NFA_DEACTIVATED_EVT, &conn_evt);
          } else {
            conn_evt.ce_deactivated.handle =
                NFA_HANDLE_GROUP_CE | ((tNFA_HANDLE)i);
            conn_evt.ce_deactivated.type = deact_type;
            if (p_cb->p_active_conn_cback)
              (*p_cb->p_active_conn_cback)(NFA_CE_DEACTIVATED_EVT, &conn_evt);
          }
        }
      } else if ((p_cb->activation_params.protocol == NFA_PROTOCOL_T3T) &&
                 (p_cb->listen_info[i].protocol_mask & NFA_PROTOCOL_MASK_T3T)) {
        /* Don't send NFA_DEACTIVATED_EVT if NFA_ACTIVATED_EVT wasn't sent */
        if (!(p_cb->listen_info[i].flags &
              NFA_CE_LISTEN_INFO_T3T_ACTIVATE_PND)) {
          if (i == NFA_CE_LISTEN_INFO_IDX_NDEF) {
            conn_evt.deactivated.type = deact_type;
            if (p_cb->p_active_conn_cback)
              (*p_cb->p_active_conn_cback)(NFA_DEACTIVATED_EVT, &conn_evt);
          } else {
            conn_evt.ce_deactivated.handle =
                NFA_HANDLE_GROUP_CE | ((tNFA_HANDLE)i);
            conn_evt.ce_deactivated.type = deact_type;
            if (p_cb->p_active_conn_cback) {
              (*p_cb->p_active_conn_cback)(NFA_CE_DEACTIVATED_EVT, &conn_evt);
            } else {
              android_errorWriteLog(0x534e4554, "120846143");
            }
          }
        }
      }
    }
  }

  /* Check if app initiated the deactivation (due to API deregister). If so,
   * remove entry from listen_info table. */
  if (p_cb->flags & NFA_CE_FLAGS_APP_INIT_DEACTIVATION) {
    p_cb->flags &= ~NFA_CE_FLAGS_APP_INIT_DEACTIVATION;
    nfa_ce_remove_listen_info_entry(p_cb->idx_cur_active, true);
  }

  p_cb->p_active_conn_cback = nullptr;
  p_cb->idx_cur_active = NFA_CE_LISTEN_INFO_IDX_INVALID;

  /* Restart listening (if any listen_info entries are still active) */
  nfa_ce_restart_listen_check();

  return true;
}

/*******************************************************************************
**
** Function         nfa_ce_disable_local_tag
**
** Description      Disable local NDEF tag
**                      - clean up control block
**                      - remove NDEF discovery configuration
**
** Returns          Nothing
**
*******************************************************************************/
void nfa_ce_disable_local_tag(void) {
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  tNFA_CONN_EVT_DATA evt_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("Disabling local NDEF tag");

  /* If local NDEF tag is in use, then disable it */
  if (p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].flags &
      NFA_CE_LISTEN_INFO_IN_USE) {
    /* NDEF Tag is in not idle state */
    if ((p_cb->flags & NFA_CE_FLAGS_LISTEN_ACTIVE_SLEEP) &&
        (p_cb->idx_cur_active == NFA_CE_LISTEN_INFO_IDX_NDEF)) {
      /* wait for deactivation */
      p_cb->flags |= NFA_CE_FLAGS_APP_INIT_DEACTIVATION;
      nfa_dm_rf_deactivate(NFA_DEACTIVATE_TYPE_IDLE);
    } else {
      /* Notify DM to stop listening for ndef  */
      if (p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].rf_disc_handle !=
          NFA_HANDLE_INVALID) {
        nfa_dm_delete_rf_discover(
            p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].rf_disc_handle);
        p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].rf_disc_handle =
            NFA_HANDLE_INVALID;
      }
      nfa_ce_remove_listen_info_entry(NFA_CE_LISTEN_INFO_IDX_NDEF, true);
    }
  } else {
    /* Notify application */
    evt_data.status = NFA_STATUS_OK;
    nfa_dm_conn_cback_event_notify(NFA_CE_LOCAL_TAG_CONFIGURED_EVT, &evt_data);
  }
}

/*******************************************************************************
**
** Function         nfa_ce_api_cfg_local_tag
**
** Description      Configure local NDEF tag
**                      - store ndef attributes in to control block
**                      - update discovery configuration
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
bool nfa_ce_api_cfg_local_tag(tNFA_CE_MSG* p_ce_msg) {
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  tNFA_CONN_EVT_DATA conn_evt;

  /* Check if disabling local tag */
  if (p_ce_msg->local_tag.protocol_mask == 0) {
    nfa_ce_disable_local_tag();
    return true;
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "Configuring local NDEF tag: protocol_mask=%01x cur_size=%i, "
      "max_size=%i, readonly=%i uid_len=%i",
      p_ce_msg->local_tag.protocol_mask, p_ce_msg->local_tag.ndef_cur_size,
      p_ce_msg->local_tag.ndef_max_size, p_ce_msg->local_tag.read_only,
      p_ce_msg->local_tag.uid_len);

  /* If local tag was already set, then check if NFA_CeConfigureLocalTag called
   * to change protocol mask  */
  if ((p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].flags &
       NFA_CE_LISTEN_INFO_IN_USE) &&
      (p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].rf_disc_handle !=
       NFA_HANDLE_INVALID) &&
      ((p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].protocol_mask &
        (NFA_PROTOCOL_MASK_T3T | NFA_PROTOCOL_MASK_ISO_DEP)) !=
       (p_ce_msg->local_tag.protocol_mask &
        (NFA_PROTOCOL_MASK_T3T | NFA_PROTOCOL_MASK_ISO_DEP)))) {
    /* Listening for different tag protocols. Stop discovery */
    nfa_dm_delete_rf_discover(
        p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].rf_disc_handle);
    p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].rf_disc_handle =
        NFA_HANDLE_INVALID;

    /* clear NDEF contents */
    CE_T3tSetLocalNDEFMsg(true, 0, 0, nullptr, nullptr);
    CE_T4tSetLocalNDEFMsg(true, 0, 0, nullptr, nullptr);
  }

  /* Store NDEF info to control block */
  p_cb->p_ndef_data = p_ce_msg->local_tag.p_ndef_data;
  p_cb->ndef_cur_size = p_ce_msg->local_tag.ndef_cur_size;
  p_cb->ndef_max_size = p_ce_msg->local_tag.ndef_max_size;

  /* Fill in LISTEN_INFO entry for NDEF */
  p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].flags =
      NFA_CE_LISTEN_INFO_IN_USE;
  p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].protocol_mask =
      p_ce_msg->local_tag.protocol_mask;
  p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].p_conn_cback =
      nfa_dm_conn_cback_event_notify;
  if (p_ce_msg->local_tag.read_only)
    p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].flags |=
        NFC_CE_LISTEN_INFO_READONLY_NDEF;
  p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].t3t_system_code =
      T3T_SYSTEM_CODE_NDEF;

  /* Set NDEF contents */
  conn_evt.status = NFA_STATUS_FAILED;

  if (p_cb->listen_info[NFA_CE_LISTEN_INFO_IDX_NDEF].protocol_mask &
      (NFA_PROTOCOL_MASK_T3T | NFA_PROTOCOL_MASK_ISO_DEP)) {
    /* Ok to set contents now */
    if (nfa_ce_set_content() != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "nfa_ce_api_cfg_local_tag: could not set contents");
      nfa_dm_conn_cback_event_notify(NFA_CE_LOCAL_TAG_CONFIGURED_EVT,
                                     &conn_evt);
      return true;
    }

    /* Start listening and notify app of status */
    conn_evt.status = nfa_ce_start_listening();
    nfa_dm_conn_cback_event_notify(NFA_CE_LOCAL_TAG_CONFIGURED_EVT, &conn_evt);
  }

  return true;
}

/*******************************************************************************
**
** Function         nfa_ce_api_reg_listen
**
** Description      Register listen params for Felica system code, T4T AID,
**                  or UICC
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
bool nfa_ce_api_reg_listen(tNFA_CE_MSG* p_ce_msg) {
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  tNFA_CONN_EVT_DATA conn_evt;
  uint8_t i;
  uint8_t listen_info_idx = NFA_CE_LISTEN_INFO_IDX_INVALID;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("Registering UICC/Felica/Type-4 tag listener. Type=%i",
                      p_ce_msg->reg_listen.listen_type);

  /* Look for available entry in listen_info table */
  /* - If registering UICC listen, make sure there isn't another entry for the
   * ee_handle  */
  /* - Skip over entry 0 (reserved for local NDEF tag) */
  for (i = 1; i < NFA_CE_LISTEN_INFO_MAX; i++) {
    if ((p_ce_msg->reg_listen.listen_type == NFA_CE_REG_TYPE_UICC) &&
        (p_cb->listen_info[i].flags & NFA_CE_LISTEN_INFO_IN_USE) &&
        (p_cb->listen_info[i].flags & NFA_CE_LISTEN_INFO_UICC) &&
        (p_cb->listen_info[i].ee_handle == p_ce_msg->reg_listen.ee_handle)) {
      if(p_cb->listen_info[i].tech_mask == p_ce_msg->reg_listen.tech_mask) {
        LOG(ERROR) << StringPrintf("UICC (0x%x) listening already specified",
                                 p_ce_msg->reg_listen.ee_handle);
        conn_evt.status = NFA_STATUS_FAILED;
        nfa_dm_conn_cback_event_notify(NFA_CE_UICC_LISTEN_CONFIGURED_EVT,
                                     &conn_evt);
        return true;
      } else {
        DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("UICC (0x%x) listening parameter changed to %x",
                                 p_ce_msg->reg_listen.ee_handle, p_ce_msg->reg_listen.tech_mask);
        listen_info_idx = i;
        break;
      }
    }
    /* If this is a free entry, and we haven't found one yet, remember it */
    else if ((!(p_cb->listen_info[i].flags & NFA_CE_LISTEN_INFO_IN_USE)) &&
             (listen_info_idx == NFA_CE_LISTEN_INFO_IDX_INVALID)) {
      listen_info_idx = i;
    }
  }

  /* Add new entry to listen_info table */
  if (listen_info_idx == NFA_CE_LISTEN_INFO_IDX_INVALID) {
    LOG(ERROR) << StringPrintf("Maximum listen callbacks exceeded (%i)",
                               NFA_CE_LISTEN_INFO_MAX);

    if (p_ce_msg->reg_listen.listen_type == NFA_CE_REG_TYPE_UICC) {
      conn_evt.status = NFA_STATUS_FAILED;
      nfa_dm_conn_cback_event_notify(NFA_CE_UICC_LISTEN_CONFIGURED_EVT,
                                     &conn_evt);
    } else {
      /* Notify application */
      conn_evt.ce_registered.handle = NFA_HANDLE_INVALID;
      conn_evt.ce_registered.status = NFA_STATUS_FAILED;
      (*p_ce_msg->reg_listen.p_conn_cback)(NFA_CE_REGISTERED_EVT, &conn_evt);
    }
    return true;
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("NFA_CE: adding listen_info entry %i", listen_info_idx);

    /* Store common parameters */
    /* Mark entry as 'in-use', and NFA_CE_LISTEN_INFO_START_NTF_PND */
    /* (LISTEN_START_EVT will be notified when discovery successfully starts */
    p_cb->listen_info[listen_info_idx].flags =
        NFA_CE_LISTEN_INFO_IN_USE | NFA_CE_LISTEN_INFO_START_NTF_PND;
    p_cb->listen_info[listen_info_idx].rf_disc_handle = NFA_HANDLE_INVALID;
    p_cb->listen_info[listen_info_idx].protocol_mask = 0;

    /* Store type-specific parameters */
    switch (p_ce_msg->reg_listen.listen_type) {
      case NFA_CE_REG_TYPE_ISO_DEP:
        p_cb->listen_info[listen_info_idx].protocol_mask =
            NFA_PROTOCOL_MASK_ISO_DEP;
        p_cb->listen_info[listen_info_idx].flags |= NFA_CE_LISTEN_INFO_T4T_AID;
        p_cb->listen_info[listen_info_idx].p_conn_cback =
            p_ce_msg->reg_listen.p_conn_cback;

        /* Register this AID with CE_T4T */
        p_cb->listen_info[listen_info_idx].t4t_aid_handle = CE_T4tRegisterAID(
            p_ce_msg->reg_listen.aid_len, p_ce_msg->reg_listen.aid,
            nfa_ce_handle_t4t_aid_evt);
        if (p_cb->listen_info[listen_info_idx].t4t_aid_handle ==
            CE_T4T_AID_HANDLE_INVALID) {
          LOG(ERROR) << StringPrintf("Unable to register AID");
          p_cb->listen_info[listen_info_idx].flags = 0;

          /* Notify application */
          conn_evt.ce_registered.handle = NFA_HANDLE_INVALID;
          conn_evt.ce_registered.status = NFA_STATUS_FAILED;
          (*p_ce_msg->reg_listen.p_conn_cback)(NFA_CE_REGISTERED_EVT,
                                               &conn_evt);

          return true;
        }
        if (p_cb->listen_info[listen_info_idx].t4t_aid_handle ==
            CE_T4T_WILDCARD_AID_HANDLE)
          nfa_ce_cb.idx_wild_card = listen_info_idx;
        break;

      case NFA_CE_REG_TYPE_FELICA:
        p_cb->listen_info[listen_info_idx].protocol_mask =
            NFA_PROTOCOL_MASK_T3T;
        p_cb->listen_info[listen_info_idx].flags |= NFA_CE_LISTEN_INFO_FELICA;
        p_cb->listen_info[listen_info_idx].p_conn_cback =
            p_ce_msg->reg_listen.p_conn_cback;

        /* Store system code and nfcid2 */
        p_cb->listen_info[listen_info_idx].t3t_system_code =
            p_ce_msg->reg_listen.system_code;
        memcpy(p_cb->listen_info[listen_info_idx].t3t_nfcid2,
               p_ce_msg->reg_listen.nfcid2, NCI_RF_F_UID_LEN);
        memcpy(p_cb->listen_info[listen_info_idx].t3t_pmm,
               p_ce_msg->reg_listen.t3tPmm, NCI_T3T_PMM_LEN);
        break;

#if (NFC_NFCEE_INCLUDED == TRUE)
      case NFA_CE_REG_TYPE_UICC:
        p_cb->listen_info[listen_info_idx].flags |= NFA_CE_LISTEN_INFO_UICC;
        p_cb->listen_info[listen_info_idx].p_conn_cback =
            &nfa_dm_conn_cback_event_notify;

        /* Store EE handle and Tech */
        p_cb->listen_info[listen_info_idx].ee_handle =
            p_ce_msg->reg_listen.ee_handle;
        p_cb->listen_info[listen_info_idx].tech_mask =
            p_ce_msg->reg_listen.tech_mask;
        break;
#endif
    }
  }

  /* Start listening */
  conn_evt.status = nfa_ce_start_listening();
  if (conn_evt.status != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf(
        "nfa_ce_api_reg_listen: unable to register new listen params with DM");
    p_cb->listen_info[listen_info_idx].flags = 0;
  }

  /* Nofitify app of status */
  if (p_ce_msg->reg_listen.listen_type == NFA_CE_REG_TYPE_UICC) {
    (*p_cb->listen_info[listen_info_idx].p_conn_cback)(
        NFA_CE_UICC_LISTEN_CONFIGURED_EVT, &conn_evt);
  } else {
    conn_evt.ce_registered.handle = NFA_HANDLE_GROUP_CE | listen_info_idx;
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("nfa_ce_api_reg_listen: registered handle 0x%04X",
                        conn_evt.ce_registered.handle);
    (*p_cb->listen_info[listen_info_idx].p_conn_cback)(NFA_CE_REGISTERED_EVT,
                                                       &conn_evt);
  }

  return true;
}

/*******************************************************************************
**
** Function         nfa_ce_api_dereg_listen
**
** Description      Deregister listen params
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
bool nfa_ce_api_dereg_listen(tNFA_CE_MSG* p_ce_msg) {
  tNFA_CE_CB* p_cb = &nfa_ce_cb;
  uint8_t listen_info_idx;
  tNFA_CONN_EVT_DATA conn_evt;

#if (NFC_NFCEE_INCLUDED == TRUE)
  /* Check if deregistering UICC , or virtual secure element listen */
  if (p_ce_msg->dereg_listen.listen_info == NFA_CE_LISTEN_INFO_UICC) {
    /* Deregistering UICC listen. Look for listen_info for this UICC ee handle
     */
    for (listen_info_idx = 0; listen_info_idx < NFA_CE_LISTEN_INFO_MAX;
         listen_info_idx++) {
      if ((p_cb->listen_info[listen_info_idx].flags &
           NFA_CE_LISTEN_INFO_IN_USE) &&
          (p_cb->listen_info[listen_info_idx].flags &
           NFA_CE_LISTEN_INFO_UICC) &&
          (p_cb->listen_info[listen_info_idx].ee_handle ==
           p_ce_msg->dereg_listen.handle)) {
        /* UICC is in not idle state */
        if ((p_cb->flags & NFA_CE_FLAGS_LISTEN_ACTIVE_SLEEP) &&
            (p_cb->idx_cur_active == listen_info_idx)) {
          /* wait for deactivation */
          p_cb->flags |= NFA_CE_FLAGS_APP_INIT_DEACTIVATION;
          nfa_dm_rf_deactivate(NFA_DEACTIVATE_TYPE_IDLE);
        } else {
          /* Stop listening */
          if (p_cb->listen_info[listen_info_idx].rf_disc_handle !=
              NFA_HANDLE_INVALID) {
            nfa_dm_delete_rf_discover(
                p_cb->listen_info[listen_info_idx].rf_disc_handle);
            p_cb->listen_info[listen_info_idx].rf_disc_handle =
                NFA_HANDLE_INVALID;
          }

          /* Remove entry and notify application */
          nfa_ce_remove_listen_info_entry(listen_info_idx, true);
        }
        break;
      }
    }

    if (listen_info_idx == NFA_CE_LISTEN_INFO_MAX) {
      LOG(ERROR) << StringPrintf("cannot find listen_info for UICC");
      conn_evt.status = NFA_STATUS_INVALID_PARAM;
      nfa_dm_conn_cback_event_notify(NFA_CE_UICC_LISTEN_CONFIGURED_EVT,
                                     &conn_evt);
    }
  } else
#endif
  {
    /* Deregistering virtual secure element listen */
    listen_info_idx = p_ce_msg->dereg_listen.handle & NFA_HANDLE_MASK;
    if (nfa_ce_cb.idx_wild_card == listen_info_idx) {
      nfa_ce_cb.idx_wild_card = NFA_CE_LISTEN_INFO_IDX_INVALID;
    }

    if ((listen_info_idx < NFA_CE_LISTEN_INFO_MAX) &&
        (p_cb->listen_info[listen_info_idx].flags &
         NFA_CE_LISTEN_INFO_IN_USE)) {
      /* virtual secure element is in not idle state */
      if ((p_cb->flags & NFA_CE_FLAGS_LISTEN_ACTIVE_SLEEP) &&
          (p_cb->idx_cur_active == listen_info_idx)) {
        /* wait for deactivation */
        p_cb->flags |= NFA_CE_FLAGS_APP_INIT_DEACTIVATION;
        nfa_dm_rf_deactivate(NFA_DEACTIVATE_TYPE_IDLE);
      } else {
        /* Stop listening */
        if (p_cb->listen_info[listen_info_idx].rf_disc_handle !=
            NFA_HANDLE_INVALID) {
          nfa_dm_delete_rf_discover(
              p_cb->listen_info[listen_info_idx].rf_disc_handle);
          p_cb->listen_info[listen_info_idx].rf_disc_handle =
              NFA_HANDLE_INVALID;
        }

        /* Remove entry and notify application */
        nfa_ce_remove_listen_info_entry(listen_info_idx, true);
      }
    } else {
      LOG(ERROR) << StringPrintf(
          "cannot find listen_info for "
          "Felica/T4tAID");
      conn_evt.status = NFA_STATUS_INVALID_PARAM;
      nfa_dm_conn_cback_event_notify(NFA_CE_DEREGISTERED_EVT, &conn_evt);
    }
  }

  return true;
}

/*******************************************************************************
**
** Function         nfa_ce_api_cfg_isodep_tech
**
** Description      Configure the technologies (NFC-A and/or NFC-B) to listen
**                  for ISO-DEP
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
bool nfa_ce_api_cfg_isodep_tech(tNFA_CE_MSG* p_ce_msg) {
  nfa_ce_cb.isodep_disc_mask = 0;
  if (p_ce_msg->hdr.layer_specific & NFA_TECHNOLOGY_MASK_A)
    nfa_ce_cb.isodep_disc_mask = NFA_DM_DISC_MASK_LA_ISO_DEP;

  if (p_ce_msg->hdr.layer_specific & NFA_TECHNOLOGY_MASK_B)
    nfa_ce_cb.isodep_disc_mask |= NFA_DM_DISC_MASK_LB_ISO_DEP;
  return true;
}
