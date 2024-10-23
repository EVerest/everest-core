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
 *  This file contains the implementation for Type 1 tag in Reader/Writer
 *  mode.
 *
 ******************************************************************************/
#include <string>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nfc_target.h"

#include "gki.h"
#include "nci_hmsgs.h"
#include "nfc_api.h"
#include "nfc_int.h"
#include "rw_api.h"
#include "rw_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;
extern unsigned char appl_dta_mode_flag;

/* Local Functions */
static tRW_EVENT rw_t1t_handle_rid_rsp(NFC_HDR* p_pkt);
static void rw_t1t_data_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                              tNFC_CONN* p_data);
static void rw_t1t_process_frame_error(void);
static void rw_t1t_process_error(void);
static void rw_t1t_handle_presence_check_rsp(tNFC_STATUS status);
static std::string rw_t1t_get_state_name(uint8_t state);

/*******************************************************************************
**
** Function         rw_t1t_data_cback
**
** Description      This callback function handles data from NFCC.
**
** Returns          none
**
*******************************************************************************/
static void rw_t1t_data_cback(__attribute__((unused)) uint8_t conn_id,
                              __attribute__((unused)) tNFC_CONN_EVT event,
                              tNFC_CONN* p_data) {
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;
  tRW_EVENT rw_event = RW_RAW_FRAME_EVT;
  bool b_notify = true;
  tRW_DATA evt_data;
  NFC_HDR* p_pkt;
  uint8_t* p;
  tT1T_CMD_RSP_INFO* p_cmd_rsp_info =
      (tT1T_CMD_RSP_INFO*)rw_cb.tcb.t1t.p_cmd_rsp_info;
  uint8_t begin_state = p_t1t->state;

  p_pkt = (NFC_HDR*)(p_data->data.p_data);
  if (p_pkt == nullptr) return;
  /* Assume the data is just the response byte sequence */
  p = (uint8_t*)(p_pkt + 1) + p_pkt->offset;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "state:%s (%d)", rw_t1t_get_state_name(p_t1t->state).c_str(),
      p_t1t->state);

  evt_data.status = NFC_STATUS_OK;

  if ((p_t1t->state == RW_T1T_STATE_IDLE) || (!p_cmd_rsp_info)) {
    /* If previous command was retransmitted and if response is pending to
     * previous command retransmission,
     * check if lenght and ADD/ADD8/ADDS field matches the expected value of
     * previous
     * retransmited command response. However, ignore ADD field if the command
     * was RALL/RID
     */
    if ((p_t1t->prev_cmd_rsp_info.pend_retx_rsp) &&
        (p_t1t->prev_cmd_rsp_info.rsp_len == p_pkt->len) &&
        ((p_t1t->prev_cmd_rsp_info.op_code == T1T_CMD_RID) ||
         (p_t1t->prev_cmd_rsp_info.op_code == T1T_CMD_RALL) ||
         (p_t1t->prev_cmd_rsp_info.addr == *p))) {
      /* Response to previous command retransmission */
      LOG(ERROR) << StringPrintf(
          "T1T Response to previous command in Idle state. command=0x%02x, "
          "Remaining max retx rsp:0x%02x ",
          p_t1t->prev_cmd_rsp_info.op_code,
          p_t1t->prev_cmd_rsp_info.pend_retx_rsp - 1);
      p_t1t->prev_cmd_rsp_info.pend_retx_rsp--;
      GKI_freebuf(p_pkt);
    } else {
      /* Raw frame event */
      evt_data.data.p_data = p_pkt;
      (*rw_cb.p_cback)(RW_T1T_RAW_FRAME_EVT, &evt_data);
    }
    return;
  }

#if (RW_STATS_INCLUDED == TRUE)
  /* Update rx stats */
  rw_main_update_rx_stats(p_pkt->len);
#endif /* RW_STATS_INCLUDED */

  if ((p_pkt->len != p_cmd_rsp_info->rsp_len) ||
      ((p_cmd_rsp_info->opcode != T1T_CMD_RALL) &&
       (p_cmd_rsp_info->opcode != T1T_CMD_RID) && (*p != p_t1t->addr)))

  {
    /* If previous command was retransmitted and if response is pending to
     * previous command retransmission,
     * then check if lenght and ADD/ADD8/ADDS field matches the expected value
     * of previous
     * retransmited command response. However, ignore ADD field if the command
     * was RALL/RID
     */
    if ((p_t1t->prev_cmd_rsp_info.pend_retx_rsp) &&
        (p_t1t->prev_cmd_rsp_info.rsp_len == p_pkt->len) &&
        ((p_t1t->prev_cmd_rsp_info.op_code == T1T_CMD_RID) ||
         (p_t1t->prev_cmd_rsp_info.op_code == T1T_CMD_RALL) ||
         (p_t1t->prev_cmd_rsp_info.addr == *p))) {
      LOG(ERROR) << StringPrintf(
          "T1T Response to previous command. command=0x%02x, Remaining max "
          "retx rsp:0x%02x",
          p_t1t->prev_cmd_rsp_info.op_code,
          p_t1t->prev_cmd_rsp_info.pend_retx_rsp - 1);
      p_t1t->prev_cmd_rsp_info.pend_retx_rsp--;
    } else {
      /* Stop timer as some response to current command is received */
      nfc_stop_quick_timer(&p_t1t->timer);
/* Retrasmit the last sent command if retry-count < max retry */
      LOG(ERROR) << StringPrintf(
          "T1T Frame error. state=%s command (opcode) = 0x%02x",
          rw_t1t_get_state_name(p_t1t->state).c_str(), p_cmd_rsp_info->opcode);
      rw_t1t_process_frame_error();
    }
    GKI_freebuf(p_pkt);
    return;
  }

  /* Stop timer as response to current command is received */
  nfc_stop_quick_timer(&p_t1t->timer);

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("RW RECV [%s]:0x%x RSP", t1t_info_to_str(p_cmd_rsp_info),
                      p_cmd_rsp_info->opcode);

  /* If we did not receive response to all retransmitted previous command,
   * dont expect that as response have come for the current command itself.
   */
  if (p_t1t->prev_cmd_rsp_info.pend_retx_rsp)
    memset(&(p_t1t->prev_cmd_rsp_info), 0, sizeof(tRW_T1T_PREV_CMD_RSP_INFO));

  if (rw_cb.cur_retry) {
    /* If the current command was retransmitted to get this response, we might
       get
       response later to all or some of the retrasnmission of the current
       command
     */
    p_t1t->prev_cmd_rsp_info.addr = ((p_cmd_rsp_info->opcode != T1T_CMD_RALL) &&
                                     (p_cmd_rsp_info->opcode != T1T_CMD_RID))
                                        ? p_t1t->addr
                                        : 0;
    p_t1t->prev_cmd_rsp_info.rsp_len = p_cmd_rsp_info->rsp_len;
    p_t1t->prev_cmd_rsp_info.op_code = p_cmd_rsp_info->opcode;
    p_t1t->prev_cmd_rsp_info.pend_retx_rsp = (uint8_t)rw_cb.cur_retry;
  }

  rw_cb.cur_retry = 0;

  if (p_cmd_rsp_info->opcode == T1T_CMD_RID) {
    rw_event = rw_t1t_handle_rid_rsp(p_pkt);
  } else {
    rw_event =
        rw_t1t_handle_rsp(p_cmd_rsp_info, &b_notify, p, &evt_data.status);
  }

  if (b_notify) {
    if ((p_t1t->state != RW_T1T_STATE_READ) &&
        (p_t1t->state != RW_T1T_STATE_WRITE)) {
      GKI_freebuf(p_pkt);
      evt_data.data.p_data = nullptr;
    } else {
      evt_data.data.p_data = p_pkt;
    }
    rw_t1t_handle_op_complete();
    (*rw_cb.p_cback)(rw_event, &evt_data);
  } else
    GKI_freebuf(p_pkt);

  if (begin_state != p_t1t->state) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("RW T1T state changed:<%s> -> <%s>",
                        rw_t1t_get_state_name(begin_state).c_str(),
                        rw_t1t_get_state_name(p_t1t->state).c_str());
  }
}

/*******************************************************************************
**
** Function         rw_t1t_conn_cback
**
** Description      This callback function receives the events/data from NFCC.
**
** Returns          none
**
*******************************************************************************/
void rw_t1t_conn_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                       tNFC_CONN* p_data) {
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "rw_t1t_conn_cback: conn_id=%i, evt=0x%x", conn_id, event);
  /* Only handle static conn_id */
  if (conn_id != NFC_RF_CONN_ID) {
    LOG(WARNING) << StringPrintf(
        "rw_t1t_conn_cback - Not static connection id: =%i", conn_id);
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
#endif /* RW_STATS_INCLUDED */

      /* Stop t1t timer (if started) */
      nfc_stop_quick_timer(&p_t1t->timer);

      /* Free cmd buf for retransmissions */
      if (p_t1t->p_cur_cmd_buf) {
        GKI_freebuf(p_t1t->p_cur_cmd_buf);
        p_t1t->p_cur_cmd_buf = nullptr;
      }

      p_t1t->state = RW_T1T_STATE_NOT_ACTIVATED;
      NFC_SetStaticRfCback(nullptr);
      break;

    case NFC_DATA_CEVT:
      if (p_data != nullptr) {
        if (p_data->data.status == NFC_STATUS_OK) {
          rw_t1t_data_cback(conn_id, event, p_data);
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
      if ((p_t1t->state == RW_T1T_STATE_NOT_ACTIVATED) ||
          (p_t1t->state == RW_T1T_STATE_IDLE)) {
#if (RW_STATS_INCLUDED == TRUE)
        rw_main_update_trans_error_stats();
#endif /* RW_STATS_INCLUDED */

        tRW_READ_DATA evt_data;
        if (event == NFC_ERROR_CEVT)
          evt_data.status = (tNFC_STATUS)(*(uint8_t*)p_data);
        else if (p_data)
          evt_data.status = p_data->status;
        else
          evt_data.status = NFC_STATUS_FAILED;

        evt_data.p_data = nullptr;
        tRW_DATA rw_data;
        rw_data.data = evt_data;
        (*rw_cb.p_cback)(RW_T1T_INTF_ERROR_EVT, &rw_data);
        break;
      }
      nfc_stop_quick_timer(&p_t1t->timer);

#if (RW_STATS_INCLUDED == TRUE)
      rw_main_update_trans_error_stats();
#endif /* RW_STATS_INCLUDED */

      if (p_t1t->state == RW_T1T_STATE_CHECK_PRESENCE) {
        rw_t1t_handle_presence_check_rsp(NFC_STATUS_FAILED);
      } else {
        rw_t1t_process_error();
      }
      break;

    default:
      break;
  }
}

/*******************************************************************************
**
** Function         rw_t1t_send_static_cmd
**
** Description      This function composes a Type 1 Tag command for static
**                  memory and send through NCI to NFCC.
**
** Returns          NFC_STATUS_OK if the command is successfuly sent to NCI
**                  otherwise, error status
**
*******************************************************************************/
tNFC_STATUS rw_t1t_send_static_cmd(uint8_t opcode, uint8_t add, uint8_t dat) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;
  const tT1T_CMD_RSP_INFO* p_cmd_rsp_info = t1t_cmd_to_rsp_info(opcode);
  NFC_HDR* p_data;
  uint8_t* p;

  if (p_cmd_rsp_info) {
    /* a valid opcode for RW */
    p_data = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
    if (p_data) {
      p_t1t->p_cmd_rsp_info = (tT1T_CMD_RSP_INFO*)p_cmd_rsp_info;
      p_t1t->addr = add;
      p_data->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
      p = (uint8_t*)(p_data + 1) + p_data->offset;
      UINT8_TO_BE_STREAM(p, opcode);
      UINT8_TO_BE_STREAM(p, add);
      UINT8_TO_BE_STREAM(p, dat);

      ARRAY_TO_STREAM(p, p_t1t->mem, T1T_CMD_UID_LEN);
      p_data->len = p_cmd_rsp_info->cmd_len;

      /* Indicate first attempt to send command, back up cmd buffer in case
       * needed for retransmission */
      rw_cb.cur_retry = 0;
      memcpy(p_t1t->p_cur_cmd_buf, p_data,
             sizeof(NFC_HDR) + p_data->offset + p_data->len);

#if (RW_STATS_INCLUDED == TRUE)
      /* Update stats */
      rw_main_update_tx_stats(p_data->len, false);
#endif /* RW_STATS_INCLUDED */

      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "RW SENT [%s]:0x%x CMD", t1t_info_to_str(p_cmd_rsp_info),
          p_cmd_rsp_info->opcode);
      status = NFC_SendData(NFC_RF_CONN_ID, p_data);
      if (status == NFC_STATUS_OK) {
        nfc_start_quick_timer(
            &p_t1t->timer, NFC_TTYPE_RW_T1T_RESPONSE,
            (RW_T1T_TOUT_RESP * QUICK_TIMER_TICKS_PER_SEC) / 1000);
      }
    } else {
      status = NFC_STATUS_NO_BUFFERS;
    }
  }
  return status;
}

/*******************************************************************************
**
** Function         rw_t1t_send_dyn_cmd
**
** Description      This function composes a Type 1 Tag command for dynamic
**                  memory and send through NCI to NFCC.
**
** Returns          NFC_STATUS_OK if the command is successfuly sent to NCI
**                  otherwise, error status
**
*******************************************************************************/
tNFC_STATUS rw_t1t_send_dyn_cmd(uint8_t opcode, uint8_t add, uint8_t* p_dat) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;
  const tT1T_CMD_RSP_INFO* p_cmd_rsp_info = t1t_cmd_to_rsp_info(opcode);
  NFC_HDR* p_data;
  uint8_t* p;

  if (p_cmd_rsp_info) {
    /* a valid opcode for RW */
    p_data = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
    if (p_data) {
      p_t1t->p_cmd_rsp_info = (tT1T_CMD_RSP_INFO*)p_cmd_rsp_info;
      p_t1t->addr = add;
      p_data->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
      p = (uint8_t*)(p_data + 1) + p_data->offset;
      UINT8_TO_BE_STREAM(p, opcode);
      UINT8_TO_BE_STREAM(p, add);

      if (p_dat) {
        ARRAY_TO_STREAM(p, p_dat, 8);
      } else {
        memset(p, 0, 8);
        p += 8;
      }
      ARRAY_TO_STREAM(p, p_t1t->mem, T1T_CMD_UID_LEN);
      p_data->len = p_cmd_rsp_info->cmd_len;

      /* Indicate first attempt to send command, back up cmd buffer in case
       * needed for retransmission */
      rw_cb.cur_retry = 0;
      memcpy(p_t1t->p_cur_cmd_buf, p_data,
             sizeof(NFC_HDR) + p_data->offset + p_data->len);

#if (RW_STATS_INCLUDED == TRUE)
      /* Update stats */
      rw_main_update_tx_stats(p_data->len, false);
#endif /* RW_STATS_INCLUDED */

      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "RW SENT [%s]:0x%x CMD", t1t_info_to_str(p_cmd_rsp_info),
          p_cmd_rsp_info->opcode);

      status = NFC_SendData(NFC_RF_CONN_ID, p_data);
      if (status == NFC_STATUS_OK) {
        nfc_start_quick_timer(
            &p_t1t->timer, NFC_TTYPE_RW_T1T_RESPONSE,
            (RW_T1T_TOUT_RESP * QUICK_TIMER_TICKS_PER_SEC) / 1000);
      }
    } else {
      status = NFC_STATUS_NO_BUFFERS;
    }
  }
  return status;
}

/*****************************************************************************
**
** Function         rw_t1t_handle_rid_rsp
**
** Description      Handles response to RID: Collects HR, UID, notify up the
**                  stack
**
** Returns          event to notify application
**
*****************************************************************************/
static tRW_EVENT rw_t1t_handle_rid_rsp(NFC_HDR* p_pkt) {
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;
  tRW_DATA evt_data;
  uint8_t* p_rid_rsp;

  evt_data.status = NFC_STATUS_OK;
  evt_data.data.p_data = p_pkt;

  /* Assume the data is just the response byte sequence */
  p_rid_rsp = (uint8_t*)(p_pkt + 1) + p_pkt->offset;

  /* Response indicates tag is present */
  if (p_t1t->state == RW_T1T_STATE_CHECK_PRESENCE) {
    /* If checking for the presence of the tag then just notify */
    return RW_T1T_PRESENCE_CHECK_EVT;
  }

  /* Extract HR and UID from response */
  STREAM_TO_ARRAY(p_t1t->hr, p_rid_rsp, T1T_HR_LEN);

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("hr0:0x%x, hr1:0x%x", p_t1t->hr[0], p_t1t->hr[1]);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("UID0-3=%02x%02x%02x%02x", p_rid_rsp[0], p_rid_rsp[1],
                      p_rid_rsp[2], p_rid_rsp[3]);

  /* Fetch UID0-3 from RID response message */
  STREAM_TO_ARRAY(p_t1t->mem, p_rid_rsp, T1T_CMD_UID_LEN);

  /* Notify RID response Event */
  return RW_T1T_RID_EVT;
}

/*******************************************************************************
**
** Function         rw_t1t_select
**
** Description      This function will set the callback function to
**                  receive data from lower layers and also send rid command
**
** Returns          none
**
*******************************************************************************/
tNFC_STATUS rw_t1t_select(uint8_t hr[T1T_HR_LEN],
                          uint8_t uid[T1T_CMD_UID_LEN]) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;

  p_t1t->state = RW_T1T_STATE_NOT_ACTIVATED;

  /* Alloc cmd buf for retransmissions */
  if (p_t1t->p_cur_cmd_buf == nullptr) {
    p_t1t->p_cur_cmd_buf = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
    if (p_t1t->p_cur_cmd_buf == nullptr) {
      LOG(ERROR) << StringPrintf(
          "rw_t1t_select: unable to allocate buffer for retransmission");
      return status;
    }
  }

  memcpy(p_t1t->hr, hr, T1T_HR_LEN);
  memcpy(p_t1t->mem, uid, T1T_CMD_UID_LEN);

  NFC_SetStaticRfCback(rw_t1t_conn_cback);

  p_t1t->state = RW_T1T_STATE_IDLE;

  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         rw_t1t_process_timeout
**
** Description      process timeout event
**
** Returns          none
**
*******************************************************************************/
void rw_t1t_process_timeout(__attribute__((unused)) TIMER_LIST_ENT* p_tle) {
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;

  LOG(ERROR) << StringPrintf("T1T timeout. state=%s command (opcode)=0x%02x ",
                             rw_t1t_get_state_name(p_t1t->state).c_str(),
                             (rw_cb.tcb.t1t.p_cmd_rsp_info)->opcode);

  if (p_t1t->state == RW_T1T_STATE_CHECK_PRESENCE) {
    /* Tag has moved from range */
    rw_t1t_handle_presence_check_rsp(NFC_STATUS_FAILED);
  } else if (p_t1t->state != RW_T1T_STATE_IDLE) {
    rw_t1t_process_error();
  }
}

/*******************************************************************************
**
** Function         rw_t1t_process_frame_error
**
** Description      Process frame crc error
**
** Returns          none
**
*******************************************************************************/
static void rw_t1t_process_frame_error(void) {
#if (RW_STATS_INCLUDED == TRUE)
  /* Update stats */
  rw_main_update_crc_error_stats();
#endif /* RW_STATS_INCLUDED */

  /* Process the error */
  rw_t1t_process_error();
}

/*******************************************************************************
**
** Function         rw_t1t_process_error
**
** Description      process timeout event
**
** Returns          none
**
*******************************************************************************/
static void rw_t1t_process_error(void) {
  tRW_EVENT rw_event;
  NFC_HDR* p_cmd_buf;
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;
  tT1T_CMD_RSP_INFO* p_cmd_rsp_info =
      (tT1T_CMD_RSP_INFO*)rw_cb.tcb.t1t.p_cmd_rsp_info;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("State: %u", p_t1t->state);

  /* Retry sending command if retry-count < max */
  if (rw_cb.cur_retry < RW_MAX_RETRIES) {
    /* retry sending the command */
    rw_cb.cur_retry++;

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "T1T retransmission attempt %i of %i", rw_cb.cur_retry, RW_MAX_RETRIES);

    /* allocate a new buffer for message */
    p_cmd_buf = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
    if (p_cmd_buf != nullptr) {
      memcpy(p_cmd_buf, p_t1t->p_cur_cmd_buf, sizeof(NFC_HDR) +
                                                  p_t1t->p_cur_cmd_buf->offset +
                                                  p_t1t->p_cur_cmd_buf->len);

#if (RW_STATS_INCLUDED == TRUE)
      /* Update stats */
      rw_main_update_tx_stats(p_cmd_buf->len, true);
#endif /* RW_STATS_INCLUDED */

      if (NFC_SendData(NFC_RF_CONN_ID, p_cmd_buf) == NFC_STATUS_OK) {
        /* Start timer for waiting for response */
        nfc_start_quick_timer(
            &p_t1t->timer, NFC_TTYPE_RW_T1T_RESPONSE,
            (RW_T1T_TOUT_RESP * QUICK_TIMER_TICKS_PER_SEC) / 1000);

        return;
      }
    }
  } else {
    /* we might get response later to all or some of the retrasnmission
     * of the current command, update previous command response information */
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "T1T maximum retransmission attempts reached (%i)", RW_MAX_RETRIES);
    p_t1t->prev_cmd_rsp_info.addr = ((p_cmd_rsp_info->opcode != T1T_CMD_RALL) &&
                                     (p_cmd_rsp_info->opcode != T1T_CMD_RID))
                                        ? p_t1t->addr
                                        : 0;
    p_t1t->prev_cmd_rsp_info.rsp_len = p_cmd_rsp_info->rsp_len;
    p_t1t->prev_cmd_rsp_info.op_code = p_cmd_rsp_info->opcode;
    p_t1t->prev_cmd_rsp_info.pend_retx_rsp = RW_MAX_RETRIES;
  }

#if (RW_STATS_INCLUDED == TRUE)
  /* update failure count */
  rw_main_update_fail_stats();
#endif /* RW_STATS_INCLUDED */

  rw_event = rw_t1t_info_to_event(p_cmd_rsp_info);
  if (p_t1t->state != RW_T1T_STATE_NOT_ACTIVATED) rw_t1t_handle_op_complete();

  if (rw_event == RW_T2T_NDEF_DETECT_EVT) {
    tRW_DETECT_NDEF_DATA ndef_data;
    ndef_data.status = NFC_STATUS_TIMEOUT;
    ndef_data.protocol = NFC_PROTOCOL_T1T;
    ndef_data.flags = RW_NDEF_FL_UNKNOWN;
    ndef_data.max_size = 0;
    ndef_data.cur_size = 0;
    tRW_DATA rw_data;
    rw_data.ndef = ndef_data;
    (*rw_cb.p_cback)(rw_event, &rw_data);
  } else {
    tRW_READ_DATA evt_data;
    evt_data.status = NFC_STATUS_TIMEOUT;
    evt_data.p_data = nullptr;
    tRW_DATA rw_data;
    rw_data.data = evt_data;
    (*rw_cb.p_cback)(rw_event, &rw_data);
  }
}

/*****************************************************************************
**
** Function         rw_t1t_handle_presence_check_rsp
**
** Description      Handle response to presence check
**
** Returns          Nothing
**
*****************************************************************************/
void rw_t1t_handle_presence_check_rsp(tNFC_STATUS status) {
  tRW_DATA rw_data;

  /* Notify, Tag is present or not */
  rw_data.data.status = status;
  rw_t1t_handle_op_complete();

  (*(rw_cb.p_cback))(RW_T1T_PRESENCE_CHECK_EVT, &rw_data);
}

/*****************************************************************************
**
** Function         rw_t1t_handle_op_complete
**
** Description      Reset to IDLE state
**
** Returns          Nothing
**
*****************************************************************************/
void rw_t1t_handle_op_complete(void) {
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;

  p_t1t->state = RW_T1T_STATE_IDLE;
#if (RW_NDEF_INCLUDED == TRUE)
  if (appl_dta_mode_flag == 0 && (p_t1t->state != RW_T1T_STATE_READ_NDEF)) {
    p_t1t->b_update = false;
    p_t1t->b_rseg = false;
  }
  p_t1t->substate = RW_T1T_SUBSTATE_NONE;
#endif
  return;
}

/*****************************************************************************
**
** Function         RW_T1tPresenceCheck
**
** Description
**      Check if the tag is still in the field.
**
**      The RW_T1T_PRESENCE_CHECK_EVT w/ status is used to indicate presence
**      or non-presence.
**
** Returns
**      NFC_STATUS_OK, if raw data frame sent
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_T1tPresenceCheck(void) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_DATA evt_data;
  tRW_CB* p_rw_cb = &rw_cb;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* If RW_SelectTagType was not called (no conn_callback) return failure */
  if (!p_rw_cb->p_cback) {
    retval = NFC_STATUS_FAILED;
  }
  /* If we are not activated, then RW_T1T_PRESENCE_CHECK_EVT status=FAIL */
  else if (p_rw_cb->tcb.t1t.state == RW_T1T_STATE_NOT_ACTIVATED) {
    evt_data.status = NFC_STATUS_FAILED;
    (*p_rw_cb->p_cback)(RW_T1T_PRESENCE_CHECK_EVT, &evt_data);
  }
  /* If command is pending, assume tag is still present */
  else if (p_rw_cb->tcb.t1t.state != RW_T1T_STATE_IDLE) {
    evt_data.status = NFC_STATUS_OK;
    (*p_rw_cb->p_cback)(RW_T1T_PRESENCE_CHECK_EVT, &evt_data);
  } else {
    /* IDLE state: send a RID command to the tag to see if it responds */
    retval = rw_t1t_send_static_cmd(T1T_CMD_RID, 0, 0);
    if (retval == NFC_STATUS_OK) {
      p_rw_cb->tcb.t1t.state = RW_T1T_STATE_CHECK_PRESENCE;
    }
  }

  return (retval);
}

/*****************************************************************************
**
** Function         RW_T1tRid
**
** Description
**      This function sends a RID command for Reader/Writer mode.
**
** Returns
**      NFC_STATUS_OK, if raw data frame sent
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_T1tRid(void) {
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;
  tNFC_STATUS status = NFC_STATUS_FAILED;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (p_t1t->state != RW_T1T_STATE_IDLE) {
    LOG(WARNING) << StringPrintf("RW_T1tRid - Busy - State: %u", p_t1t->state);
    return (NFC_STATUS_BUSY);
  }

  /* send a RID command */
  status = rw_t1t_send_static_cmd(T1T_CMD_RID, 0, 0);
  if (status == NFC_STATUS_OK) {
    p_t1t->state = RW_T1T_STATE_READ;
  }

  return (status);
}

/*******************************************************************************
**
** Function         RW_T1tReadAll
**
** Description      This function sends a RALL command for Reader/Writer mode.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS RW_T1tReadAll(void) {
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;
  tNFC_STATUS status = NFC_STATUS_FAILED;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (p_t1t->state != RW_T1T_STATE_IDLE) {
    LOG(WARNING) << StringPrintf("RW_T1tReadAll - Busy - State: %u",
                                 p_t1t->state);
    return (NFC_STATUS_BUSY);
  }

  /* send RALL command */
  status = rw_t1t_send_static_cmd(T1T_CMD_RALL, 0, 0);
  if (status == NFC_STATUS_OK) {
    p_t1t->state = RW_T1T_STATE_READ;
  }

  return status;
}

/*******************************************************************************
**
** Function         RW_T1tRead
**
** Description      This function sends a READ command for Reader/Writer mode.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS RW_T1tRead(uint8_t block, uint8_t byte) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;
  uint8_t addr;

  if (p_t1t->state != RW_T1T_STATE_IDLE) {
    LOG(WARNING) << StringPrintf("RW_T1tRead - Busy - State: %u", p_t1t->state);
    return (NFC_STATUS_BUSY);
  }

  /* send READ command */
  RW_T1T_BLD_ADD((addr), (block), (byte));
  status = rw_t1t_send_static_cmd(T1T_CMD_READ, addr, 0);
  if (status == NFC_STATUS_OK) {
    p_t1t->state = RW_T1T_STATE_READ;
  }
  return status;
}

/*******************************************************************************
**
** Function         RW_T1tWriteErase
**
** Description      This function sends a WRITE-E command for Reader/Writer
**                  mode.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS RW_T1tWriteErase(uint8_t block, uint8_t byte, uint8_t new_byte) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;
  uint8_t addr;

  if (p_t1t->state != RW_T1T_STATE_IDLE) {
    LOG(WARNING) << StringPrintf("RW_T1tWriteErase - Busy - State: %u",
                                 p_t1t->state);
    return (NFC_STATUS_BUSY);
  }
  if ((p_t1t->tag_attribute == RW_T1_TAG_ATTRB_READ_ONLY) &&
      (block != T1T_CC_BLOCK) && (byte != T1T_CC_RWA_OFFSET)) {
    LOG(ERROR) << StringPrintf("RW_T1tWriteErase - Tag is in Read only state");
    return (NFC_STATUS_REFUSED);
  }
  if ((block >= T1T_STATIC_BLOCKS) || (byte >= T1T_BLOCK_SIZE)) {
    LOG(ERROR) << StringPrintf("RW_T1tWriteErase - Invalid Block/byte: %u / %u",
                               block, byte);
    return (NFC_STATUS_REFUSED);
  }
  if ((block == T1T_UID_BLOCK) || (block == T1T_RES_BLOCK)) {
    LOG(WARNING) << StringPrintf(
        "RW_T1tWriteErase - Cannot write to Locked block: %u", block);
    return (NFC_STATUS_REFUSED);
  }
  /* send WRITE-E command */
  RW_T1T_BLD_ADD((addr), (block), (byte));
  status = rw_t1t_send_static_cmd(T1T_CMD_WRITE_E, addr, new_byte);
  if (status == NFC_STATUS_OK) {
    p_t1t->state = RW_T1T_STATE_WRITE;
    if (block < T1T_BLOCKS_PER_SEGMENT) {
      p_t1t->b_update = false;
      p_t1t->b_rseg = false;
    }
  }
  return status;
}

/*******************************************************************************
**
** Function         RW_T1tWriteNoErase
**
** Description      This function sends a WRITE-NE command for Reader/Writer
**                  mode.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS RW_T1tWriteNoErase(uint8_t block, uint8_t byte, uint8_t new_byte) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;
  uint8_t addr;

  if (p_t1t->state != RW_T1T_STATE_IDLE) {
    LOG(WARNING) << StringPrintf("RW_T1tWriteNoErase - Busy - State: %u",
                                 p_t1t->state);
    return (NFC_STATUS_BUSY);
  }
  if ((p_t1t->tag_attribute == RW_T1_TAG_ATTRB_READ_ONLY) &&
      (block != T1T_CC_BLOCK) && (byte != T1T_CC_RWA_OFFSET)) {
    LOG(ERROR) << StringPrintf("RW_T1tWriteErase - Tag is in Read only state");
    return (NFC_STATUS_REFUSED);
  }
  if ((block >= T1T_STATIC_BLOCKS) || (byte >= T1T_BLOCK_SIZE)) {
    LOG(ERROR) << StringPrintf("RW_T1tWriteErase - Invalid Block/byte: %u / %u",
                               block, byte);
    return (NFC_STATUS_REFUSED);
  }
  if ((block == T1T_UID_BLOCK) || (block == T1T_RES_BLOCK)) {
    LOG(WARNING) << StringPrintf(
        "RW_T1tWriteNoErase - Cannot write to Locked block: %u", block);
    return (NFC_STATUS_REFUSED);
  }
  /* send WRITE-NE command */
  RW_T1T_BLD_ADD((addr), (block), (byte));
  status = rw_t1t_send_static_cmd(T1T_CMD_WRITE_NE, addr, new_byte);
  if (status == NFC_STATUS_OK) {
    p_t1t->state = RW_T1T_STATE_WRITE;
    if (block < T1T_BLOCKS_PER_SEGMENT) {
      p_t1t->b_update = false;
      p_t1t->b_rseg = false;
    }
  }
  return status;
}

/*******************************************************************************
**
** Function         RW_T1tReadSeg
**
** Description      This function sends a RSEG command for Reader/Writer mode.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS RW_T1tReadSeg(uint8_t segment) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;
  uint8_t adds;

  if (p_t1t->state != RW_T1T_STATE_IDLE) {
    LOG(WARNING) << StringPrintf("RW_T1tReadSeg - Busy - State: %u",
                                 p_t1t->state);
    return (NFC_STATUS_BUSY);
  }
  if (segment >= T1T_MAX_SEGMENTS) {
    LOG(ERROR) << StringPrintf("RW_T1tReadSeg - Invalid Segment: %u", segment);
    return (NFC_STATUS_REFUSED);
  }
  if (rw_cb.tcb.t1t.hr[0] != T1T_STATIC_HR0) {
    /* send RSEG command */
    RW_T1T_BLD_ADDS((adds), (segment));
    status = rw_t1t_send_dyn_cmd(T1T_CMD_RSEG, adds, nullptr);
    if (status == NFC_STATUS_OK) {
      p_t1t->state = RW_T1T_STATE_READ;
    }
  }
  return status;
}

/*******************************************************************************
**
** Function         RW_T1tRead8
**
** Description      This function sends a READ8 command for Reader/Writer mode.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS RW_T1tRead8(uint8_t block) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;

  if (p_t1t->state != RW_T1T_STATE_IDLE) {
    LOG(WARNING) << StringPrintf("RW_T1tRead8 - Busy - State: %u",
                                 p_t1t->state);
    return (NFC_STATUS_BUSY);
  }

  if (rw_cb.tcb.t1t.hr[0] != T1T_STATIC_HR0 ||
      rw_cb.tcb.t1t.hr[1] >= RW_T1T_HR1_MIN) {
    /* send READ8 command */
    status = rw_t1t_send_dyn_cmd(T1T_CMD_READ8, block, nullptr);
    if (status == NFC_STATUS_OK) {
      p_t1t->state = RW_T1T_STATE_READ;
    }
  }
  return status;
}

/*******************************************************************************
**
** Function         RW_T1tWriteErase8
**
** Description      This function sends a WRITE-E8 command for Reader/Writer
**                  mode.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS RW_T1tWriteErase8(uint8_t block, uint8_t* p_new_dat) {
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;
  tNFC_STATUS status = NFC_STATUS_FAILED;

  if (p_t1t->state != RW_T1T_STATE_IDLE) {
    LOG(WARNING) << StringPrintf("RW_T1tWriteErase8 - Busy - State: %u",
                                 p_t1t->state);
    return (NFC_STATUS_BUSY);
  }

  if ((p_t1t->tag_attribute == RW_T1_TAG_ATTRB_READ_ONLY) &&
      (block != T1T_CC_BLOCK)) {
    LOG(ERROR) << StringPrintf("RW_T1tWriteErase8 - Tag is in Read only state");
    return (NFC_STATUS_REFUSED);
  }

  if ((block == T1T_UID_BLOCK) || (block == T1T_RES_BLOCK)) {
    LOG(WARNING) << StringPrintf(
        "RW_T1tWriteErase8 - Cannot write to Locked block: %u", block);
    return (NFC_STATUS_REFUSED);
  }

  if (rw_cb.tcb.t1t.hr[0] != T1T_STATIC_HR0 ||
      rw_cb.tcb.t1t.hr[1] >= RW_T1T_HR1_MIN) {
    /* send WRITE-E8 command */
    status = rw_t1t_send_dyn_cmd(T1T_CMD_WRITE_E8, block, p_new_dat);
    if (status == NFC_STATUS_OK) {
      p_t1t->state = RW_T1T_STATE_WRITE;
      if (block < T1T_BLOCKS_PER_SEGMENT) {
        p_t1t->b_update = false;
        p_t1t->b_rseg = false;
      }
    }
  }
  return status;
}

/*******************************************************************************
**
** Function         RW_T1tWriteNoErase8
**
** Description      This function sends a WRITE-NE8 command for Reader/Writer
**                  mode.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS RW_T1tWriteNoErase8(uint8_t block, uint8_t* p_new_dat) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tRW_T1T_CB* p_t1t = &rw_cb.tcb.t1t;

  if (p_t1t->state != RW_T1T_STATE_IDLE) {
    LOG(WARNING) << StringPrintf("RW_T1tWriteNoErase8 - Busy - State: %u",
                                 p_t1t->state);
    return (NFC_STATUS_BUSY);
  }

  if ((p_t1t->tag_attribute == RW_T1_TAG_ATTRB_READ_ONLY) &&
      (block != T1T_CC_BLOCK)) {
    LOG(ERROR) << StringPrintf(
        "RW_T1tWriteNoErase8 - Tag is in Read only state");
    return (NFC_STATUS_REFUSED);
  }

  if ((block == T1T_UID_BLOCK) || (block == T1T_RES_BLOCK)) {
    LOG(WARNING) << StringPrintf(
        "RW_T1tWriteNoErase8 - Cannot write to Locked block: %u", block);
    return (NFC_STATUS_REFUSED);
  }

  if (rw_cb.tcb.t1t.hr[0] != T1T_STATIC_HR0 ||
      rw_cb.tcb.t1t.hr[1] >= RW_T1T_HR1_MIN) {
    /* send WRITE-NE command */
    status = rw_t1t_send_dyn_cmd(T1T_CMD_WRITE_NE8, block, p_new_dat);
    if (status == NFC_STATUS_OK) {
      p_t1t->state = RW_T1T_STATE_WRITE;
      if (block < T1T_BLOCKS_PER_SEGMENT) {
        p_t1t->b_update = false;
        p_t1t->b_rseg = false;
      }
    }
  }
  return status;
}

/*******************************************************************************
**
** Function         rw_t1t_get_state_name
**
** Description      This function returns the state name.
**
** NOTE             conditionally compiled to save memory.
**
** Returns          pointer to the name
**
*******************************************************************************/
static std::string rw_t1t_get_state_name(uint8_t state) {
  switch (state) {
    case RW_T1T_STATE_IDLE:
      return "IDLE";
    case RW_T1T_STATE_NOT_ACTIVATED:
      return "NOT_ACTIVATED";
    case RW_T1T_STATE_READ:
      return "APP_READ";
    case RW_T1T_STATE_WRITE:
      return "APP_WRITE";
    case RW_T1T_STATE_TLV_DETECT:
      return "TLV_DETECTION";
    case RW_T1T_STATE_READ_NDEF:
      return "READING_NDEF";
    case RW_T1T_STATE_WRITE_NDEF:
      return "WRITING_NDEF";
    case RW_T1T_STATE_SET_TAG_RO:
      return "SET_TAG_RO";
    case RW_T1T_STATE_CHECK_PRESENCE:
      return "CHECK_PRESENCE";
    case RW_T1T_STATE_FORMAT_TAG:
      return "FORMAT_TAG";
    default:
      return "???? UNKNOWN STATE";
  }
}
