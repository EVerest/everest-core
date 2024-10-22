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
#include "nci_hmsgs.h"
#include "nfc_api.h"
#include "rw_api.h"
#include "rw_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

tRW_CB rw_cb;

/*******************************************************************************
*******************************************************************************/
void rw_init(void) {
  memset(&rw_cb, 0, sizeof(tRW_CB));
}

#if (RW_STATS_INCLUDED == TRUE)
/*******************************************************************************
* Internal functions for statistics
*******************************************************************************/
/*******************************************************************************
**
** Function         rw_main_reset_stats
**
** Description      Reset counters for statistics
**
** Returns          void
**
*******************************************************************************/
void rw_main_reset_stats(void) {
  memset(&rw_cb.stats, 0, sizeof(tRW_STATS));

  /* Get current tick count */
  rw_cb.stats.start_tick = GKI_get_tick_count();
}

/*******************************************************************************
**
** Function         rw_main_update_tx_stats
**
** Description      Update stats for tx
**
** Returns          void
**
*******************************************************************************/
void rw_main_update_tx_stats(uint32_t num_bytes, bool is_retry) {
  rw_cb.stats.bytes_sent += num_bytes;
  rw_cb.stats.num_ops++;

  if (is_retry) rw_cb.stats.num_retries++;
}

/*******************************************************************************
**
** Function         rw_main_update_fail_stats
**
** Description      Increment failure count
**
** Returns          void
**
*******************************************************************************/
void rw_main_update_fail_stats(void) { rw_cb.stats.num_fail++; }

/*******************************************************************************
**
** Function         rw_main_update_crc_error_stats
**
** Description      Increment crc error count
**
** Returns          void
**
*******************************************************************************/
void rw_main_update_crc_error_stats(void) { rw_cb.stats.num_crc++; }

/*******************************************************************************
**
** Function         rw_main_update_trans_error_stats
**
** Description      Increment trans error count
**
** Returns          void
**
*******************************************************************************/
void rw_main_update_trans_error_stats(void) { rw_cb.stats.num_trans_err++; }

/*******************************************************************************
**
** Function         rw_main_update_rx_stats
**
** Description      Update stats for rx
**
** Returns          void
**
*******************************************************************************/
void rw_main_update_rx_stats(uint32_t num_bytes) {
  rw_cb.stats.bytes_received += num_bytes;
}

/*******************************************************************************
**
** Function         rw_main_log_stats
**
** Description      Dump stats
**
** Returns          void
**
*******************************************************************************/
void rw_main_log_stats(void) {
  uint32_t ticks, elapsed_ms;

  ticks = GKI_get_tick_count() - rw_cb.stats.start_tick;
  elapsed_ms = GKI_TICKS_TO_MS(ticks);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "NFC tx stats: cmds:%i, retries:%i, aborted: %i, tx_errs: %i, bytes "
      "sent:%i",
      rw_cb.stats.num_ops, rw_cb.stats.num_retries, rw_cb.stats.num_fail,
      rw_cb.stats.num_trans_err, rw_cb.stats.bytes_sent);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("    rx stats: rx-crc errors %i, bytes received: %i",
                      rw_cb.stats.num_crc, rw_cb.stats.bytes_received);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("    time activated %i ms", elapsed_ms);
}
#endif /* RW_STATS_INCLUDED */

/*******************************************************************************
**
** Function         RW_SendRawFrame
**
** Description      This function sends a raw frame to the peer device.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS RW_SendRawFrame(uint8_t* p_raw_data, uint16_t data_len) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  NFC_HDR* p_data;
  uint8_t* p;

  if (rw_cb.p_cback) {
    /* a valid opcode for RW - remove */
    p_data = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
    if (p_data) {
      p_data->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
      p = (uint8_t*)(p_data + 1) + p_data->offset;
      memcpy(p, p_raw_data, data_len);
      p_data->len = data_len;

      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("RW SENT raw frame (0x%x)", data_len);
      status = NFC_SendData(NFC_RF_CONN_ID, p_data);
    }
  }
  return status;
}

/*******************************************************************************
**
** Function         RW_SetActivatedTagType
**
** Description      This function selects the tag type for Reader/Writer mode.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS RW_SetActivatedTagType(tNFC_ACTIVATE_DEVT* p_activate_params,
                                   tRW_CBACK* p_cback) {
  tNFC_STATUS status = NFC_STATUS_FAILED;

  /* check for null cback here / remove checks from rw_t?t */
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "RW_SetActivatedTagType protocol:%d, technology:%d, SAK:%d",
      p_activate_params->protocol, p_activate_params->rf_tech_param.mode,
      p_activate_params->rf_tech_param.param.pa.sel_rsp);

  if (p_cback == nullptr) {
    LOG(ERROR) << StringPrintf(
        "RW_SetActivatedTagType called with NULL callback");
    return (NFC_STATUS_FAILED);
  }

  /* Reset tag-specific area of control block */
  memset(&rw_cb.tcb, 0, sizeof(tRW_TCB));

#if (RW_STATS_INCLUDED == TRUE)
  /* Reset RW stats */
  rw_main_reset_stats();
#endif /* RW_STATS_INCLUDED */

  rw_cb.p_cback = p_cback;
  /* not a tag NFC_PROTOCOL_NFCIP1:   NFCDEP/LLCP - NFC-A or NFC-F */
  if (NFC_PROTOCOL_T1T == p_activate_params->protocol) {
    /* Type1Tag    - NFC-A */
    if (p_activate_params->rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) {
      status = rw_t1t_select(p_activate_params->rf_tech_param.param.pa.hr,
                             p_activate_params->rf_tech_param.param.pa.nfcid1);
    }
  } else if (NFC_PROTOCOL_T2T == p_activate_params->protocol) {
    /* Type2Tag    - NFC-A */
    if (p_activate_params->rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) {
      if (p_activate_params->rf_tech_param.param.pa.sel_rsp ==
          NFC_SEL_RES_NFC_FORUM_T2T)
        status = rw_t2t_select();
    }
  } else if (NFC_PROTOCOL_T3T == p_activate_params->protocol) {
    /* Type3Tag    - NFC-F */
    if (p_activate_params->rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_F) {
      status =
          rw_t3t_select(p_activate_params->rf_tech_param.param.pf.nfcid2,
                        p_activate_params->rf_tech_param.param.pf.mrti_check,
                        p_activate_params->rf_tech_param.param.pf.mrti_update);
    }
  } else if (NFC_PROTOCOL_ISO_DEP == p_activate_params->protocol) {
    /* ISODEP/4A,4B- NFC-A or NFC-B */
    if ((p_activate_params->rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_B) ||
        (p_activate_params->rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A)) {
      status = rw_t4t_select();
    }
  } else if (NFC_PROTOCOL_T5T == p_activate_params->protocol) {
    /* T5T */
    if (p_activate_params->rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_V) {
      status = rw_i93_select(p_activate_params->rf_tech_param.param.pi93.uid);
    }
  } else if (NFC_PROTOCOL_MIFARE == p_activate_params->protocol) {
    /* Mifare Classic*/
    if (p_activate_params->rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) {
      status = rw_mfc_select(
          p_activate_params->rf_tech_param.param.pa.sel_rsp,
          p_activate_params->rf_tech_param.param.pa.nfcid1 +
              p_activate_params->rf_tech_param.param.pa.nfcid1_len - 4);
    }
  }
  /* TODO set up callback for proprietary protocol */
  else {
    LOG(ERROR) << StringPrintf("RW_SetActivatedTagType Invalid protocol");
  }

  if (status != NFC_STATUS_OK) rw_cb.p_cback = nullptr;
  return status;
}

#if (NXP_EXTNS == TRUE)
/*******************************************************************************
**
** Function         RW_SetT4tNfceeInfo
**
** Description      This function selects the T4t Nfcee  for Reader/Writer mode.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS RW_SetT4tNfceeInfo(tRW_CBACK* p_cback, uint8_t conn_id) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  /* Reset tag-specific area of control block */
      LOG(ERROR) << StringPrintf("RW_SetActivatedTagType %d ",conn_id);

  memset(&rw_cb.tcb, 0, sizeof(tRW_TCB));

  if (p_cback != NULL) {
    rw_cb.p_cback = p_cback;
    status = RW_T4tNfceeInitCb();
    if (status != NFC_STATUS_OK) {
      rw_cb.p_cback = NULL;
    }
  } else {
    rw_cb.p_cback = NULL;
  }
  return status;
}
#endif
