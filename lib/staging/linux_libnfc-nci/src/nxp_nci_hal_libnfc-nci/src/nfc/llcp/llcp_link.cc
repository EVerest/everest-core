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
 *  This file contains the LLCP Link Management
 *
 ******************************************************************************/

#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <log/log.h>
#include "bt_types.h"
#include "gki.h"
#include "llcp_defs.h"
#include "llcp_int.h"
#include "nfa_dm_int.h"
#include "nfc_int.h"

//using android::base::StringPrintf;

const uint16_t llcp_link_rwt
    [15] = /* RWT = (302us)*2**WT; 302us = 256*16/fc; fc = 13.56MHz */
    {
        1,    /* WT=0,     302us */
        1,    /* WT=1,     604us */
        2,    /* WT=2,    1208us */
        3,    /* WT=3,     2.4ms */
        5,    /* WT=4,     4.8ms */
        10,   /* WT=5,     9.7ms */
        20,   /* WT=6,    19.3ms */
        39,   /* WT=7,    38.7ms */
        78,   /* WT=8,    77.3ms */
        155,  /* WT=9,   154.6ms */
        310,  /* WT=10,  309.2ms */
        619,  /* WT=11,  618.5ms */
        1237, /* WT=12, 1237.0ms */
        2474, /* WT=13, 2474.0ms */
        4948, /* WT=14, 4948.0ms */
};

static bool llcp_link_parse_gen_bytes(uint8_t gen_bytes_len,
                                      uint8_t* p_gen_bytes);
static bool llcp_link_version_agreement(void);

static void llcp_link_send_SYMM(void);
static void llcp_link_update_status(bool is_activated);
static void llcp_link_check_congestion(void);
static void llcp_link_check_uncongested(void);
static void llcp_link_proc_ui_pdu(uint8_t local_sap, uint8_t remote_sap,
                                  uint16_t ui_pdu_length, uint8_t* p_ui_pdu,
                                  NFC_HDR* p_msg);
static void llcp_link_proc_agf_pdu(NFC_HDR* p_msg);
static void llcp_link_proc_rx_pdu(uint8_t dsap, uint8_t ptype, uint8_t ssap,
                                  NFC_HDR* p_msg);
static void llcp_link_proc_rx_data(NFC_HDR* p_msg);

static NFC_HDR* llcp_link_get_next_pdu(bool length_only,
                                       uint16_t* p_next_pdu_length);
static NFC_HDR* llcp_link_build_next_pdu(NFC_HDR* p_agf);
static void llcp_link_send_to_lower(NFC_HDR* p_msg);

#if (LLCP_TEST_INCLUDED == TRUE) /* this is for LLCP testing */
extern tLLCP_TEST_PARAMS llcp_test_params;
#endif

extern bool nfc_debug_enabled;
extern unsigned char appl_dta_mode_flag;

/* debug functions type */
static std::string llcp_pdu_type(uint8_t ptype);

/*******************************************************************************
**
** Function         llcp_link_start_inactivity_timer
**
** Description      This function start LLCP link inactivity timer.
**
** Returns          void
**
*******************************************************************************/
static void llcp_link_start_inactivity_timer(void) {
  if ((llcp_cb.lcb.inact_timer.in_use == false) &&
      (llcp_cb.lcb.inact_timeout > 0)) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "Start inactivity_timer: %d ms", llcp_cb.lcb.inact_timeout);

    nfc_start_quick_timer(&llcp_cb.lcb.inact_timer, NFC_TTYPE_LLCP_LINK_INACT,
                          ((uint32_t)llcp_cb.lcb.inact_timeout) *
                              QUICK_TIMER_TICKS_PER_SEC / 1000);
  }
}

/*******************************************************************************
**
** Function         llcp_link_stop_inactivity_timer
**
** Description      This function stop LLCP link inactivity timer.
**
** Returns          void
**
*******************************************************************************/
static void llcp_link_stop_inactivity_timer(void) {
  if (llcp_cb.lcb.inact_timer.in_use) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("Stop inactivity_timer");

    nfc_stop_quick_timer(&llcp_cb.lcb.inact_timer);
  }
}

/*******************************************************************************
**
** Function         llcp_link_start_link_timer
**
** Description      This function starts LLCP link timer (LTO or delay response)
**
** Returns          void
**
*******************************************************************************/
static void llcp_link_start_link_timer(void) {
  if (llcp_cb.lcb.symm_state == LLCP_LINK_SYMM_LOCAL_XMIT_NEXT) {
    /* wait for application layer sending data */
    nfc_start_quick_timer(
        &llcp_cb.lcb.timer, NFC_TTYPE_LLCP_LINK_MANAGER,
        (((uint32_t)llcp_cb.lcb.symm_delay) * QUICK_TIMER_TICKS_PER_SEC) /
            1000);
  } else {
    /* wait for data to receive from remote */
    nfc_start_quick_timer(
        &llcp_cb.lcb.timer, NFC_TTYPE_LLCP_LINK_MANAGER,
        ((uint32_t)llcp_cb.lcb.peer_lto) * QUICK_TIMER_TICKS_PER_SEC / 1000);
  }
}

/*******************************************************************************
**
** Function         llcp_link_stop_link_timer
**
** Description      This function stop LLCP link timer (LTO or delay response).
**
** Returns          void
**
*******************************************************************************/
static void llcp_link_stop_link_timer(void) {
  nfc_stop_quick_timer(&llcp_cb.lcb.timer);
}

/*******************************************************************************
**
** Function         llcp_link_activate
**
** Description      Activate LLCP link
**
** Returns          tLLCP_STATUS
**
*******************************************************************************/
tLLCP_STATUS llcp_link_activate(tLLCP_ACTIVATE_CONFIG* p_config) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* At this point, MAC link activation procedure has been successfully
   * completed */

  /* The Length Reduction values LRi and LRt MUST be 11b. (254bytes) */
  if (p_config->max_payload_size != LLCP_NCI_MAX_PAYL_SIZE) {
    LOG(WARNING) << StringPrintf("max payload size (%d) must be %d bytes",
                                 p_config->max_payload_size,
                                 LLCP_NCI_MAX_PAYL_SIZE);
  }

  /* Processing the parametes that have been received with the MAC link
   * activation */
  if (llcp_link_parse_gen_bytes(p_config->gen_bytes_len,
                                p_config->p_gen_bytes) == false) {
    LOG(ERROR) << StringPrintf("Failed to parse general bytes");
    /* For LLCP DTA test, In case of bad magic bytes normal p2p communication is
     * expected,but in case of wrong magic bytes in ATR_REQ, LLC layer will be
     * disconnected but P2P connection is expected to be in connected state
     * and non LLC PDU is expected.
     * As per NFC forum expectation below changes is to send PDU after
     * disconnect of LLCP PDU.
     * This is fix for TC_MAC_TAR_BI_01 LLCP test case */

    if (appl_dta_mode_flag == 1 && p_config->is_initiator == FALSE) {
      NFC_HDR* p_msg = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);

      if (p_msg) {
        /*LLCP test scenario requires non LLC PDU to be sent in case of wrong
          magic bytes. So sending NFC-DEP pdu with size 1 (0x00)*/
        p_msg->len = 1;
        p_msg->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;

        NFC_SendData(NFC_RF_CONN_ID, p_msg);
      }
    }

    (*llcp_cb.lcb.p_link_cback)(LLCP_LINK_ACTIVATION_FAILED_EVT,
                                LLCP_LINK_BAD_GEN_BYTES);

    if (p_config->is_initiator == false) {
      /* repond to any incoming PDU with invalid LLCP PDU */
      llcp_cb.lcb.link_state = LLCP_LINK_STATE_ACTIVATION_FAILED;
      NFC_SetStaticRfCback(llcp_link_connection_cback);
    }
    return LLCP_STATUS_FAIL;
  }

  /*
  ** For the Target device, the scaled value of RWT MUST be less than or equal
  ** to the scaled value of the LLC Link Timeout (LTO).
  */
  if ((p_config->is_initiator) &&
      (llcp_link_rwt[p_config->waiting_time] > llcp_cb.lcb.peer_lto)) {
    LOG(WARNING) << StringPrintf(
        "WT (%d, %dms) must be less than or equal to "
        "LTO (%dms)",
        p_config->waiting_time, llcp_link_rwt[p_config->waiting_time],
        llcp_cb.lcb.peer_lto);
  }
  /* For DTA mode Peer LTO Should not include TX RX Delay, Just llcp deactivate
   * after Peer LTO time */
  if (!appl_dta_mode_flag) {
    /* extend LTO as much as internally required processing time and propagation
     * delays */
    llcp_cb.lcb.peer_lto += LLCP_INTERNAL_TX_DELAY + LLCP_INTERNAL_RX_DELAY;
  }
  /* LLCP version number agreement */
  if (llcp_link_version_agreement() == false) {
    LOG(ERROR) << StringPrintf("Failed to agree version");
    (*llcp_cb.lcb.p_link_cback)(LLCP_LINK_ACTIVATION_FAILED_EVT,
                                LLCP_LINK_VERSION_FAILED);

    if (p_config->is_initiator == false) {
      /* repond to any incoming PDU with invalid LLCP PDU */
      llcp_cb.lcb.link_state = LLCP_LINK_STATE_ACTIVATION_FAILED;
      NFC_SetStaticRfCback(llcp_link_connection_cback);
    }
    return LLCP_STATUS_FAIL;
  }

  llcp_cb.lcb.received_first_packet = false;
  llcp_cb.lcb.is_initiator = p_config->is_initiator;

  /* reset internal flags */
  llcp_cb.lcb.flags = 0x00;

  /* set tx MIU to MIN (MIU of local LLCP, MIU of peer LLCP) */

  if (llcp_cb.lcb.local_link_miu >= llcp_cb.lcb.peer_miu)
    llcp_cb.lcb.effective_miu = llcp_cb.lcb.peer_miu;
  else
    llcp_cb.lcb.effective_miu = llcp_cb.lcb.local_link_miu;

  /*
  ** When entering the normal operation phase, LLCP shall initialize the
  ** symmetry procedure.
  */
  if (llcp_cb.lcb.is_initiator) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("Connected as Initiator");

    llcp_cb.lcb.inact_timeout = llcp_cb.lcb.inact_timeout_init;
    llcp_cb.lcb.symm_state = LLCP_LINK_SYMM_LOCAL_XMIT_NEXT;

    if (llcp_cb.lcb.delay_first_pdu_timeout > 0) {
      /* give a chance to upper layer to send PDU if need */
      nfc_start_quick_timer(&llcp_cb.lcb.timer, NFC_TTYPE_LLCP_DELAY_FIRST_PDU,
                            (((uint32_t)llcp_cb.lcb.delay_first_pdu_timeout) *
                             QUICK_TIMER_TICKS_PER_SEC) /
                                1000);
    } else {
      llcp_link_send_SYMM();
    }
  } else {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("Connected as Target");
    llcp_cb.lcb.inact_timeout = llcp_cb.lcb.inact_timeout_target;
    llcp_cb.lcb.symm_state = LLCP_LINK_SYMM_REMOTE_XMIT_NEXT;

    /* wait for data to receive from remote */
    llcp_link_start_link_timer();
  }

  /*
  ** Set state to LLCP_LINK_STATE_ACTIVATED and notify activation before set
  ** data callback because LLCP PDU could be in NCI queue.
  */
  llcp_cb.lcb.link_state = LLCP_LINK_STATE_ACTIVATED;

  /* LLCP Link Activation completed */
  (*llcp_cb.lcb.p_link_cback)(LLCP_LINK_ACTIVATION_COMPLETE_EVT,
                              LLCP_LINK_SUCCESS);

  /* Update link status to service layer */
  llcp_link_update_status(true);

  NFC_SetStaticRfCback(llcp_link_connection_cback);

  return (LLCP_STATUS_SUCCESS);
}

/*******************************************************************************
**
** Function         llcp_deactivate_cleanup
**
** Description      Clean up for link deactivation
**
** Returns          void
**
*******************************************************************************/
static void llcp_deactivate_cleanup(uint8_t reason) {
  /* report SDP failure for any pending request */
  llcp_sdp_proc_deactivation();

  /* Update link status to service layer */
  llcp_link_update_status(false);

  /* We had sent out DISC */
  llcp_cb.lcb.link_state = LLCP_LINK_STATE_DEACTIVATED;

  llcp_link_stop_link_timer();

  /* stop inactivity timer */
  llcp_link_stop_inactivity_timer();

  /* Let upper layer deactivate local link */
  (*llcp_cb.lcb.p_link_cback)(LLCP_LINK_DEACTIVATED_EVT, reason);
}

/*******************************************************************************
**
** Function         llcp_link_process_link_timeout
**
** Description      Process timeout events for LTO, SYMM and deactivating
**
** Returns          void
**
*******************************************************************************/
void llcp_link_process_link_timeout(void) {
  if (llcp_cb.lcb.link_state == LLCP_LINK_STATE_ACTIVATED) {
    if ((llcp_cb.lcb.symm_delay > 0) &&
        (llcp_cb.lcb.symm_state == LLCP_LINK_SYMM_LOCAL_XMIT_NEXT)) {
      /* upper layer doesn't have anything to send */
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "LEVT_TIMEOUT in state of "
          "LLCP_LINK_SYMM_LOCAL_XMIT_NEXT");
      llcp_link_send_SYMM();

      /* wait for data to receive from remote */
      llcp_link_start_link_timer();

      /* start inactivity timer */
      if (llcp_cb.num_data_link_connection == 0) {
        llcp_link_start_inactivity_timer();
      }
    } else {
      LOG(ERROR) << StringPrintf(
          "LEVT_TIMEOUT in state of "
          "LLCP_LINK_SYMM_REMOTE_XMIT_NEXT");
      llcp_link_deactivate(LLCP_LINK_TIMEOUT);
    }
  } else if (llcp_cb.lcb.link_state == LLCP_LINK_STATE_DEACTIVATING) {
    llcp_deactivate_cleanup(llcp_cb.lcb.link_deact_reason);

    NFC_SetStaticRfCback(nullptr);
  }
}

/*******************************************************************************
**
** Function         llcp_link_deactivate
**
** Description      Deactivate LLCP link
**
** Returns          void
**
*******************************************************************************/
void llcp_link_deactivate(uint8_t reason) {
  uint8_t local_sap, idx;
  tLLCP_DLCB* p_dlcb;
  tLLCP_APP_CB* p_app_cb;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("reason = 0x%x", reason);

  /* Release any held buffers in signaling PDU queue */
  while (llcp_cb.lcb.sig_xmit_q.p_first)
    GKI_freebuf(GKI_dequeue(&llcp_cb.lcb.sig_xmit_q));

  /* Release any held buffers in UI PDU queue */
  for (local_sap = LLCP_SAP_SDP + 1; local_sap < LLCP_NUM_SAPS; local_sap++) {
    p_app_cb = llcp_util_get_app_cb(local_sap);

    if ((p_app_cb) && (p_app_cb->p_app_cback)) {
      while (p_app_cb->ui_xmit_q.p_first)
        GKI_freebuf(GKI_dequeue(&p_app_cb->ui_xmit_q));

      p_app_cb->is_ui_tx_congested = false;

      while (p_app_cb->ui_rx_q.p_first)
        GKI_freebuf(GKI_dequeue(&p_app_cb->ui_rx_q));
    }
  }

  llcp_cb.total_tx_ui_pdu = 0;
  llcp_cb.total_rx_ui_pdu = 0;

  /* Notify all of data link */
  for (idx = 0; idx < LLCP_MAX_DATA_LINK; idx++) {
    if (llcp_cb.dlcb[idx].state != LLCP_DLC_STATE_IDLE) {
      p_dlcb = &(llcp_cb.dlcb[idx]);

      llcp_dlsm_execute(p_dlcb, LLCP_DLC_EVENT_LINK_ERROR, nullptr);
    }
  }
  llcp_cb.total_tx_i_pdu = 0;
  llcp_cb.total_rx_i_pdu = 0;

  llcp_cb.overall_tx_congested = false;
  llcp_cb.overall_rx_congested = false;

  /* As per the LLCP test specification v1.2.00 for test case TC_LLC_TAR_BV_04
   * the receiving LLC shall commence sending an LLC PDU to the remote
   * LLC. So, after IUT receives DISC PDU from LT(remote device), IUT shall
   * send DISC PDU to LT. appl_dta_mode_flag condition is added to fulfil
   * above requirement. Only in CR8, the IUT shall acknoweledge with SYMM for
   * DISC PDU. For other CRx, send DISC PDU.
   */
  if ((reason == LLCP_LINK_FRAME_ERROR) ||
      (reason == LLCP_LINK_LOCAL_INITIATED) ||
      (appl_dta_mode_flag && reason == LLCP_LINK_REMOTE_INITIATED &&
       llcp_cb.lcb.is_initiator == false &&
       (nfa_dm_cb.eDtaMode & 0xF0) != NFA_DTA_CR8)) {
    /* get rid of the data pending in NFC tx queue, so DISC PDU can be sent ASAP
     */
    NFC_FlushData(NFC_RF_CONN_ID);

    llcp_util_send_disc(LLCP_SAP_LM, LLCP_SAP_LM);

    /* Wait until DISC is sent to peer */
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("Wait until DISC is sent to peer");

    llcp_cb.lcb.link_state = LLCP_LINK_STATE_DEACTIVATING;

    if (llcp_cb.lcb.sig_xmit_q.count == 0) {
      /* if DISC is sent to NFCC, wait for short period for NFCC to send it to
       * peer */
      nfc_start_quick_timer(&llcp_cb.lcb.timer, NFC_TTYPE_LLCP_LINK_MANAGER,
                            ((uint32_t)50) * QUICK_TIMER_TICKS_PER_SEC / 1000);
    }

    llcp_cb.lcb.link_deact_reason = reason;
    return;
  } else if ((reason == LLCP_LINK_REMOTE_INITIATED) &&
             (!llcp_cb.lcb.is_initiator)) {
    /* if received DISC to deactivate LLCP link as target role, send SYMM PDU */
    llcp_link_send_SYMM();
  } else /*  for link timeout and interface error */
  {
    /* if got RF link loss receiving no LLC PDU from peer */
    if ((reason == LLCP_LINK_RF_LINK_LOSS_ERR) &&
        (!(llcp_cb.lcb.flags & LLCP_LINK_FLAGS_RX_ANY_LLC_PDU))) {
      reason = LLCP_LINK_RF_LINK_LOSS_NO_RX_LLC;
    }

    NFC_FlushData(NFC_RF_CONN_ID);
  }

  llcp_deactivate_cleanup(reason);
}

/*******************************************************************************
**
** Function         llcp_link_parse_gen_bytes
**
** Description      Check LLCP magic number and get parameters in general bytes
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool llcp_link_parse_gen_bytes(uint8_t gen_bytes_len,
                                      uint8_t* p_gen_bytes) {
  uint8_t* p = p_gen_bytes + LLCP_MAGIC_NUMBER_LEN;
  uint8_t length = gen_bytes_len - LLCP_MAGIC_NUMBER_LEN;

  if ((gen_bytes_len >= LLCP_MAGIC_NUMBER_LEN) &&
      (*(p_gen_bytes) == LLCP_MAGIC_NUMBER_BYTE0) &&
      (*(p_gen_bytes + 1) == LLCP_MAGIC_NUMBER_BYTE1) &&
      (*(p_gen_bytes + 2) == LLCP_MAGIC_NUMBER_BYTE2)) {
    /* in case peer didn't include these */
    llcp_cb.lcb.peer_miu = LLCP_DEFAULT_MIU;
    llcp_cb.lcb.peer_lto = LLCP_DEFAULT_LTO_IN_MS;

    return (llcp_util_parse_link_params(length, p));
  } else /* if this is not LLCP */
  {
    return false;
  }

  return true;
}

/*******************************************************************************
**
** Function         llcp_link_version_agreement
**
** Description      LLCP version number agreement
**
** Returns          TRUE if success
**
*******************************************************************************/
static bool llcp_link_version_agreement(void) {
  uint8_t peer_major_version, peer_minor_version;

  peer_major_version = LLCP_GET_MAJOR_VERSION(llcp_cb.lcb.peer_version);
  peer_minor_version = LLCP_GET_MINOR_VERSION(llcp_cb.lcb.peer_version);

  if (peer_major_version < LLCP_MIN_MAJOR_VERSION) {
    LOG(ERROR) << StringPrintf(
        "unsupported peer version number. Peer "
        "Major Version:%d",
        peer_major_version);
    return false;
  } else {
    if (peer_major_version == LLCP_VERSION_MAJOR) {
      llcp_cb.lcb.agreed_major_version = LLCP_VERSION_MAJOR;
      if (peer_minor_version >= LLCP_VERSION_MINOR) {
        llcp_cb.lcb.agreed_minor_version = LLCP_VERSION_MINOR;
      } else {
        llcp_cb.lcb.agreed_minor_version = peer_minor_version;
      }
    } else if (peer_major_version < LLCP_VERSION_MAJOR) {
      /* so far we can support backward compatibility */
      llcp_cb.lcb.agreed_major_version = peer_major_version;
      llcp_cb.lcb.agreed_minor_version = peer_minor_version;
    } else {
      /* let peer (higher major version) decide it */
      llcp_cb.lcb.agreed_major_version = LLCP_VERSION_MAJOR;
      llcp_cb.lcb.agreed_minor_version = LLCP_VERSION_MINOR;
    }

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "local version:%d.%d, remote version:%d.%d, agreed version:%d.%d",
        LLCP_VERSION_MAJOR, LLCP_VERSION_MINOR, peer_major_version,
        peer_minor_version, llcp_cb.lcb.agreed_major_version,
        llcp_cb.lcb.agreed_minor_version);

    return true;
  }
}

/*******************************************************************************
**
** Function         llcp_link_update_status
**
** Description      Notify all of service layer client link status change
**
** Returns          void
**
*******************************************************************************/
static void llcp_link_update_status(bool is_activated) {
  tLLCP_SAP_CBACK_DATA data;
  tLLCP_APP_CB* p_app_cb;
  uint8_t sap;

  data.link_status.event = LLCP_SAP_EVT_LINK_STATUS;
  data.link_status.is_activated = is_activated;
  data.link_status.is_initiator = llcp_cb.lcb.is_initiator;

  /* notify all SAP so they can create connection while link is activated */
  for (sap = LLCP_SAP_SDP + 1; sap < LLCP_NUM_SAPS; sap++) {
    p_app_cb = llcp_util_get_app_cb(sap);

    if ((p_app_cb) && (p_app_cb->p_app_cback)) {
      data.link_status.local_sap = sap;
      p_app_cb->p_app_cback(&data);
    }
  }
}

/*******************************************************************************
**
** Function         llcp_link_check_congestion
**
** Description      Check overall congestion status
**                  Notify to all of upper layer if congested
**
** Returns          void
**
*******************************************************************************/
static void llcp_link_check_congestion(void) {
  tLLCP_SAP_CBACK_DATA data;
  tLLCP_APP_CB* p_app_cb;
  uint8_t sap, idx;

  if (llcp_cb.overall_tx_congested) {
    /* already congested so no need to check again */
    return;
  }

  if (llcp_cb.total_tx_ui_pdu + llcp_cb.total_tx_i_pdu >=
      llcp_cb.max_num_tx_buff) {
    /* overall buffer usage is high */
    llcp_cb.overall_tx_congested = true;

    LOG(WARNING) << StringPrintf(
        "overall tx congestion start: total_tx_ui_pdu=%d, total_tx_i_pdu=%d",
        llcp_cb.total_tx_ui_pdu, llcp_cb.total_tx_i_pdu);

    data.congest.event = LLCP_SAP_EVT_CONGEST;
    data.congest.is_congested = true;

    /* notify logical data link congestion status */
    data.congest.remote_sap = LLCP_INVALID_SAP;
    data.congest.link_type = LLCP_LINK_TYPE_LOGICAL_DATA_LINK;

    for (sap = LLCP_SAP_SDP + 1; sap < LLCP_NUM_SAPS; sap++) {
      p_app_cb = llcp_util_get_app_cb(sap);

      if ((p_app_cb) && (p_app_cb->p_app_cback) &&
          (p_app_cb->link_type & LLCP_LINK_TYPE_LOGICAL_DATA_LINK)) {
        /* if already congested then no need to notify again */
        if (!p_app_cb->is_ui_tx_congested) {
          p_app_cb->is_ui_tx_congested = true;

          LOG(WARNING) << StringPrintf(
              "Logical link (SAP=0x%X) congestion start: count=%d", sap,
              p_app_cb->ui_xmit_q.count);

          data.congest.local_sap = sap;
          p_app_cb->p_app_cback(&data);
        }
      }
    }

    /* notify data link connection congestion status */
    data.congest.link_type = LLCP_LINK_TYPE_DATA_LINK_CONNECTION;

    for (idx = 0; idx < LLCP_MAX_DATA_LINK; idx++) {
      if ((llcp_cb.dlcb[idx].state == LLCP_DLC_STATE_CONNECTED) &&
          (llcp_cb.dlcb[idx].remote_busy == false) &&
          (llcp_cb.dlcb[idx].is_tx_congested == false)) {
        llcp_cb.dlcb[idx].is_tx_congested = true;

        LOG(WARNING) << StringPrintf(
            "Data link (SSAP:DSAP=0x%X:0x%X) congestion start: count=%d",
            llcp_cb.dlcb[idx].local_sap, llcp_cb.dlcb[idx].remote_sap,
            llcp_cb.dlcb[idx].i_xmit_q.count);

        data.congest.local_sap = llcp_cb.dlcb[idx].local_sap;
        data.congest.remote_sap = llcp_cb.dlcb[idx].remote_sap;

        (*llcp_cb.dlcb[idx].p_app_cb->p_app_cback)(&data);
      }
    }
  }
}

/*******************************************************************************
**
** Function         llcp_link_check_uncongested
**
** Description      Check overall congestion status, logical data link and
**                  data link connection congestion status
**                  Notify to each upper layer if uncongested
**
** Returns          void
**
*******************************************************************************/
static void llcp_link_check_uncongested(void) {
  tLLCP_SAP_CBACK_DATA data;
  tLLCP_APP_CB* p_app_cb;
  uint8_t xx, sap, idx;

  if (llcp_cb.overall_tx_congested) {
    if (llcp_cb.total_tx_ui_pdu + llcp_cb.total_tx_i_pdu <=
        llcp_cb.max_num_tx_buff / 2) {
      /* overall congestion is cleared */
      llcp_cb.overall_tx_congested = false;

      LOG(WARNING) << StringPrintf(
          "overall tx congestion end: total_tx_ui_pdu=%d, total_tx_i_pdu=%d",
          llcp_cb.total_tx_ui_pdu, llcp_cb.total_tx_i_pdu);
    } else {
      /* wait until more data packets are sent out */
      return;
    }
  }

  data.congest.event = LLCP_SAP_EVT_CONGEST;
  data.congest.is_congested = false;

  /* if total number of UI PDU is below threshold */
  if (llcp_cb.total_tx_ui_pdu < llcp_cb.max_num_ll_tx_buff) {
    /* check and notify logical data link congestion status */
    data.congest.remote_sap = LLCP_INVALID_SAP;
    data.congest.link_type = LLCP_LINK_TYPE_LOGICAL_DATA_LINK;

    /*
    ** start point of uncongested status notification is in round robin
    ** so each logical data link has equal chance of transmitting.
    */
    sap = llcp_cb.ll_tx_uncongest_ntf_start_sap;

    for (xx = LLCP_SAP_SDP + 1; xx < LLCP_NUM_SAPS; xx++) {
      /* no logical data link on LM and SDP */
      if (sap > LLCP_SAP_SDP) {
        p_app_cb = llcp_util_get_app_cb(sap);

        if ((p_app_cb) && (p_app_cb->p_app_cback) &&
            (p_app_cb->link_type & LLCP_LINK_TYPE_LOGICAL_DATA_LINK) &&
            (p_app_cb->is_ui_tx_congested) &&
            (p_app_cb->ui_xmit_q.count <= llcp_cb.ll_tx_congest_end)) {
          /* if it was congested but now tx queue count is below threshold */
          p_app_cb->is_ui_tx_congested = false;

          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "Logical link (SAP=0x%X) congestion end: count=%d", sap,
              p_app_cb->ui_xmit_q.count);

          data.congest.local_sap = sap;
          p_app_cb->p_app_cback(&data);
        }
      }

      sap = (sap + 1) % LLCP_NUM_SAPS;
    }

    /* move start point for next logical data link */
    for (xx = 0; xx < LLCP_NUM_SAPS; xx++) {
      sap = (llcp_cb.ll_tx_uncongest_ntf_start_sap + 1) % LLCP_NUM_SAPS;

      if (sap > LLCP_SAP_SDP) {
        p_app_cb = llcp_util_get_app_cb(sap);

        if ((p_app_cb) && (p_app_cb->p_app_cback) &&
            (p_app_cb->link_type & LLCP_LINK_TYPE_LOGICAL_DATA_LINK)) {
          llcp_cb.ll_tx_uncongest_ntf_start_sap = sap;
          break;
        }
      }
    }
  }

  /* notify data link connection congestion status */
  data.congest.link_type = LLCP_LINK_TYPE_DATA_LINK_CONNECTION;

  /*
  ** start point of uncongested status notification is in round robin
  ** so each data link connection has equal chance of transmitting.
  */
  idx = llcp_cb.dl_tx_uncongest_ntf_start_idx;

  for (xx = 0; xx < LLCP_MAX_DATA_LINK; xx++) {
    /* if it was congested but now tx queue is below threshold (receiving
     * window) */
    if ((llcp_cb.dlcb[idx].state == LLCP_DLC_STATE_CONNECTED) &&
        (llcp_cb.dlcb[idx].is_tx_congested) &&
        (llcp_cb.dlcb[idx].i_xmit_q.count <= llcp_cb.dlcb[idx].remote_rw / 2)) {
      llcp_cb.dlcb[idx].is_tx_congested = false;

      if (llcp_cb.dlcb[idx].remote_busy == false) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "Data link (SSAP:DSAP=0x%X:0x%X) congestion end: count=%d",
            llcp_cb.dlcb[idx].local_sap, llcp_cb.dlcb[idx].remote_sap,
            llcp_cb.dlcb[idx].i_xmit_q.count);

        data.congest.local_sap = llcp_cb.dlcb[idx].local_sap;
        data.congest.remote_sap = llcp_cb.dlcb[idx].remote_sap;

        (*llcp_cb.dlcb[idx].p_app_cb->p_app_cback)(&data);
      }
    }
    idx = (idx + 1) % LLCP_MAX_DATA_LINK;
  }

  /* move start point for next data link connection */
  for (xx = 0; xx < LLCP_MAX_DATA_LINK; xx++) {
    idx = (llcp_cb.dl_tx_uncongest_ntf_start_idx + 1) % LLCP_MAX_DATA_LINK;
    if (llcp_cb.dlcb[idx].state == LLCP_DLC_STATE_CONNECTED) {
      llcp_cb.dl_tx_uncongest_ntf_start_idx = idx;
      break;
    }
  }
}

/*******************************************************************************
**
** Function         llcp_link_send_SYMM
**
** Description      Send SYMM PDU
**
** Returns          void
**
*******************************************************************************/
static void llcp_link_send_SYMM(void) {
  NFC_HDR* p_msg;
  uint8_t* p;

  p_msg = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);

  if (p_msg) {
    p_msg->len = LLCP_PDU_SYMM_SIZE;
    p_msg->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;

    p = (uint8_t*)(p_msg + 1) + p_msg->offset;
    UINT16_TO_BE_STREAM(
        p, LLCP_GET_PDU_HEADER(LLCP_SAP_LM, LLCP_PDU_SYMM_TYPE, LLCP_SAP_LM));

    llcp_link_send_to_lower(p_msg);
  }
}

/*******************************************************************************
**
** Function         llcp_link_send_invalid_pdu
**
** Description      Send invalid LLC PDU in LLCP_LINK_STATE_ACTIVATION_FAILED
**
** Returns          void
**
*******************************************************************************/
static void llcp_link_send_invalid_pdu(void) {
  NFC_HDR* p_msg;
  uint8_t* p;

  p_msg = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);

  if (p_msg) {
    /* send one byte of 0x00 as invalid LLC PDU */
    p_msg->len = 1;
    p_msg->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;

    p = (uint8_t*)(p_msg + 1) + p_msg->offset;
    *p = 0x00;

    NFC_SendData(NFC_RF_CONN_ID, p_msg);
  }
}

/*******************************************************************************
**
** Function         llcp_link_check_send_data
**
** Description      Send PDU to peer
**
** Returns          void
**
*******************************************************************************/
void llcp_link_check_send_data(void) {
  NFC_HDR* p_pdu;

  /* don't re-enter while processing to prevent out of sequence */
  if (llcp_cb.lcb.is_sending_data)
    return;
  else
    llcp_cb.lcb.is_sending_data = true;

  /*
  ** check overall congestion due to high usage of buffer pool
  ** if congested then notify all of upper layers not to send any more data
  */
  llcp_link_check_congestion();

  if (llcp_cb.lcb.symm_state == LLCP_LINK_SYMM_LOCAL_XMIT_NEXT ||
      (appl_dta_mode_flag &&
       llcp_cb.lcb.link_state == LLCP_LINK_STATE_DEACTIVATING)) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "in state of "
        "LLCP_LINK_SYMM_LOCAL_XMIT_NEXT");

    p_pdu = llcp_link_build_next_pdu(nullptr);

    /*
    ** For data link connection,
    ** V(RA) was updated and N(R) was set to V(RA), if I PDU was added in
    ** this transmission. If there was no I PDU to carry V(RA) and V(RA) is
    ** not V(R) and it's not congested, then RR PDU will be sent.
    ** If there was no I PDU to carry V(RA) and V(RA) is not V(R) and it's
    ** congested, then RNR PDU will be sent.
    ** If local busy state has been changed then RR or RNR PDU may be sent.
    */
    llcp_dlc_check_to_send_rr_rnr();

    /* add RR/RNR PDU to be sent if any */
    p_pdu = llcp_link_build_next_pdu(p_pdu);

    if (p_pdu != nullptr) {
      llcp_link_send_to_lower(p_pdu);

      /* stop inactivity timer */
      llcp_link_stop_inactivity_timer();

      /* check congestion status after sending out some data */
      llcp_link_check_uncongested();
    } else {
      /* There is no data to send, so send SYMM */
      if (llcp_cb.lcb.link_state == LLCP_LINK_STATE_ACTIVATED) {
        if (llcp_cb.lcb.symm_delay > 0) {
          /* wait for application layer sending data */
          llcp_link_start_link_timer();
          llcp_cb.lcb.is_sending_data = false;
          return;
        } else {
          llcp_link_send_SYMM();

          /* start inactivity timer */
          if (llcp_cb.num_data_link_connection == 0) {
            llcp_link_start_inactivity_timer();
          }
        }
      } else {
        llcp_cb.lcb.is_sending_data = false;
        return;
      }
    }

    if (llcp_cb.lcb.link_state == LLCP_LINK_STATE_DEACTIVATING) {
      /* wait for short period for NFCC to send DISC */
      nfc_start_quick_timer(&llcp_cb.lcb.timer, NFC_TTYPE_LLCP_LINK_MANAGER,
                            ((uint32_t)50) * QUICK_TIMER_TICKS_PER_SEC / 1000);
    } else {
      /* wait for data to receive from remote */
      llcp_link_start_link_timer();
    }
  }

  llcp_cb.lcb.is_sending_data = false;
}

/*******************************************************************************
**
** Function         llcp_link_proc_ui_pdu
**
** Description      Process UI PDU from peer device
**
** Returns          None
**
*******************************************************************************/
static void llcp_link_proc_ui_pdu(uint8_t local_sap, uint8_t remote_sap,
                                  uint16_t ui_pdu_length, uint8_t* p_ui_pdu,
                                  NFC_HDR* p_msg) {
  bool appended;
  NFC_HDR* p_last_buf;
  uint16_t available_bytes;
  uint8_t* p_dst;
  tLLCP_APP_CB* p_app_cb;
  tLLCP_SAP_CBACK_DATA data;
  tLLCP_DLCB* p_dlcb;

  p_app_cb = llcp_util_get_app_cb(local_sap);
  /*if UI PDU sent to SAP with data link connection*/
  p_dlcb = llcp_dlc_find_dlcb_by_sap(local_sap, remote_sap);
  if (p_dlcb) {
    llcp_util_send_frmr(p_dlcb, LLCP_FRMR_W_ERROR_FLAG, LLCP_PDU_UI_TYPE, 0);
    llcp_dlsm_execute(p_dlcb, LLCP_DLC_EVENT_FRAME_ERROR, nullptr);
    if (p_msg) {
      GKI_freebuf(p_msg);
    }
    return;
  }

  /* if application is registered and expecting UI PDU on logical data link */
  if ((p_app_cb) && (p_app_cb->p_app_cback) &&
      (p_app_cb->link_type & LLCP_LINK_TYPE_LOGICAL_DATA_LINK)) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "Local SAP:0x%x, Remote SAP:0x%x", local_sap, remote_sap);

    /* if this is not from AGF PDU */
    if (p_msg) {
      ui_pdu_length = p_msg->len; /* including LLCP header */
      p_ui_pdu = (uint8_t*)(p_msg + 1) + p_msg->offset;
    }

    appended = false;

    /* get last buffer in rx queue */
    p_last_buf = (NFC_HDR*)GKI_getlast(&p_app_cb->ui_rx_q);

    if (p_last_buf) {
      /* get max length to append at the end of buffer */
      available_bytes = GKI_get_buf_size(p_last_buf) - NFC_HDR_SIZE -
                        p_last_buf->offset - p_last_buf->len;

      /* if new UI PDU with length can be attached at the end of buffer */
      if (available_bytes >= LLCP_PDU_AGF_LEN_SIZE + ui_pdu_length) {
        p_dst =
            (uint8_t*)(p_last_buf + 1) + p_last_buf->offset + p_last_buf->len;

        /* add length of UI PDU */
        UINT16_TO_BE_STREAM(p_dst, ui_pdu_length);

        /* copy UI PDU with LLCP header */
        memcpy(p_dst, p_ui_pdu, ui_pdu_length);

        p_last_buf->len += LLCP_PDU_AGF_LEN_SIZE + ui_pdu_length;

        if (p_msg) GKI_freebuf(p_msg);

        appended = true;
      }
    }

    /* if it is not available to append */
    if (!appended) {
      /* if it's not from AGF PDU */
      if (p_msg) {
        /* add length of PDU in front of UI PDU (reuse room for NCI header) */
        p_ui_pdu -= LLCP_PDU_AGF_LEN_SIZE;
        UINT16_TO_BE_STREAM(p_ui_pdu, ui_pdu_length);

        p_msg->offset -= LLCP_PDU_AGF_LEN_SIZE;
        p_msg->len += LLCP_PDU_AGF_LEN_SIZE;
        p_msg->layer_specific = 0;
      } else {
        p_msg = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);

        if (p_msg) {
          p_dst = (uint8_t*)(p_msg + 1);

          /* add length of PDU in front of UI PDU */
          UINT16_TO_BE_STREAM(p_dst, ui_pdu_length);

          memcpy(p_dst, p_ui_pdu, ui_pdu_length);

          p_msg->offset = 0;
          p_msg->len = LLCP_PDU_AGF_LEN_SIZE + ui_pdu_length;
          p_msg->layer_specific = 0;
        } else {
          LOG(ERROR) << StringPrintf("out of buffer");
        }
      }

      /* insert UI PDU in rx queue */
      if (p_msg) {
        GKI_enqueue(&p_app_cb->ui_rx_q, p_msg);
        llcp_cb.total_rx_ui_pdu++;
      }
    }

    if (p_app_cb->ui_rx_q.count > llcp_cb.ll_rx_congest_start) {
      LOG(WARNING) << StringPrintf(
          "SAP:0x%x, rx link is congested (%d), "
          "discard oldest UI PDU",
          local_sap, p_app_cb->ui_rx_q.count);

      GKI_freebuf(GKI_dequeue(&p_app_cb->ui_rx_q));
      llcp_cb.total_rx_ui_pdu--;
    }

    if ((p_app_cb->ui_rx_q.count == 1) && (appended == false)) {
      data.data_ind.event = LLCP_SAP_EVT_DATA_IND;
      data.data_ind.local_sap = local_sap;
      data.data_ind.remote_sap = remote_sap;
      data.data_ind.link_type = LLCP_LINK_TYPE_LOGICAL_DATA_LINK;
      (*p_app_cb->p_app_cback)(&data);
    }
  } else {
    LOG(ERROR) << StringPrintf("Unregistered SAP:0x%x", local_sap);

    if (p_msg) {
      GKI_freebuf(p_msg);
    }
  }
}

/*******************************************************************************
**
** Function         llcp_link_proc_agf_pdu
**
** Description      Process AGF PDU from peer device
**
** Returns          void
**
*******************************************************************************/
static void llcp_link_proc_agf_pdu(NFC_HDR* p_agf) {
  uint16_t agf_length;
  uint8_t *p, *p_info, *p_pdu_length;
  uint16_t pdu_hdr, pdu_length, pdu_num;
  uint8_t dsap, ptype, ssap;

  p_agf->len -= LLCP_PDU_HEADER_SIZE;
  p_agf->offset += LLCP_PDU_HEADER_SIZE;

  /*
  ** check integrity of AGF PDU and get number of PDUs in AGF PDU
  */
  agf_length = p_agf->len;
  p = (uint8_t*)(p_agf + 1) + p_agf->offset;
  pdu_num = 0;

  while (agf_length > 0) {
    if (agf_length > LLCP_PDU_AGF_LEN_SIZE) {
      BE_STREAM_TO_UINT16(pdu_length, p);
      if (pdu_length < LLCP_PDU_HEADER_SIZE) {
        LOG(ERROR) << StringPrintf("Received invalid encapsulated PDU");
        break;
      }
      agf_length -= LLCP_PDU_AGF_LEN_SIZE;
    } else {
      break;
    }

    if (pdu_length <= agf_length) {
      p += pdu_length;
      agf_length -= pdu_length;
      pdu_num++;
    } else {
      break;
    }
  }

  if (agf_length != 0 || pdu_num < 2) {
    android_errorWriteLog(0x534e4554, "116791157");
    LOG(ERROR) << StringPrintf("Received invalid AGF PDU");
    GKI_freebuf(p_agf);
    return;
  }

  /*
  ** Process PDUs in AGF
  */
  agf_length = p_agf->len;
  p = (uint8_t*)(p_agf + 1) + p_agf->offset;

  while (agf_length >= (LLCP_PDU_HEADER_SIZE + LLCP_PDU_AGF_LEN_SIZE)) {
    /* get length of PDU */
    p_pdu_length = p;
    BE_STREAM_TO_UINT16(pdu_length, p);
    agf_length -= LLCP_PDU_AGF_LEN_SIZE;

    /* get DSAP/PTYPE/SSAP */
    p_info = p;
    BE_STREAM_TO_UINT16(pdu_hdr, p_info);

    dsap = LLCP_GET_DSAP(pdu_hdr);
    ptype = (uint8_t)(LLCP_GET_PTYPE(pdu_hdr));
    ssap = LLCP_GET_SSAP(pdu_hdr);

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "Rx DSAP:0x%x, PTYPE:%s (0x%x), SSAP:0x%x "
        "in AGF",
        dsap, llcp_pdu_type(ptype).c_str(), ptype, ssap);

    if ((ptype == LLCP_PDU_DISC_TYPE) && (dsap == LLCP_SAP_LM) &&
        (ssap == LLCP_SAP_LM)) {
      GKI_freebuf(p_agf);
      llcp_link_deactivate(LLCP_LINK_REMOTE_INITIATED);
      return;
    } else if (ptype == LLCP_PDU_AGF_TYPE) {
      LOG(ERROR) << StringPrintf("AGF PDU shall not be in AGF");
    } else if (ptype == LLCP_PDU_SYMM_TYPE) {
      LOG(ERROR) << StringPrintf("SYMM PDU exchange shall not be in AGF");
    } else if (ptype == LLCP_PDU_PAX_TYPE) {
      LOG(ERROR) << StringPrintf("PAX PDU exchange shall not be used");
    } else if (ptype == LLCP_PDU_SNL_TYPE) {
      llcp_sdp_proc_snl((uint16_t)(pdu_length - LLCP_PDU_HEADER_SIZE), p_info);
    } else if ((ptype == LLCP_PDU_UI_TYPE) &&
               (pdu_length > LLCP_PDU_HEADER_SIZE)) {
      llcp_link_proc_ui_pdu(dsap, ssap, pdu_length, p, nullptr);
    } else if (ptype == LLCP_PDU_I_TYPE) {
      llcp_dlc_proc_i_pdu(dsap, ssap, pdu_length, p, nullptr);
    } else /* let data link connection handle PDU */
    {
      llcp_dlc_proc_rx_pdu(dsap, ptype, ssap,
                           (uint16_t)(pdu_length - LLCP_PDU_HEADER_SIZE),
                           p_info);
    }

    p += pdu_length;
    agf_length -= pdu_length;
  }

  GKI_freebuf(p_agf);
}

/*******************************************************************************
**
** Function         llcp_link_proc_rx_pdu
**
** Description      Process received PDU from peer device
**
** Returns          void
**
*******************************************************************************/
static void llcp_link_proc_rx_pdu(uint8_t dsap, uint8_t ptype, uint8_t ssap,
                                  NFC_HDR* p_msg) {
  bool free_buffer = true;
  uint8_t* p_data;

  switch (ptype) {
    case LLCP_PDU_PAX_TYPE:
      LOG(ERROR) << StringPrintf("; PAX PDU exchange shall not be used");
      break;

    case LLCP_PDU_DISC_TYPE:
      if ((dsap == LLCP_SAP_LM) && (ssap == LLCP_SAP_LM)) {
        llcp_link_deactivate(LLCP_LINK_REMOTE_INITIATED);
      } else {
        p_data = (uint8_t*)(p_msg + 1) + p_msg->offset + LLCP_PDU_HEADER_SIZE;
        llcp_dlc_proc_rx_pdu(dsap, ptype, ssap,
                             (uint16_t)(p_msg->len - LLCP_PDU_HEADER_SIZE),
                             p_data);
      }
      break;

    case LLCP_PDU_SNL_TYPE:
      p_data = (uint8_t*)(p_msg + 1) + p_msg->offset + LLCP_PDU_HEADER_SIZE;
      llcp_sdp_proc_snl((uint16_t)(p_msg->len - LLCP_PDU_HEADER_SIZE), p_data);
      break;

    case LLCP_PDU_AGF_TYPE:
      llcp_link_proc_agf_pdu(p_msg);
      free_buffer = false;
      break;

    case LLCP_PDU_UI_TYPE:
      llcp_link_proc_ui_pdu(dsap, ssap, 0, nullptr, p_msg);
      free_buffer = false;
      break;

    case LLCP_PDU_I_TYPE:
      llcp_dlc_proc_i_pdu(dsap, ssap, 0, nullptr, p_msg);
      free_buffer = false;
      break;

    default:
      p_data = (uint8_t*)(p_msg + 1) + p_msg->offset + LLCP_PDU_HEADER_SIZE;
      llcp_dlc_proc_rx_pdu(dsap, ptype, ssap,
                           (uint16_t)(p_msg->len - LLCP_PDU_HEADER_SIZE),
                           p_data);
      break;
  }

  if (free_buffer) GKI_freebuf(p_msg);
}

/*******************************************************************************
**
** Function         llcp_link_proc_rx_data
**
** Description      Process received data from NFCC and maintain symmetry state
**
** Returns          void
**
*******************************************************************************/
static void llcp_link_proc_rx_data(NFC_HDR* p_msg) {
  uint8_t* p;
  uint16_t pdu_hdr, info_length = 0;
  uint8_t dsap, ptype, ssap;
  bool free_buffer = true;
  bool frame_error = false;

  if (llcp_cb.lcb.symm_state == LLCP_LINK_SYMM_REMOTE_XMIT_NEXT) {
    llcp_link_stop_link_timer();

    if (llcp_cb.lcb.received_first_packet == false) {
      llcp_cb.lcb.received_first_packet = true;
      (*llcp_cb.lcb.p_link_cback)(LLCP_LINK_FIRST_PACKET_RECEIVED_EVT,
                                  LLCP_LINK_SUCCESS);
    }
    if ((llcp_cb.lcb.link_state == LLCP_LINK_STATE_DEACTIVATING) &&
        (llcp_cb.lcb.sig_xmit_q.count == 0)) {
      /* this indicates that DISC PDU had been sent out to peer */
      /* initiator may wait for SYMM PDU */
      if (appl_dta_mode_flag == 0x01)
        llcp_util_send_disc(LLCP_SAP_LM, LLCP_SAP_LM);
      else
        llcp_link_process_link_timeout();
    } else {
      if (p_msg->len < LLCP_PDU_HEADER_SIZE) {
        LOG(ERROR) << StringPrintf("Received too small PDU: got %d bytes",
                                   p_msg->len);
        frame_error = true;
      } else {
        p = (uint8_t*)(p_msg + 1) + p_msg->offset;
        BE_STREAM_TO_UINT16(pdu_hdr, p);

        dsap = LLCP_GET_DSAP(pdu_hdr);
        ptype = (uint8_t)(LLCP_GET_PTYPE(pdu_hdr));
        ssap = LLCP_GET_SSAP(pdu_hdr);

        /* get length of information per PDU type */
        if ((ptype == LLCP_PDU_I_TYPE) || (ptype == LLCP_PDU_RR_TYPE) ||
            (ptype == LLCP_PDU_RNR_TYPE)) {
          if (p_msg->len >= LLCP_PDU_HEADER_SIZE + LLCP_SEQUENCE_SIZE) {
            info_length =
                p_msg->len - LLCP_PDU_HEADER_SIZE - LLCP_SEQUENCE_SIZE;
          } else {
            LOG(ERROR) << StringPrintf(
                "Received I/RR/RNR PDU without sequence");
            frame_error = true;
          }
        } else {
          info_length = p_msg->len - LLCP_PDU_HEADER_SIZE;
        }

        /* check if length of information is bigger than link MIU */
        if ((!frame_error) && (info_length > llcp_cb.lcb.local_link_miu)) {
          LOG(ERROR) << StringPrintf(
              "Received exceeding MIU (%d): got %d bytes SDU",
              llcp_cb.lcb.local_link_miu, info_length);

          frame_error = true;
        } else {
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "DSAP:0x%x, PTYPE:%s (0x%x), "
              "SSAP:0x%x",
              dsap, llcp_pdu_type(ptype).c_str(), ptype, ssap);

          if (ptype == LLCP_PDU_SYMM_TYPE) {
            if (info_length > 0) {
              LOG(ERROR) << StringPrintf(
                  "Received extra data (%d bytes) in SYMM PDU", info_length);
              frame_error = true;
            }
          } else {
            /* received other than SYMM */
            llcp_link_stop_inactivity_timer();

            llcp_link_proc_rx_pdu(dsap, ptype, ssap, p_msg);
            free_buffer = false;
          }
        }
      }

      llcp_cb.lcb.symm_state = LLCP_LINK_SYMM_LOCAL_XMIT_NEXT;

      /* check if any pending packet */
      llcp_link_check_send_data();
    }
  } else {
    LOG(ERROR) << StringPrintf("Received PDU in state of SYMM_MUST_XMIT_NEXT");
  }

  if (free_buffer) GKI_freebuf(p_msg);
}

/*******************************************************************************
**
** Function         llcp_link_get_next_pdu
**
** Description      Get next PDU from link manager or data links w/wo dequeue
**
** Returns          pointer of a PDU to send if length_only is FALSE
**                  NULL otherwise
**
*******************************************************************************/
static NFC_HDR* llcp_link_get_next_pdu(bool length_only,
                                       uint16_t* p_next_pdu_length) {
  NFC_HDR* p_msg;
  int count, xx;
  tLLCP_APP_CB* p_app_cb;

  /* processing signalling PDU first */
  if (llcp_cb.lcb.sig_xmit_q.p_first) {
    if (length_only) {
      p_msg = (NFC_HDR*)llcp_cb.lcb.sig_xmit_q.p_first;
      *p_next_pdu_length = p_msg->len;
      return nullptr;
    } else
      p_msg = (NFC_HDR*)GKI_dequeue(&llcp_cb.lcb.sig_xmit_q);

    return p_msg;
  } else {
    /* transmitting logical data link and data link connection equaly */
    for (xx = 0; xx < 2; xx++) {
      if (!llcp_cb.lcb.ll_served) {
        /* Get one from logical link connection */
        for (count = 0; count < LLCP_NUM_SAPS; count++) {
          /* round robin schedule without priority  */
          p_app_cb = llcp_util_get_app_cb(llcp_cb.lcb.ll_idx);

          if ((p_app_cb) && (p_app_cb->p_app_cback) &&
              (p_app_cb->ui_xmit_q.count)) {
            if (length_only) {
              /* don't alternate next data link to return the same length of PDU
               */
              p_msg = (NFC_HDR*)p_app_cb->ui_xmit_q.p_first;
              *p_next_pdu_length = p_msg->len;
              return nullptr;
            } else {
              /* check data link connection first in next time */
              llcp_cb.lcb.ll_served = !llcp_cb.lcb.ll_served;

              p_msg = (NFC_HDR*)GKI_dequeue(&p_app_cb->ui_xmit_q);
              llcp_cb.total_tx_ui_pdu--;

              /* this logical link has been served, so start from next logical
               * link next time */
              llcp_cb.lcb.ll_idx = (llcp_cb.lcb.ll_idx + 1) % LLCP_NUM_SAPS;

              return p_msg;
            }
          } else {
            /* check next logical link connection */
            llcp_cb.lcb.ll_idx = (llcp_cb.lcb.ll_idx + 1) % LLCP_NUM_SAPS;
          }
        }

        /* no data, so check data link connection if not checked yet */
        llcp_cb.lcb.ll_served = !llcp_cb.lcb.ll_served;
      } else {
        /* Get one from data link connection */
        for (count = 0; count < LLCP_MAX_DATA_LINK; count++) {
          /* round robin schedule without priority  */
          if (llcp_cb.dlcb[llcp_cb.lcb.dl_idx].state != LLCP_DLC_STATE_IDLE) {
            if (length_only) {
              *p_next_pdu_length = llcp_dlc_get_next_pdu_length(
                  &llcp_cb.dlcb[llcp_cb.lcb.dl_idx]);

              if (*p_next_pdu_length > 0) {
                /* don't change data link connection to return the same length
                 * of PDU */
                return nullptr;
              } else {
                /* no data, so check next data link connection */
                llcp_cb.lcb.dl_idx =
                    (llcp_cb.lcb.dl_idx + 1) % LLCP_MAX_DATA_LINK;
              }
            } else {
              p_msg = llcp_dlc_get_next_pdu(&llcp_cb.dlcb[llcp_cb.lcb.dl_idx]);

              /* this data link has been served, so start from next data link
               * next time */
              llcp_cb.lcb.dl_idx =
                  (llcp_cb.lcb.dl_idx + 1) % LLCP_MAX_DATA_LINK;

              if (p_msg) {
                /* serve logical data link next time */
                llcp_cb.lcb.ll_served = !llcp_cb.lcb.ll_served;
                return p_msg;
              }
            }
          } else {
            /* check next data link connection */
            llcp_cb.lcb.dl_idx = (llcp_cb.lcb.dl_idx + 1) % LLCP_MAX_DATA_LINK;
          }
        }

        /* if all of data link connection doesn't have data to send */
        if (count >= LLCP_MAX_DATA_LINK) {
          llcp_cb.lcb.ll_served = !llcp_cb.lcb.ll_served;
        }
      }
    }
  }

  /* nothing to send */
  *p_next_pdu_length = 0;
  return nullptr;
}

/*******************************************************************************
**
** Function         llcp_link_build_next_pdu
**
** Description      Build a PDU from Link Manager and Data Link
**                  Perform aggregation procedure if necessary
**
** Returns          NFC_HDR* if sent any PDU
**
*******************************************************************************/
static NFC_HDR* llcp_link_build_next_pdu(NFC_HDR* p_pdu) {
  NFC_HDR *p_agf = nullptr, *p_msg = nullptr, *p_next_pdu;
  uint8_t *p, ptype;
  uint16_t next_pdu_length, pdu_hdr;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* add any pending SNL PDU into sig_xmit_q for transmitting */
  llcp_sdp_check_send_snl();

  if (p_pdu) {
    /* get PDU type */
    p = (uint8_t*)(p_pdu + 1) + p_pdu->offset;
    BE_STREAM_TO_UINT16(pdu_hdr, p);

    ptype = (uint8_t)(LLCP_GET_PTYPE(pdu_hdr));

    if (ptype == LLCP_PDU_AGF_TYPE) {
      /* add more PDU into this AGF PDU */
      p_agf = p_pdu;
    } else {
      p_msg = p_pdu;
    }
  } else {
    /* Get a PDU from link manager or data links */
    p_msg = llcp_link_get_next_pdu(false, &next_pdu_length);

    if (!p_msg) {
      return nullptr;
    }
  }

  /* Get length of next PDU from link manager or data links without dequeue */
  llcp_link_get_next_pdu(true, &next_pdu_length);
  while (next_pdu_length > 0) {
    /* if it's first visit */
    if (!p_agf) {
      /* if next PDU fits into MIU, allocate AGF PDU and copy the first PDU */
      if (2 + p_msg->len + 2 + next_pdu_length <= llcp_cb.lcb.effective_miu) {
        p_agf = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);
        if (p_agf) {
          p_agf->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;

          p = (uint8_t*)(p_agf + 1) + p_agf->offset;

          UINT16_TO_BE_STREAM(
              p,
              LLCP_GET_PDU_HEADER(LLCP_SAP_LM, LLCP_PDU_AGF_TYPE, LLCP_SAP_LM));
          UINT16_TO_BE_STREAM(p, p_msg->len);
          memcpy(p, (uint8_t*)(p_msg + 1) + p_msg->offset, p_msg->len);

          p_agf->len = LLCP_PDU_HEADER_SIZE + 2 + p_msg->len;

          GKI_freebuf(p_msg);
          p_msg = p_agf;
        } else {
          LOG(ERROR) << StringPrintf("Out of buffer");
          return p_msg;
        }
      } else {
        break;
      }
    }

    /* if next PDU fits into MIU, copy the next PDU into AGF */
    if (p_agf->len - LLCP_PDU_HEADER_SIZE + 2 + next_pdu_length <=
        llcp_cb.lcb.effective_miu) {
      /* Get a next PDU from link manager or data links */
      p_next_pdu = llcp_link_get_next_pdu(false, &next_pdu_length);

      p = (uint8_t*)(p_agf + 1) + p_agf->offset + p_agf->len;

      UINT16_TO_BE_STREAM(p, p_next_pdu->len);
      memcpy(p, (uint8_t*)(p_next_pdu + 1) + p_next_pdu->offset,
             p_next_pdu->len);

      p_agf->len += 2 + p_next_pdu->len;

      GKI_freebuf(p_next_pdu);

      /* Get next PDU length from link manager or data links without dequeue */
      llcp_link_get_next_pdu(true, &next_pdu_length);
    } else {
      break;
    }
  }

  if (p_agf)
    return p_agf;
  else
    return p_msg;
}

/*******************************************************************************
**
** Function         llcp_link_send_to_lower
**
** Description      Send PDU to lower layer
**
** Returns          void
**
*******************************************************************************/
static void llcp_link_send_to_lower(NFC_HDR* p_pdu) {
  llcp_cb.lcb.symm_state = LLCP_LINK_SYMM_REMOTE_XMIT_NEXT;
  NFC_SendData(NFC_RF_CONN_ID, p_pdu);
}

/*******************************************************************************
**
** Function         llcp_link_connection_cback
**
** Description      processing incoming data
**
** Returns          void
**
*******************************************************************************/
void llcp_link_connection_cback(__attribute__((unused)) uint8_t conn_id,
                                tNFC_CONN_EVT event, tNFC_CONN* p_data) {
  if (event == NFC_DATA_CEVT) {
    if (llcp_cb.lcb.link_state == LLCP_LINK_STATE_DEACTIVATED) {
      /* respoding SYMM while LLCP is deactivated but RF link is not deactivated
       * yet */
      llcp_link_send_SYMM();
      GKI_freebuf((NFC_HDR*)p_data->data.p_data);
    } else if (llcp_cb.lcb.link_state == LLCP_LINK_STATE_ACTIVATION_FAILED) {
      /* respoding with invalid LLC PDU until initiator deactivates RF link
      *after LLCP activation was failed,
      ** so that initiator knows LLCP link activation was failed.
      */
      llcp_link_send_invalid_pdu();
      GKI_freebuf((NFC_HDR*)p_data->data.p_data);
    } else {
      llcp_cb.lcb.flags |= LLCP_LINK_FLAGS_RX_ANY_LLC_PDU;
      llcp_link_proc_rx_data((NFC_HDR*)p_data->data.p_data);
    }
  } else if (event == NFC_ERROR_CEVT) {
    /* RF interface specific status code */
    llcp_link_deactivate(*(uint8_t*)p_data);
  } else if (event == NFC_DEACTIVATE_CEVT) {
    if ((llcp_cb.lcb.link_state == LLCP_LINK_STATE_DEACTIVATING) &&
        (!llcp_cb.lcb.is_initiator)) {
      /* peer initiates NFC link deactivation before timeout */
      llcp_link_stop_link_timer();
      llcp_link_process_link_timeout();
    } else if (llcp_cb.lcb.link_state == LLCP_LINK_STATE_ACTIVATION_FAILED) {
      /* do not notify to upper layer because activation failure was already
       * notified */
      NFC_FlushData(NFC_RF_CONN_ID);
      llcp_cb.lcb.link_state = LLCP_LINK_STATE_DEACTIVATED;
    } else if (llcp_cb.lcb.link_state != LLCP_LINK_STATE_DEACTIVATED) {
      llcp_link_deactivate(LLCP_LINK_RF_LINK_LOSS_ERR);
    }

    NFC_SetStaticRfCback(nullptr);
  } else if (event == NFC_DATA_START_CEVT) {
    if (llcp_cb.lcb.symm_state == LLCP_LINK_SYMM_REMOTE_XMIT_NEXT) {
      /* LLCP shall stop LTO timer when receiving the first bit of LLC PDU */
      llcp_link_stop_link_timer();
    }
  }

  /* LLCP ignores the following events

      NFC_CONN_CREATE_CEVT
      NFC_CONN_CLOSE_CEVT
  */
}

/*******************************************************************************
**
** Function         llcp_pdu_type
**
** Description
**
** Returns          string of PDU type
**
*******************************************************************************/
static std::string llcp_pdu_type(uint8_t ptype) {
  switch (ptype) {
    case LLCP_PDU_SYMM_TYPE:
      return "SYMM";
    case LLCP_PDU_PAX_TYPE:
      return "PAX";
    case LLCP_PDU_AGF_TYPE:
      return "AGF";
    case LLCP_PDU_UI_TYPE:
      return "UI";
    case LLCP_PDU_CONNECT_TYPE:
      return "CONNECT";
    case LLCP_PDU_DISC_TYPE:
      return "DISC";
    case LLCP_PDU_CC_TYPE:
      return "CC";
    case LLCP_PDU_DM_TYPE:
      return "DM";
    case LLCP_PDU_FRMR_TYPE:
      return "FRMR";
    case LLCP_PDU_SNL_TYPE:
      return "SNL";
    case LLCP_PDU_I_TYPE:
      return "I";
    case LLCP_PDU_RR_TYPE:
      return "RR";
    case LLCP_PDU_RNR_TYPE:
      return "RNR";
    default:
      return "RESERVED";
  }
}
