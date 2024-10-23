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
 *  This file contains the implementation for Type 2 tag in Reader/Writer
 *  mode.
 *
 ******************************************************************************/
#include <string>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nfc_target.h"

#include "bt_types.h"
#include "gki.h"
#include "nci_hmsgs.h"
#include "nfc_api.h"
#include "nfc_int.h"
#include "rw_api.h"
#include "rw_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/* Static local functions */
static void rw_t2t_proc_data(uint8_t conn_id, tNFC_DATA_CEVT* p_data);
static tNFC_STATUS rw_t2t_send_cmd(uint8_t opcode, uint8_t* p_dat);
static void rw_t2t_process_error(void);
static void rw_t2t_process_frame_error(void);
static void rw_t2t_handle_presence_check_rsp(tNFC_STATUS status);
static void rw_t2t_resume_op(void);

static std::string rw_t2t_get_state_name(uint8_t state);
static std::string rw_t2t_get_substate_name(uint8_t substate);

/*******************************************************************************
**
** Function         rw_t2t_proc_data
**
** Description      This function handles data evt received from NFC Controller.
**
** Returns          none
**
*******************************************************************************/
static void rw_t2t_proc_data(uint8_t conn_id, tNFC_DATA_CEVT* p_data) {
  tRW_EVENT rw_event = RW_RAW_FRAME_EVT;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  NFC_HDR* p_pkt = p_data->p_data;
  bool b_notify = true;
  bool b_release = true;
  uint8_t* p;
  tRW_READ_DATA evt_data = {};
  tT2T_CMD_RSP_INFO* p_cmd_rsp_info =
      (tT2T_CMD_RSP_INFO*)rw_cb.tcb.t2t.p_cmd_rsp_info;
  tRW_DETECT_NDEF_DATA ndef_data;
  uint8_t begin_state = p_t2t->state;

  if ((p_t2t->state == RW_T2T_STATE_IDLE) || (p_cmd_rsp_info == nullptr)) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("RW T2T Raw Frame: Len [0x%X] Status [%s]", p_pkt->len,
                        NFC_GetStatusName(p_data->status).c_str());
    evt_data.status = p_data->status;
    evt_data.p_data = p_pkt;
    tRW_DATA rw_data;
    rw_data.data = evt_data;
    (*rw_cb.p_cback)(RW_T2T_RAW_FRAME_EVT, &rw_data);
    return;
  }
#if (RW_STATS_INCLUDED == TRUE)
  /* Update rx stats */
  rw_main_update_rx_stats(p_pkt->len);
#endif
  /* Stop timer as response is received */
  nfc_stop_quick_timer(&p_t2t->t2_timer);

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("RW RECV [%s]:0x%x RSP", t2t_info_to_str(p_cmd_rsp_info),
                      p_cmd_rsp_info->opcode);

  if (((p_pkt->len != p_cmd_rsp_info->rsp_len) &&
       (p_pkt->len != p_cmd_rsp_info->nack_rsp_len) &&
       (p_t2t->substate != RW_T2T_SUBSTATE_WAIT_SELECT_SECTOR)) ||
      (p_t2t->state == RW_T2T_STATE_HALT)) {
    LOG(ERROR) << StringPrintf("T2T Frame error. state=%s ",
                               rw_t2t_get_state_name(p_t2t->state).c_str());
    if (p_t2t->state != RW_T2T_STATE_HALT) {
      /* Retrasmit the last sent command if retry-count < max retry */
      rw_t2t_process_frame_error();
      p_t2t->check_tag_halt = false;
    }
    GKI_freebuf(p_pkt);
    return;
  }
  rw_cb.cur_retry = 0;

  /* Assume the data is just the response byte sequence */
  p = (uint8_t*)(p_pkt + 1) + p_pkt->offset;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "rw_t2t_proc_data State: %u  conn_id: %u  len: %u  data[0]: 0x%02x",
      p_t2t->state, conn_id, p_pkt->len, *p);

  evt_data.p_data = nullptr;

  if (p_t2t->substate == RW_T2T_SUBSTATE_WAIT_SELECT_SECTOR_SUPPORT) {
    /* The select process happens in two steps */
    if ((*p & 0x0f) == T2T_RSP_ACK) {
      if (rw_t2t_sector_change(p_t2t->select_sector) == NFC_STATUS_OK)
        b_notify = false;
      else
        evt_data.status = NFC_STATUS_FAILED;
    } else {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "rw_t2t_proc_data - Received NACK response(0x%x) to SEC-SELCT CMD",
          (*p & 0x0f));
      evt_data.status = NFC_STATUS_REJECTED;
    }
  } else if (p_t2t->substate == RW_T2T_SUBSTATE_WAIT_SELECT_SECTOR) {
    evt_data.status = NFC_STATUS_FAILED;
  } else if ((p_pkt->len != p_cmd_rsp_info->rsp_len) ||
             ((p_cmd_rsp_info->opcode == T2T_CMD_WRITE) &&
              ((*p & 0x0f) != T2T_RSP_ACK))) {
    /* Received NACK response */
    evt_data.p_data = p_pkt;
    if (p_t2t->state == RW_T2T_STATE_READ) b_release = false;

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "rw_t2t_proc_data - Received NACK response(0x%x)", (*p & 0x0f));

    if (!p_t2t->check_tag_halt) {
      /* Just received first NACK. Retry just one time to find if tag went in to
       * HALT State */
      b_notify = false;
      rw_t2t_process_error();
      /* Assume Tag is in HALT State, untill we get response to retry command */
      p_t2t->check_tag_halt = true;
    } else {
      p_t2t->check_tag_halt = false;
      /* Got consecutive NACK so tag not really halt after first NACK, but
       * current operation failed */
      evt_data.status = NFC_STATUS_FAILED;
    }
  } else {
    /* If the response length indicates positive response or cannot be known
     * from length then assume success */
    evt_data.status = NFC_STATUS_OK;
    p_t2t->check_tag_halt = false;

    /* The response data depends on what the current operation was */
    switch (p_t2t->state) {
      case RW_T2T_STATE_CHECK_PRESENCE:
        b_notify = false;
        rw_t2t_handle_presence_check_rsp(NFC_STATUS_OK);
        break;

      case RW_T2T_STATE_READ:
        evt_data.p_data = p_pkt;
        b_release = false;
        if (p_t2t->block_read == 0) {
          p_t2t->b_read_hdr = true;
          memcpy(p_t2t->tag_hdr, p, T2T_READ_DATA_LEN);
        }
        break;

      case RW_T2T_STATE_WRITE:
        /* Write operation completed successfully */
        break;

      default:
        /* NDEF/other Tlv Operation/Format-Tag/Config Tag as Read only */
        b_notify = false;
        rw_t2t_handle_rsp(p);
        break;
    }
  }

  if (b_notify) {
    rw_event = rw_t2t_info_to_event(p_cmd_rsp_info);

    if (rw_event == RW_T2T_NDEF_DETECT_EVT) {
      ndef_data.status = evt_data.status;
      ndef_data.protocol = NFC_PROTOCOL_T2T;
      ndef_data.flags = RW_NDEF_FL_UNKNOWN;
      if (p_t2t->substate == RW_T2T_SUBSTATE_WAIT_READ_LOCKS)
        ndef_data.flags = RW_NDEF_FL_FORMATED;
      ndef_data.max_size = 0;
      ndef_data.cur_size = 0;
      /* Move back to idle state */
      rw_t2t_handle_op_complete();
      tRW_DATA rw_data;
      rw_data.ndef = ndef_data;
      (*rw_cb.p_cback)(rw_event, &rw_data);
    } else {
      /* Move back to idle state */
      rw_t2t_handle_op_complete();
      tRW_DATA rw_data;
      rw_data.data = evt_data;
      (*rw_cb.p_cback)(rw_event, &rw_data);
    }
  }

  if (b_release) GKI_freebuf(p_pkt);

  if (begin_state != p_t2t->state) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("RW T2T state changed:<%s> -> <%s>",
                        rw_t2t_get_state_name(begin_state).c_str(),
                        rw_t2t_get_state_name(p_t2t->state).c_str());
  }
}

/*******************************************************************************
**
** Function         rw_t2t_conn_cback
**
** Description      This callback function receives events/data from NFCC.
**
** Returns          none
**
*******************************************************************************/
void rw_t2t_conn_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                       tNFC_CONN* p_data) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tRW_READ_DATA evt_data;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("rw_t2t_conn_cback: conn_id=%i, evt=%i", conn_id, event);
  /* Only handle static conn_id */
  if (conn_id != NFC_RF_CONN_ID) {
    return;
  }

  switch (event) {
    case NFC_CONN_CREATE_CEVT:
    case NFC_CONN_CLOSE_CEVT:
      break;

    case NFC_DEACTIVATE_CEVT:
#if (RW_STATS_INCLUDED == TRUE)
      /* Display stats */
      rw_main_log_stats();
#endif
      /* Stop t2t timer (if started) */
      nfc_stop_quick_timer(&p_t2t->t2_timer);

      /* Free cmd buf for retransmissions */
      if (p_t2t->p_cur_cmd_buf) {
        GKI_freebuf(p_t2t->p_cur_cmd_buf);
        p_t2t->p_cur_cmd_buf = nullptr;
      }
      /* Free cmd buf used to hold command before sector change */
      if (p_t2t->p_sec_cmd_buf) {
        GKI_freebuf(p_t2t->p_sec_cmd_buf);
        p_t2t->p_sec_cmd_buf = nullptr;
      }

      p_t2t->state = RW_T2T_STATE_NOT_ACTIVATED;
      NFC_SetStaticRfCback(nullptr);
      break;

    case NFC_DATA_CEVT:
      if (p_data != nullptr) {
        if ((p_data->data.status == NFC_STATUS_OK) ||
            (p_data->data.status == NFC_STATUS_CONTINUE)) {
          rw_t2t_proc_data(conn_id, &(p_data->data));
          break;
        } else if (p_data->data.p_data != nullptr) {
          /* Free the response buffer in case of error response */
          GKI_freebuf((NFC_HDR*)(p_data->data.p_data));
          p_data->data.p_data = nullptr;
        }
      }
      /* Data event with error status...fall through to NFC_ERROR_CEVT case */
      FALLTHROUGH_INTENDED;

    case NFC_ERROR_CEVT:
      if ((p_t2t->state == RW_T2T_STATE_NOT_ACTIVATED) ||
          (p_t2t->state == RW_T2T_STATE_IDLE) ||
          (p_t2t->state == RW_T2T_STATE_HALT)) {
#if (RW_STATS_INCLUDED == TRUE)
        rw_main_update_trans_error_stats();
#endif /* RW_STATS_INCLUDED */
        if (event == NFC_ERROR_CEVT)
          evt_data.status = (tNFC_STATUS)(*(uint8_t*)p_data);
        else if (p_data)
          evt_data.status = p_data->status;
        else
          evt_data.status = NFC_STATUS_FAILED;

        evt_data.p_data = nullptr;
        tRW_DATA rw_data;
        rw_data.data = evt_data;
        (*rw_cb.p_cback)(RW_T2T_INTF_ERROR_EVT, &rw_data);
        break;
      }
      nfc_stop_quick_timer(&p_t2t->t2_timer);
#if (RW_STATS_INCLUDED == TRUE)
      rw_main_update_trans_error_stats();
#endif
      if (p_t2t->state == RW_T2T_STATE_CHECK_PRESENCE) {
        if (p_t2t->check_tag_halt) {
          p_t2t->state = RW_T2T_STATE_HALT;
          rw_t2t_handle_presence_check_rsp(NFC_STATUS_REJECTED);
        } else {
          /* Move back to idle state */
          rw_t2t_handle_presence_check_rsp(NFC_STATUS_FAILED);
        }
      } else {
        rw_t2t_process_error();
      }
      break;

    default:
      break;
  }
}

/*******************************************************************************
**
** Function         rw_t2t_send_cmd
**
** Description      This function composes a Type 2 Tag command and send it via
**                  NCI to NFCC.
**
** Returns          NFC_STATUS_OK if the command is successfuly sent to NCI
**                  otherwise, error status
**
*******************************************************************************/
tNFC_STATUS rw_t2t_send_cmd(uint8_t opcode, uint8_t* p_dat) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  const tT2T_CMD_RSP_INFO* p_cmd_rsp_info = t2t_cmd_to_rsp_info(opcode);
  NFC_HDR* p_data;
  uint8_t* p;

  if (p_cmd_rsp_info) {
    /* a valid opcode for RW */
    p_data = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
    if (p_data) {
      p_t2t->p_cmd_rsp_info = (tT2T_CMD_RSP_INFO*)p_cmd_rsp_info;
      p_data->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
      p = (uint8_t*)(p_data + 1) + p_data->offset;

      UINT8_TO_STREAM(p, opcode);

      if (p_dat) {
        ARRAY_TO_STREAM(p, p_dat, (p_cmd_rsp_info->cmd_len - 1));
      }

      p_data->len = p_cmd_rsp_info->cmd_len;

      /* Indicate first attempt to send command, back up cmd buffer in case
       * needed for retransmission */
      rw_cb.cur_retry = 0;
      memcpy(p_t2t->p_cur_cmd_buf, p_data,
             sizeof(NFC_HDR) + p_data->offset + p_data->len);

#if (RW_STATS_INCLUDED == TRUE)
      /* Update stats */
      rw_main_update_tx_stats(p_data->len, false);
#endif
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "RW SENT [%s]:0x%x CMD", t2t_info_to_str(p_cmd_rsp_info),
          p_cmd_rsp_info->opcode);

      status = NFC_SendData(NFC_RF_CONN_ID, p_data);
      if (status == NFC_STATUS_OK) {
        nfc_start_quick_timer(
            &p_t2t->t2_timer, NFC_TTYPE_RW_T2T_RESPONSE,
            (RW_T2T_TOUT_RESP * QUICK_TIMER_TICKS_PER_SEC) / 1000);
      } else {
        LOG(ERROR) << StringPrintf(
            "T2T NFC Send data failed. state=%s substate=%s ",
            rw_t2t_get_state_name(p_t2t->state).c_str(),
            rw_t2t_get_substate_name(p_t2t->substate).c_str());
      }
    } else {
      status = NFC_STATUS_NO_BUFFERS;
    }
  }
  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_process_timeout
**
** Description      handles timeout event
**
** Returns          none
**
*******************************************************************************/
void rw_t2t_process_timeout() {
  tRW_READ_DATA evt_data;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;

  if (p_t2t->state == RW_T2T_STATE_CHECK_PRESENCE) {
    if (p_t2t->check_tag_halt) {
      p_t2t->state = RW_T2T_STATE_HALT;
      rw_t2t_handle_presence_check_rsp(NFC_STATUS_REJECTED);
    } else {
      /* Move back to idle state */
      rw_t2t_handle_presence_check_rsp(NFC_STATUS_FAILED);
    }
    return;
  }

  if (p_t2t->substate == RW_T2T_SUBSTATE_WAIT_SELECT_SECTOR) {
    p_t2t->sector = p_t2t->select_sector;
    /* Here timeout is an acknowledgment for successfull sector change */
    if (p_t2t->state == RW_T2T_STATE_SELECT_SECTOR) {
      /* Notify that select sector op is successfull */
      rw_t2t_handle_op_complete();
      evt_data.status = NFC_STATUS_OK;
      evt_data.p_data = nullptr;
      tRW_DATA rw_data;
      rw_data.data = evt_data;
      (*rw_cb.p_cback)(RW_T2T_SELECT_CPLT_EVT, &rw_data);
    } else {
      /* Resume operation from where we stopped before sector change */
      rw_t2t_resume_op();
    }
  } else if (p_t2t->state != RW_T2T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("T2T timeout. state=%s ",
                               rw_t2t_get_state_name(p_t2t->state).c_str());
    /* Handle timeout error as no response to the command sent */
    rw_t2t_process_error();
  }
}

/*******************************************************************************
**
** Function         rw_t2t_process_frame_error
**
** Description      handles frame crc error
**
** Returns          none
**
*******************************************************************************/
static void rw_t2t_process_frame_error(void) {
#if (RW_STATS_INCLUDED == TRUE)
  /* Update stats */
  rw_main_update_crc_error_stats();
#endif
  /* Process the error */
  rw_t2t_process_error();
}

/*******************************************************************************
**
** Function         rw_t2t_process_error
**
** Description      Process error including Timeout, Frame error. This function
**                  will retry atleast till RW_MAX_RETRIES before give up and
**                  sending negative notification to upper layer
**
** Returns          none
**
*******************************************************************************/
static void rw_t2t_process_error(void) {
  tRW_READ_DATA evt_data;
  tRW_EVENT rw_event;
  NFC_HDR* p_cmd_buf;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tT2T_CMD_RSP_INFO* p_cmd_rsp_info =
      (tT2T_CMD_RSP_INFO*)rw_cb.tcb.t2t.p_cmd_rsp_info;
  tRW_DETECT_NDEF_DATA ndef_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("State: %u", p_t2t->state);

  /* Retry sending command if retry-count < max */
  if ((!p_t2t->check_tag_halt) && (rw_cb.cur_retry < RW_MAX_RETRIES)) {
    /* retry sending the command */
    rw_cb.cur_retry++;

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "T2T retransmission attempt %i of %i", rw_cb.cur_retry, RW_MAX_RETRIES);

    /* allocate a new buffer for message */
    p_cmd_buf = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
    if (p_cmd_buf != nullptr) {
      memcpy(p_cmd_buf, p_t2t->p_cur_cmd_buf, sizeof(NFC_HDR) +
                                                  p_t2t->p_cur_cmd_buf->offset +
                                                  p_t2t->p_cur_cmd_buf->len);
#if (RW_STATS_INCLUDED == TRUE)
      /* Update stats */
      rw_main_update_tx_stats(p_cmd_buf->len, true);
#endif
      if (NFC_SendData(NFC_RF_CONN_ID, p_cmd_buf) == NFC_STATUS_OK) {
        /* Start timer for waiting for response */
        nfc_start_quick_timer(
            &p_t2t->t2_timer, NFC_TTYPE_RW_T2T_RESPONSE,
            (RW_T2T_TOUT_RESP * QUICK_TIMER_TICKS_PER_SEC) / 1000);

        return;
      }
    }
  } else {
    if (p_t2t->check_tag_halt) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("T2T Went to HALT State!");
    } else {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "T2T maximum retransmission attempts reached (%i)", RW_MAX_RETRIES);
    }
  }
  rw_event = rw_t2t_info_to_event(p_cmd_rsp_info);
#if (RW_STATS_INCLUDED == TRUE)
  /* update failure count */
  rw_main_update_fail_stats();
#endif
  if (p_t2t->check_tag_halt) {
    evt_data.status = NFC_STATUS_REJECTED;
    p_t2t->state = RW_T2T_STATE_HALT;
  } else {
    evt_data.status = NFC_STATUS_TIMEOUT;
  }

  if (rw_event == RW_T2T_NDEF_DETECT_EVT) {
    ndef_data.status = evt_data.status;
    ndef_data.protocol = NFC_PROTOCOL_T2T;
    ndef_data.flags = RW_NDEF_FL_UNKNOWN;
    if (p_t2t->substate == RW_T2T_SUBSTATE_WAIT_READ_LOCKS)
      ndef_data.flags = RW_NDEF_FL_FORMATED;
    ndef_data.max_size = 0;
    ndef_data.cur_size = 0;
    /* If not Halt move to idle state */
    rw_t2t_handle_op_complete();

    tRW_DATA rw_data;
    rw_data.ndef = ndef_data;
    (*rw_cb.p_cback)(rw_event, &rw_data);
  } else {
    evt_data.p_data = nullptr;
    /* If activated and not Halt move to idle state */
    if (p_t2t->state != RW_T2T_STATE_NOT_ACTIVATED) rw_t2t_handle_op_complete();

    p_t2t->substate = RW_T2T_SUBSTATE_NONE;
    tRW_DATA rw_data;
    rw_data.data = evt_data;
    (*rw_cb.p_cback)(rw_event, &rw_data);
  }
}

/*****************************************************************************
**
** Function         rw_t2t_handle_presence_check_rsp
**
** Description      Handle response to presence check
**
** Returns          Nothing
**
*****************************************************************************/
void rw_t2t_handle_presence_check_rsp(tNFC_STATUS status) {
  tRW_DATA rw_data;

  /* Notify, Tag is present or not */
  rw_data.data.status = status;
  rw_t2t_handle_op_complete();

  (*rw_cb.p_cback)(RW_T2T_PRESENCE_CHECK_EVT, &rw_data);
}

/*******************************************************************************
**
** Function         rw_t2t_resume_op
**
** Description      This function will continue operation after moving to new
**                  sector
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
static void rw_t2t_resume_op(void) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tRW_READ_DATA evt_data;
  NFC_HDR* p_cmd_buf;
  tRW_EVENT event;
  const tT2T_CMD_RSP_INFO* p_cmd_rsp_info =
      (tT2T_CMD_RSP_INFO*)rw_cb.tcb.t2t.p_cmd_rsp_info;
  uint8_t* p;

  /* Move back to the substate where we were before changing sector */
  p_t2t->substate = p_t2t->prev_substate;

  p = (uint8_t*)(p_t2t->p_sec_cmd_buf + 1) + p_t2t->p_sec_cmd_buf->offset;
  p_cmd_rsp_info = t2t_cmd_to_rsp_info((uint8_t)*p);
  p_t2t->p_cmd_rsp_info = (tT2T_CMD_RSP_INFO*)p_cmd_rsp_info;

  /* allocate a new buffer for message */
  p_cmd_buf = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
  if (p_cmd_buf != nullptr) {
    memcpy(p_cmd_buf, p_t2t->p_sec_cmd_buf, sizeof(NFC_HDR) +
                                                p_t2t->p_sec_cmd_buf->offset +
                                                p_t2t->p_sec_cmd_buf->len);
    memcpy(p_t2t->p_cur_cmd_buf, p_t2t->p_sec_cmd_buf,
           sizeof(NFC_HDR) + p_t2t->p_sec_cmd_buf->offset +
               p_t2t->p_sec_cmd_buf->len);

#if (RW_STATS_INCLUDED == TRUE)
    /* Update stats */
    rw_main_update_tx_stats(p_cmd_buf->len, true);
#endif
    if (NFC_SendData(NFC_RF_CONN_ID, p_cmd_buf) == NFC_STATUS_OK) {
      /* Start timer for waiting for response */
      nfc_start_quick_timer(
          &p_t2t->t2_timer, NFC_TTYPE_RW_T2T_RESPONSE,
          (RW_T2T_TOUT_RESP * QUICK_TIMER_TICKS_PER_SEC) / 1000);
    } else {
      /* failure - could not send buffer */
      evt_data.p_data = nullptr;
      evt_data.status = NFC_STATUS_FAILED;
      event = rw_t2t_info_to_event(p_cmd_rsp_info);
      rw_t2t_handle_op_complete();
      tRW_DATA rw_data;
      rw_data.data = evt_data;
      (*rw_cb.p_cback)(event, &rw_data);
    }
  }
}

/*******************************************************************************
**
** Function         rw_t2t_sector_change
**
** Description      This function issues Type 2 Tag SECTOR-SELECT command
**                  packet 1.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_t2t_sector_change(uint8_t sector) {
  tNFC_STATUS status;
  NFC_HDR* p_data;
  uint8_t* p;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;

  p_data = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
  if (p_data == nullptr) {
    LOG(ERROR) << StringPrintf("rw_t2t_sector_change - No buffer");
    return (NFC_STATUS_NO_BUFFERS);
  }

  p_data->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_data + 1) + p_data->offset;

  UINT8_TO_BE_STREAM(p, sector);
  UINT8_TO_BE_STREAM(p, 0x00);
  UINT8_TO_BE_STREAM(p, 0x00);
  UINT8_TO_BE_STREAM(p, 0x00);

  p_data->len = 4;

  status = NFC_SendData(NFC_RF_CONN_ID, p_data);
  if (status == NFC_STATUS_OK) {
    /* Passive rsp command and suppose not to get response to this command */
    p_t2t->p_cmd_rsp_info = nullptr;
    p_t2t->substate = RW_T2T_SUBSTATE_WAIT_SELECT_SECTOR;

    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("rw_t2t_sector_change Sent Second Command");
    nfc_start_quick_timer(
        &p_t2t->t2_timer, NFC_TTYPE_RW_T2T_RESPONSE,
        (RW_T2T_SEC_SEL_TOUT_RESP * QUICK_TIMER_TICKS_PER_SEC) / 1000);
  } else {
    LOG(ERROR) << StringPrintf(
        "rw_t2t_sector_change Send failed at rw_t2t_send_cmd, error: %u",
        status);
  }

  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_read
**
** Description      This function issues Type 2 Tag READ command for the
**                  specified block. If the specified block is in different
**                  sector then it first sends command to move to new sector
**                  and after the tag moves to new sector it issues the read
**                  command for the block.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_t2t_read(uint16_t block) {
  tNFC_STATUS status;
  uint8_t* p;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint8_t sector_byte2[1];
  uint8_t read_cmd[1];

  read_cmd[0] = block % T2T_BLOCKS_PER_SECTOR;
  if (p_t2t->sector != block / T2T_BLOCKS_PER_SECTOR) {
    sector_byte2[0] = 0xFF;
    /* First Move to new sector before sending Read command */
    status = rw_t2t_send_cmd(T2T_CMD_SEC_SEL, sector_byte2);
    if (status == NFC_STATUS_OK) {
      /* Prepare command that needs to be sent after sector change op is
       * completed */
      p_t2t->select_sector = (uint8_t)(block / T2T_BLOCKS_PER_SECTOR);
      p_t2t->p_sec_cmd_buf->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;

      p = (uint8_t*)(p_t2t->p_sec_cmd_buf + 1) + p_t2t->p_sec_cmd_buf->offset;
      UINT8_TO_BE_STREAM(p, T2T_CMD_READ);
      UINT8_TO_BE_STREAM(p, read_cmd[0]);
      p_t2t->p_sec_cmd_buf->len = 2;
      p_t2t->block_read = block;

      /* Backup the current substate to move back to this substate after
       * changing sector */
      p_t2t->prev_substate = p_t2t->substate;
      p_t2t->substate = RW_T2T_SUBSTATE_WAIT_SELECT_SECTOR_SUPPORT;
      return NFC_STATUS_OK;
    }
    return NFC_STATUS_FAILED;
  }

  /* Send Read command as sector change is not needed */
  status = rw_t2t_send_cmd(T2T_CMD_READ, (uint8_t*)read_cmd);
  if (status == NFC_STATUS_OK) {
    p_t2t->block_read = block;
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("rw_t2t_read Sent Command for Block: %u", block);
  }

  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_write
**
** Description      This function issues Type 2 Tag WRITE command for the
**                  specified block.  If the specified block is in different
**                  sector then it first sends command to move to new sector
**                  and after the tag moves to new sector it issues the write
**                  command for the block.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS rw_t2t_write(uint16_t block, uint8_t* p_write_data) {
  tNFC_STATUS status;
  uint8_t* p;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint8_t write_cmd[T2T_WRITE_DATA_LEN + 1];
  uint8_t sector_byte2[1];

  p_t2t->block_written = block;
  write_cmd[0] = (uint8_t)(block % T2T_BLOCKS_PER_SECTOR);
  memcpy(&write_cmd[1], p_write_data, T2T_WRITE_DATA_LEN);

  if (p_t2t->sector != block / T2T_BLOCKS_PER_SECTOR) {
    sector_byte2[0] = 0xFF;
    /* First Move to new sector before sending Write command */
    status = rw_t2t_send_cmd(T2T_CMD_SEC_SEL, sector_byte2);
    if (status == NFC_STATUS_OK) {
      /* Prepare command that needs to be sent after sector change op is
       * completed */
      p_t2t->select_sector = (uint8_t)(block / T2T_BLOCKS_PER_SECTOR);
      p_t2t->p_sec_cmd_buf->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
      p = (uint8_t*)(p_t2t->p_sec_cmd_buf + 1) + p_t2t->p_sec_cmd_buf->offset;
      UINT8_TO_BE_STREAM(p, T2T_CMD_WRITE);
      memcpy(p, write_cmd, T2T_WRITE_DATA_LEN + 1);
      p_t2t->p_sec_cmd_buf->len = 2 + T2T_WRITE_DATA_LEN;
      p_t2t->block_written = block;

      /* Backup the current substate to move back to this substate after
       * changing sector */
      p_t2t->prev_substate = p_t2t->substate;
      p_t2t->substate = RW_T2T_SUBSTATE_WAIT_SELECT_SECTOR_SUPPORT;
      return NFC_STATUS_OK;
    }
    return NFC_STATUS_FAILED;
  }

  /* Send Write command as sector change is not needed */
  status = rw_t2t_send_cmd(T2T_CMD_WRITE, write_cmd);
  if (status == NFC_STATUS_OK) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("rw_t2t_write Sent Command for Block: %u", block);
  }

  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_select
**
** Description      This function selects type 2 tag.
**
** Returns          Tag selection status
**
*******************************************************************************/
tNFC_STATUS rw_t2t_select(void) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;

  p_t2t->state = RW_T2T_STATE_IDLE;
  p_t2t->ndef_status = T2T_NDEF_NOT_DETECTED;

  /* Alloc cmd buf for retransmissions */
  if (p_t2t->p_cur_cmd_buf == nullptr) {
    p_t2t->p_cur_cmd_buf = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
    if (p_t2t->p_cur_cmd_buf == nullptr) {
      LOG(ERROR) << StringPrintf(
          "rw_t2t_select: unable to allocate buffer for retransmission");
      return (NFC_STATUS_FAILED);
    }
  }
  /* Alloc cmd buf for holding a command untill sector changes */
  if (p_t2t->p_sec_cmd_buf == nullptr) {
    p_t2t->p_sec_cmd_buf = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
    if (p_t2t->p_sec_cmd_buf == nullptr) {
      LOG(ERROR) << StringPrintf(
          "rw_t2t_select: unable to allocate buffer used during sector change");
      return (NFC_STATUS_FAILED);
    }
  }

  NFC_SetStaticRfCback(rw_t2t_conn_cback);
  rw_t2t_handle_op_complete();
  p_t2t->check_tag_halt = false;

  return NFC_STATUS_OK;
}

/*****************************************************************************
**
** Function         rw_t2t_handle_op_complete
**
** Description      Reset to IDLE state
**
** Returns          Nothing
**
*****************************************************************************/
void rw_t2t_handle_op_complete(void) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;

  if ((p_t2t->state == RW_T2T_STATE_READ_NDEF) ||
      (p_t2t->state == RW_T2T_STATE_WRITE_NDEF)) {
    p_t2t->b_read_data = false;
  }

  if (p_t2t->state != RW_T2T_STATE_HALT) p_t2t->state = RW_T2T_STATE_IDLE;
  p_t2t->substate = RW_T2T_SUBSTATE_NONE;
  return;
}

/*****************************************************************************
**
** Function         RW_T2tPresenceCheck
**
** Description
**      Check if the tag is still in the field.
**
**      The RW_T2T_PRESENCE_CHECK_EVT w/ status is used to indicate presence
**      or non-presence.
**
** Returns
**      NFC_STATUS_OK, if raw data frame sent
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_T2tPresenceCheck(void) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_DATA evt_data;
  tRW_CB* p_rw_cb = &rw_cb;
  uint8_t sector_blk = 0; /* block 0 of current sector */

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* If RW_SelectTagType was not called (no conn_callback) return failure */
  if (!p_rw_cb->p_cback) {
    retval = NFC_STATUS_FAILED;
  }
  /* If we are not activated, then RW_T2T_PRESENCE_CHECK_EVT status=FAIL */
  else if (p_rw_cb->tcb.t2t.state == RW_T2T_STATE_NOT_ACTIVATED) {
    evt_data.status = NFC_STATUS_FAILED;
    (*p_rw_cb->p_cback)(RW_T2T_PRESENCE_CHECK_EVT, &evt_data);
  }
  /* If command is pending, assume tag is still present */
  else if (p_rw_cb->tcb.t2t.state != RW_T2T_STATE_IDLE) {
    evt_data.status = NFC_STATUS_OK;
    (*p_rw_cb->p_cback)(RW_T2T_PRESENCE_CHECK_EVT, &evt_data);
  } else {
    /* IDLE state: send a READ command to block 0 of the current sector */
    retval = rw_t2t_send_cmd(T2T_CMD_READ, &sector_blk);
    if (retval == NFC_STATUS_OK) {
      p_rw_cb->tcb.t2t.state = RW_T2T_STATE_CHECK_PRESENCE;
    }
  }

  return (retval);
}

/*******************************************************************************
**
** Function         RW_T2tRead
**
** Description      This function issues the Type 2 Tag READ command. When the
**                  operation is complete the callback function will be called
**                  with a RW_T2T_READ_EVT.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS RW_T2tRead(uint16_t block) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tNFC_STATUS status;

  if (p_t2t->state != RW_T2T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf(
        "Error: Type 2 tag not activated or Busy - State: %u", p_t2t->state);
    return (NFC_STATUS_FAILED);
  }

  status = rw_t2t_read(block);
  if (status == NFC_STATUS_OK) {
    p_t2t->state = RW_T2T_STATE_READ;
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("RW_T2tRead Sent Read command");
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_T2tWrite
**
** Description      This function issues the Type 2 Tag WRITE command. When the
**                  operation is complete the callback function will be called
**                  with a RW_T2T_WRITE_EVT.
**
**                  p_new_bytes points to the array of 4 bytes to be written
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS RW_T2tWrite(uint16_t block, uint8_t* p_write_data) {
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  tNFC_STATUS status;

  if (p_t2t->state != RW_T2T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf(
        "Error: Type 2 tag not activated or Busy - State: %u", p_t2t->state);
    return (NFC_STATUS_FAILED);
  }

  status = rw_t2t_write(block, p_write_data);
  if (status == NFC_STATUS_OK) {
    p_t2t->state = RW_T2T_STATE_WRITE;
    if (block < T2T_FIRST_DATA_BLOCK)
      p_t2t->b_read_hdr = false;
    else if (block < (T2T_FIRST_DATA_BLOCK + T2T_READ_BLOCKS))
      p_t2t->b_read_data = false;
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("RW_T2tWrite Sent Write command");
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_T2tSectorSelect
**
** Description      This function issues the Type 2 Tag SECTOR-SELECT command
**                  packet 1. If a NACK is received as the response, the
**                  callback function will be called with a
**                  RW_T2T_SECTOR_SELECT_EVT. If an ACK is received as the
**                  response, the command packet 2 with the given sector number
**                  is sent to the peer device. When the response for packet 2
**                  is received, the callback function will be called with a
**                  RW_T2T_SECTOR_SELECT_EVT.
**
**                  A sector is 256 contiguous blocks (1024 bytes).
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS RW_T2tSectorSelect(uint8_t sector) {
  tNFC_STATUS status;
  tRW_T2T_CB* p_t2t = &rw_cb.tcb.t2t;
  uint8_t sector_byte2[1];

  if (p_t2t->state != RW_T2T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf(
        "Error: Type 2 tag not activated or Busy - State: %u", p_t2t->state);
    return (NFC_STATUS_FAILED);
  }

  if (sector >= T2T_MAX_SECTOR) {
    LOG(ERROR) << StringPrintf(
        "RW_T2tSectorSelect - Invalid sector: %u, T2 Max supported sector "
        "value: %u",
        sector, T2T_MAX_SECTOR - 1);
    return (NFC_STATUS_FAILED);
  }

  sector_byte2[0] = 0xFF;

  status = rw_t2t_send_cmd(T2T_CMD_SEC_SEL, sector_byte2);
  if (status == NFC_STATUS_OK) {
    p_t2t->state = RW_T2T_STATE_SELECT_SECTOR;
    p_t2t->select_sector = sector;
    p_t2t->substate = RW_T2T_SUBSTATE_WAIT_SELECT_SECTOR_SUPPORT;

    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("RW_T2tSectorSelect Sent Sector select first command");
  }

  return status;
}

/*******************************************************************************
**
** Function         rw_t2t_get_state_name
**
** Description      This function returns the state name.
**
** NOTE             conditionally compiled to save memory.
**
** Returns          string
**
*******************************************************************************/
static std::string rw_t2t_get_state_name(uint8_t state) {
  switch (state) {
    case RW_T2T_STATE_NOT_ACTIVATED:
      return "NOT_ACTIVATED";
    case RW_T2T_STATE_IDLE:
      return "IDLE";
    case RW_T2T_STATE_READ:
      return "APP_READ";
    case RW_T2T_STATE_WRITE:
      return "APP_WRITE";
    case RW_T2T_STATE_SELECT_SECTOR:
      return "SECTOR_SELECT";
    case RW_T2T_STATE_DETECT_TLV:
      return "TLV_DETECT";
    case RW_T2T_STATE_READ_NDEF:
      return "READ_NDEF";
    case RW_T2T_STATE_WRITE_NDEF:
      return "WRITE_NDEF";
    case RW_T2T_STATE_SET_TAG_RO:
      return "SET_TAG_RO";
    case RW_T2T_STATE_CHECK_PRESENCE:
      return "CHECK_PRESENCE";
    default:
      return "???? UNKNOWN STATE";
  }
}

/*******************************************************************************
**
** Function         rw_t2t_get_substate_name
**
** Description      This function returns the substate name.
**
** NOTE             conditionally compiled to save memory.
**
** Returns          pointer to the name
**
*******************************************************************************/
static std::string rw_t2t_get_substate_name(uint8_t substate) {
  switch (substate) {
    case RW_T2T_SUBSTATE_NONE:
      return "RW_T2T_SUBSTATE_NONE";
    case RW_T2T_SUBSTATE_WAIT_SELECT_SECTOR_SUPPORT:
      return "RW_T2T_SUBSTATE_WAIT_SELECT_SECTOR_SUPPORT";
    case RW_T2T_SUBSTATE_WAIT_SELECT_SECTOR:
      return "RW_T2T_SUBSTATE_WAIT_SELECT_SECTOR";
    case RW_T2T_SUBSTATE_WAIT_READ_CC:
      return "RW_T2T_SUBSTATE_WAIT_READ_CC";
    case RW_T2T_SUBSTATE_WAIT_TLV_DETECT:
      return "RW_T2T_SUBSTATE_WAIT_TLV_DETECT";
    case RW_T2T_SUBSTATE_WAIT_FIND_LEN_FIELD_LEN:
      return "RW_T2T_SUBSTATE_WAIT_FIND_LEN_FIELD_LEN";
    case RW_T2T_SUBSTATE_WAIT_READ_TLV_LEN0:
      return "RW_T2T_SUBSTATE_WAIT_READ_TLV_LEN0";
    case RW_T2T_SUBSTATE_WAIT_READ_TLV_LEN1:
      return "RW_T2T_SUBSTATE_WAIT_READ_TLV_LEN1";
    case RW_T2T_SUBSTATE_WAIT_READ_TLV_VALUE:
      return "RW_T2T_SUBSTATE_WAIT_READ_TLV_VALUE";
    case RW_T2T_SUBSTATE_WAIT_READ_LOCKS:
      return "RW_T2T_SUBSTATE_WAIT_READ_LOCKS";
    case RW_T2T_SUBSTATE_WAIT_READ_NDEF_FIRST_BLOCK:
      return "RW_T2T_SUBSTATE_WAIT_READ_NDEF_FIRST_BLOCK";
    case RW_T2T_SUBSTATE_WAIT_READ_NDEF_LAST_BLOCK:
      return "RW_T2T_SUBSTATE_WAIT_READ_NDEF_LAST_BLOCK";
    case RW_T2T_SUBSTATE_WAIT_READ_TERM_TLV_BLOCK:
      return "RW_T2T_SUBSTATE_WAIT_READ_TERM_TLV_BLOCK";
    case RW_T2T_SUBSTATE_WAIT_READ_NDEF_NEXT_BLOCK:
      return "RW_T2T_SUBSTATE_WAIT_READ_NDEF_NEXT_BLOCK";
    case RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_NEXT_BLOCK:
      return "RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_NEXT_BLOCK";
    case RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LAST_BLOCK:
      return "RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LAST_BLOCK";
    case RW_T2T_SUBSTATE_WAIT_READ_NDEF_LEN_BLOCK:
      return "RW_T2T_SUBSTATE_WAIT_READ_NDEF_LEN_BLOCK";
    case RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LEN_BLOCK:
      return "RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LEN_BLOCK";
    case RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LEN_NEXT_BLOCK:
      return "RW_T2T_SUBSTATE_WAIT_WRITE_NDEF_LEN_NEXT_BLOCK";
    case RW_T2T_SUBSTATE_WAIT_WRITE_TERM_TLV_CMPLT:
      return "RW_T2T_SUBSTATE_WAIT_WRITE_TERM_TLV_CMPLT";
    default:
      return "???? UNKNOWN SUBSTATE";
  }
}
