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
 *  This file contains the action functions the NFA_RW state machine.
 *
 ******************************************************************************/
#include <log/log.h>
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "ndef_utils.h"
#include "nfa_dm_int.h"
#include "nfa_mem_co.h"
#include "nfa_rw_int.h"
#include "bt_trace.h"
using android::base::StringPrintf;

extern bool nfc_debug_enabled;

#define NFA_RW_OPTION_INVALID 0xFF

/* Tag sleep req cmd*/
uint8_t NFA_RW_TAG_SLP_REQ[] = {0x50, 0x00};

/* Local static function prototypes */
static tNFC_STATUS nfa_rw_start_ndef_read(void);
static tNFC_STATUS nfa_rw_start_ndef_write(void);
static tNFC_STATUS nfa_rw_start_ndef_detection(void);
static tNFC_STATUS nfa_rw_config_tag_ro(bool b_hard_lock);
static bool nfa_rw_op_req_while_busy(tNFA_RW_MSG* p_data);
static bool nfa_rw_op_req_while_inactive(tNFA_RW_MSG* p_data);
static void nfa_rw_error_cleanup(uint8_t event);
static void nfa_rw_presence_check(tNFA_RW_MSG* p_data);
static void nfa_rw_handle_t2t_evt(tRW_EVENT event, tRW_DATA* p_rw_data);
static bool nfa_rw_detect_ndef(void);
static void nfa_rw_cback(tRW_EVENT event, tRW_DATA* p_rw_data);
static void nfa_rw_handle_mfc_evt(tRW_EVENT event, tRW_DATA* p_rw_data);

/*******************************************************************************
**
** Function         nfa_rw_free_ndef_rx_buf
**
** Description      Free buffer allocated to hold incoming NDEF message
**
** Returns          Nothing
**
*******************************************************************************/
void nfa_rw_free_ndef_rx_buf(void) {
  if (nfa_rw_cb.p_ndef_buf) {
    nfa_mem_co_free(nfa_rw_cb.p_ndef_buf);
    nfa_rw_cb.p_ndef_buf = nullptr;
  }
}

/*******************************************************************************
**
** Function         nfa_rw_store_ndef_rx_buf
**
** Description      Store data into NDEF buffer
**
** Returns          Nothing
**
*******************************************************************************/
static void nfa_rw_store_ndef_rx_buf(tRW_DATA* p_rw_data) {
  uint8_t* p;

  p = (uint8_t*)(p_rw_data->data.p_data + 1) + p_rw_data->data.p_data->offset;

  if ((nfa_rw_cb.ndef_rd_offset + p_rw_data->data.p_data->len) <=
      nfa_rw_cb.ndef_cur_size) {
    /* Save data into buffer */
    memcpy(&nfa_rw_cb.p_ndef_buf[nfa_rw_cb.ndef_rd_offset], p,
           p_rw_data->data.p_data->len);
    nfa_rw_cb.ndef_rd_offset += p_rw_data->data.p_data->len;
  } else {
    LOG(ERROR) << StringPrintf("Exceed ndef_cur_size error");
  }

  GKI_freebuf(p_rw_data->data.p_data);
  p_rw_data->data.p_data = nullptr;
}

/*******************************************************************************
**
** Function         nfa_rw_send_data_to_upper
**
** Description      Send data to upper layer
**
** Returns          Nothing
**
*******************************************************************************/
static void nfa_rw_send_data_to_upper(tRW_DATA* p_rw_data) {
  tNFA_CONN_EVT_DATA conn_evt_data;

  if ((p_rw_data->status == NFC_STATUS_TIMEOUT) ||
      (p_rw_data->data.p_data == nullptr))
    return;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_rw_send_data_to_upper: Len [0x%X] Status [%s]",
                      p_rw_data->data.p_data->len,
                      NFC_GetStatusName(p_rw_data->data.status).c_str());

  /* Notify conn cback of NFA_DATA_EVT */
  conn_evt_data.data.status = p_rw_data->data.status;
  conn_evt_data.data.p_data =
      (uint8_t*)(p_rw_data->data.p_data + 1) + p_rw_data->data.p_data->offset;
  conn_evt_data.data.len = p_rw_data->data.p_data->len;

  nfa_dm_act_conn_cback_notify(NFA_DATA_EVT, &conn_evt_data);

  GKI_freebuf(p_rw_data->data.p_data);
  p_rw_data->data.p_data = nullptr;
}

/*******************************************************************************
**
** Function         nfa_rw_error_cleanup
**
** Description      Handle failure - signal command complete and notify app
**
** Returns          Nothing
**
*******************************************************************************/
static void nfa_rw_error_cleanup(uint8_t event) {
  tNFA_CONN_EVT_DATA conn_evt_data;

  nfa_rw_command_complete();

  conn_evt_data.status = NFA_STATUS_FAILED;

  nfa_dm_act_conn_cback_notify(event, &conn_evt_data);
}

/*******************************************************************************
**
** Function         nfa_rw_check_start_presence_check_timer
**
** Description      Start timer to wait for specified time before presence check
**
** Returns          Nothing
**
*******************************************************************************/
static void nfa_rw_check_start_presence_check_timer(
    uint16_t presence_check_start_delay) {
  if (!p_nfa_dm_cfg->auto_presence_check) return;

  if (nfa_rw_cb.flags & NFA_RW_FL_NOT_EXCL_RF_MODE) {
    if (presence_check_start_delay) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("Starting presence check timer...");
      nfa_sys_start_timer(&nfa_rw_cb.tle, NFA_RW_PRESENCE_CHECK_TICK_EVT,
                          presence_check_start_delay);
    } else {
      /* Presence check now */
      nfa_rw_presence_check(nullptr);
    }
  }
}

/*******************************************************************************
**
** Function         nfa_rw_stop_presence_check_timer
**
** Description      Stop timer for presence check
**
** Returns          Nothing
**
*******************************************************************************/
void nfa_rw_stop_presence_check_timer(void) {
  nfa_sys_stop_timer(&nfa_rw_cb.tle);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("Stopped presence check timer (if started)");
}

/*******************************************************************************
**
** Function         nfa_rw_handle_ndef_detect
**
** Description      Handler for NDEF detection reader/writer event
**
** Returns          Nothing
**
*******************************************************************************/
static void nfa_rw_handle_ndef_detect(tRW_DATA* p_rw_data) {
  tNFA_CONN_EVT_DATA conn_evt_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "NDEF Detection completed: cur_size=%i, max_size=%i, flags=0x%x",
      p_rw_data->ndef.cur_size, p_rw_data->ndef.max_size,
      p_rw_data->ndef.flags);

  /* Check if NDEF detection succeeded */
  if (p_rw_data->ndef.status == NFC_STATUS_OK) {
    /* Set NDEF detection state */
    nfa_rw_cb.ndef_st = NFA_RW_NDEF_ST_TRUE;
    nfa_rw_cb.flags |= NFA_RW_FL_NDEF_OK;

    /* Store ndef properties */
    conn_evt_data.ndef_detect.status = NFA_STATUS_OK;
    conn_evt_data.ndef_detect.protocol = p_rw_data->ndef.protocol;
    conn_evt_data.ndef_detect.cur_size = nfa_rw_cb.ndef_cur_size =
        p_rw_data->ndef.cur_size;
    conn_evt_data.ndef_detect.max_size = nfa_rw_cb.ndef_max_size =
        p_rw_data->ndef.max_size;
    conn_evt_data.ndef_detect.flags = p_rw_data->ndef.flags;

    if (p_rw_data->ndef.flags & RW_NDEF_FL_READ_ONLY)
      nfa_rw_cb.flags |= NFA_RW_FL_TAG_IS_READONLY;
    else
      nfa_rw_cb.flags &= ~NFA_RW_FL_TAG_IS_READONLY;

    /* Determine what operation triggered the NDEF detection procedure */
    if (nfa_rw_cb.cur_op == NFA_RW_OP_READ_NDEF) {
      /* if ndef detection was done as part of ndef-read operation, then perform
       * ndef read now */
      conn_evt_data.status = nfa_rw_start_ndef_read();
      if (conn_evt_data.status != NFA_STATUS_OK) {
        /* Failed to start NDEF Read */

        /* Command complete - perform cleanup, notify app */
        nfa_rw_command_complete();
        nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
      }
    } else if (nfa_rw_cb.cur_op == NFA_RW_OP_WRITE_NDEF) {
      /* if ndef detection was done as part of ndef-write operation, then
       * perform ndef write now */
      conn_evt_data.status = nfa_rw_start_ndef_write();
      if (conn_evt_data.status != NFA_STATUS_OK) {
        /* Failed to start NDEF Write.  */

        /* Command complete - perform cleanup, notify app */
        nfa_rw_command_complete();
        nfa_dm_act_conn_cback_notify(NFA_WRITE_CPLT_EVT, &conn_evt_data);
      }
    } else {
      /* current op was stand-alone NFA_DetectNDef. Command complete - perform
       * cleanup and notify app */
      nfa_rw_cb.cur_op = NFA_RW_OP_MAX;
      nfa_rw_command_complete();

      nfa_dm_act_conn_cback_notify(NFA_NDEF_DETECT_EVT, &conn_evt_data);
    }
  } else {
    /* NDEF detection failed... */

    /* Command complete - perform cleanup, notify app */
    nfa_rw_command_complete();
    nfa_rw_cb.ndef_st = NFA_RW_NDEF_ST_FALSE;
    conn_evt_data.status = p_rw_data->ndef.status;

    if (nfa_rw_cb.cur_op == NFA_RW_OP_READ_NDEF) {
      /* if ndef detection was done as part of ndef-read operation, then notify
       * NDEF handlers of failure */
      nfa_dm_ndef_handle_message(NFA_STATUS_FAILED, nullptr, 0);

      /* Notify app of read status */
      nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
    } else if (nfa_rw_cb.cur_op == NFA_RW_OP_WRITE_NDEF) {
      /* if ndef detection was done as part of ndef-write operation, then notify
       * app of failure */
      nfa_dm_act_conn_cback_notify(NFA_WRITE_CPLT_EVT, &conn_evt_data);
    } else if (nfa_rw_cb.cur_op == NFA_RW_OP_DETECT_NDEF) {
      conn_evt_data.ndef_detect.protocol = p_rw_data->ndef.protocol;
      /* current op was stand-alone NFA_DetectNDef. Notify app of failure */
      if (p_rw_data->ndef.status == NFC_STATUS_TIMEOUT) {
        /* Tag could have moved away */
        conn_evt_data.ndef_detect.cur_size = 0;
        conn_evt_data.ndef_detect.max_size = 0;
        conn_evt_data.ndef_detect.flags = RW_NDEF_FL_UNKNOWN;
        conn_evt_data.ndef_detect.status = NFA_STATUS_TIMEOUT;
      } else {
        /* NDEF Detection failed for other reasons */
        conn_evt_data.ndef_detect.cur_size = nfa_rw_cb.ndef_cur_size =
            p_rw_data->ndef.cur_size;
        conn_evt_data.ndef_detect.max_size = nfa_rw_cb.ndef_max_size =
            p_rw_data->ndef.max_size;
        conn_evt_data.ndef_detect.flags = p_rw_data->ndef.flags;
      }
      nfa_dm_act_conn_cback_notify(NFA_NDEF_DETECT_EVT, &conn_evt_data);
    }

    nfa_rw_cb.cur_op = NFA_RW_OP_MAX; /* clear current operation */
  }
}

/*******************************************************************************
**
** Function         nfa_rw_handle_tlv_detect
**
** Description      Handler for TLV detection reader/writer event
**
** Returns          Nothing
**
*******************************************************************************/
static void nfa_rw_handle_tlv_detect(tRW_DATA* p_rw_data) {
  tNFA_CONN_EVT_DATA conn_evt_data;

  /* Set TLV detection state */
  if (nfa_rw_cb.cur_op == NFA_RW_OP_SET_TAG_RO) {
    if (nfa_rw_cb.tlv_st == NFA_RW_TLV_DETECT_ST_OP_NOT_STARTED) {
      nfa_rw_cb.tlv_st = NFA_RW_TLV_DETECT_ST_LOCK_TLV_OP_COMPLETE;
    } else {
      nfa_rw_cb.tlv_st = NFA_RW_TLV_DETECT_ST_COMPLETE;
    }
  } else {
    if (nfa_rw_cb.cur_op == NFA_RW_OP_DETECT_LOCK_TLV) {
      nfa_rw_cb.tlv_st |= NFA_RW_TLV_DETECT_ST_LOCK_TLV_OP_COMPLETE;
    } else if (nfa_rw_cb.cur_op == NFA_RW_OP_DETECT_MEM_TLV) {
      nfa_rw_cb.tlv_st |= NFA_RW_TLV_DETECT_ST_MEM_TLV_OP_COMPLETE;
    }
  }

  /* Check if TLV detection succeeded */
  if (p_rw_data->tlv.status == NFC_STATUS_OK) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "TLV Detection succeeded: num_bytes=%i", p_rw_data->tlv.num_bytes);

    /* Store tlv properties */
    conn_evt_data.tlv_detect.status = NFA_STATUS_OK;
    conn_evt_data.tlv_detect.protocol = p_rw_data->tlv.protocol;
    conn_evt_data.tlv_detect.num_bytes = p_rw_data->tlv.num_bytes;

    /* Determine what operation triggered the TLV detection procedure */
    if (nfa_rw_cb.cur_op == NFA_RW_OP_SET_TAG_RO) {
      if (nfa_rw_config_tag_ro(nfa_rw_cb.b_hard_lock) != NFC_STATUS_OK) {
        /* Failed to set tag read only */
        conn_evt_data.tlv_detect.status = NFA_STATUS_FAILED;
        nfa_dm_act_conn_cback_notify(NFA_SET_TAG_RO_EVT, &conn_evt_data);
      }
    } else {
      /* current op was stand-alone NFA_DetectTlv. Command complete - perform
       * cleanup and notify app */
      nfa_rw_command_complete();
      nfa_dm_act_conn_cback_notify(NFA_TLV_DETECT_EVT, &conn_evt_data);
    }
  }

  /* Handle failures */
  if (p_rw_data->tlv.status != NFC_STATUS_OK) {
    /* Command complete - perform cleanup, notify the app */
    nfa_rw_command_complete();

    conn_evt_data.tlv_detect.status = NFA_STATUS_FAILED;
    if ((nfa_rw_cb.cur_op == NFA_RW_OP_DETECT_LOCK_TLV) ||
        (nfa_rw_cb.cur_op == NFA_RW_OP_DETECT_MEM_TLV)) {
      nfa_dm_act_conn_cback_notify(NFA_TLV_DETECT_EVT, &conn_evt_data);
    } else if (nfa_rw_cb.cur_op == NFA_RW_OP_SET_TAG_RO) {
      if (nfa_rw_config_tag_ro(nfa_rw_cb.b_hard_lock) != NFC_STATUS_OK) {
        /* Failed to set tag read only */
        conn_evt_data.tlv_detect.status = NFA_STATUS_FAILED;
        nfa_dm_act_conn_cback_notify(NFA_SET_TAG_RO_EVT, &conn_evt_data);
      }
    }
  }
}

/*******************************************************************************
**
** Function         nfa_rw_handle_sleep_wakeup_rsp
**
** Description      Handl sleep wakeup
**
** Returns          Nothing
**
*******************************************************************************/
void nfa_rw_handle_sleep_wakeup_rsp(tNFC_STATUS status) {
  tNFC_ACTIVATE_DEVT activate_params;
  tRW_EVENT event;

  if ((nfa_rw_cb.halt_event != RW_T2T_MAX_EVT) &&
      (nfa_rw_cb.activated_tech_mode == NFC_DISCOVERY_TYPE_POLL_A) &&
      (nfa_rw_cb.protocol == NFC_PROTOCOL_T2T) &&
      (nfa_rw_cb.pa_sel_res == NFC_SEL_RES_NFC_FORUM_T2T)) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "nfa_rw_handle_sleep_wakeup_rsp; Attempt to wake up Type 2 tag from "
        "HALT State is complete");
    if (status == NFC_STATUS_OK) {
      /* Type 2 Tag is wakeup from HALT state */
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "nfa_rw_handle_sleep_wakeup_rsp; Handle the NACK rsp received now");
      /* Initialize control block */
      activate_params.protocol = nfa_rw_cb.protocol;
      activate_params.rf_tech_param.param.pa.sel_rsp = nfa_rw_cb.pa_sel_res;
      activate_params.rf_tech_param.mode = nfa_rw_cb.activated_tech_mode;

      /* Initialize RW module */
      if ((RW_SetActivatedTagType(&activate_params, nfa_rw_cback)) !=
          NFC_STATUS_OK) {
        /* Log error (stay in NFA_RW_ST_ACTIVATED state until deactivation) */
        LOG(ERROR) << StringPrintf("RW_SetActivatedTagType failed.");
        if (nfa_rw_cb.halt_event == RW_T2T_READ_CPLT_EVT) {
          if (nfa_rw_cb.rw_data.data.p_data)
            GKI_freebuf(nfa_rw_cb.rw_data.data.p_data);
          nfa_rw_cb.rw_data.data.p_data = nullptr;
        }
        /* Do not try to detect NDEF again but just notify current operation
         * failed */
        nfa_rw_cb.halt_event = RW_T2T_MAX_EVT;
      }
    }

    /* The current operation failed with NACK rsp from type 2 tag */
    nfa_rw_cb.rw_data.status = NFC_STATUS_FAILED;
    event = nfa_rw_cb.halt_event;

    /* Got NACK rsp during presence check and legacy presence check performed */
    if (nfa_rw_cb.cur_op == NFA_RW_OP_PRESENCE_CHECK)
      nfa_rw_cb.rw_data.status = status;

    /* If cannot Sleep wakeup tag, then NDEF Detect operation is complete */
    if ((status != NFC_STATUS_OK) &&
        (nfa_rw_cb.halt_event == RW_T2T_NDEF_DETECT_EVT))
      nfa_rw_cb.halt_event = RW_T2T_MAX_EVT;

    nfa_rw_handle_t2t_evt(event, &nfa_rw_cb.rw_data);
    nfa_rw_cb.halt_event = RW_T2T_MAX_EVT;

    /* If Type 2 tag sleep wakeup failed and If in normal mode (not-exclusive RF
     * mode) then deactivate the link if sleep wakeup failed */
    if ((nfa_rw_cb.flags & NFA_RW_FL_NOT_EXCL_RF_MODE) &&
        (status != NFC_STATUS_OK)) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("Sleep wakeup failed. Deactivating...");
      nfa_dm_rf_deactivate(NFA_DEACTIVATE_TYPE_DISCOVERY);
    }
  } else {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "nfa_rw_handle_sleep_wakeup_rsp; Legacy presence check performed");
    /* Legacy presence check performed */
    nfa_rw_handle_presence_check_rsp(status);
  }
}

/*******************************************************************************
**
** Function         nfa_rw_handle_presence_check_rsp
**
** Description      Handler RW_T#t_PRESENCE_CHECK_EVT
**
** Returns          Nothing
**
*******************************************************************************/
void nfa_rw_handle_presence_check_rsp(tNFC_STATUS status) {
  NFC_HDR* p_pending_msg;

  /* Stop the presence check timer - timer may have been started when presence
   * check started */
  nfa_rw_stop_presence_check_timer();
  if (status == NFA_STATUS_OK) {
    /* Clear the BUSY flag and restart the presence-check timer */
    nfa_rw_command_complete();
  } else {
    /* If presence check failed just clear the BUSY flag */
    nfa_rw_cb.flags &= ~NFA_RW_FL_API_BUSY;
  }

  /* Handle presence check due to auto-presence-check  */
  if (nfa_rw_cb.flags & NFA_RW_FL_AUTO_PRESENCE_CHECK_BUSY) {
    nfa_rw_cb.flags &= ~NFA_RW_FL_AUTO_PRESENCE_CHECK_BUSY;

    /* If an API was called during auto-presence-check, then handle it now */
    if (nfa_rw_cb.p_pending_msg) {
      /* If NFA_RwPresenceCheck was called during auto-presence-check, notify
       * app of result */
      if (nfa_rw_cb.p_pending_msg->op_req.op == NFA_RW_OP_PRESENCE_CHECK) {
        /* Notify app of presence check status */
        tNFA_CONN_EVT_DATA nfa_conn_evt_data;
        nfa_conn_evt_data.status = status;
        nfa_dm_act_conn_cback_notify(NFA_PRESENCE_CHECK_EVT,
                                     &nfa_conn_evt_data);
        GKI_freebuf(nfa_rw_cb.p_pending_msg);
        nfa_rw_cb.p_pending_msg = nullptr;
      }
      /* For all other APIs called during auto-presence check, perform the
         command now (if tag is still present) */
      else if (status == NFC_STATUS_OK) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "Performing deferred operation after presence check...");
        p_pending_msg = (NFC_HDR*)nfa_rw_cb.p_pending_msg;
        nfa_rw_cb.p_pending_msg = nullptr;
        nfa_rw_handle_event(p_pending_msg);
        GKI_freebuf(p_pending_msg);
      } else {
        /* Tag no longer present. Free command for pending API command */
        GKI_freebuf(nfa_rw_cb.p_pending_msg);
        nfa_rw_cb.p_pending_msg = nullptr;
      }
    }

    /* Auto-presence check failed. Deactivate */
    if (status != NFC_STATUS_OK) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("Auto presence check failed. Deactivating...");
      nfa_dm_rf_deactivate(NFA_DEACTIVATE_TYPE_DISCOVERY);
    }
  }
  /* Handle presence check due to NFA_RwPresenceCheck API call */
  else {
    /* Notify app of presence check status */
    tNFA_CONN_EVT_DATA nfa_conn_evt_data;
    nfa_conn_evt_data.status = status;
    nfa_dm_act_conn_cback_notify(NFA_PRESENCE_CHECK_EVT, &nfa_conn_evt_data);

    /* If in normal mode (not-exclusive RF mode) then deactivate the link if
     * presence check failed */
    if ((nfa_rw_cb.flags & NFA_RW_FL_NOT_EXCL_RF_MODE) &&
        (nfa_conn_evt_data.status != NFC_STATUS_OK)) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("Presence check failed. Deactivating...");
      nfa_dm_rf_deactivate(NFA_DEACTIVATE_TYPE_DISCOVERY);
    }
  }
}

/*******************************************************************************
**
** Function         nfa_rw_handle_t1t_evt
**
** Description      Handler for Type-1 tag reader/writer events
**
** Returns          Nothing
**
*******************************************************************************/
static void nfa_rw_handle_t1t_evt(tRW_EVENT event, tRW_DATA* p_rw_data) {
  tNFA_CONN_EVT_DATA conn_evt_data;
  tNFA_TAG_PARAMS tag_params;
  uint8_t* p_rid_rsp;
  tNFA_STATUS activation_status;

  conn_evt_data.status = p_rw_data->data.status;
  switch (event) {
    case RW_T1T_RID_EVT:
      if (p_rw_data->data.p_data != nullptr) {
        /* Assume the data is just the response byte sequence */
        p_rid_rsp = (uint8_t*)(p_rw_data->data.p_data + 1) +
                    p_rw_data->data.p_data->offset;
        /* Fetch HR from RID response message */
        STREAM_TO_ARRAY(tag_params.t1t.hr, p_rid_rsp, T1T_HR_LEN);
        /* Fetch UID0-3 from RID response message */
        STREAM_TO_ARRAY(tag_params.t1t.uid, p_rid_rsp, T1T_CMD_UID_LEN);
        GKI_freebuf(p_rw_data->data.p_data);
        p_rw_data->data.p_data = nullptr;
      }

      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();

      if (p_rw_data->status == NFC_STATUS_TIMEOUT) {
        activation_status = NFA_STATUS_TIMEOUT;
      } else {
        activation_status = NFA_STATUS_OK;
      }

      nfa_dm_notify_activation_status(activation_status, &tag_params);
      break;

    case RW_T1T_RALL_CPLT_EVT:
    case RW_T1T_READ_CPLT_EVT:
    case RW_T1T_RSEG_CPLT_EVT:
    case RW_T1T_READ8_CPLT_EVT:
      nfa_rw_send_data_to_upper(p_rw_data);

      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
      break;

    case RW_T1T_WRITE_E_CPLT_EVT:
    case RW_T1T_WRITE_NE_CPLT_EVT:
    case RW_T1T_WRITE_E8_CPLT_EVT:
    case RW_T1T_WRITE_NE8_CPLT_EVT:
      nfa_rw_send_data_to_upper(p_rw_data);

      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_dm_act_conn_cback_notify(NFA_WRITE_CPLT_EVT, &conn_evt_data);
      break;

    case RW_T1T_TLV_DETECT_EVT:
      nfa_rw_handle_tlv_detect(p_rw_data);
      break;

    case RW_T1T_NDEF_DETECT_EVT:
      nfa_rw_cb.tlv_st = NFA_RW_TLV_DETECT_ST_COMPLETE;

      if ((p_rw_data->status != NFC_STATUS_OK) &&
          (nfa_rw_cb.cur_op == NFA_RW_OP_WRITE_NDEF) &&
          (p_rw_data->ndef.flags & NFA_RW_NDEF_FL_FORMATABLE) &&
          (!(p_rw_data->ndef.flags & NFA_RW_NDEF_FL_FORMATED)) &&
          (p_rw_data->ndef.flags & NFA_RW_NDEF_FL_SUPPORTED)) {
        /* Tag is in Initialized state, Format the tag first and then Write NDEF
         */
        if (RW_T1tFormatNDef() == NFC_STATUS_OK) break;
      }

      nfa_rw_handle_ndef_detect(p_rw_data);

      break;

    case RW_T1T_NDEF_READ_EVT:
      nfa_rw_cb.tlv_st = NFA_RW_TLV_DETECT_ST_COMPLETE;
      if (p_rw_data->status == NFC_STATUS_OK) {
        /* Process the ndef record */
        nfa_dm_ndef_handle_message(NFA_STATUS_OK, nfa_rw_cb.p_ndef_buf,
                                   nfa_rw_cb.ndef_cur_size);
      } else {
        /* Notify app of failure */
        if (nfa_rw_cb.cur_op == NFA_RW_OP_READ_NDEF) {
          /* If current operation is READ_NDEF, then notify ndef handlers of
           * failure */
          nfa_dm_ndef_handle_message(NFA_STATUS_FAILED, nullptr, 0);
        }
      }

      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);

      /* Free ndef buffer */
      nfa_rw_free_ndef_rx_buf();
      break;

    case RW_T1T_NDEF_WRITE_EVT:
      if (p_rw_data->data.status != NFA_STATUS_OK)
        nfa_rw_cb.ndef_st = NFA_RW_NDEF_ST_UNKNOWN;
      nfa_rw_cb.tlv_st = NFA_RW_TLV_DETECT_ST_COMPLETE;

      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();

      /* Notify app */
      conn_evt_data.status = (p_rw_data->data.status == NFC_STATUS_OK)
                                 ? NFA_STATUS_OK
                                 : NFA_STATUS_FAILED;
      if (nfa_rw_cb.cur_op == NFA_RW_OP_WRITE_NDEF) {
        /* Update local cursize of ndef message */
        nfa_rw_cb.ndef_cur_size = nfa_rw_cb.ndef_wr_len;
      }

      /* Notify app of ndef write complete status */
      nfa_dm_act_conn_cback_notify(NFA_WRITE_CPLT_EVT, &conn_evt_data);
      break;

    case RW_T1T_SET_TAG_RO_EVT:
      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_dm_act_conn_cback_notify(NFA_SET_TAG_RO_EVT, &conn_evt_data);
      break;

    case RW_T1T_RAW_FRAME_EVT:
      nfa_rw_send_data_to_upper(p_rw_data);
      /* Command complete - perform cleanup */
      nfa_rw_command_complete();
      break;

    case RW_T1T_PRESENCE_CHECK_EVT: /* Presence check completed */
      nfa_rw_handle_presence_check_rsp(p_rw_data->status);
      break;

    case RW_T1T_FORMAT_CPLT_EVT:

      if (p_rw_data->data.status == NFA_STATUS_OK)
        nfa_rw_cb.ndef_st = NFA_RW_NDEF_ST_UNKNOWN;

      if (nfa_rw_cb.cur_op == NFA_RW_OP_WRITE_NDEF) {
        /* if format operation was done as part of ndef-write operation, now
         * start NDEF Write */
        if ((p_rw_data->data.status != NFA_STATUS_OK) ||
            ((conn_evt_data.status = RW_T1tDetectNDef()) != NFC_STATUS_OK)) {
          /* Command complete - perform cleanup, notify app */
          nfa_rw_command_complete();
          nfa_rw_cb.ndef_st = NFA_RW_NDEF_ST_FALSE;

          /* if format operation failed or ndef detection did not start, then
           * notify app of ndef-write operation failure */
          conn_evt_data.status = NFA_STATUS_FAILED;
          nfa_dm_act_conn_cback_notify(NFA_WRITE_CPLT_EVT, &conn_evt_data);
        }
      } else {
        /* Command complete - perform cleanup, notify the app */
        nfa_rw_command_complete();
        nfa_dm_act_conn_cback_notify(NFA_FORMAT_CPLT_EVT, &conn_evt_data);
      }
      break;

    case RW_T1T_INTF_ERROR_EVT:
      nfa_dm_act_conn_cback_notify(NFA_RW_INTF_ERROR_EVT, &conn_evt_data);
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_rw_handle_t2t_evt
**
** Description      Handler for Type-2 tag reader/writer events
**
** Returns          Nothing
**
*******************************************************************************/
static void nfa_rw_handle_t2t_evt(tRW_EVENT event, tRW_DATA* p_rw_data) {
  tNFA_CONN_EVT_DATA conn_evt_data;

  conn_evt_data.status = p_rw_data->status;

  if (p_rw_data->status == NFC_STATUS_REJECTED) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "; Waking the tag first before handling the "
        "response!");
    /* Received NACK. Let DM wakeup the tag first (by putting tag to sleep and
     * then waking it up) */
    p_rw_data->status = nfa_dm_disc_sleep_wakeup();
    if (p_rw_data->status == NFC_STATUS_OK) {
      nfa_rw_cb.halt_event = event;
      memcpy(&nfa_rw_cb.rw_data, p_rw_data, sizeof(tRW_DATA));
      return;
    }
  }

  switch (event) {
    case RW_T2T_READ_CPLT_EVT: /* Read completed          */
      nfa_rw_send_data_to_upper(p_rw_data);
      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
      break;

    case RW_T2T_WRITE_CPLT_EVT: /* Write completed         */
      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_dm_act_conn_cback_notify(NFA_WRITE_CPLT_EVT, &conn_evt_data);
      break;

    case RW_T2T_SELECT_CPLT_EVT: /* Sector select completed */
      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_dm_act_conn_cback_notify(NFA_SELECT_CPLT_EVT, &conn_evt_data);
      break;

    case RW_T2T_NDEF_DETECT_EVT: /* NDEF detection complete */
      if ((p_rw_data->status == NFC_STATUS_OK) ||
          ((p_rw_data->status == NFC_STATUS_FAILED) &&
           ((p_rw_data->ndef.flags == NFA_RW_NDEF_FL_UNKNOWN) ||
            (nfa_rw_cb.halt_event == RW_T2T_MAX_EVT))) ||
          (nfa_rw_cb.skip_dyn_locks == true)) {
        /* NDEF Detection is complete */
        nfa_rw_cb.skip_dyn_locks = false;
        nfa_rw_handle_ndef_detect(p_rw_data);
      } else {
        /* Try to detect NDEF again, this time without reading dynamic lock
         * bytes */
        nfa_rw_cb.skip_dyn_locks = true;
        nfa_rw_detect_ndef();
      }
      break;

    case RW_T2T_TLV_DETECT_EVT: /* Lock control/Mem/Prop tlv detection complete
                                   */
      nfa_rw_handle_tlv_detect(p_rw_data);
      break;

    case RW_T2T_NDEF_READ_EVT: /* NDEF read completed     */
      if (p_rw_data->status == NFC_STATUS_OK) {
        /* Process the ndef record */
        nfa_dm_ndef_handle_message(NFA_STATUS_OK, nfa_rw_cb.p_ndef_buf,
                                   nfa_rw_cb.ndef_cur_size);
      } else {
        /* Notify app of failure */
        if (nfa_rw_cb.cur_op == NFA_RW_OP_READ_NDEF) {
          /* If current operation is READ_NDEF, then notify ndef handlers of
           * failure */
          nfa_dm_ndef_handle_message(NFA_STATUS_FAILED, nullptr, 0);
        }
      }

      /* Notify app of read status */
      conn_evt_data.status = p_rw_data->status;
      nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
      /* Free ndef buffer */
      nfa_rw_free_ndef_rx_buf();

      /* Command complete - perform cleanup */
      nfa_rw_command_complete();
      break;

    case RW_T2T_NDEF_WRITE_EVT: /* NDEF write complete     */

      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();

      /* Notify app */
      conn_evt_data.status = (p_rw_data->data.status == NFC_STATUS_OK)
                                 ? NFA_STATUS_OK
                                 : NFA_STATUS_FAILED;
      if (nfa_rw_cb.cur_op == NFA_RW_OP_WRITE_NDEF) {
        /* Update local cursize of ndef message */
        nfa_rw_cb.ndef_cur_size = nfa_rw_cb.ndef_wr_len;
      }

      /* Notify app of ndef write complete status */
      nfa_dm_act_conn_cback_notify(NFA_WRITE_CPLT_EVT, &conn_evt_data);

      break;

    case RW_T2T_SET_TAG_RO_EVT:
      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_dm_act_conn_cback_notify(NFA_SET_TAG_RO_EVT, &conn_evt_data);
      break;

    case RW_T2T_RAW_FRAME_EVT:
      nfa_rw_send_data_to_upper(p_rw_data);
      /* Command complete - perform cleanup */
      if (p_rw_data->status != NFC_STATUS_CONTINUE) {
        nfa_rw_command_complete();
      }
      break;

    case RW_T2T_PRESENCE_CHECK_EVT: /* Presence check completed */
      nfa_rw_handle_presence_check_rsp(p_rw_data->status);
      break;

    case RW_T2T_FORMAT_CPLT_EVT:
      if (p_rw_data->data.status == NFA_STATUS_OK)
        nfa_rw_cb.ndef_st = NFA_RW_NDEF_ST_UNKNOWN;

      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_dm_act_conn_cback_notify(NFA_FORMAT_CPLT_EVT, &conn_evt_data);
      break;

    case RW_T2T_INTF_ERROR_EVT:
      nfa_dm_act_conn_cback_notify(NFA_RW_INTF_ERROR_EVT, &conn_evt_data);
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_rw_handle_t3t_evt
**
** Description      Handler for Type-3 tag reader/writer events
**
** Returns          Nothing
**
*******************************************************************************/
static void nfa_rw_handle_t3t_evt(tRW_EVENT event, tRW_DATA* p_rw_data) {
  tNFA_CONN_EVT_DATA conn_evt_data;
  tNFA_TAG_PARAMS tag_params;

  switch (event) {
    case RW_T3T_NDEF_DETECT_EVT: /* NDEF detection complete */
      nfa_rw_handle_ndef_detect(p_rw_data);
      break;

    case RW_T3T_UPDATE_CPLT_EVT: /* Write completed */
      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();

      /* Notify app */
      conn_evt_data.status = (p_rw_data->data.status == NFC_STATUS_OK)
                                 ? NFA_STATUS_OK
                                 : NFA_STATUS_FAILED;
      if (nfa_rw_cb.cur_op == NFA_RW_OP_WRITE_NDEF) {
        /* Update local cursize of ndef message */
        nfa_rw_cb.ndef_cur_size = nfa_rw_cb.ndef_wr_len;
      }

      /* Notify app of ndef write complete status */
      nfa_dm_act_conn_cback_notify(NFA_WRITE_CPLT_EVT, &conn_evt_data);

      break;

    case RW_T3T_CHECK_CPLT_EVT: /* Read completed */
      if (p_rw_data->status == NFC_STATUS_OK) {
        /* Process the ndef record */
        nfa_dm_ndef_handle_message(NFA_STATUS_OK, nfa_rw_cb.p_ndef_buf,
                                   nfa_rw_cb.ndef_cur_size);
      } else {
        /* Notify app of failure */
        if (nfa_rw_cb.cur_op == NFA_RW_OP_READ_NDEF) {
          /* If current operation is READ_NDEF, then notify ndef handlers of
           * failure */
          nfa_dm_ndef_handle_message(NFA_STATUS_FAILED, nullptr, 0);
        }
      }

      /* Free ndef buffer */
      nfa_rw_free_ndef_rx_buf();

      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      conn_evt_data.status = p_rw_data->status;
      nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
      break;

    case RW_T3T_CHECK_EVT: /* Segment of data received from type 3 tag */
      if (nfa_rw_cb.cur_op == NFA_RW_OP_READ_NDEF) {
        nfa_rw_store_ndef_rx_buf(p_rw_data);
      } else {
        nfa_rw_send_data_to_upper(p_rw_data);
      }
      break;

    case RW_T3T_RAW_FRAME_EVT: /* SendRawFrame response */
      nfa_rw_send_data_to_upper(p_rw_data);

      if (p_rw_data->status != NFC_STATUS_CONTINUE) {
        /* Command complete - perform cleanup */
        nfa_rw_command_complete();
      }
      break;

    case RW_T3T_PRESENCE_CHECK_EVT: /* Presence check completed */
      nfa_rw_handle_presence_check_rsp(p_rw_data->status);
      break;

    case RW_T3T_GET_SYSTEM_CODES_EVT: /* Presence check completed */
      /* Command complete - perform cleanup */
      nfa_rw_command_complete();

      /* System codes retrieved - notify app of ACTIVATION */
      if (p_rw_data->status == NFC_STATUS_OK) {
        tag_params.t3t.num_system_codes = p_rw_data->t3t_sc.num_system_codes;
        tag_params.t3t.p_system_codes = p_rw_data->t3t_sc.p_system_codes;
      } else {
        tag_params.t3t.num_system_codes = 0;
        tag_params.t3t.p_system_codes = nullptr;
      }

      nfa_dm_notify_activation_status(NFA_STATUS_OK, &tag_params);
      break;

    case RW_T3T_FORMAT_CPLT_EVT: /* Format completed */
      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();

      /* Notify app */
      conn_evt_data.status = (p_rw_data->data.status == NFC_STATUS_OK)
                                 ? NFA_STATUS_OK
                                 : NFA_STATUS_FAILED;

      /* Notify app of ndef write complete status */
      nfa_dm_act_conn_cback_notify(NFA_FORMAT_CPLT_EVT, &conn_evt_data);
      break;

    case RW_T3T_INTF_ERROR_EVT:
      conn_evt_data.status = p_rw_data->status;
      nfa_dm_act_conn_cback_notify(NFA_RW_INTF_ERROR_EVT, &conn_evt_data);
      break;

    case RW_T3T_SET_READ_ONLY_CPLT_EVT:
      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();

      conn_evt_data.status = p_rw_data->status;
      nfa_dm_act_conn_cback_notify(NFA_SET_TAG_RO_EVT, &conn_evt_data);
      break;

    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("; Unhandled RW event 0x%X", event);
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_rw_handle_t4t_evt
**
** Description      Handler for Type-4 tag reader/writer events
**
** Returns          Nothing
**
*******************************************************************************/
static void nfa_rw_handle_t4t_evt(tRW_EVENT event, tRW_DATA* p_rw_data) {
  tNFA_CONN_EVT_DATA conn_evt_data;

  switch (event) {
    case RW_T4T_NDEF_DETECT_EVT: /* Result of NDEF detection procedure */
      nfa_rw_handle_ndef_detect(p_rw_data);
      break;

    case RW_T4T_NDEF_FORMAT_CPLT_EVT:
      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_rw_cb.cur_op = NFA_RW_OP_MAX;
      nfa_rw_cb.ndef_cur_size = p_rw_data->ndef.cur_size;
      nfa_rw_cb.ndef_max_size = p_rw_data->ndef.max_size;
      conn_evt_data.status = (p_rw_data->status == NFC_STATUS_OK)
                                 ? NFA_STATUS_OK
                                 : NFA_STATUS_FAILED;

      nfa_dm_act_conn_cback_notify(NFA_FORMAT_CPLT_EVT, &conn_evt_data);
      break;

    case RW_T4T_NDEF_READ_EVT: /* Segment of data received from type 4 tag */
      if (nfa_rw_cb.cur_op == NFA_RW_OP_READ_NDEF) {
        nfa_rw_store_ndef_rx_buf(p_rw_data);
      } else {
        nfa_rw_send_data_to_upper(p_rw_data);
      }
      break;

    case RW_T4T_NDEF_READ_CPLT_EVT: /* Read operation completed           */
      if (nfa_rw_cb.cur_op == NFA_RW_OP_READ_NDEF) {
        nfa_rw_store_ndef_rx_buf(p_rw_data);

        /* Process the ndef record */
        nfa_dm_ndef_handle_message(NFA_STATUS_OK, nfa_rw_cb.p_ndef_buf,
                                   nfa_rw_cb.ndef_cur_size);

        /* Free ndef buffer */
        nfa_rw_free_ndef_rx_buf();
      } else {
        nfa_rw_send_data_to_upper(p_rw_data);
      }

      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_rw_cb.cur_op = NFA_RW_OP_MAX;
      conn_evt_data.status = NFC_STATUS_OK;
      nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
      break;

    case RW_T4T_NDEF_READ_FAIL_EVT: /* Read operation failed              */
      if (nfa_rw_cb.cur_op == NFA_RW_OP_READ_NDEF) {
        /* If current operation is READ_NDEF, then notify ndef handlers of
         * failure */
        nfa_dm_ndef_handle_message(NFA_STATUS_FAILED, nullptr, 0);

        /* Free ndef buffer */
        nfa_rw_free_ndef_rx_buf();
      }

      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_rw_cb.cur_op = NFA_RW_OP_MAX;
      conn_evt_data.status = NFA_STATUS_FAILED;
      nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
      break;

    case RW_T4T_NDEF_UPDATE_CPLT_EVT: /* Update operation completed         */
    case RW_T4T_NDEF_UPDATE_FAIL_EVT: /* Update operation failed            */

      if (nfa_rw_cb.cur_op == NFA_RW_OP_WRITE_NDEF) {
        /* Update local cursize of ndef message */
        nfa_rw_cb.ndef_cur_size = nfa_rw_cb.ndef_wr_len;
      }

      /* Notify app */
      if (event == RW_T4T_NDEF_UPDATE_CPLT_EVT)
        conn_evt_data.status = NFA_STATUS_OK;
      else
        conn_evt_data.status = NFA_STATUS_FAILED;

      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_rw_cb.cur_op = NFA_RW_OP_MAX;
      nfa_dm_act_conn_cback_notify(NFA_WRITE_CPLT_EVT, &conn_evt_data);
      break;

    case RW_T4T_RAW_FRAME_EVT: /* Raw Frame data event         */
      nfa_rw_send_data_to_upper(p_rw_data);

      if (p_rw_data->status != NFC_STATUS_CONTINUE) {
        /* Command complete - perform cleanup */
        nfa_rw_command_complete();
        nfa_rw_cb.cur_op = NFA_RW_OP_MAX;
      }
      break;

    case RW_T4T_SET_TO_RO_EVT: /* Tag is set as read only          */
      conn_evt_data.status = p_rw_data->status;
      nfa_dm_act_conn_cback_notify(NFA_SET_TAG_RO_EVT, &conn_evt_data);

      nfa_rw_command_complete();
      nfa_rw_cb.cur_op = NFA_RW_OP_MAX;
      break;

    case RW_T4T_INTF_ERROR_EVT: /* RF Interface error event         */
      conn_evt_data.status = p_rw_data->status;
#if (NXP_EXTNS == TRUE)
     if((!appl_dta_mode_flag) && (conn_evt_data.status == NFC_STATUS_RF_PROTOCOL_ERR))
       nfa_dm_act_conn_cback_notify(NFA_RW_INTF_ERROR_EVT, &conn_evt_data);
     else
       nfa_dm_rf_deactivate(NFA_DEACTIVATE_TYPE_DISCOVERY);
#else
      nfa_dm_act_conn_cback_notify(NFA_RW_INTF_ERROR_EVT, &conn_evt_data);
#endif
      nfa_rw_command_complete();
      nfa_rw_cb.cur_op = NFA_RW_OP_MAX;
      break;

    case RW_T4T_PRESENCE_CHECK_EVT: /* Presence check completed */
      nfa_rw_handle_presence_check_rsp(p_rw_data->status);
      break;

    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("; Unhandled RW event 0x%X", event);
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_rw_handle_i93_evt
**
** Description      Handler for ISO 15693 tag reader/writer events
**
** Returns          Nothing
**
*******************************************************************************/
static void nfa_rw_handle_i93_evt(tRW_EVENT event, tRW_DATA* p_rw_data) {
  tNFA_CONN_EVT_DATA conn_evt_data;
  tNFA_TAG_PARAMS i93_params;

  switch (event) {
    case RW_I93_NDEF_DETECT_EVT: /* Result of NDEF detection procedure */
      nfa_rw_handle_ndef_detect(p_rw_data);
      break;

    case RW_I93_NDEF_READ_EVT: /* Segment of data received from type 4 tag */
      if (nfa_rw_cb.cur_op == NFA_RW_OP_READ_NDEF) {
        nfa_rw_store_ndef_rx_buf(p_rw_data);
      } else {
        nfa_rw_send_data_to_upper(p_rw_data);
      }
      break;

    case RW_I93_NDEF_READ_CPLT_EVT: /* Read operation completed           */
      if (nfa_rw_cb.cur_op == NFA_RW_OP_READ_NDEF) {
        nfa_rw_store_ndef_rx_buf(p_rw_data);

        /* Process the ndef record */
        nfa_dm_ndef_handle_message(NFA_STATUS_OK, nfa_rw_cb.p_ndef_buf,
                                   nfa_rw_cb.ndef_cur_size);

        /* Free ndef buffer */
        nfa_rw_free_ndef_rx_buf();
      } else {
        nfa_rw_send_data_to_upper(p_rw_data);
      }

      /* Command complete - perform cleanup, notify app */
      nfa_rw_command_complete();
      nfa_rw_cb.cur_op = NFA_RW_OP_MAX; /* clear current operation */
      conn_evt_data.status = NFC_STATUS_OK;
      nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
      break;

    case RW_I93_NDEF_READ_FAIL_EVT: /* Read operation failed              */
      if (nfa_rw_cb.cur_op == NFA_RW_OP_READ_NDEF) {
        /* If current operation is READ_NDEF, then notify ndef handlers of
         * failure */
        nfa_dm_ndef_handle_message(NFA_STATUS_FAILED, nullptr, 0);

        /* Free ndef buffer */
        nfa_rw_free_ndef_rx_buf();
      }

      /* Command complete - perform cleanup, notify app */
      nfa_rw_command_complete();
      nfa_rw_cb.cur_op = NFA_RW_OP_MAX; /* clear current operation */
      conn_evt_data.status = NFA_STATUS_FAILED;
      nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
      break;

    case RW_I93_NDEF_UPDATE_CPLT_EVT: /* Update operation completed         */
    case RW_I93_NDEF_UPDATE_FAIL_EVT: /* Update operation failed            */

      if (nfa_rw_cb.cur_op == NFA_RW_OP_WRITE_NDEF) {
        /* Update local cursize of ndef message */
        nfa_rw_cb.ndef_cur_size = nfa_rw_cb.ndef_wr_len;
      }

      /* Command complete - perform cleanup, notify app */
      nfa_rw_command_complete();
      nfa_rw_cb.cur_op = NFA_RW_OP_MAX; /* clear current operation */

      if (event == RW_I93_NDEF_UPDATE_CPLT_EVT)
        conn_evt_data.status = NFA_STATUS_OK;
      else
        conn_evt_data.status = NFA_STATUS_FAILED;

      /* Notify app of ndef write complete status */
      nfa_dm_act_conn_cback_notify(NFA_WRITE_CPLT_EVT, &conn_evt_data);
      break;

    case RW_I93_RAW_FRAME_EVT: /* Raw Frame data event         */
      nfa_rw_send_data_to_upper(p_rw_data);
      if (p_rw_data->status != NFC_STATUS_CONTINUE) {
        /* Command complete - perform cleanup */
        nfa_rw_command_complete();
      }
      break;

    case RW_I93_INTF_ERROR_EVT: /* RF Interface error event         */
      /* Command complete - perform cleanup, notify app */
      nfa_rw_command_complete();

      if (nfa_rw_cb.flags & NFA_RW_FL_ACTIVATION_NTF_PENDING) {
        nfa_rw_cb.flags &= ~NFA_RW_FL_ACTIVATION_NTF_PENDING;

        memset(&i93_params, 0x00, sizeof(tNFA_TAG_PARAMS));
        memcpy(i93_params.i93.uid, nfa_rw_cb.i93_uid, I93_UID_BYTE_LEN);

        nfa_dm_notify_activation_status(NFA_STATUS_OK, &i93_params);
      } else {
        conn_evt_data.status = p_rw_data->status;
        nfa_dm_act_conn_cback_notify(NFA_RW_INTF_ERROR_EVT, &conn_evt_data);
      }

      nfa_rw_cb.cur_op = NFA_RW_OP_MAX; /* clear current operation */
      break;

    case RW_I93_PRESENCE_CHECK_EVT: /* Presence check completed */
      nfa_rw_handle_presence_check_rsp(p_rw_data->status);
      break;

    case RW_I93_FORMAT_CPLT_EVT: /* Format procedure complete          */
      if (p_rw_data->data.status == NFA_STATUS_OK)
        nfa_rw_cb.ndef_st = NFA_RW_NDEF_ST_UNKNOWN;

      /* Command complete - perform cleanup, notify app */
      nfa_rw_command_complete();
      nfa_rw_cb.cur_op = NFA_RW_OP_MAX; /* clear current operation */
      conn_evt_data.status = p_rw_data->status;
      nfa_dm_act_conn_cback_notify(NFA_FORMAT_CPLT_EVT, &conn_evt_data);
      break;

    case RW_I93_SET_TAG_RO_EVT: /* Set read-only procedure complete   */
      nfa_rw_cb.flags |= NFA_RW_FL_TAG_IS_READONLY;

      /* Command complete - perform cleanup, notify app */
      nfa_rw_command_complete();
      nfa_rw_cb.cur_op = NFA_RW_OP_MAX; /* clear current operation */
      conn_evt_data.status = p_rw_data->status;
      nfa_dm_act_conn_cback_notify(NFA_SET_TAG_RO_EVT, &conn_evt_data);
      break;

    case RW_I93_INVENTORY_EVT: /* Response of Inventory              */

      /* Command complete - perform cleanup, notify app */
      nfa_rw_command_complete();

      conn_evt_data.i93_cmd_cplt.status = p_rw_data->i93_inventory.status;
      conn_evt_data.i93_cmd_cplt.sent_command = I93_CMD_INVENTORY;

      conn_evt_data.i93_cmd_cplt.params.inventory.dsfid =
          p_rw_data->i93_inventory.dsfid;
      memcpy(conn_evt_data.i93_cmd_cplt.params.inventory.uid,
             p_rw_data->i93_inventory.uid, I93_UID_BYTE_LEN);

      nfa_dm_act_conn_cback_notify(NFA_I93_CMD_CPLT_EVT, &conn_evt_data);

      nfa_rw_cb.cur_op = NFA_RW_OP_MAX; /* clear current operation */
      break;

    case RW_I93_DATA_EVT: /* Response of Read, Get Multi Security */

      /* Command complete - perform cleanup, notify app */
      nfa_rw_command_complete();

      conn_evt_data.data.p_data = (uint8_t*)(p_rw_data->i93_data.p_data + 1) +
                                  p_rw_data->i93_data.p_data->offset;

      if (nfa_rw_cb.flags & NFA_RW_FL_ACTIVATION_NTF_PENDING) {
        nfa_rw_cb.flags &= ~NFA_RW_FL_ACTIVATION_NTF_PENDING;

        i93_params.i93.info_flags =
            (I93_INFO_FLAG_DSFID | I93_INFO_FLAG_MEM_SIZE | I93_INFO_FLAG_AFI);
        i93_params.i93.afi =
            *(conn_evt_data.data.p_data +
              nfa_rw_cb.i93_afi_location % nfa_rw_cb.i93_block_size);
        i93_params.i93.dsfid = nfa_rw_cb.i93_dsfid;
        i93_params.i93.block_size = nfa_rw_cb.i93_block_size;
        i93_params.i93.num_block = nfa_rw_cb.i93_num_block;
        memcpy(i93_params.i93.uid, nfa_rw_cb.i93_uid, I93_UID_BYTE_LEN);

        nfa_dm_notify_activation_status(NFA_STATUS_OK, &i93_params);
      } else {
        conn_evt_data.data.len = p_rw_data->i93_data.p_data->len;

        nfa_dm_act_conn_cback_notify(NFA_DATA_EVT, &conn_evt_data);
      }

      GKI_freebuf(p_rw_data->i93_data.p_data);
      p_rw_data->i93_data.p_data = nullptr;

      nfa_rw_cb.cur_op = NFA_RW_OP_MAX; /* clear current operation */
      break;

    case RW_I93_SYS_INFO_EVT: /* Response of System Information     */

      /* Command complete - perform cleanup, notify app */
      nfa_rw_command_complete();

      if (nfa_rw_cb.flags & NFA_RW_FL_ACTIVATION_NTF_PENDING) {
        nfa_rw_cb.flags &= ~NFA_RW_FL_ACTIVATION_NTF_PENDING;

        nfa_rw_cb.i93_block_size = p_rw_data->i93_sys_info.block_size;
        nfa_rw_cb.i93_num_block = p_rw_data->i93_sys_info.num_block;

        i93_params.i93.info_flags = p_rw_data->i93_sys_info.info_flags;
        i93_params.i93.dsfid = p_rw_data->i93_sys_info.dsfid;
        i93_params.i93.afi = p_rw_data->i93_sys_info.afi;
        i93_params.i93.num_block = p_rw_data->i93_sys_info.num_block;
        i93_params.i93.block_size = p_rw_data->i93_sys_info.block_size;
        i93_params.i93.IC_reference = p_rw_data->i93_sys_info.IC_reference;
        memcpy(i93_params.i93.uid, p_rw_data->i93_sys_info.uid,
               I93_UID_BYTE_LEN);

        nfa_dm_notify_activation_status(NFA_STATUS_OK, &i93_params);
      } else {
        conn_evt_data.i93_cmd_cplt.status = p_rw_data->i93_sys_info.status;
        conn_evt_data.i93_cmd_cplt.sent_command = I93_CMD_GET_SYS_INFO;

        conn_evt_data.i93_cmd_cplt.params.sys_info.info_flags =
            p_rw_data->i93_sys_info.info_flags;
        memcpy(conn_evt_data.i93_cmd_cplt.params.sys_info.uid,
               p_rw_data->i93_sys_info.uid, I93_UID_BYTE_LEN);
        conn_evt_data.i93_cmd_cplt.params.sys_info.dsfid =
            p_rw_data->i93_sys_info.dsfid;
        conn_evt_data.i93_cmd_cplt.params.sys_info.afi =
            p_rw_data->i93_sys_info.afi;
        conn_evt_data.i93_cmd_cplt.params.sys_info.num_block =
            p_rw_data->i93_sys_info.num_block;
        conn_evt_data.i93_cmd_cplt.params.sys_info.block_size =
            p_rw_data->i93_sys_info.block_size;
        conn_evt_data.i93_cmd_cplt.params.sys_info.IC_reference =
            p_rw_data->i93_sys_info.IC_reference;

        /* store tag memory information for writing blocks */
        nfa_rw_cb.i93_block_size = p_rw_data->i93_sys_info.block_size;
        nfa_rw_cb.i93_num_block = p_rw_data->i93_sys_info.num_block;

        nfa_dm_act_conn_cback_notify(NFA_I93_CMD_CPLT_EVT, &conn_evt_data);
      }

      nfa_rw_cb.cur_op = NFA_RW_OP_MAX; /* clear current operation */
      break;

    case RW_I93_CMD_CMPL_EVT: /* Command complete                   */
      /* Command complete - perform cleanup, notify app */
      nfa_rw_command_complete();

      if (nfa_rw_cb.flags & NFA_RW_FL_ACTIVATION_NTF_PENDING) {
        /* Reader got error code from tag */

        nfa_rw_cb.flags &= ~NFA_RW_FL_ACTIVATION_NTF_PENDING;

        memset(&i93_params, 0x00, sizeof(i93_params));
        memcpy(i93_params.i93.uid, nfa_rw_cb.i93_uid, I93_UID_BYTE_LEN);

        nfa_dm_notify_activation_status(NFA_STATUS_OK, &i93_params);
      } else {
        conn_evt_data.i93_cmd_cplt.status = p_rw_data->i93_cmd_cmpl.status;
        conn_evt_data.i93_cmd_cplt.sent_command =
            p_rw_data->i93_cmd_cmpl.command;

        if (conn_evt_data.i93_cmd_cplt.status != NFC_STATUS_OK)
          conn_evt_data.i93_cmd_cplt.params.error_code =
              p_rw_data->i93_cmd_cmpl.error_code;

        nfa_dm_act_conn_cback_notify(NFA_I93_CMD_CPLT_EVT, &conn_evt_data);
      }

      nfa_rw_cb.cur_op = NFA_RW_OP_MAX; /* clear current operation */
      break;

    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("; Unhandled RW event 0x%X", event);
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_rw_handle_mfc_evt
**
** Description      Handler for Mifare Classic tag reader/writer events
**
** Returns          Nothing
**
*******************************************************************************/
static void nfa_rw_handle_mfc_evt(tRW_EVENT event, tRW_DATA* p_rw_data) {
  tNFA_CONN_EVT_DATA conn_evt_data;

  conn_evt_data.status = p_rw_data->status;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_rw_handle_mfc_evt() event = 0x%X", event);

  switch (event) {
    /* Read completed */
    case RW_MFC_NDEF_READ_CPLT_EVT:
      nfa_rw_send_data_to_upper(p_rw_data);
      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
      break;

    /* NDEF detection complete */
    case RW_MFC_NDEF_DETECT_EVT:
      nfa_rw_handle_ndef_detect(p_rw_data);
      break;

    /* NDEF read completed */
    case RW_MFC_NDEF_READ_EVT:
      if (p_rw_data->status == NFC_STATUS_OK) {
        /* Process the ndef record */
        nfa_dm_ndef_handle_message(NFA_STATUS_OK, nfa_rw_cb.p_ndef_buf,
                                   nfa_rw_cb.ndef_cur_size);
      } else {
        /* Notify app of failure */
        if (nfa_rw_cb.cur_op == NFA_RW_OP_READ_NDEF) {
          /* If current operation is READ_NDEF, then notify ndef handlers of
           * failure */
          nfa_dm_ndef_handle_message(NFA_STATUS_FAILED, NULL, 0);
        }
      }

      /* Notify app of read status */
      conn_evt_data.status = p_rw_data->status;
      nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
      /* Free ndef buffer */
      nfa_rw_free_ndef_rx_buf();

      /* Command complete - perform cleanup */
      nfa_rw_command_complete();
      break;

    /* Raw Frame data event */
    case RW_MFC_RAW_FRAME_EVT:
      nfa_rw_send_data_to_upper(p_rw_data);

      if (p_rw_data->status != NFC_STATUS_CONTINUE) {
        /* Command complete - perform cleanup */
        nfa_rw_command_complete();
        nfa_rw_cb.cur_op = NFA_RW_OP_MAX;
      }
      break;

    /* RF Interface error event */
    case RW_MFC_INTF_ERROR_EVT:
      nfa_dm_act_conn_cback_notify(NFA_RW_INTF_ERROR_EVT, &conn_evt_data);
      break;

    case RW_MFC_NDEF_FORMAT_CPLT_EVT:
      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_dm_act_conn_cback_notify(NFA_FORMAT_CPLT_EVT, &conn_evt_data);
      break;

    /* NDEF write completed or failed*/
    case RW_MFC_NDEF_WRITE_CPLT_EVT:
    case RW_MFC_NDEF_WRITE_FAIL_EVT:
      /* Command complete - perform cleanup, notify the app */
      nfa_rw_command_complete();
      nfa_dm_act_conn_cback_notify(NFA_WRITE_CPLT_EVT, &conn_evt_data);
      break;

    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("; Unhandled RW event 0x%X", event);
  }
}

/*******************************************************************************
**
** Function         nfa_rw_cback
**
** Description      Callback for reader/writer event notification
**
** Returns          Nothing
**
*******************************************************************************/
static void nfa_rw_cback(tRW_EVENT event, tRW_DATA* p_rw_data) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_rw_cback: event=0x%02x", event);

  /* Call appropriate event handler for tag type */
  if (event < RW_T1T_MAX_EVT) {
    /* Handle Type-1 tag events */
    nfa_rw_handle_t1t_evt(event, p_rw_data);
  } else if (event < RW_T2T_MAX_EVT) {
    /* Handle Type-2 tag events */
    nfa_rw_handle_t2t_evt(event, p_rw_data);
  } else if (event < RW_T3T_MAX_EVT) {
    /* Handle Type-3 tag events */
    nfa_rw_handle_t3t_evt(event, p_rw_data);
  } else if (event < RW_T4T_MAX_EVT) {
    /* Handle Type-4 tag events */
    nfa_rw_handle_t4t_evt(event, p_rw_data);
  } else if (event < RW_I93_MAX_EVT) {
    /* Handle ISO 15693 tag events */
    nfa_rw_handle_i93_evt(event, p_rw_data);
  } else if (event < RW_MFC_MAX_EVT) {
    /* Handle Mifare Classic tag events */
    nfa_rw_handle_mfc_evt(event, p_rw_data);
  } else {
    LOG(ERROR) << StringPrintf("nfa_rw_cback: unhandled event=0x%02x", event);
  }
}

/*******************************************************************************
**
** Function         nfa_rw_start_ndef_detection
**
** Description      Start NDEF detection on activated tag
**
** Returns          Nothing
**
*******************************************************************************/
static tNFC_STATUS nfa_rw_start_ndef_detection(void) {
  tNFC_PROTOCOL protocol = nfa_rw_cb.protocol;
  tNFC_STATUS status = NFC_STATUS_FAILED;

  if (NFC_PROTOCOL_T1T == protocol) {
    /* Type1Tag    - NFC-A */
    status = RW_T1tDetectNDef();
  } else if (NFC_PROTOCOL_T2T == protocol) {
    /* Type2Tag    - NFC-A */
    if (nfa_rw_cb.pa_sel_res == NFC_SEL_RES_NFC_FORUM_T2T) {
      status = RW_T2tDetectNDef(nfa_rw_cb.skip_dyn_locks);
    }
  } else if (NFC_PROTOCOL_T3T == protocol) {
    /* Type3Tag    - NFC-F */
    status = RW_T3tDetectNDef();
  } else if (NFC_PROTOCOL_ISO_DEP == protocol) {
    /* ISODEP/4A,4B- NFC-A or NFC-B */
    status = RW_T4tDetectNDef();
  } else if (NFC_PROTOCOL_T5T == protocol) {
    /* ISO 15693 */
    status = RW_I93DetectNDef();
  } else if (NFC_PROTOCOL_MIFARE == protocol) {
    status = RW_MfcDetectNDef();
  }

  return (status);
}

/*******************************************************************************
**
** Function         nfa_rw_start_ndef_read
**
** Description      Start NDEF read on activated tag
**
** Returns          Nothing
**
*******************************************************************************/
static tNFC_STATUS nfa_rw_start_ndef_read(void) {
  tNFC_PROTOCOL protocol = nfa_rw_cb.protocol;
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tNFA_CONN_EVT_DATA conn_evt_data;

  /* Handle zero length NDEF message */
  if (nfa_rw_cb.ndef_cur_size == 0) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("NDEF message is zero-length");

    /* Send zero-lengh NDEF message to ndef callback */
    nfa_dm_ndef_handle_message(NFA_STATUS_OK, nullptr, 0);

    /* Command complete - perform cleanup, notify app */
    nfa_rw_command_complete();
    conn_evt_data.status = NFA_STATUS_OK;
    nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
    return NFC_STATUS_OK;
  }

  /* Allocate buffer for incoming NDEF message (free previous NDEF rx buffer, if
   * needed) */
  nfa_rw_free_ndef_rx_buf();
  nfa_rw_cb.p_ndef_buf = (uint8_t*)nfa_mem_co_alloc(nfa_rw_cb.ndef_cur_size);
  if (nfa_rw_cb.p_ndef_buf == nullptr) {
    LOG(ERROR) << StringPrintf(
        "Unable to allocate a buffer for reading NDEF (size=%i)",
        nfa_rw_cb.ndef_cur_size);

    /* Command complete - perform cleanup, notify app */
    nfa_rw_command_complete();
    conn_evt_data.status = NFA_STATUS_FAILED;
    nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
    return NFC_STATUS_FAILED;
  }
  nfa_rw_cb.ndef_rd_offset = 0;

  if (NFC_PROTOCOL_T1T == protocol) {
    /* Type1Tag    - NFC-A */
    status =
        RW_T1tReadNDef(nfa_rw_cb.p_ndef_buf, (uint16_t)nfa_rw_cb.ndef_cur_size);
  } else if (NFC_PROTOCOL_T2T == protocol) {
    /* Type2Tag    - NFC-A */
    if (nfa_rw_cb.pa_sel_res == NFC_SEL_RES_NFC_FORUM_T2T) {
      status = RW_T2tReadNDef(nfa_rw_cb.p_ndef_buf,
                              (uint16_t)nfa_rw_cb.ndef_cur_size);
    }
  } else if (NFC_PROTOCOL_T3T == protocol) {
    /* Type3Tag    - NFC-F */
    status = RW_T3tCheckNDef();
  } else if (NFC_PROTOCOL_ISO_DEP == protocol) {
    /* ISODEP/4A,4B- NFC-A or NFC-B */
    status = RW_T4tReadNDef();
  } else if (NFC_PROTOCOL_T5T == protocol) {
    /* ISO 15693 */
    status = RW_I93ReadNDef();
  } else if (NFC_PROTOCOL_MIFARE == protocol) {
    /* Mifare Classic*/
    status =
        RW_MfcReadNDef(nfa_rw_cb.p_ndef_buf, (uint16_t)nfa_rw_cb.ndef_cur_size);
  }

  return (status);
}

/*******************************************************************************
**
** Function         nfa_rw_detect_ndef
**
** Description      Handler for NFA_RW_API_DETECT_NDEF_EVT
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_detect_ndef() {
  tNFA_CONN_EVT_DATA conn_evt_data;
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  conn_evt_data.ndef_detect.status = nfa_rw_start_ndef_detection();
  if (conn_evt_data.ndef_detect.status != NFC_STATUS_OK) {
    /* Command complete - perform cleanup, notify app */
    nfa_rw_command_complete();
    conn_evt_data.ndef_detect.cur_size = 0;
    conn_evt_data.ndef_detect.max_size = 0;
    conn_evt_data.ndef_detect.flags = RW_NDEF_FL_UNKNOWN;
    nfa_dm_act_conn_cback_notify(NFA_NDEF_DETECT_EVT, &conn_evt_data);
  }

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_start_ndef_write
**
** Description      Start NDEF write on activated tag
**
** Returns          Nothing
**
*******************************************************************************/
static tNFC_STATUS nfa_rw_start_ndef_write(void) {
  tNFC_PROTOCOL protocol = nfa_rw_cb.protocol;
  tNFC_STATUS status = NFC_STATUS_FAILED;

  if (nfa_rw_cb.flags & NFA_RW_FL_TAG_IS_READONLY) {
    /* error: ndef tag is read-only */
    status = NFC_STATUS_FAILED;
    LOG(ERROR) << StringPrintf("Unable to write NDEF. Tag is read-only");
  } else if (nfa_rw_cb.ndef_max_size < nfa_rw_cb.ndef_wr_len) {
    /* error: ndef tag size is too small */
    status = NFC_STATUS_BUFFER_FULL;
    LOG(ERROR) << StringPrintf(
        "Unable to write NDEF. Tag maxsize=%i, request write size=%i",
        nfa_rw_cb.ndef_max_size, nfa_rw_cb.ndef_wr_len);
  } else {
    if (NFC_PROTOCOL_T1T == protocol) {
      /* Type1Tag    - NFC-A */
      status = RW_T1tWriteNDef((uint16_t)nfa_rw_cb.ndef_wr_len,
                               nfa_rw_cb.p_ndef_wr_buf);
    } else if (NFC_PROTOCOL_T2T == protocol) {
      /* Type2Tag    - NFC-A */
      if (nfa_rw_cb.pa_sel_res == NFC_SEL_RES_NFC_FORUM_T2T) {
        status = RW_T2tWriteNDef((uint16_t)nfa_rw_cb.ndef_wr_len,
                                 nfa_rw_cb.p_ndef_wr_buf);
      }
    } else if (NFC_PROTOCOL_T3T == protocol) {
      /* Type3Tag    - NFC-F */
      status = RW_T3tUpdateNDef(nfa_rw_cb.ndef_wr_len, nfa_rw_cb.p_ndef_wr_buf);
    } else if (NFC_PROTOCOL_ISO_DEP == protocol) {
      /* ISODEP/4A,4B- NFC-A or NFC-B */
      status = RW_T4tUpdateNDef((uint16_t)nfa_rw_cb.ndef_wr_len,
                                nfa_rw_cb.p_ndef_wr_buf);
    } else if (NFC_PROTOCOL_T5T == protocol) {
      /* ISO 15693 */
      status = RW_I93UpdateNDef((uint16_t)nfa_rw_cb.ndef_wr_len,
                                nfa_rw_cb.p_ndef_wr_buf);
    } else if (NFC_PROTOCOL_MIFARE == protocol) {
      /* Mifare Tag */
      status = RW_MfcWriteNDef((uint16_t)nfa_rw_cb.ndef_wr_len,
                               nfa_rw_cb.p_ndef_wr_buf);
    }
  }

  return (status);
}

/*******************************************************************************
**
** Function         nfa_rw_read_ndef
**
** Description      Handler for NFA_RW_API_READ_NDEF_EVT
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_read_ndef() {
  tNFA_STATUS status = NFA_STATUS_OK;
  tNFA_CONN_EVT_DATA conn_evt_data;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* Check if ndef detection has been performed yet */
  if (nfa_rw_cb.ndef_st == NFA_RW_NDEF_ST_UNKNOWN) {
    /* Perform ndef detection first */
    status = nfa_rw_start_ndef_detection();
  } else if (nfa_rw_cb.ndef_st == NFA_RW_NDEF_ST_FALSE) {
    /* Tag is not NDEF */
    status = NFA_STATUS_FAILED;
  } else {
    /* Perform the NDEF read operation */
    status = nfa_rw_start_ndef_read();
  }

  /* Handle failure */
  if (status != NFA_STATUS_OK) {
    /* Command complete - perform cleanup, notify app */
    nfa_rw_command_complete();
    conn_evt_data.status = status;
    nfa_dm_act_conn_cback_notify(NFA_READ_CPLT_EVT, &conn_evt_data);
  }

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_write_ndef
**
** Description      Handler for NFA_RW_API_WRITE_NDEF_EVT
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_write_ndef(tNFA_RW_MSG* p_data) {
  tNDEF_STATUS ndef_status;
  tNFA_STATUS write_status = NFA_STATUS_OK;
  tNFA_CONN_EVT_DATA conn_evt_data;
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* Validate NDEF message */
  ndef_status = NDEF_MsgValidate(p_data->op_req.params.write_ndef.p_data,
                                 p_data->op_req.params.write_ndef.len, false);
  if (ndef_status != NDEF_OK) {
    LOG(ERROR) << StringPrintf(
        "Invalid NDEF message. NDEF_MsgValidate returned %i", ndef_status);

    /* Command complete - perform cleanup, notify app */
    nfa_rw_command_complete();
    conn_evt_data.status = NFA_STATUS_FAILED;
    nfa_dm_act_conn_cback_notify(NFA_WRITE_CPLT_EVT, &conn_evt_data);
    return true;
  }

  /* Store pointer to source NDEF */
  nfa_rw_cb.p_ndef_wr_buf = p_data->op_req.params.write_ndef.p_data;
  nfa_rw_cb.ndef_wr_len = p_data->op_req.params.write_ndef.len;

  /* Check if ndef detection has been performed yet */
  if (nfa_rw_cb.ndef_st == NFA_RW_NDEF_ST_UNKNOWN) {
    /* Perform ndef detection first */
    write_status = nfa_rw_start_ndef_detection();
  } else if (nfa_rw_cb.ndef_st == NFA_RW_NDEF_ST_FALSE) {
    if (nfa_rw_cb.protocol == NFC_PROTOCOL_T1T) {
      /* For Type 1 tag, NDEF can be written on Initialized tag
      *  Perform ndef detection first to check if tag is in Initialized state to
      * Write NDEF */
      write_status = nfa_rw_start_ndef_detection();
    } else {
      /* Tag is not NDEF */
      write_status = NFA_STATUS_FAILED;
    }
  } else {
    /* Perform the NDEF read operation */
    write_status = nfa_rw_start_ndef_write();
  }

  /* Handle failure */
  if (write_status != NFA_STATUS_OK) {
    /* Command complete - perform cleanup, notify app */
    nfa_rw_command_complete();
    conn_evt_data.status = write_status;
    nfa_dm_act_conn_cback_notify(NFA_WRITE_CPLT_EVT, &conn_evt_data);
  }

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_presence_check
**
** Description      Handler for NFA_RW_API_PRESENCE_CHECK
**
** Returns          Nothing
**
*******************************************************************************/
void nfa_rw_presence_check(tNFA_RW_MSG* p_data) {
  tNFC_PROTOCOL protocol = nfa_rw_cb.protocol;
  uint8_t sel_res = nfa_rw_cb.pa_sel_res;
  tNFC_STATUS status = NFC_STATUS_FAILED;
  bool unsupported = false;
  uint8_t option = NFA_RW_OPTION_INVALID;
  tNFA_RW_PRES_CHK_OPTION op_param = NFA_RW_PRES_CHK_DEFAULT;

  if (NFC_PROTOCOL_T1T == protocol) {
    /* Type1Tag    - NFC-A */
    status = RW_T1tPresenceCheck();
  } else if (NFC_PROTOCOL_T2T == protocol) {
    /* If T2T NFC-Forum, then let RW handle presence check */
    if (sel_res == NFC_SEL_RES_NFC_FORUM_T2T) {
      /* Type 2 tag have not sent NACK after activation */
      status = RW_T2tPresenceCheck();
    } else {
      /* Will fall back to deactivate/reactivate */
      unsupported = true;
    }
  } else if (NFC_PROTOCOL_T3T == protocol) {
    /* Type3Tag    - NFC-F */
    status = RW_T3tPresenceCheck();
  } else if (NFC_PROTOCOL_ISO_DEP == protocol) {
    /* ISODEP/4A,4B- NFC-A or NFC-B */
    if (p_data) {
      op_param = p_data->op_req.params.option;
    }

    switch (op_param) {
      case NFA_RW_PRES_CHK_I_BLOCK:
        option = RW_T4T_CHK_EMPTY_I_BLOCK;
        break;

      case NFA_RW_PRES_CHK_ISO_DEP_NAK:
        if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
          option = RW_T4T_CHK_ISO_DEP_NAK_PRES_CHK;
        }
        break;
      default:
        /* empty I block */
        option = RW_T4T_CHK_EMPTY_I_BLOCK;
    }

    if (option != NFA_RW_OPTION_INVALID) {
      /* use the presence check with the chosen option */
      status = RW_T4tPresenceCheck(option);
    } else {
      /* use sleep/wake for presence check */
      unsupported = true;
    }
  } else if (NFC_PROTOCOL_T5T == protocol) {
    /* T5T/ISO 15693 */
    status = RW_I93PresenceCheck();
  } else {
    /* Protocol unsupported by RW module... */
    unsupported = true;
  }

  if (unsupported) {
    if (nfa_rw_cb.activated_tech_mode == NFC_DISCOVERY_TYPE_POLL_KOVIO) {
      /* start Kovio presence check (deactivate and wait for activation) */
      status = nfa_dm_disc_start_kovio_presence_check();
    } else {
      /* Let DM perform presence check (by putting tag to sleep and then waking
       * it up) */
      status = nfa_dm_disc_sleep_wakeup();
    }
  }

  /* Handle presence check failure */
  if (status != NFC_STATUS_OK)
    nfa_rw_handle_presence_check_rsp(NFC_STATUS_FAILED);
  else if (!unsupported) {
    nfa_sys_start_timer(&nfa_rw_cb.tle, NFA_RW_PRESENCE_CHECK_TIMEOUT_EVT,
                        p_nfa_dm_cfg->presence_check_timeout);
  }
}

/*******************************************************************************
**
** Function         nfa_rw_presence_check_tick
**
** Description      Called on expiration of NFA_RW_PRESENCE_CHECK_INTERVAL
**                  Initiate presence check
**
** Returns          TRUE (caller frees message buffer)
**
*******************************************************************************/
bool nfa_rw_presence_check_tick(__attribute__((unused)) tNFA_RW_MSG* p_data) {
  /* Store the current operation */
  nfa_rw_cb.cur_op = NFA_RW_OP_PRESENCE_CHECK;
  nfa_rw_cb.flags |= NFA_RW_FL_AUTO_PRESENCE_CHECK_BUSY;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("Auto-presence check starting...");

  /* Perform presence check */
  nfa_rw_presence_check(nullptr);

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_presence_check_timeout
**
** Description      presence check timeout: report presence check failure
**
** Returns          TRUE (caller frees message buffer)
**
*******************************************************************************/
bool nfa_rw_presence_check_timeout(__attribute__((unused))
                                   tNFA_RW_MSG* p_data) {
  nfa_rw_handle_presence_check_rsp(NFC_STATUS_FAILED);
  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_format_tag
**
** Description      Handler for NFA_RW_API_FORMAT_TAG
**
** Returns          Nothing
**
*******************************************************************************/
static void nfa_rw_format_tag() {
  tNFC_PROTOCOL protocol = nfa_rw_cb.protocol;
  tNFC_STATUS status = NFC_STATUS_FAILED;

  if (protocol == NFC_PROTOCOL_T1T) {
    status = RW_T1tFormatNDef();
  } else if ((protocol == NFC_PROTOCOL_T2T) &&
             (nfa_rw_cb.pa_sel_res == NFC_SEL_RES_NFC_FORUM_T2T)) {
    status = RW_T2tFormatNDef();
  } else if (protocol == NFC_PROTOCOL_T3T) {
    status = RW_T3tFormatNDef();
  } else if (protocol == NFC_PROTOCOL_T5T) {
    status = RW_I93FormatNDef();
  } else if (protocol == NFC_PROTOCOL_ISO_DEP) {
    status = RW_T4tFormatNDef();
  } else if (protocol == NFC_PROTOCOL_MIFARE) {
    status = RW_MfcFormatNDef();
  }

  /* If unable to format NDEF, notify the app */
  if (status != NFC_STATUS_OK) nfa_rw_error_cleanup(NFA_FORMAT_CPLT_EVT);
}

/*******************************************************************************
**
** Function         nfa_rw_detect_tlv
**
** Description      Handler for NFA_RW_API_DETECT_NDEF_EVT
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_detect_tlv(uint8_t tlv) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  switch (nfa_rw_cb.protocol) {
    case NFC_PROTOCOL_T1T:
      if (RW_T1tLocateTlv(tlv) != NFC_STATUS_OK)
        nfa_rw_error_cleanup(NFA_TLV_DETECT_EVT);
      break;

    case NFC_PROTOCOL_T2T:
      if (nfa_rw_cb.pa_sel_res == NFC_SEL_RES_NFC_FORUM_T2T) {
        if (RW_T2tLocateTlv(tlv) != NFC_STATUS_OK)
          nfa_rw_error_cleanup(NFA_TLV_DETECT_EVT);
      }
      break;

    default:
      break;
  }

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_config_tag_ro
**
** Description      Handler for NFA_RW_OP_SET_TAG_RO
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static tNFC_STATUS nfa_rw_config_tag_ro(bool b_hard_lock) {
  tNFC_PROTOCOL protocol = nfa_rw_cb.protocol;
  tNFC_STATUS status = NFC_STATUS_FAILED;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (NFC_PROTOCOL_T1T == protocol) {
    /* Type1Tag    - NFC-A */
    if ((nfa_rw_cb.tlv_st == NFA_RW_TLV_DETECT_ST_OP_NOT_STARTED) ||
        (nfa_rw_cb.tlv_st == NFA_RW_TLV_DETECT_ST_MEM_TLV_OP_COMPLETE)) {
      status = RW_T1tLocateTlv(TAG_LOCK_CTRL_TLV);
      return (status);
    } else {
      status = RW_T1tSetTagReadOnly(b_hard_lock);
    }
  } else if (NFC_PROTOCOL_T2T == protocol) {
    /* Type2Tag    - NFC-A */
    if (nfa_rw_cb.pa_sel_res == NFC_SEL_RES_NFC_FORUM_T2T) {
      status = RW_T2tSetTagReadOnly(b_hard_lock);
    }
  } else if (NFC_PROTOCOL_T3T == protocol) {
    /* Type3Tag    - NFC-F */
    status = RW_T3tSetReadOnly(b_hard_lock);
  } else if (NFC_PROTOCOL_ISO_DEP == protocol) {
    /* ISODEP/4A,4B- NFC-A or NFC-B */
    status = RW_T4tSetNDefReadOnly();
  } else if (NFC_PROTOCOL_T5T == protocol) {
    /* ISO 15693 */
    status = RW_I93SetTagReadOnly();
  }

  if (status == NFC_STATUS_OK) {
    nfa_rw_cb.ndef_st = NFA_RW_NDEF_ST_UNKNOWN;
  } else {
    nfa_rw_error_cleanup(NFA_SET_TAG_RO_EVT);
  }

  return (status);
}

/*******************************************************************************
**
** Function         nfa_rw_t1t_rid
**
** Description      Handler for T1T_RID API
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_t1t_rid() {
  if (RW_T1tRid() != NFC_STATUS_OK) nfa_rw_error_cleanup(NFA_READ_CPLT_EVT);

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_t1t_rall
**
** Description      Handler for T1T_ReadAll API
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_t1t_rall() {
  if (RW_T1tReadAll() != NFC_STATUS_OK) nfa_rw_error_cleanup(NFA_READ_CPLT_EVT);

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_t1t_read
**
** Description      Handler for T1T_Read API
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_t1t_read(tNFA_RW_MSG* p_data) {
  tNFA_RW_OP_PARAMS_T1T_READ* p_t1t_read =
      (tNFA_RW_OP_PARAMS_T1T_READ*)&(p_data->op_req.params.t1t_read);

  if (RW_T1tRead(p_t1t_read->block_number, p_t1t_read->index) != NFC_STATUS_OK)
    nfa_rw_error_cleanup(NFA_READ_CPLT_EVT);

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_t1t_write
**
** Description      Handler for T1T_WriteErase/T1T_WriteNoErase API
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_t1t_write(tNFA_RW_MSG* p_data) {
  tNFA_RW_OP_PARAMS_T1T_WRITE* p_t1t_write =
      (tNFA_RW_OP_PARAMS_T1T_WRITE*)&(p_data->op_req.params.t1t_write);
  tNFC_STATUS status;

  if (p_t1t_write->b_erase) {
    status = RW_T1tWriteErase(p_t1t_write->block_number, p_t1t_write->index,
                              p_t1t_write->p_block_data[0]);
  } else {
    status = RW_T1tWriteNoErase(p_t1t_write->block_number, p_t1t_write->index,
                                p_t1t_write->p_block_data[0]);
  }

  if (status != NFC_STATUS_OK) {
    nfa_rw_error_cleanup(NFA_WRITE_CPLT_EVT);
  } else {
    if (p_t1t_write->block_number == 0x01)
      nfa_rw_cb.ndef_st = NFA_RW_NDEF_ST_UNKNOWN;
  }

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_t1t_rseg
**
** Description      Handler for T1t_ReadSeg API
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_t1t_rseg(tNFA_RW_MSG* p_data) {
  tNFA_RW_OP_PARAMS_T1T_READ* p_t1t_read =
      (tNFA_RW_OP_PARAMS_T1T_READ*)&(p_data->op_req.params.t1t_read);

  if (RW_T1tReadSeg(p_t1t_read->segment_number) != NFC_STATUS_OK)
    nfa_rw_error_cleanup(NFA_READ_CPLT_EVT);

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_t1t_read8
**
** Description      Handler for T1T_Read8 API
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_t1t_read8(tNFA_RW_MSG* p_data) {
  tNFA_RW_OP_PARAMS_T1T_READ* p_t1t_read =
      (tNFA_RW_OP_PARAMS_T1T_READ*)&(p_data->op_req.params.t1t_read);

  if (RW_T1tRead8(p_t1t_read->block_number) != NFC_STATUS_OK)
    nfa_rw_error_cleanup(NFA_READ_CPLT_EVT);

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_t1t_write8
**
** Description      Handler for T1T_WriteErase8/T1T_WriteNoErase8 API
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_t1t_write8(tNFA_RW_MSG* p_data) {
  tNFA_RW_OP_PARAMS_T1T_WRITE* p_t1t_write =
      (tNFA_RW_OP_PARAMS_T1T_WRITE*)&(p_data->op_req.params.t1t_write);
  tNFC_STATUS status;

  if (p_t1t_write->b_erase) {
    status =
        RW_T1tWriteErase8(p_t1t_write->block_number, p_t1t_write->p_block_data);
  } else {
    status = RW_T1tWriteNoErase8(p_t1t_write->block_number,
                                 p_t1t_write->p_block_data);
  }

  if (status != NFC_STATUS_OK) {
    nfa_rw_error_cleanup(NFA_WRITE_CPLT_EVT);
  } else {
    if (p_t1t_write->block_number == 0x01)
      nfa_rw_cb.ndef_st = NFA_RW_NDEF_ST_UNKNOWN;
  }

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_t2t_read
**
** Description      Handler for T2T_Read API
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_t2t_read(tNFA_RW_MSG* p_data) {
  tNFA_RW_OP_PARAMS_T2T_READ* p_t2t_read =
      (tNFA_RW_OP_PARAMS_T2T_READ*)&(p_data->op_req.params.t2t_read);
  tNFC_STATUS status = NFC_STATUS_FAILED;

  if (nfa_rw_cb.pa_sel_res == NFC_SEL_RES_NFC_FORUM_T2T)
    status = RW_T2tRead(p_t2t_read->block_number);

  if (status != NFC_STATUS_OK) nfa_rw_error_cleanup(NFA_READ_CPLT_EVT);

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_t2t_write
**
** Description      Handler for T2T_Write API
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_t2t_write(tNFA_RW_MSG* p_data) {
  tNFA_RW_OP_PARAMS_T2T_WRITE* p_t2t_write =
      (tNFA_RW_OP_PARAMS_T2T_WRITE*)&(p_data->op_req.params.t2t_write);

  if (RW_T2tWrite(p_t2t_write->block_number, p_t2t_write->p_block_data) !=
      NFC_STATUS_OK) {
    nfa_rw_error_cleanup(NFA_WRITE_CPLT_EVT);
  } else {
    if (p_t2t_write->block_number == 0x03)
      nfa_rw_cb.ndef_st = NFA_RW_NDEF_ST_UNKNOWN;
  }

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_t2t_sector_select
**
** Description      Handler for T2T_Sector_Select API
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_t2t_sector_select(tNFA_RW_MSG* p_data) {
  tNFA_RW_OP_PARAMS_T2T_SECTOR_SELECT* p_t2t_sector_select =
      (tNFA_RW_OP_PARAMS_T2T_SECTOR_SELECT*)&(
          p_data->op_req.params.t2t_sector_select);

  if (RW_T2tSectorSelect(p_t2t_sector_select->sector_number) != NFC_STATUS_OK)
    nfa_rw_error_cleanup(NFA_SELECT_CPLT_EVT);

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_t3t_read
**
** Description      Handler for T3T_Read API
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_t3t_read(tNFA_RW_MSG* p_data) {
  tNFA_RW_OP_PARAMS_T3T_READ* p_t3t_read =
      (tNFA_RW_OP_PARAMS_T3T_READ*)&(p_data->op_req.params.t3t_read);

  if (RW_T3tCheck(p_t3t_read->num_blocks,
                  (tT3T_BLOCK_DESC*)p_t3t_read->p_block_desc) != NFC_STATUS_OK)
    nfa_rw_error_cleanup(NFA_READ_CPLT_EVT);

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_t3t_write
**
** Description      Handler for T3T_Write API
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_t3t_write(tNFA_RW_MSG* p_data) {
  tNFA_RW_OP_PARAMS_T3T_WRITE* p_t3t_write =
      (tNFA_RW_OP_PARAMS_T3T_WRITE*)&(p_data->op_req.params.t3t_write);

  if (RW_T3tUpdate(p_t3t_write->num_blocks,
                   (tT3T_BLOCK_DESC*)p_t3t_write->p_block_desc,
                   p_t3t_write->p_block_data) != NFC_STATUS_OK)
    nfa_rw_error_cleanup(NFA_WRITE_CPLT_EVT);

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_t3t_get_system_codes
**
** Description      Get system codes (initiated by NFA after activation)
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_t3t_get_system_codes() {
  tNFC_STATUS status;
  tNFA_TAG_PARAMS tag_params;

  status = RW_T3tGetSystemCodes();

  if (status != NFC_STATUS_OK) {
    /* Command complete - perform cleanup, notify app */
    nfa_rw_command_complete();
    tag_params.t3t.num_system_codes = 0;
    tag_params.t3t.p_system_codes = nullptr;

    nfa_dm_notify_activation_status(NFA_STATUS_OK, &tag_params);
  }

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_i93_command
**
** Description      Handler for ISO 15693 command
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
static bool nfa_rw_i93_command(tNFA_RW_MSG* p_data) {
  tNFA_CONN_EVT_DATA conn_evt_data;
  tNFC_STATUS status = NFC_STATUS_OK;
  uint8_t i93_command = I93_CMD_STAY_QUIET;

  switch (p_data->op_req.op) {
    case NFA_RW_OP_I93_INVENTORY:
      i93_command = I93_CMD_INVENTORY;
      if (p_data->op_req.params.i93_cmd.uid_present) {
        status = RW_I93Inventory(p_data->op_req.params.i93_cmd.afi_present,
                                 p_data->op_req.params.i93_cmd.afi,
                                 p_data->op_req.params.i93_cmd.uid);
      } else {
        status = RW_I93Inventory(p_data->op_req.params.i93_cmd.afi_present,
                                 p_data->op_req.params.i93_cmd.afi, nullptr);
      }
      break;

    case NFA_RW_OP_I93_STAY_QUIET:
      i93_command = I93_CMD_STAY_QUIET;
      status = RW_I93StayQuiet();
      break;

    case NFA_RW_OP_I93_READ_SINGLE_BLOCK:
      i93_command = I93_CMD_READ_SINGLE_BLOCK;
      status = RW_I93ReadSingleBlock(
          p_data->op_req.params.i93_cmd.first_block_number);
      break;

    case NFA_RW_OP_I93_WRITE_SINGLE_BLOCK:
      i93_command = I93_CMD_WRITE_SINGLE_BLOCK;
      status = RW_I93WriteSingleBlock(
          p_data->op_req.params.i93_cmd.first_block_number,
          p_data->op_req.params.i93_cmd.p_data);
      break;

    case NFA_RW_OP_I93_LOCK_BLOCK:
      i93_command = I93_CMD_LOCK_BLOCK;
      status = RW_I93LockBlock(
          (uint8_t)p_data->op_req.params.i93_cmd.first_block_number);
      break;

    case NFA_RW_OP_I93_READ_MULTI_BLOCK:
      i93_command = I93_CMD_READ_MULTI_BLOCK;
      status = RW_I93ReadMultipleBlocks(
          p_data->op_req.params.i93_cmd.first_block_number,
          p_data->op_req.params.i93_cmd.number_blocks);
      break;

    case NFA_RW_OP_I93_WRITE_MULTI_BLOCK:
      i93_command = I93_CMD_WRITE_MULTI_BLOCK;
      status = RW_I93WriteMultipleBlocks(
          (uint8_t)p_data->op_req.params.i93_cmd.first_block_number,
          p_data->op_req.params.i93_cmd.number_blocks,
          p_data->op_req.params.i93_cmd.p_data);
      break;

    case NFA_RW_OP_I93_SELECT:
      i93_command = I93_CMD_SELECT;
      status = RW_I93Select(p_data->op_req.params.i93_cmd.p_data);
      break;

    case NFA_RW_OP_I93_RESET_TO_READY:
      i93_command = I93_CMD_RESET_TO_READY;
      status = RW_I93ResetToReady();
      break;

    case NFA_RW_OP_I93_WRITE_AFI:
      i93_command = I93_CMD_WRITE_AFI;
      status = RW_I93WriteAFI(p_data->op_req.params.i93_cmd.afi);
      break;

    case NFA_RW_OP_I93_LOCK_AFI:
      i93_command = I93_CMD_LOCK_AFI;
      status = RW_I93LockAFI();
      break;

    case NFA_RW_OP_I93_WRITE_DSFID:
      i93_command = I93_CMD_WRITE_DSFID;
      status = RW_I93WriteDSFID(p_data->op_req.params.i93_cmd.dsfid);
      break;

    case NFA_RW_OP_I93_LOCK_DSFID:
      i93_command = I93_CMD_LOCK_DSFID;
      status = RW_I93LockDSFID();
      break;

    case NFA_RW_OP_I93_GET_SYS_INFO:
      i93_command = I93_CMD_GET_SYS_INFO;
      if (p_data->op_req.params.i93_cmd.uid_present) {
        status = RW_I93GetSysInfo(p_data->op_req.params.i93_cmd.uid);
      } else {
        status = RW_I93GetSysInfo(nullptr);
      }
      break;

    case NFA_RW_OP_I93_GET_MULTI_BLOCK_STATUS:
      i93_command = I93_CMD_GET_MULTI_BLK_SEC;
      status = RW_I93GetMultiBlockSecurityStatus(
          p_data->op_req.params.i93_cmd.first_block_number,
          p_data->op_req.params.i93_cmd.number_blocks);
      break;

    default:
      break;
  }

  if (status != NFC_STATUS_OK) {
    /* Command complete - perform cleanup, notify app */
    nfa_rw_command_complete();

    conn_evt_data.i93_cmd_cplt.status = NFA_STATUS_FAILED;
    conn_evt_data.i93_cmd_cplt.sent_command = i93_command;

    nfa_dm_act_conn_cback_notify(NFA_I93_CMD_CPLT_EVT, &conn_evt_data);
  }

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_raw_mode_data_cback
**
** Description      Handler for incoming tag data for unsupported tag protocols
**                  (forward data to upper layer)
**
** Returns          nothing
**
*******************************************************************************/
static void nfa_rw_raw_mode_data_cback(__attribute__((unused)) uint8_t conn_id,
                                       tNFC_CONN_EVT event, tNFC_CONN* p_data) {
  NFC_HDR* p_msg;
  tNFA_CONN_EVT_DATA evt_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("event = 0x%X", event);

  if ((event == NFC_DATA_CEVT) &&
      ((p_data->data.status == NFC_STATUS_OK) ||
       (p_data->data.status == NFC_STATUS_CONTINUE))) {
    p_msg = (NFC_HDR*)p_data->data.p_data;

    if (p_msg) {
      evt_data.data.status = p_data->data.status;
      evt_data.data.p_data = (uint8_t*)(p_msg + 1) + p_msg->offset;
      evt_data.data.len = p_msg->len;

      nfa_dm_conn_cback_event_notify(NFA_DATA_EVT, &evt_data);

      GKI_freebuf(p_msg);
    } else {
      LOG(ERROR) << StringPrintf(
          "received NFC_DATA_CEVT with NULL data pointer");
    }
  } else if (event == NFC_DEACTIVATE_CEVT) {
    NFC_SetStaticRfCback(nullptr);
  }
}

/*******************************************************************************
**
** Function         nfa_rw_activate_ntf
**
** Description      Handler for NFA_RW_ACTIVATE_NTF
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
bool nfa_rw_activate_ntf(tNFA_RW_MSG* p_data) {
  tNFC_ACTIVATE_DEVT* p_activate_params =
      p_data->activate_ntf.p_activate_params;
  tNFA_TAG_PARAMS tag_params;
  bool activate_notify = true;
  uint8_t* p;

  if ((nfa_rw_cb.halt_event != RW_T2T_MAX_EVT) &&
      (nfa_rw_cb.activated_tech_mode == NFC_DISCOVERY_TYPE_POLL_A) &&
      (nfa_rw_cb.protocol == NFC_PROTOCOL_T2T) &&
      (nfa_rw_cb.pa_sel_res == NFC_SEL_RES_NFC_FORUM_T2T)) {
    /* Type 2 tag is wake up from HALT State */
    if (nfa_dm_cb.p_activate_ntf != nullptr) {
      GKI_freebuf(nfa_dm_cb.p_activate_ntf);
      nfa_dm_cb.p_activate_ntf = nullptr;
    }
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("- Type 2 tag wake up from HALT State");
    return true;
  }

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* Initialize control block */
  nfa_rw_cb.protocol = p_activate_params->protocol;
  nfa_rw_cb.intf_type = p_activate_params->intf_param.type;
  nfa_rw_cb.pa_sel_res = p_activate_params->rf_tech_param.param.pa.sel_rsp;
  nfa_rw_cb.activated_tech_mode = p_activate_params->rf_tech_param.mode;
  nfa_rw_cb.flags = NFA_RW_FL_ACTIVATED;
  nfa_rw_cb.cur_op = NFA_RW_OP_MAX;
  nfa_rw_cb.halt_event = RW_T2T_MAX_EVT;
  nfa_rw_cb.skip_dyn_locks = false;
  nfa_rw_cb.ndef_st = NFA_RW_NDEF_ST_UNKNOWN;
  nfa_rw_cb.tlv_st = NFA_RW_TLV_DETECT_ST_OP_NOT_STARTED;

  memset(&tag_params, 0, sizeof(tNFA_TAG_PARAMS));

  /* Check if we are in exclusive RF mode */
  if (p_data->activate_ntf.excl_rf_not_active) {
    /* Not in exclusive RF mode */
    nfa_rw_cb.flags |= NFA_RW_FL_NOT_EXCL_RF_MODE;
  }

  /* check if the protocol is activated with supported interface */
  if (p_activate_params->intf_param.type == NCI_INTERFACE_FRAME) {
    if ((p_activate_params->protocol != NFA_PROTOCOL_T1T) &&
        (p_activate_params->protocol != NFA_PROTOCOL_T2T) &&
        (p_activate_params->protocol != NFA_PROTOCOL_T3T) &&
        (p_activate_params->protocol != NFA_PROTOCOL_T5T)) {
      nfa_rw_cb.protocol = NFA_PROTOCOL_INVALID;
    }
  } else if (p_activate_params->intf_param.type == NCI_INTERFACE_ISO_DEP) {
    if (p_activate_params->protocol != NFA_PROTOCOL_ISO_DEP) {
      nfa_rw_cb.protocol = NFA_PROTOCOL_INVALID;
    }
  }

  if (nfa_rw_cb.protocol == NFA_PROTOCOL_INVALID) {
    /* Only sending raw frame and presence check are supported in this state */

    NFC_SetStaticRfCback(nfa_rw_raw_mode_data_cback);

    /* Notify app of NFA_ACTIVATED_EVT and start presence check timer */
    nfa_dm_notify_activation_status(NFA_STATUS_OK, nullptr);
    nfa_rw_check_start_presence_check_timer(NFA_RW_PRESENCE_CHECK_INTERVAL);
    return true;
  }

  /* If protocol not supported by RW module, notify app of NFA_ACTIVATED_EVT and
   * start presence check if needed */
  if (!nfa_dm_is_protocol_supported(
          p_activate_params->protocol,
          p_activate_params->rf_tech_param.param.pa.sel_rsp)) {
    /* Notify upper layer of NFA_ACTIVATED_EVT if needed, and start presence
     * check timer */
    /* Set data callback (pass all incoming data to upper layer using
     * NFA_DATA_EVT) */
    NFC_SetStaticRfCback(nfa_rw_raw_mode_data_cback);

    /* Notify app of NFA_ACTIVATED_EVT and start presence check timer */
    nfa_dm_notify_activation_status(NFA_STATUS_OK, nullptr);
    nfa_rw_check_start_presence_check_timer(NFA_RW_PRESENCE_CHECK_INTERVAL);
    return true;
  }

  /* Initialize RW module */
  if ((RW_SetActivatedTagType(p_activate_params, nfa_rw_cback)) !=
      NFC_STATUS_OK) {
    /* Log error (stay in NFA_RW_ST_ACTIVATED state until deactivation) */
    LOG(ERROR) << StringPrintf("RW_SetActivatedTagType failed.");
    return true;
  }

  /* Perform protocol-specific actions */
  if (NFC_PROTOCOL_T1T == nfa_rw_cb.protocol) {
    /* Retrieve HR and UID fields from activation notification */
    memcpy(tag_params.t1t.uid, p_activate_params->rf_tech_param.param.pa.nfcid1,
           p_activate_params->rf_tech_param.param.pa.nfcid1_len);

    if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
      memcpy(tag_params.t1t.hr, p_activate_params->rf_tech_param.param.pa.hr,
             NFA_T1T_HR_LEN);
    } else {
      memcpy(tag_params.t1t.hr,
             p_activate_params->intf_param.intf_param.frame.param,
             NFA_T1T_HR_LEN);
      tNFA_RW_MSG msg;
      msg.op_req.op = NFA_RW_OP_T1T_RID;
      bool free_buf = nfa_rw_handle_op_req(&msg);
      CHECK(free_buf);
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("nfa_rw_handle_op_req is holding on to soon-garbage stack memory.");
      /* Delay notifying upper layer of NFA_ACTIVATED_EVT
         until HR0/HR1 is received */
      activate_notify = false;
    }
  } else if (NFC_PROTOCOL_T2T == nfa_rw_cb.protocol) {
    /* Retrieve UID fields from activation notification */
    memcpy(tag_params.t2t.uid, p_activate_params->rf_tech_param.param.pa.nfcid1,
           p_activate_params->rf_tech_param.param.pa.nfcid1_len);
  } else if (NFC_PROTOCOL_T3T == nfa_rw_cb.protocol) {
    if (appl_dta_mode_flag) {
      /* Incase of DTA mode Dont send commands to get system code. Just notify
       * activation */
      activate_notify = true;
    } else {
      /* Delay notifying upper layer of NFA_ACTIVATED_EVT until system codes
       * are retrieved */
      activate_notify = false;

      /* Issue command to get Felica system codes */
      tNFA_RW_MSG msg;
      msg.op_req.op = NFA_RW_OP_T3T_GET_SYSTEM_CODES;
      bool free_buf = nfa_rw_handle_op_req(&msg);
      CHECK(free_buf);
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("nfa_rw_handle_op_req is holding on to soon-garbage stack memory.");
    }
  } else if (NFA_PROTOCOL_T5T == nfa_rw_cb.protocol) {
    /* Delay notifying upper layer of NFA_ACTIVATED_EVT to retrieve additional
     * tag infomation */
    nfa_rw_cb.flags |= NFA_RW_FL_ACTIVATION_NTF_PENDING;
    activate_notify = false;

    /* store DSFID and UID from activation NTF */
    nfa_rw_cb.i93_dsfid = p_activate_params->rf_tech_param.param.pi93.dsfid;

    p = nfa_rw_cb.i93_uid;
    ARRAY8_TO_STREAM(p, p_activate_params->rf_tech_param.param.pi93.uid);

    if ((nfa_rw_cb.i93_uid[1] == I93_UID_IC_MFG_CODE_TI) &&
        (((nfa_rw_cb.i93_uid[2] & I93_UID_TAG_IT_HF_I_PRODUCT_ID_MASK) ==
          I93_UID_TAG_IT_HF_I_STD_CHIP_INLAY) ||
         ((nfa_rw_cb.i93_uid[2] & I93_UID_TAG_IT_HF_I_PRODUCT_ID_MASK) ==
          I93_UID_TAG_IT_HF_I_PRO_CHIP_INLAY))) {
      /* these don't support Get System Information Command */
      nfa_rw_cb.i93_block_size = I93_TAG_IT_HF_I_STD_PRO_CHIP_INLAY_BLK_SIZE;
      nfa_rw_cb.i93_afi_location =
          I93_TAG_IT_HF_I_STD_PRO_CHIP_INLAY_AFI_LOCATION;

      if ((nfa_rw_cb.i93_uid[2] & I93_UID_TAG_IT_HF_I_PRODUCT_ID_MASK) ==
          I93_UID_TAG_IT_HF_I_STD_CHIP_INLAY) {
        nfa_rw_cb.i93_num_block = I93_TAG_IT_HF_I_STD_CHIP_INLAY_NUM_TOTAL_BLK;
      } else {
        nfa_rw_cb.i93_num_block = I93_TAG_IT_HF_I_PRO_CHIP_INLAY_NUM_TOTAL_BLK;
      }

      /* read AFI */
      if (RW_I93ReadSingleBlock((uint8_t)(nfa_rw_cb.i93_afi_location /
                                          nfa_rw_cb.i93_block_size)) !=
          NFC_STATUS_OK) {
        /* notify activation without AFI/IC-Ref */
        nfa_rw_cb.flags &= ~NFA_RW_FL_ACTIVATION_NTF_PENDING;
        activate_notify = true;

        tag_params.i93.info_flags =
            (I93_INFO_FLAG_DSFID | I93_INFO_FLAG_MEM_SIZE);
        tag_params.i93.dsfid = nfa_rw_cb.i93_dsfid;
        tag_params.i93.block_size = nfa_rw_cb.i93_block_size;
        tag_params.i93.num_block = nfa_rw_cb.i93_num_block;
        memcpy(tag_params.i93.uid, nfa_rw_cb.i93_uid, I93_UID_BYTE_LEN);
      }
    } else {
      /* All of ICODE supports Get System Information Command */
      /* Tag-it HF-I Plus Chip/Inlay supports Get System Information Command */
      /* just try for others */

      if (RW_I93GetSysInfo(nfa_rw_cb.i93_uid) != NFC_STATUS_OK) {
        /* notify activation without AFI/MEM size/IC-Ref */
        nfa_rw_cb.flags &= ~NFA_RW_FL_ACTIVATION_NTF_PENDING;
        activate_notify = true;

        tag_params.i93.info_flags = I93_INFO_FLAG_DSFID;
        tag_params.i93.dsfid = nfa_rw_cb.i93_dsfid;
        tag_params.i93.block_size = 0;
        tag_params.i93.num_block = 0;
        memcpy(tag_params.i93.uid, nfa_rw_cb.i93_uid, I93_UID_BYTE_LEN);
      } else {
        /* reset memory size */
        nfa_rw_cb.i93_block_size = 0;
        nfa_rw_cb.i93_num_block = 0;
      }
    }
  }

  /* Notify upper layer of NFA_ACTIVATED_EVT if needed, and start presence check
   * timer */
  if (activate_notify) {
    nfa_dm_notify_activation_status(NFA_STATUS_OK, &tag_params);
    nfa_rw_check_start_presence_check_timer(NFA_RW_PRESENCE_CHECK_INTERVAL);
  }

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_deactivate_ntf
**
** Description      Handler for NFA_RW_DEACTIVATE_NTF
**
** Returns          TRUE (message buffer to be freed by caller)
**
*******************************************************************************/
bool nfa_rw_deactivate_ntf(__attribute__((unused)) tNFA_RW_MSG* p_data) {
  /* Clear the activated flag */
  nfa_rw_cb.flags &= ~NFA_RW_FL_ACTIVATED;

  /* Free buffer for incoming NDEF message, in case we were in the middle of a
   * read operation */
  nfa_rw_free_ndef_rx_buf();

  /* If there is a pending command message, then free it */
  if (nfa_rw_cb.p_pending_msg) {
    if ((nfa_rw_cb.p_pending_msg->op_req.op == NFA_RW_OP_SEND_RAW_FRAME) &&
        (nfa_rw_cb.p_pending_msg->op_req.params.send_raw_frame.p_data)) {
      GKI_freebuf(nfa_rw_cb.p_pending_msg->op_req.params.send_raw_frame.p_data);
    }

    GKI_freebuf(nfa_rw_cb.p_pending_msg);
    nfa_rw_cb.p_pending_msg = nullptr;
  }

  /* If we are in the process of waking up tag from HALT state */
  if (nfa_rw_cb.halt_event == RW_T2T_READ_CPLT_EVT) {
    if (nfa_rw_cb.rw_data.data.p_data)
      GKI_freebuf(nfa_rw_cb.rw_data.data.p_data);
    nfa_rw_cb.rw_data.data.p_data = nullptr;
  }

  /* Stop presence check timer (if started) */
  nfa_rw_stop_presence_check_timer();

  return true;
}

/*******************************************************************************
**
** Function         nfa_rw_handle_op_req
**
** Description      Handler for NFA_RW_OP_REQUEST_EVT, operation request
**
** Returns          TRUE if caller should free p_data
**                  FALSE if caller does not need to free p_data
**
*******************************************************************************/
bool nfa_rw_handle_op_req(tNFA_RW_MSG* p_data) {
  bool freebuf = true;
  uint16_t presence_check_start_delay = 0;

  /* Check if activated */
  if (!(nfa_rw_cb.flags & NFA_RW_FL_ACTIVATED)) {
    LOG(ERROR) << StringPrintf("nfa_rw_handle_op_req: not activated");
    return (nfa_rw_op_req_while_inactive(p_data));
  }
  /* Check if currently busy with another API call */
  else if (nfa_rw_cb.flags & NFA_RW_FL_API_BUSY) {
    return (nfa_rw_op_req_while_busy(p_data));
  }
  /* Check if currently busy with auto-presence check */
  else if (nfa_rw_cb.flags & NFA_RW_FL_AUTO_PRESENCE_CHECK_BUSY) {
    /* Cache the command (will be handled once auto-presence check is completed)
     */
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "Deferring operation %i until after auto-presence check is completed",
        p_data->op_req.op);
    nfa_rw_cb.p_pending_msg = p_data;
    nfa_rw_cb.flags |= NFA_RW_FL_API_BUSY;
    return false;
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_rw_handle_op_req: op=0x%02x", p_data->op_req.op);

  nfa_rw_cb.flags |= NFA_RW_FL_API_BUSY;

  /* Stop the presence check timer */
  nfa_rw_stop_presence_check_timer();

  /* Store the current operation */
  nfa_rw_cb.cur_op = p_data->op_req.op;

  /* Call appropriate handler for requested operation */
  switch (p_data->op_req.op) {
    case NFA_RW_OP_DETECT_NDEF:
      nfa_rw_cb.skip_dyn_locks = false;
      nfa_rw_detect_ndef();
      break;

    case NFA_RW_OP_READ_NDEF:
      nfa_rw_read_ndef();
      break;

    case NFA_RW_OP_WRITE_NDEF:
      nfa_rw_write_ndef(p_data);
      break;

    case NFA_RW_OP_SEND_RAW_FRAME:
      presence_check_start_delay =
          p_data->op_req.params.send_raw_frame.p_data->layer_specific;

      NFC_SendData(NFC_RF_CONN_ID, p_data->op_req.params.send_raw_frame.p_data);

      /* Clear the busy flag */
      nfa_rw_cb.flags &= ~NFA_RW_FL_API_BUSY;

      /* Start presence_check after specified delay */
      nfa_rw_check_start_presence_check_timer(presence_check_start_delay);
      break;

    case NFA_RW_OP_PRESENCE_CHECK:
      nfa_rw_presence_check(p_data);
      break;

    case NFA_RW_OP_FORMAT_TAG:
      nfa_rw_format_tag();
      break;

    case NFA_RW_OP_DETECT_LOCK_TLV:
      nfa_rw_detect_tlv(TAG_LOCK_CTRL_TLV);
      break;

    case NFA_RW_OP_DETECT_MEM_TLV:
      nfa_rw_detect_tlv(TAG_MEM_CTRL_TLV);
      break;

    case NFA_RW_OP_SET_TAG_RO:
      nfa_rw_cb.b_hard_lock = p_data->op_req.params.set_readonly.b_hard_lock;
      nfa_rw_config_tag_ro(nfa_rw_cb.b_hard_lock);
      break;

    case NFA_RW_OP_T1T_RID:
      nfa_rw_t1t_rid();
      break;

    case NFA_RW_OP_T1T_RALL:
      nfa_rw_t1t_rall();
      break;

    case NFA_RW_OP_T1T_READ:
      nfa_rw_t1t_read(p_data);
      break;

    case NFA_RW_OP_T1T_WRITE:
      nfa_rw_t1t_write(p_data);
      break;

    case NFA_RW_OP_T1T_RSEG:
      nfa_rw_t1t_rseg(p_data);
      break;

    case NFA_RW_OP_T1T_READ8:
      nfa_rw_t1t_read8(p_data);
      break;

    case NFA_RW_OP_T1T_WRITE8:
      nfa_rw_t1t_write8(p_data);
      break;

    /* Type-2 tag commands */
    case NFA_RW_OP_T2T_READ:
      nfa_rw_t2t_read(p_data);
      break;

    case NFA_RW_OP_T2T_WRITE:
      nfa_rw_t2t_write(p_data);
      break;

    case NFA_RW_OP_T2T_SECTOR_SELECT:
      nfa_rw_t2t_sector_select(p_data);
      break;

    /* Type-3 tag commands */
    case NFA_RW_OP_T3T_READ:
      nfa_rw_t3t_read(p_data);
      break;

    case NFA_RW_OP_T3T_WRITE:
      nfa_rw_t3t_write(p_data);
      break;

    case NFA_RW_OP_T3T_GET_SYSTEM_CODES:
      nfa_rw_t3t_get_system_codes();
      break;

    /* ISO 15693 tag commands */
    case NFA_RW_OP_I93_INVENTORY:
    case NFA_RW_OP_I93_STAY_QUIET:
    case NFA_RW_OP_I93_READ_SINGLE_BLOCK:
    case NFA_RW_OP_I93_WRITE_SINGLE_BLOCK:
    case NFA_RW_OP_I93_LOCK_BLOCK:
    case NFA_RW_OP_I93_READ_MULTI_BLOCK:
    case NFA_RW_OP_I93_WRITE_MULTI_BLOCK:
    case NFA_RW_OP_I93_SELECT:
    case NFA_RW_OP_I93_RESET_TO_READY:
    case NFA_RW_OP_I93_WRITE_AFI:
    case NFA_RW_OP_I93_LOCK_AFI:
    case NFA_RW_OP_I93_WRITE_DSFID:
    case NFA_RW_OP_I93_LOCK_DSFID:
    case NFA_RW_OP_I93_GET_SYS_INFO:
    case NFA_RW_OP_I93_GET_MULTI_BLOCK_STATUS:
      nfa_rw_i93_command(p_data);
      break;

    default:
      LOG(ERROR) << StringPrintf("nfa_rw_handle_api: unhandled operation: %i",
                                 p_data->op_req.op);
      break;
  }

  return (freebuf);
}

/*******************************************************************************
**
** Function         nfa_rw_op_req_while_busy
**
** Description      Handle operation request while busy
**
** Returns          TRUE if caller should free p_data
**                  FALSE if caller does not need to free p_data
**
*******************************************************************************/
static bool nfa_rw_op_req_while_busy(tNFA_RW_MSG* p_data) {
  bool freebuf = true;
  tNFA_CONN_EVT_DATA conn_evt_data;
  uint8_t event;

  LOG(ERROR) << StringPrintf("nfa_rw_op_req_while_busy: unable to handle API");

  /* Return appropriate event for requested API, with status=BUSY */
  conn_evt_data.status = NFA_STATUS_BUSY;

  switch (p_data->op_req.op) {
    case NFA_RW_OP_DETECT_NDEF:
      conn_evt_data.ndef_detect.cur_size = 0;
      conn_evt_data.ndef_detect.max_size = 0;
      conn_evt_data.ndef_detect.flags = RW_NDEF_FL_UNKNOWN;
      event = NFA_NDEF_DETECT_EVT;
      break;
    case NFA_RW_OP_READ_NDEF:
    case NFA_RW_OP_T1T_RID:
    case NFA_RW_OP_T1T_RALL:
    case NFA_RW_OP_T1T_READ:
    case NFA_RW_OP_T1T_RSEG:
    case NFA_RW_OP_T1T_READ8:
    case NFA_RW_OP_T2T_READ:
    case NFA_RW_OP_T3T_READ:
      event = NFA_READ_CPLT_EVT;
      break;
    case NFA_RW_OP_WRITE_NDEF:
    case NFA_RW_OP_T1T_WRITE:
    case NFA_RW_OP_T1T_WRITE8:
    case NFA_RW_OP_T2T_WRITE:
    case NFA_RW_OP_T3T_WRITE:
      event = NFA_WRITE_CPLT_EVT;
      break;
    case NFA_RW_OP_FORMAT_TAG:
      event = NFA_FORMAT_CPLT_EVT;
      break;
    case NFA_RW_OP_DETECT_LOCK_TLV:
    case NFA_RW_OP_DETECT_MEM_TLV:
      event = NFA_TLV_DETECT_EVT;
      break;
    case NFA_RW_OP_SET_TAG_RO:
      event = NFA_SET_TAG_RO_EVT;
      break;
    case NFA_RW_OP_T2T_SECTOR_SELECT:
      event = NFA_SELECT_CPLT_EVT;
      break;
    case NFA_RW_OP_I93_INVENTORY:
    case NFA_RW_OP_I93_STAY_QUIET:
    case NFA_RW_OP_I93_READ_SINGLE_BLOCK:
    case NFA_RW_OP_I93_WRITE_SINGLE_BLOCK:
    case NFA_RW_OP_I93_LOCK_BLOCK:
    case NFA_RW_OP_I93_READ_MULTI_BLOCK:
    case NFA_RW_OP_I93_WRITE_MULTI_BLOCK:
    case NFA_RW_OP_I93_SELECT:
    case NFA_RW_OP_I93_RESET_TO_READY:
    case NFA_RW_OP_I93_WRITE_AFI:
    case NFA_RW_OP_I93_LOCK_AFI:
    case NFA_RW_OP_I93_WRITE_DSFID:
    case NFA_RW_OP_I93_LOCK_DSFID:
    case NFA_RW_OP_I93_GET_SYS_INFO:
    case NFA_RW_OP_I93_GET_MULTI_BLOCK_STATUS:
      event = NFA_I93_CMD_CPLT_EVT;
      break;
    default:
      return (freebuf);
  }
  nfa_dm_act_conn_cback_notify(event, &conn_evt_data);

  return (freebuf);
}

/*******************************************************************************
**
** Function         nfa_rw_op_req_while_inactive
**
** Description      Handle operation request while inactive
**
** Returns          TRUE if caller should free p_data
**                  FALSE if caller does not need to free p_data
**
*******************************************************************************/
static bool nfa_rw_op_req_while_inactive(tNFA_RW_MSG* p_data) {
  bool freebuf = true;
  tNFA_CONN_EVT_DATA conn_evt_data;
  uint8_t event;

  LOG(ERROR) << StringPrintf(
      "nfa_rw_op_req_while_inactive: unable to handle API");

  /* Return appropriate event for requested API, with status=REJECTED */
  conn_evt_data.status = NFA_STATUS_REJECTED;

  switch (p_data->op_req.op) {
    case NFA_RW_OP_DETECT_NDEF:
      conn_evt_data.ndef_detect.cur_size = 0;
      conn_evt_data.ndef_detect.max_size = 0;
      conn_evt_data.ndef_detect.flags = RW_NDEF_FL_UNKNOWN;
      event = NFA_NDEF_DETECT_EVT;
      break;
    case NFA_RW_OP_READ_NDEF:
    case NFA_RW_OP_T1T_RID:
    case NFA_RW_OP_T1T_RALL:
    case NFA_RW_OP_T1T_READ:
    case NFA_RW_OP_T1T_RSEG:
    case NFA_RW_OP_T1T_READ8:
    case NFA_RW_OP_T2T_READ:
    case NFA_RW_OP_T3T_READ:
      event = NFA_READ_CPLT_EVT;
      break;
    case NFA_RW_OP_WRITE_NDEF:
    case NFA_RW_OP_T1T_WRITE:
    case NFA_RW_OP_T1T_WRITE8:
    case NFA_RW_OP_T2T_WRITE:
    case NFA_RW_OP_T3T_WRITE:
      event = NFA_WRITE_CPLT_EVT;
      break;
    case NFA_RW_OP_FORMAT_TAG:
      event = NFA_FORMAT_CPLT_EVT;
      break;
    case NFA_RW_OP_DETECT_LOCK_TLV:
    case NFA_RW_OP_DETECT_MEM_TLV:
      event = NFA_TLV_DETECT_EVT;
      break;
    case NFA_RW_OP_SET_TAG_RO:
      event = NFA_SET_TAG_RO_EVT;
      break;
    case NFA_RW_OP_T2T_SECTOR_SELECT:
      event = NFA_SELECT_CPLT_EVT;
      break;
    case NFA_RW_OP_I93_INVENTORY:
    case NFA_RW_OP_I93_STAY_QUIET:
    case NFA_RW_OP_I93_READ_SINGLE_BLOCK:
    case NFA_RW_OP_I93_WRITE_SINGLE_BLOCK:
    case NFA_RW_OP_I93_LOCK_BLOCK:
    case NFA_RW_OP_I93_READ_MULTI_BLOCK:
    case NFA_RW_OP_I93_WRITE_MULTI_BLOCK:
    case NFA_RW_OP_I93_SELECT:
    case NFA_RW_OP_I93_RESET_TO_READY:
    case NFA_RW_OP_I93_WRITE_AFI:
    case NFA_RW_OP_I93_LOCK_AFI:
    case NFA_RW_OP_I93_WRITE_DSFID:
    case NFA_RW_OP_I93_LOCK_DSFID:
    case NFA_RW_OP_I93_GET_SYS_INFO:
    case NFA_RW_OP_I93_GET_MULTI_BLOCK_STATUS:
      event = NFA_I93_CMD_CPLT_EVT;
      break;
    default:
      return (freebuf);
  }
  nfa_dm_act_conn_cback_notify(event, &conn_evt_data);

  return (freebuf);
}

/*******************************************************************************
**
** Function         nfa_rw_command_complete
**
** Description      Handle command complete: clear the busy flag,
**                  and start the presence check timer if applicable.
**
** Returns          None
**
*******************************************************************************/
void nfa_rw_command_complete(void) {
  /* Clear the busy flag */
  nfa_rw_cb.flags &= ~NFA_RW_FL_API_BUSY;

  /* Restart presence_check timer */
  nfa_rw_check_start_presence_check_timer(NFA_RW_PRESENCE_CHECK_INTERVAL);
}
