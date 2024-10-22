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
 *  This file contains the LLCP utilities
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
#include "nfc_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/*******************************************************************************
**
** Function         llcp_util_parse_link_params
**
** Description      Parse LLCP Link parameters
**
** Returns          TRUE if success
**
*******************************************************************************/
bool llcp_util_parse_link_params(uint16_t length, uint8_t* p_bytes) {
  uint8_t param_type, param_len, *p = p_bytes;

  while (length >= 2) {
    BE_STREAM_TO_UINT8(param_type, p);
    BE_STREAM_TO_UINT8(param_len, p);
    if (length < param_len + 2) {
      android_errorWriteLog(0x534e4554, "114238578");
      LOG(ERROR) << StringPrintf("Bad TLV's");
      return false;
    }
    length -= param_len + 2;

    switch (param_type) {
      case LLCP_VERSION_TYPE:
        if (param_len != LLCP_VERSION_LEN) {
          android_errorWriteLog(0x534e4554, "114238578");
          LOG(ERROR) << StringPrintf("Bad TLV's");
          return false;
        }
        BE_STREAM_TO_UINT8(llcp_cb.lcb.peer_version, p);
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("Peer Version - 0x%02X", llcp_cb.lcb.peer_version);
        break;

      case LLCP_MIUX_TYPE:
        if (param_len != LLCP_MIUX_LEN) {
          android_errorWriteLog(0x534e4554, "114238578");
          LOG(ERROR) << StringPrintf("Bad TLV's");
          return false;
        }
        BE_STREAM_TO_UINT16(llcp_cb.lcb.peer_miu, p);
        llcp_cb.lcb.peer_miu &= LLCP_MIUX_MASK;
        llcp_cb.lcb.peer_miu += LLCP_DEFAULT_MIU;
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("Peer MIU - %d bytes", llcp_cb.lcb.peer_miu);
        break;

      case LLCP_WKS_TYPE:
        if (param_len != LLCP_WKS_LEN) {
          android_errorWriteLog(0x534e4554, "114238578");
          LOG(ERROR) << StringPrintf("Bad TLV's");
          return false;
        }
        BE_STREAM_TO_UINT16(llcp_cb.lcb.peer_wks, p);
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("Peer WKS - 0x%04X", llcp_cb.lcb.peer_wks);
        break;

      case LLCP_LTO_TYPE:
        if (param_len != LLCP_LTO_LEN) {
          android_errorWriteLog(0x534e4554, "114238578");
          LOG(ERROR) << StringPrintf("Bad TLV's");
          return false;
        }
        BE_STREAM_TO_UINT8(llcp_cb.lcb.peer_lto, p);
        llcp_cb.lcb.peer_lto *= LLCP_LTO_UNIT; /* 10ms unit */
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("Peer LTO - %d ms", llcp_cb.lcb.peer_lto);
        break;

      case LLCP_OPT_TYPE:
        if (param_len != LLCP_OPT_LEN) {
          android_errorWriteLog(0x534e4554, "114238578");
          LOG(ERROR) << StringPrintf("Bad TLV's");
          return false;
        }
        BE_STREAM_TO_UINT8(llcp_cb.lcb.peer_opt, p);
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("Peer OPT - 0x%02X", llcp_cb.lcb.peer_opt);
        break;

      default:
        LOG(ERROR) << StringPrintf("Unexpected type 0x%x", param_type);
        p += param_len;
        break;
    }
  }
  return true;
}

/*******************************************************************************
**
** Function         llcp_util_adjust_ll_congestion
**
** Description      adjust tx/rx congestion thresholds on logical link
**
** Returns          void
**
*******************************************************************************/
void llcp_util_adjust_ll_congestion(void) {
  /* buffer quota is allocated equally for each logical data link */
  if (llcp_cb.num_logical_data_link) {
    llcp_cb.ll_tx_congest_start =
        llcp_cb.max_num_ll_tx_buff / llcp_cb.num_logical_data_link;
    llcp_cb.ll_rx_congest_start =
        llcp_cb.max_num_ll_rx_buff / llcp_cb.num_logical_data_link;
  } else {
    llcp_cb.ll_tx_congest_start = llcp_cb.max_num_ll_tx_buff;
    llcp_cb.ll_rx_congest_start = llcp_cb.max_num_ll_rx_buff;
  }

  /* at least one for each logical data link */
  if (llcp_cb.ll_tx_congest_start == 0) {
    llcp_cb.ll_tx_congest_start = 1;
  }
  if (llcp_cb.ll_rx_congest_start == 0) {
    llcp_cb.ll_rx_congest_start = 1;
  }

  if (llcp_cb.ll_tx_congest_start > 1) {
    llcp_cb.ll_tx_congest_end = 1;
  } else {
    llcp_cb.ll_tx_congest_end = 0;
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "num_logical_data_link=%d, ll_tx_congest_start=%d, ll_tx_congest_end=%d, "
      "ll_rx_congest_start=%d",
      llcp_cb.num_logical_data_link, llcp_cb.ll_tx_congest_start,
      llcp_cb.ll_tx_congest_end, llcp_cb.ll_rx_congest_start);
}

/*******************************************************************************
**
** Function         llcp_util_adjust_dl_rx_congestion
**
** Description      adjust rx congestion thresholds on data link
**
** Returns          void
**
*******************************************************************************/
void llcp_util_adjust_dl_rx_congestion(void) {
  uint8_t idx, rx_congest_start;

  if (llcp_cb.num_data_link_connection) {
    rx_congest_start = llcp_cb.num_rx_buff / llcp_cb.num_data_link_connection;

    for (idx = 0; idx < LLCP_MAX_DATA_LINK; idx++) {
      if (llcp_cb.dlcb[idx].state == LLCP_DLC_STATE_CONNECTED) {
        if (rx_congest_start > llcp_cb.dlcb[idx].local_rw) {
          /*
          ** set rx congestion threshold LLCP_DL_MIN_RX_CONGEST at
          ** least so, we don't need to flow off too often.
          */
          if (llcp_cb.dlcb[idx].local_rw + 1 > LLCP_DL_MIN_RX_CONGEST)
            llcp_cb.dlcb[idx].rx_congest_threshold =
                llcp_cb.dlcb[idx].local_rw + 1;
          else
            llcp_cb.dlcb[idx].rx_congest_threshold = LLCP_DL_MIN_RX_CONGEST;
        } else {
          llcp_cb.dlcb[idx].rx_congest_threshold = LLCP_DL_MIN_RX_CONGEST;
        }

        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "DLC[%d], local_rw=%d, rx_congest_threshold=%d", idx,
            llcp_cb.dlcb[idx].local_rw, llcp_cb.dlcb[idx].rx_congest_threshold);
      }
    }
  }
}

/*******************************************************************************
**
** Function         llcp_util_check_rx_congested_status
**
** Description      Update rx congested status
**
** Returns          void
**
*******************************************************************************/
void llcp_util_check_rx_congested_status(void) {
  uint8_t idx;

  if (llcp_cb.overall_rx_congested) {
    /* check if rx congestion clear */
    if (llcp_cb.total_rx_ui_pdu + llcp_cb.total_rx_i_pdu <=
        llcp_cb.overall_rx_congest_end) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "rx link is uncongested, "
          "%d+%d <= %d",
          llcp_cb.total_rx_ui_pdu, llcp_cb.total_rx_i_pdu,
          llcp_cb.overall_rx_congest_end);

      llcp_cb.overall_rx_congested = false;

      for (idx = 0; idx < LLCP_MAX_DATA_LINK; idx++) {
        /* set flag to clear local busy status on data link connections */
        if ((llcp_cb.dlcb[idx].state == LLCP_DLC_STATE_CONNECTED) &&
            (llcp_cb.dlcb[idx].is_rx_congested == false)) {
          llcp_cb.dlcb[idx].flags |= LLCP_DATA_LINK_FLAG_PENDING_RR_RNR;
        }
      }
    }
  } else {
    /* check if rx link is congested */
    if (llcp_cb.total_rx_ui_pdu + llcp_cb.total_rx_i_pdu >=
        llcp_cb.overall_rx_congest_start) {
      LOG(WARNING) << StringPrintf(
          "rx link is congested, %d+%d "
          ">= %d",
          llcp_cb.total_rx_ui_pdu, llcp_cb.total_rx_i_pdu,
          llcp_cb.overall_rx_congest_start);

      llcp_cb.overall_rx_congested = true;

      /* rx link congestion is started, send RNR to remote end point */
      for (idx = 0; idx < LLCP_MAX_DATA_LINK; idx++) {
        if ((llcp_cb.dlcb[idx].state == LLCP_DLC_STATE_CONNECTED) &&
            (llcp_cb.dlcb[idx].is_rx_congested == false)) {
          llcp_cb.dlcb[idx].flags |= LLCP_DATA_LINK_FLAG_PENDING_RR_RNR;
        }
      }
    }
  }
}

/*******************************************************************************
**
** Function         llcp_util_send_ui
**
** Description      Send UI PDU
**
** Returns          tLLCP_STATUS
**
*******************************************************************************/
tLLCP_STATUS llcp_util_send_ui(uint8_t ssap, uint8_t dsap,
                               tLLCP_APP_CB* p_app_cb, NFC_HDR* p_msg) {
  uint8_t* p;
  tLLCP_STATUS status = LLCP_STATUS_SUCCESS;

  p_msg->offset -= LLCP_PDU_HEADER_SIZE;
  p_msg->len += LLCP_PDU_HEADER_SIZE;

  p = (uint8_t*)(p_msg + 1) + p_msg->offset;
  UINT16_TO_BE_STREAM(p, LLCP_GET_PDU_HEADER(dsap, LLCP_PDU_UI_TYPE, ssap));

  GKI_enqueue(&p_app_cb->ui_xmit_q, p_msg);
  llcp_cb.total_tx_ui_pdu++;

  llcp_link_check_send_data();

  if ((p_app_cb->is_ui_tx_congested) ||
      (p_app_cb->ui_xmit_q.count >= llcp_cb.ll_tx_congest_start) ||
      (llcp_cb.overall_tx_congested) ||
      (llcp_cb.total_tx_ui_pdu >= llcp_cb.max_num_ll_tx_buff)) {
    /* set congested here so overall congestion check routine will not report
     * event again, */
    /* or notify uncongestion later */
    p_app_cb->is_ui_tx_congested = true;

    LOG(WARNING) << StringPrintf(
        "Logical link (SAP=0x%X) congested: ui_xmit_q.count=%d", ssap,
        p_app_cb->ui_xmit_q.count);

    status = LLCP_STATUS_CONGESTED;
  }

  return status;
}

/*******************************************************************************
**
** Function         llcp_util_send_disc
**
** Description      Send DISC PDU
**
** Returns          void
**
*******************************************************************************/
void llcp_util_send_disc(uint8_t dsap, uint8_t ssap) {
  NFC_HDR* p_msg;
  uint8_t* p;

  p_msg = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);

  if (p_msg) {
    p_msg->len = LLCP_PDU_DISC_SIZE;
    p_msg->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;

    p = (uint8_t*)(p_msg + 1) + p_msg->offset;
    UINT16_TO_BE_STREAM(p, LLCP_GET_PDU_HEADER(dsap, LLCP_PDU_DISC_TYPE, ssap));

    GKI_enqueue(&llcp_cb.lcb.sig_xmit_q, p_msg);
    llcp_link_check_send_data();
  }
}

/*******************************************************************************
**
** Function         llcp_util_allocate_data_link
**
** Description      Allocate tLLCP_DLCB for data link connection
**
** Returns          tLLCP_DLCB *
**
******************************************************************************/
tLLCP_DLCB* llcp_util_allocate_data_link(uint8_t reg_sap, uint8_t remote_sap) {
  tLLCP_DLCB* p_dlcb = nullptr;
  int idx;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("reg_sap = 0x%x, remote_sap = 0x%x", reg_sap, remote_sap);

  for (idx = 0; idx < LLCP_MAX_DATA_LINK; idx++) {
    if (llcp_cb.dlcb[idx].state == LLCP_DLC_STATE_IDLE) {
      p_dlcb = &(llcp_cb.dlcb[idx]);

      memset(p_dlcb, 0, sizeof(tLLCP_DLCB));
      break;
    }
  }

  if (!p_dlcb) {
    LOG(ERROR) << StringPrintf("Out of DLCB");
  } else {
    p_dlcb->p_app_cb = llcp_util_get_app_cb(reg_sap);
    p_dlcb->local_sap = reg_sap;
    p_dlcb->remote_sap = remote_sap;
    p_dlcb->timer.param = (uintptr_t)p_dlcb;

    /* this is for inactivity timer and congestion control. */
    llcp_cb.num_data_link_connection++;

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "local_sap = 0x%x, remote_sap = 0x%x, "
        "num_data_link_connection = %d",
        p_dlcb->local_sap, p_dlcb->remote_sap,
        llcp_cb.num_data_link_connection);
  }
  return p_dlcb;
}

/*******************************************************************************
**
** Function         llcp_util_deallocate_data_link
**
** Description      Deallocate tLLCP_DLCB
**
** Returns          void
**
******************************************************************************/
void llcp_util_deallocate_data_link(tLLCP_DLCB* p_dlcb) {
  if (p_dlcb) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("local_sap = 0x%x", p_dlcb->local_sap);

    if (p_dlcb->state != LLCP_DLC_STATE_IDLE) {
      nfc_stop_quick_timer(&p_dlcb->timer);
      llcp_dlc_flush_q(p_dlcb);

      p_dlcb->state = LLCP_DLC_STATE_IDLE;

      if (llcp_cb.num_data_link_connection > 0) {
        llcp_cb.num_data_link_connection--;
      }

      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "num_data_link_connection = %d", llcp_cb.num_data_link_connection);
    }
  }
}

/*******************************************************************************
**
** Function         llcp_util_send_connect
**
** Description      Send CONNECT PDU
**
** Returns          tLLCP_STATUS
**
******************************************************************************/
tLLCP_STATUS llcp_util_send_connect(tLLCP_DLCB* p_dlcb,
                                    tLLCP_CONNECTION_PARAMS* p_params) {
  NFC_HDR* p_msg;
  uint8_t* p;
  uint16_t miu_len = 0, rw_len = 0, sn_len = 0;

  if (p_params->miu != LLCP_DEFAULT_MIU) {
    miu_len = 4; /* TYPE, LEN, 2 bytes MIU */
  }
  if (p_params->rw != LLCP_DEFAULT_RW) {
    rw_len = 3;           /* TYPE, LEN, 1 byte RW */
    p_params->rw &= 0x0F; /* only 4 bits  */
  }
  if ((strlen(p_params->sn)) && (p_dlcb->remote_sap == LLCP_SAP_SDP)) {
    sn_len = (uint16_t)(2 + strlen(p_params->sn)); /* TYPE, LEN, SN */
  }

  p_msg = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);

  if (p_msg) {
    p_msg->len = LLCP_PDU_HEADER_SIZE + miu_len + rw_len + sn_len;
    p_msg->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;

    p = (uint8_t*)(p_msg + 1) + p_msg->offset;

    UINT16_TO_BE_STREAM(
        p, LLCP_GET_PDU_HEADER(p_dlcb->remote_sap, LLCP_PDU_CONNECT_TYPE,
                               p_dlcb->local_sap));

    if (miu_len) {
      UINT8_TO_BE_STREAM(p, LLCP_MIUX_TYPE);
      UINT8_TO_BE_STREAM(p, LLCP_MIUX_LEN);
      UINT16_TO_BE_STREAM(p, p_params->miu - LLCP_DEFAULT_MIU);
    }

    if (rw_len) {
      UINT8_TO_BE_STREAM(p, LLCP_RW_TYPE);
      UINT8_TO_BE_STREAM(p, LLCP_RW_LEN);
      UINT8_TO_BE_STREAM(p, p_params->rw);
    }

    if (sn_len) {
      UINT8_TO_BE_STREAM(p, LLCP_SN_TYPE);
      UINT8_TO_BE_STREAM(p, sn_len - 2);
      memcpy(p, p_params->sn, sn_len - 2);
    }

    GKI_enqueue(&llcp_cb.lcb.sig_xmit_q, p_msg);
    llcp_link_check_send_data();

    return LLCP_STATUS_SUCCESS;
  }

  return LLCP_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         llcp_util_parse_connect
**
** Description      Parse CONNECT PDU
**
** Returns          tLLCP_STATUS
**
*******************************************************************************/
tLLCP_STATUS llcp_util_parse_connect(uint8_t* p_bytes, uint16_t length,
                                     tLLCP_CONNECTION_PARAMS* p_params) {
  uint8_t param_type, param_len, *p = p_bytes;

  p_params->miu = LLCP_DEFAULT_MIU;
  p_params->rw = LLCP_DEFAULT_RW;
  p_params->sn[0] = 0;
  p_params->sn[1] = 0;

  while (length >= 2) {
    BE_STREAM_TO_UINT8(param_type, p);
    BE_STREAM_TO_UINT8(param_len, p);
    /* check remaining lengh */
    if (length < param_len + 2) {
      android_errorWriteLog(0x534e4554, "111660010");
      LOG(ERROR) << StringPrintf("Bad TLV's");
      return LLCP_STATUS_FAIL;
    }
    length -= param_len + 2;

    switch (param_type) {
      case LLCP_MIUX_TYPE:
        if (param_len != LLCP_MIUX_LEN) {
          android_errorWriteLog(0x534e4554, "111660010");
          LOG(ERROR) << StringPrintf("Bad TLV's");
          return LLCP_STATUS_FAIL;
        }
        BE_STREAM_TO_UINT16(p_params->miu, p);
        p_params->miu &= LLCP_MIUX_MASK;
        p_params->miu += LLCP_DEFAULT_MIU;

        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("LLCP_MIUX_TYPE:%d", p_params->miu);
        break;

      case LLCP_RW_TYPE:
        if (param_len != LLCP_RW_LEN) {
          android_errorWriteLog(0x534e4554, "111660010");
          LOG(ERROR) << StringPrintf("Bad TLV's");
          return LLCP_STATUS_FAIL;
        }
        BE_STREAM_TO_UINT8(p_params->rw, p);
        p_params->rw &= 0x0F;

        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("LLCP_RW_TYPE:%d", p_params->rw);
        break;

      case LLCP_SN_TYPE:
        if (param_len == 0) {
          p_params->sn[0] = 0;  // make "sn" null terminated zero-length string
          /* indicate that SN type is included without SN */
          p_params->sn[1] = LLCP_SN_TYPE;
        } else if (param_len <= LLCP_MAX_SN_LEN) {
          memcpy(p_params->sn, p, param_len);
          p_params->sn[param_len] = 0;
        } else {
          memcpy(p_params->sn, p, LLCP_MAX_SN_LEN);
          p_params->sn[LLCP_MAX_SN_LEN] = 0;
        }
        p += param_len;

        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("LLCP_SN_TYPE:<%s>", p_params->sn);
        break;

      default:
        LOG(ERROR) << StringPrintf("Unexpected type 0x%x", param_type);
        p += param_len;
        break;
    }
  }
  return LLCP_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         llcp_util_send_cc
**
** Description      Send CC PDU
**
** Returns          tLLCP_STATUS
**
******************************************************************************/
tLLCP_STATUS llcp_util_send_cc(tLLCP_DLCB* p_dlcb,
                               tLLCP_CONNECTION_PARAMS* p_params) {
  NFC_HDR* p_msg;
  uint8_t* p;
  uint16_t miu_len = 0, rw_len = 0;

  if (p_params->miu != LLCP_DEFAULT_MIU) {
    miu_len = 4;
  }
  if (p_params->rw != LLCP_DEFAULT_RW) {
    rw_len = 3;
    p_params->rw &= 0x0F;
  }

  p_msg = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);

  if (p_msg) {
    p_msg->len = LLCP_PDU_HEADER_SIZE + miu_len + rw_len;
    p_msg->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;

    p = (uint8_t*)(p_msg + 1) + p_msg->offset;

    UINT16_TO_BE_STREAM(
        p, LLCP_GET_PDU_HEADER(p_dlcb->remote_sap, LLCP_PDU_CC_TYPE,
                               p_dlcb->local_sap));

    if (miu_len) {
      UINT8_TO_BE_STREAM(p, LLCP_MIUX_TYPE);
      UINT8_TO_BE_STREAM(p, LLCP_MIUX_LEN);
      UINT16_TO_BE_STREAM(p, p_params->miu - LLCP_DEFAULT_MIU);
    }

    if (rw_len) {
      UINT8_TO_BE_STREAM(p, LLCP_RW_TYPE);
      UINT8_TO_BE_STREAM(p, LLCP_RW_LEN);
      UINT8_TO_BE_STREAM(p, p_params->rw);
    }

    GKI_enqueue(&llcp_cb.lcb.sig_xmit_q, p_msg);
    llcp_link_check_send_data();

    return LLCP_STATUS_SUCCESS;
  }

  return LLCP_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         llcp_util_parse_cc
**
** Description      Parse CC PDU
**
** Returns          tLLCP_STATUS
**
*******************************************************************************/
tLLCP_STATUS llcp_util_parse_cc(uint8_t* p_bytes, uint16_t length,
                                uint16_t* p_miu, uint8_t* p_rw) {
  uint8_t param_type, param_len, *p = p_bytes;

  *p_miu = LLCP_DEFAULT_MIU;
  *p_rw = LLCP_DEFAULT_RW;

  while (length >= 2) {
    BE_STREAM_TO_UINT8(param_type, p);
    BE_STREAM_TO_UINT8(param_len, p);
    if (length < param_len + 2) {
      android_errorWriteLog(0x534e4554, "114237888");
      LOG(ERROR) << StringPrintf("Bad TLV's");
      return LLCP_STATUS_FAIL;
    }
    length -= param_len + 2;

    switch (param_type) {
      case LLCP_MIUX_TYPE:
        if (param_len != LLCP_MIUX_LEN) {
          android_errorWriteLog(0x534e4554, "114237888");
          LOG(ERROR) << StringPrintf("Bad TLV's");
          return LLCP_STATUS_FAIL;
        }
        BE_STREAM_TO_UINT16((*p_miu), p);
        (*p_miu) &= LLCP_MIUX_MASK;
        (*p_miu) += LLCP_DEFAULT_MIU;

        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("LLCP_MIUX_TYPE:%d", *p_miu);
        break;

      case LLCP_RW_TYPE:
        if (param_len != LLCP_RW_LEN) {
          android_errorWriteLog(0x534e4554, "114237888");
          LOG(ERROR) << StringPrintf("Bad TLV's");
          return LLCP_STATUS_FAIL;
        }
        BE_STREAM_TO_UINT8((*p_rw), p);
        (*p_rw) &= 0x0F;

        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("LLCP_RW_TYPE:%d", *p_rw);
        break;

      default:
        LOG(ERROR) << StringPrintf("Unexpected type 0x%x", param_type);
        p += param_len;
        break;
    }
  }
  return LLCP_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         llcp_util_send_dm
**
** Description      Send DM PDU
**
** Returns          void
**
*******************************************************************************/
void llcp_util_send_dm(uint8_t dsap, uint8_t ssap, uint8_t reason) {
  NFC_HDR* p_msg;
  uint8_t* p;

  p_msg = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);

  if (p_msg) {
    p_msg->len = LLCP_PDU_DM_SIZE;
    p_msg->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;

    p = (uint8_t*)(p_msg + 1) + p_msg->offset;
    UINT16_TO_BE_STREAM(p, LLCP_GET_PDU_HEADER(dsap, LLCP_PDU_DM_TYPE, ssap));
    UINT8_TO_BE_STREAM(p, reason);

    GKI_enqueue(&llcp_cb.lcb.sig_xmit_q, p_msg);
    llcp_link_check_send_data();
  }
}

/*******************************************************************************
**
** Function         llcp_util_build_info_pdu
**
** Description      Add DSAP, PTYPE, SSAP and sequence numbers and update local
**                  ack sequence
**
** Returns          void
**
*******************************************************************************/
void llcp_util_build_info_pdu(tLLCP_DLCB* p_dlcb, NFC_HDR* p_msg) {
  uint8_t* p;
  uint8_t rcv_seq;

  p_msg->offset -= LLCP_PDU_HEADER_SIZE + LLCP_SEQUENCE_SIZE;
  p_msg->len += LLCP_PDU_HEADER_SIZE + LLCP_SEQUENCE_SIZE;
  p = (uint8_t*)(p_msg + 1) + p_msg->offset;

  UINT16_TO_BE_STREAM(p,
                      LLCP_GET_PDU_HEADER(p_dlcb->remote_sap, LLCP_PDU_I_TYPE,
                                          p_dlcb->local_sap));

  /* if local_busy or rx congested then do not update receive sequence number to
   * flow off */
  if ((p_dlcb->local_busy) || (p_dlcb->is_rx_congested) ||
      (llcp_cb.overall_rx_congested)) {
    rcv_seq = p_dlcb->sent_ack_seq;
  } else {
    p_dlcb->sent_ack_seq = p_dlcb->next_rx_seq;
    rcv_seq = p_dlcb->sent_ack_seq;
  }
  UINT8_TO_BE_STREAM(p, LLCP_GET_SEQUENCE(p_dlcb->next_tx_seq, rcv_seq));
}

/*******************************************************************************
**
** Function         llcp_util_send_frmr
**
** Description      Send FRMR PDU
**
** Returns          tLLCP_STATUS
**
*******************************************************************************/
tLLCP_STATUS llcp_util_send_frmr(tLLCP_DLCB* p_dlcb, uint8_t flags,
                                 uint8_t ptype, uint8_t sequence) {
  NFC_HDR* p_msg;
  uint8_t* p;

  p_msg = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);

  if (p_msg) {
    p_msg->len = LLCP_PDU_FRMR_SIZE;
    p_msg->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;

    p = (uint8_t*)(p_msg + 1) + p_msg->offset;

    UINT16_TO_BE_STREAM(
        p, LLCP_GET_PDU_HEADER(p_dlcb->remote_sap, LLCP_PDU_FRMR_TYPE,
                               p_dlcb->local_sap));
    UINT8_TO_BE_STREAM(p, (flags << 4) | ptype);
    UINT8_TO_BE_STREAM(p, sequence);
    UINT8_TO_BE_STREAM(p, (p_dlcb->next_tx_seq << 4) | p_dlcb->next_rx_seq);
    UINT8_TO_BE_STREAM(p, (p_dlcb->rcvd_ack_seq << 4) | p_dlcb->sent_ack_seq);

    GKI_enqueue(&llcp_cb.lcb.sig_xmit_q, p_msg);
    llcp_link_check_send_data();

    return LLCP_STATUS_SUCCESS;
  } else {
    LOG(ERROR) << StringPrintf("Out of resource");
    return LLCP_STATUS_FAIL;
  }
}

/*******************************************************************************
**
** Function         llcp_util_send_rr_rnr
**
** Description      Send RR or RNR PDU
**
** Returns          void
**
*******************************************************************************/
void llcp_util_send_rr_rnr(tLLCP_DLCB* p_dlcb) {
  NFC_HDR* p_msg;
  uint8_t* p;
  uint8_t pdu_type;
  uint8_t pdu_size;
  uint8_t rcv_seq;

  /* if no indication of change in local busy or rx congestion */
  if ((p_dlcb->flags & LLCP_DATA_LINK_FLAG_PENDING_RR_RNR) == 0) {
    /* if all ack is sent */
    if (p_dlcb->sent_ack_seq == p_dlcb->next_rx_seq) {
      /* we don't need to send RR/RNR */
      return;
    } else {
      /* if rx flow off because of local busy or congestion */
      if ((p_dlcb->local_busy) || (p_dlcb->is_rx_congested) ||
          (llcp_cb.overall_rx_congested)) {
        /* don't send RR/RNR */
        return;
      }
    }
  }

  if ((p_dlcb->local_busy) || (p_dlcb->is_rx_congested) ||
      (llcp_cb.overall_rx_congested)) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        ""
        "local_busy=%d,is_rx_congested=%d,overall_rx_congested=%d",
        p_dlcb->local_busy, p_dlcb->is_rx_congested,
        llcp_cb.overall_rx_congested);

    /* if local_busy or rx congested then do not update receive sequence number
     * to flow off */
    pdu_type = LLCP_PDU_RNR_TYPE;
    pdu_size = LLCP_PDU_RNR_SIZE;
    rcv_seq = p_dlcb->sent_ack_seq;
  } else {
    pdu_type = LLCP_PDU_RR_TYPE;
    pdu_size = LLCP_PDU_RR_SIZE;

    p_dlcb->sent_ack_seq = p_dlcb->next_rx_seq;
    rcv_seq = p_dlcb->sent_ack_seq;
  }

  p_msg = (NFC_HDR*)GKI_getpoolbuf(LLCP_POOL_ID);

  if (p_msg) {
    p_dlcb->flags &= ~LLCP_DATA_LINK_FLAG_PENDING_RR_RNR;

    p_msg->len = pdu_size;
    p_msg->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;

    p = (uint8_t*)(p_msg + 1) + p_msg->offset;

    UINT16_TO_BE_STREAM(p, LLCP_GET_PDU_HEADER(p_dlcb->remote_sap, pdu_type,
                                               p_dlcb->local_sap));

    UINT8_TO_BE_STREAM(p, rcv_seq);

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "LLCP TX - N(S,R):(NA,%d) V(S,SA,R,RA):(%d,%d,%d,%d)",
        p_dlcb->next_rx_seq, p_dlcb->next_tx_seq, p_dlcb->rcvd_ack_seq,
        p_dlcb->next_rx_seq, p_dlcb->sent_ack_seq);
    GKI_enqueue(&llcp_cb.lcb.sig_xmit_q, p_msg);
    llcp_link_check_send_data();
  } else {
    LOG(ERROR) << StringPrintf("Out of resource");
  }
}

/*******************************************************************************
**
** Function         llcp_util_get_app_cb
**
** Description      get pointer of application registered control block by SAP
**
** Returns          tLLCP_APP_CB *
**
*******************************************************************************/
tLLCP_APP_CB* llcp_util_get_app_cb(uint8_t local_sap) {
  tLLCP_APP_CB* p_app_cb = nullptr;

  if (local_sap <= LLCP_UPPER_BOUND_WK_SAP) {
    if ((local_sap != LLCP_SAP_LM) && (local_sap < LLCP_MAX_WKS)) {
      p_app_cb = &llcp_cb.wks_cb[local_sap];
    }
  } else if (local_sap <= LLCP_UPPER_BOUND_SDP_SAP) {
    if (local_sap - LLCP_LOWER_BOUND_SDP_SAP < LLCP_MAX_SERVER) {
      p_app_cb = &llcp_cb.server_cb[local_sap - LLCP_LOWER_BOUND_SDP_SAP];
    }
  } else if (local_sap <= LLCP_UPPER_BOUND_LOCAL_SAP) {
    if (local_sap - LLCP_LOWER_BOUND_LOCAL_SAP < LLCP_MAX_CLIENT) {
      p_app_cb = &llcp_cb.client_cb[local_sap - LLCP_LOWER_BOUND_LOCAL_SAP];
    }
  }

  return (p_app_cb);
}
