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
 *  This file contains the implementation for Type 3 tag in Reader/Writer
 *  mode.
 *
 ******************************************************************************/
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <log/log.h>

#include "nfc_target.h"

#include "bt_types.h"
#include "nci_hmsgs.h"
#include "nfc_api.h"
#include "nfc_int.h"
#include "rw_api.h"
#include "rw_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/* Definitions for constructing t3t command messages */
#define RW_T3T_FL_PADDING 0x01 /* Padding needed for last NDEF block */
/* Maximum number of NDEF blocks updates that can fit into one command (when all
 * block-numbers are < 256) */
#define RW_T3T_MAX_NDEF_BLOCKS_PER_UPDATE_1_BYTE_FORMAT (13)
/* Maximum number of NDEF blocks updates that can fit into one command (when all
 * block-numbers are >= 256) */
#define RW_T3T_MAX_NDEF_BLOCKS_PER_UPDATE_2_BYTE_FORMAT (12)

/* Definitions for SENSF_RES */
/* Offset of RD in SENSF_RES from NCI_POLL NTF (includes 1 byte SENSF_RES
 * length) */
#define RW_T3T_SENSF_RES_RD_OFFSET 17
#define RW_T3T_SENSF_RES_RD_LEN 2 /* Size of RD in SENSF_RES   */

/* Timeout definitions for commands */
#define RW_T3T_POLL_CMD_TIMEOUT_TICKS \
  ((RW_T3T_TOUT_RESP * 2 * QUICK_TIMER_TICKS_PER_SEC) / 1000)
#define RW_T3T_DEFAULT_CMD_TIMEOUT_TICKS \
  ((RW_T3T_TOUT_RESP * QUICK_TIMER_TICKS_PER_SEC) / 1000)
#define RW_T3T_RAW_FRAME_CMD_TIMEOUT_TICKS \
  (RW_T3T_DEFAULT_CMD_TIMEOUT_TICKS * 4)
#define RW_T3T_MIN_TIMEOUT_TICKS 10

/* Macro to extract major version from NDEF version byte */
#define T3T_GET_MAJOR_VERSION(ver) ((ver) >> 4)

/* Enumeration of API commands */
enum {
  RW_T3T_CMD_DETECT_NDEF,
  RW_T3T_CMD_CHECK_NDEF,
  RW_T3T_CMD_UPDATE_NDEF,
  RW_T3T_CMD_CHECK,
  RW_T3T_CMD_UPDATE,
  RW_T3T_CMD_SEND_RAW_FRAME,
  RW_T3T_CMD_GET_SYSTEM_CODES,
  RW_T3T_CMD_FORMAT,
  RW_T3T_CMD_SET_READ_ONLY_SOFT,
  RW_T3T_CMD_SET_READ_ONLY_HARD,

  RW_T3T_CMD_MAX
};

/* RW_CBACK events corresponding to API comands */
const uint8_t rw_t3t_api_res_evt[RW_T3T_CMD_MAX] = {
    RW_T3T_NDEF_DETECT_EVT,       /* RW_T3T_CMD_DETECT_NDEF */
    RW_T3T_CHECK_CPLT_EVT,        /* RW_T3T_CMD_CHECK_NDEF  */
    RW_T3T_UPDATE_CPLT_EVT,       /* RW_T3T_CMD_UPDATE_NDEF */
    RW_T3T_CHECK_CPLT_EVT,        /* RW_T3T_CMD_CHECK */
    RW_T3T_UPDATE_CPLT_EVT,       /* RW_T3T_CMD_UPDATE */
    RW_T3T_RAW_FRAME_EVT,         /* RW_T3T_CMD_SEND_RAW_FRAME */
    RW_T3T_GET_SYSTEM_CODES_EVT,  /* RW_T3T_CMD_GET_SYSTEM_CODES */
    RW_T3T_FORMAT_CPLT_EVT,       /* RW_T3T_CMD_FORMAT */
    RW_T3T_SET_READ_ONLY_CPLT_EVT /* RW_T3T_CMD_SET_READ_ONLY */
};

/* States */
enum {
  RW_T3T_STATE_NOT_ACTIVATED,
  RW_T3T_STATE_IDLE,
  RW_T3T_STATE_COMMAND_PENDING
};

/* Sub-states */
enum {
  /* Sub states for formatting Felica-Lite */
  RW_T3T_FMT_SST_POLL_FELICA_LITE, /* Waiting for POLL Felica-Lite response (for
                                      formatting) */
  RW_T3T_FMT_SST_CHECK_MC_BLK,     /* Waiting for Felica-Lite MC (MemoryControl)
                                      block-read to complete */
  RW_T3T_FMT_SST_UPDATE_MC_BLK,    /* Waiting for Felica-Lite MC (MemoryControl)
                                      block-write to complete */
  RW_T3T_FMT_SST_UPDATE_NDEF_ATTRIB, /* Waiting for NDEF attribute block-write
                                        to complete */

  /* Sub states for setting Felica-Lite read only */
  RW_T3T_SRO_SST_POLL_FELICA_LITE, /* Waiting for POLL Felica-Lite response (for
                                      setting read only) */
  RW_T3T_SRO_SST_UPDATE_NDEF_ATTRIB, /* Waiting for NDEF attribute block-write
                                        to complete */
  RW_T3T_SRO_SST_CHECK_MC_BLK, /* Waiting for Felica-Lite MC (MemoryControl)
                                  block-read to complete */
  RW_T3T_SRO_SST_UPDATE_MC_BLK /* Waiting for Felica-Lite MC (MemoryControl)
                                  block-write to complete */
};

static std::string rw_t3t_cmd_str(uint8_t cmd_id);
static std::string rw_t3t_state_str(uint8_t state_id);

/* Local static functions */
static void rw_t3t_update_ndef_flag(uint8_t* p_flag);
static tNFC_STATUS rw_t3t_unselect();
static NFC_HDR* rw_t3t_get_cmd_buf(void);
static tNFC_STATUS rw_t3t_send_to_lower(NFC_HDR* p_msg);
static void rw_t3t_handle_get_system_codes_cplt(void);
static void rw_t3t_handle_get_sc_poll_rsp(tRW_T3T_CB* p_cb, uint8_t nci_status,
                                          uint8_t num_responses,
                                          uint8_t sensf_res_buf_size,
                                          uint8_t* p_sensf_res_buf);
static void rw_t3t_handle_ndef_detect_poll_rsp(tRW_T3T_CB* p_cb,
                                               uint8_t nci_status,
                                               uint8_t num_responses);
static void rw_t3t_handle_fmt_poll_rsp(tRW_T3T_CB* p_cb, uint8_t nci_status,
                                       uint8_t num_responses);
static void rw_t3t_handle_sro_poll_rsp(tRW_T3T_CB* p_cb, uint8_t nci_status,
                                       uint8_t num_responses);

/* Default NDEF attribute information block (used when formatting Felica-Lite
 * tags) */
/* NBr (max block reads per cmd)*/
#define RW_T3T_DEFAULT_FELICALITE_NBR 4
/* NBw (max block write per cmd)*/
#define RW_T3T_DEFAULT_FELICALITE_NBW 1
#define RW_T3T_DEFAULT_FELICALITE_NMAXB (T3T_FELICALITE_NMAXB)
#define RW_T3T_DEFAULT_FELICALITE_ATTRIB_INFO_CHECKSUM                       \
  ((T3T_MSG_NDEF_VERSION + RW_T3T_DEFAULT_FELICALITE_NBR +                   \
    RW_T3T_DEFAULT_FELICALITE_NBW + (RW_T3T_DEFAULT_FELICALITE_NMAXB >> 8) + \
    (RW_T3T_DEFAULT_FELICALITE_NMAXB & 0xFF) + T3T_MSG_NDEF_WRITEF_OFF +     \
    T3T_MSG_NDEF_RWFLAG_RW) &                                                \
   0xFFFF)

const uint8_t rw_t3t_default_attrib_info[T3T_MSG_BLOCKSIZE] = {
    T3T_MSG_NDEF_VERSION,                     /* Ver                          */
    RW_T3T_DEFAULT_FELICALITE_NBR,            /* NBr (max block reads per cmd)*/
    RW_T3T_DEFAULT_FELICALITE_NBW,            /* NBw (max block write per cmd)*/
    (RW_T3T_DEFAULT_FELICALITE_NMAXB >> 8),   /* Nmaxb (max size in blocks)   */
    (RW_T3T_DEFAULT_FELICALITE_NMAXB & 0xFF), /* Nmaxb (max size in blocks)   */
    0, 0, 0, 0,                               /* Unused                       */
    T3T_MSG_NDEF_WRITEF_OFF,                  /* WriteF                       */
    T3T_MSG_NDEF_RWFLAG_RW,                   /* RW Flag                      */
    0, 0, 0,                                  /* Ln (current size in bytes)   */

    (RW_T3T_DEFAULT_FELICALITE_ATTRIB_INFO_CHECKSUM >>
     8), /* checksum (high-byte) */
    (RW_T3T_DEFAULT_FELICALITE_ATTRIB_INFO_CHECKSUM &
     0xFF) /* checksum (low-byte)  */

};

/* This is (T/t3t * 4^E) , E is the index of the array. The unit is .0001 ms */
static const uint32_t rw_t3t_mrti_base[] = {302, 1208, 4832, 19328};

/*******************************************************************************
**
** Function         rw_t3t_check_timeout
**
** Description      The timeout value is a + b * number_blocks)
**
** Returns          timeout value in ticks
**
*******************************************************************************/
static uint32_t rw_t3t_check_timeout(uint16_t num_blocks) {
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;
  uint32_t timeout;
  uint32_t extra;

  timeout = (p_cb->check_tout_a + num_blocks * p_cb->check_tout_b) *
            QUICK_TIMER_TICKS_PER_SEC / 1000000;
  /* allow some extra time for driver */
  extra = (timeout / 10) + RW_T3T_MIN_TIMEOUT_TICKS;
  timeout += extra;

  return timeout;
}

/*******************************************************************************
**
** Function         rw_t3t_update_timeout
**
** Description      The timeout value is a + b * number_blocks)
**
** Returns          timeout value in ticks
**
*******************************************************************************/
static uint32_t rw_t3t_update_timeout(uint16_t num_blocks) {
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;
  uint32_t timeout;
  uint32_t extra;

  timeout = (p_cb->update_tout_a + num_blocks * p_cb->update_tout_b) *
            QUICK_TIMER_TICKS_PER_SEC / 1000000;
  /* allow some extra time for driver */
  extra = (timeout / 10) + RW_T3T_MIN_TIMEOUT_TICKS;
  timeout += extra;

  return timeout;
}
/*******************************************************************************
**
** Function         rw_t3t_process_error
**
** Description      Process error (timeout or CRC error)
**
** Returns          none
**
*******************************************************************************/
void rw_t3t_process_error(tNFC_STATUS status) {
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;
  uint8_t evt;
  tRW_DATA evt_data;
  NFC_HDR* p_cmd_buf;

  if (p_cb->rw_state == RW_T3T_STATE_COMMAND_PENDING) {
    if (p_cb->cur_cmd == RW_T3T_CMD_GET_SYSTEM_CODES) {
      /* For GetSystemCode: tag did not respond to requested POLL */
      rw_t3t_handle_get_system_codes_cplt();
      return;
    }
    /* Retry sending command if retry-count < max */
    else if (rw_cb.cur_retry < RW_MAX_RETRIES) {
      /* retry sending the command */
      rw_cb.cur_retry++;

      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("T3T retransmission attempt %i of %i",
                          rw_cb.cur_retry, RW_MAX_RETRIES);

      /* allocate a new buffer for message */
      p_cmd_buf = rw_t3t_get_cmd_buf();
      if (p_cmd_buf != nullptr) {
        memcpy(p_cmd_buf, p_cb->p_cur_cmd_buf, sizeof(NFC_HDR) +
                                                   p_cb->p_cur_cmd_buf->offset +
                                                   p_cb->p_cur_cmd_buf->len);

        if (rw_t3t_send_to_lower(p_cmd_buf) == NFC_STATUS_OK) {
          /* Start timer for waiting for response */
          nfc_start_quick_timer(&p_cb->timer, NFC_TTYPE_RW_T3T_RESPONSE,
                                p_cb->cur_tout);
          return;
        } else {
          /* failure - could not send buffer */
          GKI_freebuf(p_cmd_buf);
        }
      }
    } else {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "T3T maximum retransmission attempts reached (%i)", RW_MAX_RETRIES);
    }

#if (RW_STATS_INCLUDED == TRUE)
    /* update failure count */
    rw_main_update_fail_stats();
#endif /* RW_STATS_INCLUDED */

    p_cb->rw_state = RW_T3T_STATE_IDLE;

    /* Notify app of result (if there was a pending command) */
    if (p_cb->cur_cmd < RW_T3T_CMD_MAX) {
      /* If doing presence check, use status=NFC_STATUS_FAILED, otherwise
       * NFC_STATUS_TIMEOUT */
      evt_data.status = status;
      if (rw_cb.cur_retry < RW_MAX_RETRIES)
        evt = rw_t3t_api_res_evt[p_cb->cur_cmd];
      else
        evt = RW_T3T_INTF_ERROR_EVT;

      /* Set additional flags for RW_T3T_NDEF_DETECT_EVT */
      if (evt == RW_T3T_NDEF_DETECT_EVT) {
        evt_data.ndef.flags = RW_NDEF_FL_UNKNOWN;
        rw_t3t_update_ndef_flag(&evt_data.ndef.flags);
      }

      (*(rw_cb.p_cback))(evt, &evt_data);
    }
  } else {
    evt_data.status = status;
    (*(rw_cb.p_cback))(RW_T3T_INTF_ERROR_EVT, &evt_data);
  }
}

/*******************************************************************************
**
** Function         rw_t3t_start_poll_timer
**
** Description      Start the timer for T3T POLL Command
**
** Returns          none
**
*******************************************************************************/
void rw_t3t_start_poll_timer(tRW_T3T_CB* p_cb) {
  nfc_start_quick_timer(&p_cb->poll_timer, NFC_TTYPE_RW_T3T_RESPONSE,
                        RW_T3T_POLL_CMD_TIMEOUT_TICKS);
}

/*******************************************************************************
**
** Function         rw_t3t_handle_nci_poll_ntf
**
** Description      Handle NCI_T3T_POLLING_NTF
**
** Returns          none
**
*******************************************************************************/
void rw_t3t_handle_nci_poll_ntf(uint8_t nci_status, uint8_t num_responses,
                                uint8_t sensf_res_buf_size,
                                uint8_t* p_sensf_res_buf) {
  tRW_DATA evt_data;
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;

  /* stop timer for poll response */
  nfc_stop_quick_timer(&p_cb->poll_timer);

  /* Stop t3t timer (if started) */
  if (p_cb->flags & RW_T3T_FL_W4_PRESENCE_CHECK_POLL_RSP) {
    p_cb->flags &= ~RW_T3T_FL_W4_PRESENCE_CHECK_POLL_RSP;
    evt_data.status = nci_status;
    p_cb->rw_state = RW_T3T_STATE_IDLE;
    (*(rw_cb.p_cback))(RW_T3T_PRESENCE_CHECK_EVT, &evt_data);
  } else if (p_cb->flags & RW_T3T_FL_W4_GET_SC_POLL_RSP) {
    /* Handle POLL ntf in response to get system codes */
    p_cb->flags &= ~RW_T3T_FL_W4_GET_SC_POLL_RSP;
    rw_t3t_handle_get_sc_poll_rsp(p_cb, nci_status, num_responses,
                                  sensf_res_buf_size, p_sensf_res_buf);
  } else if (p_cb->flags & RW_T3T_FL_W4_FMT_FELICA_LITE_POLL_RSP) {
    /* Handle POLL ntf in response to get system codes */
    p_cb->flags &= ~RW_T3T_FL_W4_FMT_FELICA_LITE_POLL_RSP;
    rw_t3t_handle_fmt_poll_rsp(p_cb, nci_status, num_responses);
  } else if (p_cb->flags & RW_T3T_FL_W4_SRO_FELICA_LITE_POLL_RSP) {
    /* Handle POLL ntf in response to get system codes */
    p_cb->flags &= ~RW_T3T_FL_W4_SRO_FELICA_LITE_POLL_RSP;
    rw_t3t_handle_sro_poll_rsp(p_cb, nci_status, num_responses);
  } else if (p_cb->flags & RW_T3T_FL_W4_NDEF_DETECT_POLL_RSP) {
    /* Handle POLL ntf in response to ndef detection */
    p_cb->flags &= ~RW_T3T_FL_W4_NDEF_DETECT_POLL_RSP;
    rw_t3t_handle_ndef_detect_poll_rsp(p_cb, nci_status, num_responses);
  } else {
    /* Handle POLL ntf in response to RW_T3tPoll */
    evt_data.t3t_poll.status = nci_status;
    if (evt_data.t3t_poll.status == NCI_STATUS_OK) {
      evt_data.t3t_poll.rc = p_cb->cur_poll_rc;
      evt_data.t3t_poll.response_num = num_responses;
      evt_data.t3t_poll.response_bufsize = sensf_res_buf_size;
      evt_data.t3t_poll.response_buf = p_sensf_res_buf;
    }

    p_cb->rw_state = RW_T3T_STATE_IDLE;
    (*(rw_cb.p_cback))(RW_T3T_POLL_EVT, &evt_data);
  }
}

/*******************************************************************************
**
** Function         rw_t3t_handle_get_system_codes_cplt
**
** Description      Notify upper layer of system codes
**
** Returns          none
**
*******************************************************************************/
void rw_t3t_handle_get_system_codes_cplt(void) {
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;
  tRW_DATA evt_data;
  uint8_t i;

  evt_data.t3t_sc.status = NFC_STATUS_OK;
  evt_data.t3t_sc.num_system_codes = p_cb->num_system_codes;
  evt_data.t3t_sc.p_system_codes = p_cb->system_codes;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "number of systems: %i", evt_data.t3t_sc.num_system_codes);
  for (i = 0; i < evt_data.t3t_sc.num_system_codes; i++) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "system %i: %04X", i, evt_data.t3t_sc.p_system_codes[i]);
  }

  p_cb->rw_state = RW_T3T_STATE_IDLE;
  (*(rw_cb.p_cback))(RW_T3T_GET_SYSTEM_CODES_EVT, &evt_data);
}

/*******************************************************************************
**
** Function         rw_t3t_format_cplt
**
** Description      Notify upper layer of format complete
**
** Returns          none
**
*******************************************************************************/
void rw_t3t_format_cplt(tNFC_STATUS status) {
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;
  tRW_DATA evt_data;

  p_cb->rw_state = RW_T3T_STATE_IDLE;

  /* Update ndef info */
  p_cb->ndef_attrib.status = status;
  if (status == NFC_STATUS_OK) {
    p_cb->ndef_attrib.version = T3T_MSG_NDEF_VERSION;
    p_cb->ndef_attrib.nbr = RW_T3T_DEFAULT_FELICALITE_NBR;
    p_cb->ndef_attrib.nbw = RW_T3T_DEFAULT_FELICALITE_NBW;
    p_cb->ndef_attrib.nmaxb = RW_T3T_DEFAULT_FELICALITE_NMAXB;
    p_cb->ndef_attrib.writef = T3T_MSG_NDEF_WRITEF_OFF;
    p_cb->ndef_attrib.rwflag = T3T_MSG_NDEF_RWFLAG_RW;
    p_cb->ndef_attrib.ln = 0;
  }

  /* Notify upper layer of format complete */
  evt_data.status = status;
  (*(rw_cb.p_cback))(RW_T3T_FORMAT_CPLT_EVT, &evt_data);
}

/*******************************************************************************
**
** Function         rw_t3t_set_readonly_cplt
**
** Description      Notify upper layer of set read only complete
**
** Returns          none
**
*******************************************************************************/
void rw_t3t_set_readonly_cplt(tNFC_STATUS status) {
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;
  tRW_DATA evt_data;

  p_cb->rw_state = RW_T3T_STATE_IDLE;

  /* Notify upper layer of format complete */
  evt_data.status = status;
  (*(rw_cb.p_cback))(RW_T3T_SET_READ_ONLY_CPLT_EVT, &evt_data);
}

/*******************************************************************************
**
** Function         rw_t3t_process_timeout
**
** Description      Process timeout
**
** Returns          none
**
*******************************************************************************/
void rw_t3t_process_timeout(TIMER_LIST_ENT* p_tle) {
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;
  tRW_DATA evt_data;

  /* Check which timer timed out */
  if (p_tle == &p_cb->timer) {
/* UPDATE/CHECK response timeout */
LOG(ERROR) << StringPrintf("T3T timeout. state=%s cur_cmd=0x%02X (%s)",
                           rw_t3t_state_str(rw_cb.tcb.t3t.rw_state).c_str(),
                           rw_cb.tcb.t3t.cur_cmd,
                           rw_t3t_cmd_str(rw_cb.tcb.t3t.cur_cmd).c_str());

rw_t3t_process_error(NFC_STATUS_TIMEOUT);
  } else {
    LOG(ERROR) << StringPrintf("T3T POLL timeout.");

    /* POLL response timeout */
    if (p_cb->flags & RW_T3T_FL_W4_PRESENCE_CHECK_POLL_RSP) {
      /* POLL timeout for presence check */
      p_cb->flags &= ~RW_T3T_FL_W4_PRESENCE_CHECK_POLL_RSP;
      evt_data.status = NFC_STATUS_FAILED;
      p_cb->rw_state = RW_T3T_STATE_IDLE;
      (*(rw_cb.p_cback))(RW_T3T_PRESENCE_CHECK_EVT, &evt_data);
    } else if (p_cb->flags & RW_T3T_FL_W4_GET_SC_POLL_RSP) {
      /* POLL timeout for getting system codes */
      p_cb->flags &= ~RW_T3T_FL_W4_GET_SC_POLL_RSP;
      rw_t3t_handle_get_system_codes_cplt();
    } else if (p_cb->flags & RW_T3T_FL_W4_FMT_FELICA_LITE_POLL_RSP) {
      /* POLL timeout for formatting Felica Lite */
      p_cb->flags &= ~RW_T3T_FL_W4_FMT_FELICA_LITE_POLL_RSP;
      LOG(ERROR) << StringPrintf("Felica-Lite tag not detected");
      rw_t3t_format_cplt(NFC_STATUS_FAILED);
    } else if (p_cb->flags & RW_T3T_FL_W4_SRO_FELICA_LITE_POLL_RSP) {
      /* POLL timeout for configuring Felica Lite read only */
      p_cb->flags &= ~RW_T3T_FL_W4_SRO_FELICA_LITE_POLL_RSP;
      LOG(ERROR) << StringPrintf("Felica-Lite tag not detected");
      rw_t3t_set_readonly_cplt(NFC_STATUS_FAILED);
    } else if (p_cb->flags & RW_T3T_FL_W4_NDEF_DETECT_POLL_RSP) {
      /* POLL timeout for ndef detection */
      p_cb->flags &= ~RW_T3T_FL_W4_NDEF_DETECT_POLL_RSP;
      rw_t3t_handle_ndef_detect_poll_rsp(p_cb, NFC_STATUS_TIMEOUT, 0);
    } else {
      /* Timeout waiting for response for RW_T3tPoll */
      evt_data.t3t_poll.status = NFC_STATUS_FAILED;
      p_cb->rw_state = RW_T3T_STATE_IDLE;
      (*(rw_cb.p_cback))(RW_T3T_POLL_EVT, &evt_data);
    }
  }
}

/*******************************************************************************
**
** Function         rw_t3t_process_frame_error
**
** Description      Process frame crc error
**
** Returns          none
**
*******************************************************************************/
void rw_t3t_process_frame_error(void) {
  LOG(ERROR) << StringPrintf("T3T frame error. state=%s cur_cmd=0x%02X (%s)",
                             rw_t3t_state_str(rw_cb.tcb.t3t.rw_state).c_str(),
                             rw_cb.tcb.t3t.cur_cmd,
                             rw_t3t_cmd_str(rw_cb.tcb.t3t.cur_cmd).c_str());

#if (RW_STATS_INCLUDED == TRUE)
  /* Update stats */
  rw_main_update_crc_error_stats();
#endif /* RW_STATS_INCLUDED */

  /* Process the error */
  rw_t3t_process_error(NFC_STATUS_MSG_CORRUPTED);
}

/*******************************************************************************
**
** Function         rw_t3t_send_to_lower
**
** Description      Send command to lower layer
**
** Returns          status of the send
**
*******************************************************************************/
tNFC_STATUS rw_t3t_send_to_lower(NFC_HDR* p_msg) {
  uint8_t* p;

#if (RW_STATS_INCLUDED == TRUE)
  bool is_retry;
  /* Update stats */
  rw_main_update_tx_stats(p_msg->len, ((rw_cb.cur_retry == 0) ? false : true));
#endif /* RW_STATS_INCLUDED */

  /* Set NFC-F SoD field (payload len + 1) */
  if (p_msg->offset) p_msg->offset -= 1; /* Point to SoD field */
  p = (uint8_t*)(p_msg + 1) + p_msg->offset;
  UINT8_TO_STREAM(p, (p_msg->len + 1));
  p_msg->len += 1; /* Increment len to include SoD */

  return (NFC_SendData(NFC_RF_CONN_ID, p_msg));
}

/*****************************************************************************
**
** Function         rw_t3t_get_cmd_buf
**
** Description      Get a buffer for sending T3T messages
**
** Returns          NFC_HDR *
**
*****************************************************************************/
NFC_HDR* rw_t3t_get_cmd_buf(void) {
  NFC_HDR* p_cmd_buf;

  p_cmd_buf = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
  if (p_cmd_buf != nullptr) {
    /* Reserve offset for NCI_DATA_HDR and NFC-F Sod (LEN) field */
    p_cmd_buf->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE + 1;
    p_cmd_buf->len = 0;
  }

  return (p_cmd_buf);
}

/*****************************************************************************
**
** Function         rw_t3t_send_cmd
**
** Description      Send command to tag, and start timer for response
**
** Returns          tNFC_STATUS
**
*****************************************************************************/
tNFC_STATUS rw_t3t_send_cmd(tRW_T3T_CB* p_cb, uint8_t rw_t3t_cmd,
                            NFC_HDR* p_cmd_buf, uint32_t timeout_ticks) {
  tNFC_STATUS retval;

  /* Indicate first attempt to send command, back up cmd buffer in case needed
   * for retransmission */
  rw_cb.cur_retry = 0;
  memcpy(p_cb->p_cur_cmd_buf, p_cmd_buf,
         sizeof(NFC_HDR) + p_cmd_buf->offset + p_cmd_buf->len);

  p_cb->cur_cmd = rw_t3t_cmd;
  p_cb->cur_tout = timeout_ticks;
  p_cb->rw_state = RW_T3T_STATE_COMMAND_PENDING;

  retval = rw_t3t_send_to_lower(p_cmd_buf);
  if (retval == NFC_STATUS_OK) {
    /* Start timer for waiting for response */
    nfc_start_quick_timer(&p_cb->timer, NFC_TTYPE_RW_T3T_RESPONSE,
                          timeout_ticks);
  } else {
    /* Error sending */
    p_cb->rw_state = RW_T3T_STATE_IDLE;
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("cur_tout: %d, timeout_ticks: %d ret:%d", p_cb->cur_tout,
                      timeout_ticks, retval);
  return (retval);
}

/*****************************************************************************
**
** Function         rw_t3t_send_update_ndef_attribute_cmd
**
** Description      Send UPDATE command for Attribute Information
**
** Returns          tNFC_STATUS
**
*****************************************************************************/
tNFC_STATUS rw_t3t_send_update_ndef_attribute_cmd(tRW_T3T_CB* p_cb,
                                                  bool write_in_progress) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  NFC_HDR* p_cmd_buf;
  uint8_t *p_cmd_start, *p;
  uint16_t checksum, i;
  uint8_t write_f;
  uint32_t ln;
  uint8_t* p_ndef_attr_info_start;

  p_cmd_buf = rw_t3t_get_cmd_buf();
  if (p_cmd_buf != nullptr) {
    /* Construct T3T message */
    p = p_cmd_start = (uint8_t*)(p_cmd_buf + 1) + p_cmd_buf->offset;

    /* Add UPDATE opcode to message  */
    UINT8_TO_STREAM(p, T3T_MSG_OPC_UPDATE_CMD);

    /* Add IDm to message */
    ARRAY_TO_STREAM(p, p_cb->peer_nfcid2, NCI_NFCID2_LEN);

    /* Add Service code list */
    UINT8_TO_STREAM(p, 1); /* Number of services (only 1 service: NDEF) */
    UINT16_TO_STREAM(
        p, T3T_MSG_NDEF_SC_RW); /* Service code (little-endian format) */

    /* Add number of blocks in this UPDATE command */
    UINT8_TO_STREAM(p, 1); /* Number of blocks to write in this command */

    /* Block List element: the NDEF attribute information block (block 0) */
    UINT8_TO_STREAM(p, T3T_MSG_MASK_TWO_BYTE_BLOCK_DESC_FORMAT);
    UINT8_TO_STREAM(p, 0);

    /* Add payload (Attribute information block) */
    p_ndef_attr_info_start =
        p; /* Save start of a NDEF attribute info block for checksum */
    UINT8_TO_STREAM(p, T3T_MSG_NDEF_VERSION);
    UINT8_TO_STREAM(p, p_cb->ndef_attrib.nbr);
    UINT8_TO_STREAM(p, p_cb->ndef_attrib.nbw);
    UINT16_TO_BE_STREAM(p, p_cb->ndef_attrib.nmaxb);
    UINT32_TO_STREAM(p, 0);

    /* If starting NDEF write: set WriteF=ON, and ln=current ndef length */
    if (write_in_progress) {
      write_f = T3T_MSG_NDEF_WRITEF_ON;
      ln = p_cb->ndef_attrib.ln;
    }
    /* If finishing NDEF write: set WriteF=OFF, and ln=new ndef len */
    else {
      write_f = T3T_MSG_NDEF_WRITEF_OFF;
      ln = p_cb->ndef_msg_len;
    }
    UINT8_TO_STREAM(p, write_f);
    UINT8_TO_STREAM(p, p_cb->ndef_attrib.rwflag);
    UINT8_TO_STREAM(p, (ln >> 16) & 0xFF); /* High byte (of 3) of Ln */
    UINT8_TO_STREAM(p, (ln >> 8) & 0xFF);  /* Middle byte (of 3) of Ln */
    UINT8_TO_STREAM(p, (ln)&0xFF);         /* Low byte (of 3) of Ln */

    /* Calculate and append Checksum */
    checksum = 0;
    for (i = 0; i < T3T_MSG_NDEF_ATTR_INFO_SIZE; i++) {
      checksum += p_ndef_attr_info_start[i];
    }
    UINT16_TO_BE_STREAM(p, checksum);

    /* Calculate length of message */
    p_cmd_buf->len = (uint16_t)(p - p_cmd_start);

    /* Send the T3T message */
    retval = rw_t3t_send_cmd(p_cb, RW_T3T_CMD_UPDATE_NDEF, p_cmd_buf,
                             rw_t3t_update_timeout(1));
  } else {
    retval = NFC_STATUS_NO_BUFFERS;
  }

  return (retval);
}

/*****************************************************************************
**
** Function         rw_t3t_send_next_ndef_update_cmd
**
** Description      Send next segment of NDEF message to update
**
** Returns          tNFC_STATUS
**
*****************************************************************************/
tNFC_STATUS rw_t3t_send_next_ndef_update_cmd(tRW_T3T_CB* p_cb) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  uint16_t block_id;
  uint16_t first_block_to_write;
  uint16_t ndef_blocks_to_write, ndef_blocks_remaining;
  uint32_t ndef_bytes_remaining, ndef_padding = 0;
  uint8_t flags = 0;
  uint8_t* p_cur_ndef_src_offset;
  NFC_HDR* p_cmd_buf;
  uint8_t *p_cmd_start, *p;
  uint8_t blocks_per_update;
  uint32_t timeout;

  p_cmd_buf = rw_t3t_get_cmd_buf();
  if (p_cmd_buf != nullptr) {
    /* Construct T3T message */
    p = p_cmd_start = (uint8_t*)(p_cmd_buf + 1) + p_cmd_buf->offset;

    /* Calculate number of ndef bytes remaining to write */
    ndef_bytes_remaining = p_cb->ndef_msg_len - p_cb->ndef_msg_bytes_sent;

    /* Calculate number of blocks remaining to write */
    ndef_blocks_remaining =
        (uint16_t)((ndef_bytes_remaining + 15) >>
                   4); /* ndef blocks remaining (rounded upward) */

    /* Calculate first NDEF block ID for this UPDATE command */
    first_block_to_write = (uint16_t)((p_cb->ndef_msg_bytes_sent >> 4) + 1);

    /* Calculate max number of blocks per write. */
    if ((first_block_to_write +
         RW_T3T_MAX_NDEF_BLOCKS_PER_UPDATE_1_BYTE_FORMAT) < 0x100) {
      /* All block-numbers are < 0x100 (i.e. can be specified using one-byte
       * format) */
      blocks_per_update = RW_T3T_MAX_NDEF_BLOCKS_PER_UPDATE_1_BYTE_FORMAT;
    } else {
      /* Block-numbers are >= 0x100 (i.e. need to be specified using two-byte
       * format) */
      blocks_per_update = RW_T3T_MAX_NDEF_BLOCKS_PER_UPDATE_2_BYTE_FORMAT;
    }

    /* Check if blocks_per_update is bigger than what peer allows */
    if (blocks_per_update > p_cb->ndef_attrib.nbw)
      blocks_per_update = p_cb->ndef_attrib.nbw;

    /* Check if remaining blocks can fit into one UPDATE command */
    if (ndef_blocks_remaining <= blocks_per_update) {
      /* remaining blocks can fit into one UPDATE command */
      ndef_blocks_to_write = ndef_blocks_remaining;
    } else {
      /* Remaining blocks cannot fit into one UPDATE command */
      ndef_blocks_to_write = blocks_per_update;
    }

    /* Write to command header for UPDATE */

    /* Add UPDATE opcode to message  */
    UINT8_TO_STREAM(p, T3T_MSG_OPC_UPDATE_CMD);

    /* Add IDm to message */
    ARRAY_TO_STREAM(p, p_cb->peer_nfcid2, NCI_NFCID2_LEN);

    /* Add Service code list */
    UINT8_TO_STREAM(p, 1); /* Number of services (only 1 service: NDEF) */
    UINT16_TO_STREAM(
        p, T3T_MSG_NDEF_SC_RW); /* Service code (little-endian format) */

    /* Add number of blocks in this UPDATE command */
    UINT8_TO_STREAM(
        p,
        ndef_blocks_to_write); /* Number of blocks to write in this command */
    timeout = rw_t3t_update_timeout(ndef_blocks_to_write);

    for (block_id = first_block_to_write;
         block_id < (first_block_to_write + ndef_blocks_to_write); block_id++) {
      if (block_id < 256) {
        /* Block IDs 0-255 can be specified in '2-byte' format: byte0=0,
         * byte1=blocknumber */
        UINT8_TO_STREAM(
            p, T3T_MSG_MASK_TWO_BYTE_BLOCK_DESC_FORMAT); /* byte0: len=1;
                                                            access-mode=0;
                                                            service code list
                                                            order=0 */
        UINT8_TO_STREAM(p, block_id); /* byte1: block number */
      } else {
        /* Block IDs 256+ must be specified in '3-byte' format: byte0=80h,
         * followed by blocknumber */
        UINT8_TO_STREAM(
            p,
            0x00); /* byte0: len=0; access-mode=0; service code list order=0 */
        UINT16_TO_STREAM(
            p, block_id); /* byte1-2: block number in little-endian format */
      }
    }

    /* Add NDEF payload */

    /* If this sending last block of NDEF,  check if padding is needed to make
     * payload a multiple of 16 bytes */
    if (ndef_blocks_to_write == ndef_blocks_remaining) {
      ndef_padding = (16 - (ndef_bytes_remaining & 0x0F)) & 0x0F;
      if (ndef_padding) {
        flags |= RW_T3T_FL_PADDING;
        ndef_blocks_to_write--; /* handle the last block separately if it needs
                                   padding */
      }
    }

    /* Add NDEF payload to the message */
    p_cur_ndef_src_offset = &p_cb->ndef_msg[p_cb->ndef_msg_bytes_sent];

    ARRAY_TO_STREAM(p, p_cur_ndef_src_offset, (ndef_blocks_to_write * 16));
    p_cb->ndef_msg_bytes_sent += ((uint32_t)ndef_blocks_to_write * 16);

    if (flags & RW_T3T_FL_PADDING) {
      /* Add last of the NDEF message */
      p_cur_ndef_src_offset = &p_cb->ndef_msg[p_cb->ndef_msg_bytes_sent];
      ARRAY_TO_STREAM(p, p_cur_ndef_src_offset, (int)(16 - ndef_padding));
      p_cb->ndef_msg_bytes_sent += (16 - ndef_padding);

      /* Add padding */
      memset(p, 0, ndef_padding);
      p += ndef_padding;
    }

    /* Calculate length of message */
    p_cmd_buf->len = (uint16_t)(p - p_cmd_start);

    /* Send the T3T message */
    retval = rw_t3t_send_cmd(p_cb, RW_T3T_CMD_UPDATE_NDEF, p_cmd_buf, timeout);
  } else {
    retval = NFC_STATUS_NO_BUFFERS;
  }

  return (retval);
}

/*****************************************************************************
**
** Function         rw_t3t_send_next_ndef_check_cmd
**
** Description      Send command for reading next segment of NDEF message
**
** Returns          tNFC_STATUS
**
*****************************************************************************/
tNFC_STATUS rw_t3t_send_next_ndef_check_cmd(tRW_T3T_CB* p_cb) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  uint16_t block_id;
  uint16_t ndef_blocks_remaining, first_block_to_read, cur_blocks_to_read;
  uint32_t ndef_bytes_remaining;
  NFC_HDR* p_cmd_buf;
  uint8_t *p_cmd_start, *p;

  p_cmd_buf = rw_t3t_get_cmd_buf();
  if (p_cmd_buf != nullptr) {
    /* Construct T3T message */
    p = p_cmd_start = (uint8_t*)(p_cmd_buf + 1) + p_cmd_buf->offset;

    /* Calculate number of ndef bytes remaining to read */
    ndef_bytes_remaining = p_cb->ndef_attrib.ln - p_cb->ndef_rx_offset;

    /* Calculate number of blocks remaining to read */
    ndef_blocks_remaining =
        (uint16_t)((ndef_bytes_remaining + 15) >>
                   4); /* ndef blocks remaining (rounded upward) */

    /* Calculate first NDEF block ID */
    first_block_to_read = (uint16_t)((p_cb->ndef_rx_offset >> 4) + 1);

    /* Check if remaining blocks can fit into one CHECK command */
    if (ndef_blocks_remaining <= p_cb->ndef_attrib.nbr) {
      /* remaining blocks can fit into one CHECK command */
      cur_blocks_to_read = ndef_blocks_remaining;
      p_cb->ndef_rx_readlen = ndef_bytes_remaining;
      p_cb->flags |= RW_T3T_FL_IS_FINAL_NDEF_SEGMENT;
    } else {
      /* Remaining blocks cannot fit into one CHECK command */
      cur_blocks_to_read =
          p_cb->ndef_attrib
              .nbr; /* Read maximum number of blocks allowed by the peer */
      p_cb->ndef_rx_readlen = ((uint32_t)p_cb->ndef_attrib.nbr * 16);
    }

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "bytes_remaining: %i, cur_blocks_to_read: %i, is_final: %i",
        ndef_bytes_remaining, cur_blocks_to_read,
        (p_cb->flags & RW_T3T_FL_IS_FINAL_NDEF_SEGMENT));

    /* Add CHECK opcode to message  */
    UINT8_TO_STREAM(p, T3T_MSG_OPC_CHECK_CMD);

    /* Add IDm to message */
    ARRAY_TO_STREAM(p, p_cb->peer_nfcid2, NCI_NFCID2_LEN);

    /* Add Service code list */
    UINT8_TO_STREAM(p, 1); /* Number of services (only 1 service: NDEF) */

    /* Service code (little-endian format) . If NDEF is read-only, then use
     * T3T_MSG_NDEF_SC_RO, otherwise use T3T_MSG_NDEF_SC_RW */
    if (p_cb->ndef_attrib.rwflag == T3T_MSG_NDEF_RWFLAG_RO) {
      UINT16_TO_STREAM(p, T3T_MSG_NDEF_SC_RO);
    } else {
      UINT16_TO_STREAM(p, T3T_MSG_NDEF_SC_RW);
    }

    /* Add number of blocks in this CHECK command */
    UINT8_TO_STREAM(
        p, cur_blocks_to_read); /* Number of blocks to check in this command */

    for (block_id = first_block_to_read;
         block_id < (first_block_to_read + cur_blocks_to_read); block_id++) {
      if (block_id < 256) {
        /* Block IDs 0-255 can be specified in '2-byte' format: byte0=0,
         * byte1=blocknumber */
        UINT8_TO_STREAM(
            p, T3T_MSG_MASK_TWO_BYTE_BLOCK_DESC_FORMAT); /* byte1: len=0;
                                                            access-mode=0;
                                                            service code list
                                                            order=0 */
        UINT8_TO_STREAM(p, block_id); /* byte1: block number */
      } else {
        /* Block IDs 256+ must be specified in '3-byte' format: byte0=80h,
         * followed by blocknumber */
        UINT8_TO_STREAM(
            p,
            0x00); /* byte0: len=1; access-mode=0; service code list order=0 */
        UINT16_TO_STREAM(
            p, block_id); /* byte1-2: block number in little-endian format */
      }
    }

    /* Calculate length of message */
    p_cmd_buf->len = (uint16_t)(p - p_cmd_start);

    /* Send the T3T message */
    retval = rw_t3t_send_cmd(p_cb, RW_T3T_CMD_CHECK_NDEF, p_cmd_buf,
                             rw_t3t_check_timeout(cur_blocks_to_read));
  } else {
    retval = NFC_STATUS_NO_BUFFERS;
  }

  return (retval);
}

/*****************************************************************************
**
** Function         rw_t3t_message_set_block_list
**
** Description      Add block list to T3T message
**
** Returns          Number of bytes added to message
**
*****************************************************************************/
void rw_t3t_message_set_block_list(tRW_T3T_CB* p_cb, uint8_t** p,
                                   uint8_t num_blocks,
                                   tT3T_BLOCK_DESC* p_t3t_blocks) {
  uint16_t i, cur_service_code;
  uint8_t service_code_idx, num_services = 0;
  uint8_t* p_msg_num_services;
  uint16_t service_list[T3T_MSG_SERVICE_LIST_MAX];

  /* Add CHECK or UPDATE opcode to message  */
  UINT8_TO_STREAM(
      (*p), ((p_cb->cur_cmd == RW_T3T_CMD_CHECK) ? T3T_MSG_OPC_CHECK_CMD
                                                 : T3T_MSG_OPC_UPDATE_CMD));

  /* Add IDm to message */
  ARRAY_TO_STREAM((*p), p_cb->peer_nfcid2, NCI_NFCID2_LEN);

  /* Skip over Number of Services field */
  p_msg_num_services = (*p); /* pointer to Number of Services offset */
  (*p)++;

  /* Count number of different services are specified in the list, and add
   * services to Service Code list */
  for (i = 0; i < num_blocks; i++) {
    cur_service_code = p_t3t_blocks[i].service_code;

    /* Check if current service_code is already in the service_list */
    for (service_code_idx = 0; service_code_idx < num_services;
         service_code_idx++) {
      if (service_list[service_code_idx] == cur_service_code) break;
    }

    if (service_code_idx == num_services) {
      /* Service not in the list yet. Add it. */
      service_list[service_code_idx] = cur_service_code;
      num_services++;

      /* Add service code to T3T message */
      UINT16_TO_STREAM((*p), cur_service_code);

      /* Validate num_services */
      if (num_services >= T3T_MSG_SERVICE_LIST_MAX) {
        LOG(ERROR) << StringPrintf(
            "RW T3T: num_services (%i) reaches maximum (%i)", num_services,
            T3T_MSG_SERVICE_LIST_MAX);
        break;
      }
    }
  }

  /* Add 'Number of Sservices' to the message */
  *p_msg_num_services = num_services;

  /* Add 'number of blocks' to the message */
  UINT8_TO_STREAM((*p), num_blocks);

  /* Add block descriptors */
  for (i = 0; i < num_blocks; i++) {
    cur_service_code = p_t3t_blocks[i].service_code;

    /* Check if current service_code is already in the service_list */
    for (service_code_idx = 0; service_code_idx < num_services;
         service_code_idx++) {
      if (service_list[service_code_idx] == cur_service_code) break;
    }

    /* Add decriptor to T3T message */
    if (p_t3t_blocks[i].block_number > 0xFF) {
      UINT8_TO_STREAM((*p), service_code_idx);
      UINT16_TO_STREAM((*p), p_t3t_blocks[i].block_number);
    } else {
      service_code_idx |= T3T_MSG_MASK_TWO_BYTE_BLOCK_DESC_FORMAT;
      UINT8_TO_STREAM((*p), service_code_idx);
      UINT8_TO_STREAM((*p), p_t3t_blocks[i].block_number);
    }
  }
}

/*****************************************************************************
**
** Function         rw_t3t_send_check_cmd
**
** Description      Send CHECK command
**
** Returns          tNFC_STATUS
**
*****************************************************************************/
tNFC_STATUS rw_t3t_send_check_cmd(tRW_T3T_CB* p_cb, uint8_t num_blocks,
                                  tT3T_BLOCK_DESC* p_t3t_blocks) {
  NFC_HDR* p_cmd_buf;
  uint8_t *p, *p_cmd_start;
  tNFC_STATUS retval = NFC_STATUS_OK;

  p_cb->cur_cmd = RW_T3T_CMD_CHECK;
  p_cmd_buf = rw_t3t_get_cmd_buf();
  if (p_cmd_buf != nullptr) {
    /* Construct T3T message */
    p = p_cmd_start = (uint8_t*)(p_cmd_buf + 1) + p_cmd_buf->offset;
    rw_t3t_message_set_block_list(p_cb, &p, num_blocks, p_t3t_blocks);

    /* Calculate length of message */
    p_cmd_buf->len = (uint16_t)(p - p_cmd_start);

    /* Send the T3T message */
    retval = rw_t3t_send_cmd(p_cb, RW_T3T_CMD_CHECK, p_cmd_buf,
                             rw_t3t_check_timeout(num_blocks));
  } else {
    retval = NFC_STATUS_NO_BUFFERS;
  }

  return (retval);
}

/*****************************************************************************
**
** Function         rw_t3t_send_update_cmd
**
** Description      Send UPDATE command
**
** Returns          tNFC_STATUS
**
*****************************************************************************/
tNFC_STATUS rw_t3t_send_update_cmd(tRW_T3T_CB* p_cb, uint8_t num_blocks,
                                   tT3T_BLOCK_DESC* p_t3t_blocks,
                                   uint8_t* p_data) {
  NFC_HDR* p_cmd_buf;
  uint8_t *p, *p_cmd_start;
  tNFC_STATUS retval = NFC_STATUS_OK;

  p_cb->cur_cmd = RW_T3T_CMD_UPDATE;
  p_cmd_buf = rw_t3t_get_cmd_buf();
  if (p_cmd_buf != nullptr) {
    /* Construct T3T message */
    p = p_cmd_start = (uint8_t*)(p_cmd_buf + 1) + p_cmd_buf->offset;
    rw_t3t_message_set_block_list(p_cb, &p, num_blocks, p_t3t_blocks);

    /* Add data blocks to the message */
    ARRAY_TO_STREAM(p, p_data, num_blocks * 16);

    /* Calculate length of message */
    p_cmd_buf->len = (uint16_t)(p - p_cmd_start);

    /* Send the T3T message */
    retval = rw_t3t_send_cmd(p_cb, RW_T3T_CMD_UPDATE, p_cmd_buf,
                             rw_t3t_update_timeout(num_blocks));
  } else {
    retval = NFC_STATUS_NO_BUFFERS;
  }

  return (retval);
}

/*****************************************************************************
**
** Function         rw_t3t_check_mc_block
**
** Description      Send command to check Memory Configuration Block
**
** Returns          tNFC_STATUS
**
*****************************************************************************/
tNFC_STATUS rw_t3t_check_mc_block(tRW_T3T_CB* p_cb) {
  NFC_HDR* p_cmd_buf;
  uint8_t *p, *p_cmd_start;

  /* Read Memory Configuration block */
  p_cmd_buf = rw_t3t_get_cmd_buf();
  if (p_cmd_buf != nullptr) {
    /* Construct T3T message */
    p = p_cmd_start = (uint8_t*)(p_cmd_buf + 1) + p_cmd_buf->offset;

    /* Add CHECK opcode to message  */
    UINT8_TO_STREAM(p, T3T_MSG_OPC_CHECK_CMD);

    /* Add IDm to message */
    ARRAY_TO_STREAM(p, p_cb->peer_nfcid2, NCI_NFCID2_LEN);

    /* Add Service code list */
    UINT8_TO_STREAM(p, 1); /* Number of services (only 1 service: NDEF) */
    UINT16_TO_STREAM(
        p, T3T_MSG_NDEF_SC_RO); /* Service code (little-endian format) */

    /* Number of blocks */
    UINT8_TO_STREAM(p, 1); /* Number of blocks (only 1 block: Memory
                              Configuration Information ) */

    /* Block List element: the Memory Configuration block (block 0x88) */
    UINT8_TO_STREAM(p, T3T_MSG_MASK_TWO_BYTE_BLOCK_DESC_FORMAT);
    UINT8_TO_STREAM(p, T3T_MSG_FELICALITE_BLOCK_ID_MC);

    /* Calculate length of message */
    p_cmd_buf->len = (uint16_t)(p - p_cmd_start);

    /* Send the T3T message */
    return rw_t3t_send_cmd(p_cb, p_cb->cur_cmd, p_cmd_buf,
                           rw_t3t_check_timeout(1));
  } else {
    LOG(ERROR) << StringPrintf("Unable to allocate buffer to read MC block");
    return (NFC_STATUS_NO_BUFFERS);
  }
}

/*****************************************************************************
**
** Function         rw_t3t_send_raw_frame
**
** Description      Send raw frame
**
** Returns          tNFC_STATUS
**
*****************************************************************************/
tNFC_STATUS rw_t3t_send_raw_frame(tRW_T3T_CB* p_cb, uint16_t len,
                                  uint8_t* p_data) {
  NFC_HDR* p_cmd_buf;
  uint8_t* p;
  tNFC_STATUS retval = NFC_STATUS_OK;

  p_cmd_buf = rw_t3t_get_cmd_buf();
  if (p_cmd_buf != nullptr) {
    /* Construct T3T message */
    p = (uint8_t*)(p_cmd_buf + 1) + p_cmd_buf->offset;

    /* Add data blocks to the message */
    ARRAY_TO_STREAM(p, p_data, len);

    /* Calculate length of message */
    p_cmd_buf->len = len;

    /* Send the T3T message */
    retval = rw_t3t_send_cmd(p_cb, RW_T3T_CMD_SEND_RAW_FRAME, p_cmd_buf,
                             RW_T3T_RAW_FRAME_CMD_TIMEOUT_TICKS);
  } else {
    retval = NFC_STATUS_NO_BUFFERS;
  }

  return (retval);
}

/*****************************************************************************
**  TAG RESPONSE HANDLERS
*****************************************************************************/

/*****************************************************************************
**
** Function         rw_t3t_act_handle_ndef_detect_rsp
**
** Description      Handle response to NDEF detection
**
** Returns          Nothing
**
*****************************************************************************/
void rw_t3t_act_handle_ndef_detect_rsp(tRW_T3T_CB* p_cb, NFC_HDR* p_msg_rsp) {
  uint8_t* p;
  uint32_t temp;
  uint8_t i;
  uint16_t checksum_calc, checksum_rx;
  tRW_DETECT_NDEF_DATA evt_data = tRW_DETECT_NDEF_DATA();
  uint8_t* p_t3t_rsp = (uint8_t*)(p_msg_rsp + 1) + p_msg_rsp->offset;

  evt_data.status = NFC_STATUS_FAILED;
  evt_data.flags = RW_NDEF_FL_UNKNOWN;

  /* Check if response code is CHECK resp (for reading NDEF attribute block) */
  if (p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE] != T3T_MSG_OPC_CHECK_RSP) {
    LOG(ERROR) << StringPrintf(
        "Response error: expecting rsp_code %02X, but got %02X",
        T3T_MSG_OPC_CHECK_RSP, p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE]);
    evt_data.status = NFC_STATUS_FAILED;
  }
  /* Validate status code and NFCID2 response from tag */
  else if ((p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1] !=
            T3T_MSG_RSP_STATUS_OK) /* verify response status code */
           || (memcmp(p_cb->peer_nfcid2, &p_t3t_rsp[T3T_MSG_RSP_OFFSET_IDM],
                      NCI_NFCID2_LEN) != 0)) /* verify response IDm */
  {
    evt_data.status = NFC_STATUS_FAILED;
  } else if (p_msg_rsp->len <
             (T3T_MSG_RSP_OFFSET_CHECK_DATA + T3T_MSG_BLOCKSIZE)) {
    evt_data.status = NFC_STATUS_FAILED;
    android_errorWriteLog(0x534e4554, "120428041");
  } else {
    /* Get checksum from received ndef attribute msg */
    p = &p_t3t_rsp[T3T_MSG_RSP_OFFSET_CHECK_DATA + T3T_MSG_NDEF_ATTR_INFO_SIZE];
    BE_STREAM_TO_UINT16(checksum_rx, p);

    /* Calculate checksum - move check for checsum to beginning */
    checksum_calc = 0;
    p = &p_t3t_rsp[T3T_MSG_RSP_OFFSET_CHECK_DATA];
    for (i = 0; i < T3T_MSG_NDEF_ATTR_INFO_SIZE; i++) {
      checksum_calc += p[i];
    }

    /* Validate checksum */
    if (checksum_calc != checksum_rx) {
      p_cb->ndef_attrib.status =
          NFC_STATUS_FAILED; /* only ok or failed passed to the app. can be
                                boolean*/

      LOG(ERROR) << StringPrintf("RW_T3tDetectNDEF checksum failed");
    } else {
      p_cb->ndef_attrib.status = NFC_STATUS_OK;

      /* Validate version number */
      STREAM_TO_UINT8(p_cb->ndef_attrib.version, p);

      if (T3T_GET_MAJOR_VERSION(T3T_MSG_NDEF_VERSION) <
          T3T_GET_MAJOR_VERSION(p_cb->ndef_attrib.version)) {
        /* Remote tag's MajorVer is newer than our's. Reject NDEF as per T3TOP
         * RQ_T3T_NDA_024 */
        LOG(ERROR) << StringPrintf(
            "RW_T3tDetectNDEF: incompatible NDEF version. Local=0x%02x, "
            "Remote=0x%02x",
            T3T_MSG_NDEF_VERSION, p_cb->ndef_attrib.version);
        p_cb->ndef_attrib.status = NFC_STATUS_FAILED;
        evt_data.status = NFC_STATUS_BAD_RESP;
      } else {
        /* Remote tag's MajorVer is equal or older than our's. NDEF is
         * compatible with our version. */

        /* Update NDEF info */
        STREAM_TO_UINT8(
            p_cb->ndef_attrib.nbr,
            p); /* NBr: number of blocks that can be read using one Check
                   command */
        STREAM_TO_UINT8(p_cb->ndef_attrib.nbw,
                        p); /* Nbw: number of blocks that can be written using
                               one Update command */
        BE_STREAM_TO_UINT16(
            p_cb->ndef_attrib.nmaxb,
            p); /* Nmaxb: maximum number of blocks available for NDEF data */
        BE_STREAM_TO_UINT32(temp, p);
        STREAM_TO_UINT8(p_cb->ndef_attrib.writef,
                        p); /* WriteFlag: 00h if writing data finished; 0Fh if
                               writing data in progress */
        STREAM_TO_UINT8(
            p_cb->ndef_attrib.rwflag,
            p); /* RWFlag: 00h NDEF is read-only; 01h if read/write available */

        /* Get length (3-byte, big-endian) */
        STREAM_TO_UINT8(temp, p);                     /* Ln: high-byte */
        BE_STREAM_TO_UINT16(p_cb->ndef_attrib.ln, p); /* Ln: lo-word */
        p_cb->ndef_attrib.ln += (temp << 16);

        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "Detected NDEF Ver: 0x%02x", p_cb->ndef_attrib.version);
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "Detected NDEF Attributes: Nbr=%i, Nbw=%i, Nmaxb=%i, WriteF=%i, "
            "RWFlag=%i, Ln=%i",
            p_cb->ndef_attrib.nbr, p_cb->ndef_attrib.nbw,
            p_cb->ndef_attrib.nmaxb, p_cb->ndef_attrib.writef,
            p_cb->ndef_attrib.rwflag, p_cb->ndef_attrib.ln);
        if (p_cb->ndef_attrib.nbr > T3T_MSG_NUM_BLOCKS_CHECK_MAX ||
            p_cb->ndef_attrib.nbw > T3T_MSG_NUM_BLOCKS_UPDATE_MAX) {
          /* It would result in CHECK Responses exceeding the maximum length
           * of an NFC-F Frame */
          LOG(ERROR) << StringPrintf(
              "Unsupported NDEF Attributes value: Nbr=%i, Nbw=%i, Nmaxb=%i,"
              "WriteF=%i, RWFlag=%i, Ln=%i",
              p_cb->ndef_attrib.nbr, p_cb->ndef_attrib.nbw,
              p_cb->ndef_attrib.nmaxb, p_cb->ndef_attrib.writef,
              p_cb->ndef_attrib.rwflag, p_cb->ndef_attrib.ln);
          p_cb->ndef_attrib.status = NFC_STATUS_FAILED;
          evt_data.status = NFC_STATUS_BAD_RESP;
        } else {
          /* Set data for RW_T3T_NDEF_DETECT_EVT */
          evt_data.status = p_cb->ndef_attrib.status;
          evt_data.cur_size = p_cb->ndef_attrib.ln;
          evt_data.max_size = (uint32_t)p_cb->ndef_attrib.nmaxb * 16;
          evt_data.protocol = NFC_PROTOCOL_T3T;
          evt_data.flags = (RW_NDEF_FL_SUPPORTED | RW_NDEF_FL_FORMATED);
          if (p_cb->ndef_attrib.rwflag == T3T_MSG_NDEF_RWFLAG_RO)
            evt_data.flags |= RW_NDEF_FL_READ_ONLY;
        }
      }
    }
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("RW_T3tDetectNDEF response: %i", evt_data.status);

  p_cb->rw_state = RW_T3T_STATE_IDLE;
  rw_t3t_update_ndef_flag(&evt_data.flags);
  /* Notify app of NDEF detection result */
  tRW_DATA rw_data;
  rw_data.ndef = evt_data;
  (*(rw_cb.p_cback))(RW_T3T_NDEF_DETECT_EVT, &rw_data);

  GKI_freebuf(p_msg_rsp);
}

/*****************************************************************************
**
** Function         rw_t3t_act_handle_check_rsp
**
** Description      Handle response to CHECK command
**
** Returns          Nothing
**
*****************************************************************************/
void rw_t3t_act_handle_check_rsp(tRW_T3T_CB* p_cb, NFC_HDR* p_msg_rsp) {
  uint8_t* p_t3t_rsp = (uint8_t*)(p_msg_rsp + 1) + p_msg_rsp->offset;
  tRW_READ_DATA evt_data;
  tNFC_STATUS nfc_status = NFC_STATUS_OK;

  /* Validate response from tag */
  if ((p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1] !=
       T3T_MSG_RSP_STATUS_OK) /* verify response status code */
      || (memcmp(p_cb->peer_nfcid2, &p_t3t_rsp[T3T_MSG_RSP_OFFSET_IDM],
                 NCI_NFCID2_LEN) != 0)) /* verify response IDm */
  {
    nfc_status = NFC_STATUS_FAILED;
    GKI_freebuf(p_msg_rsp);
  } else if (p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE] != T3T_MSG_OPC_CHECK_RSP) {
    LOG(ERROR) << StringPrintf(
        "Response error: expecting rsp_code %02X, but got %02X",
        T3T_MSG_OPC_CHECK_RSP, p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE]);
    nfc_status = NFC_STATUS_FAILED;
    GKI_freebuf(p_msg_rsp);
  } else if (p_msg_rsp->len >= T3T_MSG_RSP_OFFSET_CHECK_DATA) {
    /* Copy incoming data into buffer */
    p_msg_rsp->offset +=
        T3T_MSG_RSP_OFFSET_CHECK_DATA; /* Skip over t3t header */
    p_msg_rsp->len -= T3T_MSG_RSP_OFFSET_CHECK_DATA;
    evt_data.status = NFC_STATUS_OK;
    evt_data.p_data = p_msg_rsp;
    tRW_DATA rw_data;
    rw_data.data = evt_data;
    (*(rw_cb.p_cback))(RW_T3T_CHECK_EVT, &rw_data);
  } else {
    android_errorWriteLog(0x534e4554, "120503926");
    nfc_status = NFC_STATUS_FAILED;
    GKI_freebuf(p_msg_rsp);
  }

  p_cb->rw_state = RW_T3T_STATE_IDLE;

  tRW_DATA rw_data;
  rw_data.status = nfc_status;
  (*(rw_cb.p_cback))(RW_T3T_CHECK_CPLT_EVT, &rw_data);
}

/*****************************************************************************
**
** Function         rw_t3t_act_handle_update_rsp
**
** Description      Handle response to UPDATE command
**
** Returns          Nothing
**
*****************************************************************************/
void rw_t3t_act_handle_update_rsp(tRW_T3T_CB* p_cb, NFC_HDR* p_msg_rsp) {
  uint8_t* p_t3t_rsp = (uint8_t*)(p_msg_rsp + 1) + p_msg_rsp->offset;
  tRW_READ_DATA evt_data = tRW_READ_DATA();

  /* Validate response from tag */
  if ((p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1] !=
       T3T_MSG_RSP_STATUS_OK) /* verify response status code */
      || (memcmp(p_cb->peer_nfcid2, &p_t3t_rsp[T3T_MSG_RSP_OFFSET_IDM],
                 NCI_NFCID2_LEN) != 0)) /* verify response IDm */
  {
    evt_data.status = NFC_STATUS_FAILED;
  } else if (p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE] != T3T_MSG_OPC_UPDATE_RSP) {
    LOG(ERROR) << StringPrintf(
        "Response error: expecting rsp_code %02X, but got %02X",
        T3T_MSG_OPC_UPDATE_RSP, p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE]);
    evt_data.status = NFC_STATUS_FAILED;
  } else {
    /* Copy incoming data into buffer */
    evt_data.status = NFC_STATUS_OK;
  }

  p_cb->rw_state = RW_T3T_STATE_IDLE;

  tRW_DATA rw_data;
  rw_data.data = evt_data;
  (*(rw_cb.p_cback))(RW_T3T_UPDATE_CPLT_EVT, &rw_data);

  GKI_freebuf(p_msg_rsp);
}

/*****************************************************************************
**
** Function         rw_t3t_act_handle_raw_senddata_rsp
**
** Description      Handle response to NDEF detection
**
** Returns          Nothing
**
*****************************************************************************/
void rw_t3t_act_handle_raw_senddata_rsp(tRW_T3T_CB* p_cb,
                                        tNFC_DATA_CEVT* p_data) {
  tRW_READ_DATA evt_data;
  NFC_HDR* p_pkt = p_data->p_data;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("RW T3T Raw Frame: Len [0x%X] Status [%s]", p_pkt->len,
                      NFC_GetStatusName(p_data->status).c_str());

  /* Copy incoming data into buffer */
  evt_data.status = p_data->status;
  evt_data.p_data = p_pkt;

  p_cb->rw_state = RW_T3T_STATE_IDLE;

  tRW_DATA rw_data;
  rw_data.data = evt_data;
  (*(rw_cb.p_cback))(RW_T3T_RAW_FRAME_EVT, &rw_data);
}

/*****************************************************************************
**
** Function         rw_t3t_act_handle_check_ndef_rsp
**
** Description      Handle response to NDEF read segment
**
** Returns          Nothing
**
*****************************************************************************/
void rw_t3t_act_handle_check_ndef_rsp(tRW_T3T_CB* p_cb, NFC_HDR* p_msg_rsp) {
  bool check_complete = true;
  tNFC_STATUS nfc_status = NFC_STATUS_OK;
  uint8_t* p_t3t_rsp = (uint8_t*)(p_msg_rsp + 1) + p_msg_rsp->offset;
  uint8_t rsp_num_bytes_rx;

  if (p_msg_rsp->len < T3T_MSG_RSP_OFFSET_CHECK_DATA) {
    LOG(ERROR) << StringPrintf("%s invalid len", __func__);
    nfc_status = NFC_STATUS_FAILED;
    GKI_freebuf(p_msg_rsp);
    android_errorWriteLog(0x534e4554, "120428637");
    /* Validate response from tag */
  } else if ((p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1] !=
              T3T_MSG_RSP_STATUS_OK) /* verify response status code */
             || (memcmp(p_cb->peer_nfcid2, &p_t3t_rsp[T3T_MSG_RSP_OFFSET_IDM],
                        NCI_NFCID2_LEN) != 0) /* verify response IDm */
             || (p_t3t_rsp[T3T_MSG_RSP_OFFSET_NUMBLOCKS] !=
                 ((p_cb->ndef_rx_readlen + 15) >>
                  4))) /* verify length of response */
  {
    LOG(ERROR) << StringPrintf(
        "Response error: bad status, nfcid2, or invalid len: %i %i",
        p_t3t_rsp[T3T_MSG_RSP_OFFSET_NUMBLOCKS],
        ((p_cb->ndef_rx_readlen + 15) >> 4));
    nfc_status = NFC_STATUS_FAILED;
    GKI_freebuf(p_msg_rsp);
  } else if (p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE] != T3T_MSG_OPC_CHECK_RSP) {
    LOG(ERROR) << StringPrintf(
        "Response error: expecting rsp_code %02X, but got %02X",
        T3T_MSG_OPC_CHECK_RSP, p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE]);
    nfc_status = NFC_STATUS_FAILED;
    GKI_freebuf(p_msg_rsp);
  } else if (p_msg_rsp->len >= T3T_MSG_RSP_OFFSET_CHECK_DATA &&
             p_t3t_rsp[T3T_MSG_RSP_OFFSET_NUMBLOCKS] > 0) {
    /* Notify app of NDEF segment received */
    /* Number of bytes received, according to header */
    rsp_num_bytes_rx = p_t3t_rsp[T3T_MSG_RSP_OFFSET_NUMBLOCKS] * 16;
    p_cb->ndef_rx_offset += p_cb->ndef_rx_readlen;
    p_msg_rsp->offset +=
        T3T_MSG_RSP_OFFSET_CHECK_DATA; /* Skip over t3t header (point to block
                                          data) */
    p_msg_rsp->len -= T3T_MSG_RSP_OFFSET_CHECK_DATA;

    /* Verify that the bytes received is really the amount indicated in the
     * check-response header */
    if (rsp_num_bytes_rx > p_msg_rsp->len) {
      LOG(ERROR) << StringPrintf(
          "Response error: CHECK rsp header indicates %i bytes, but only "
          "received %i bytes",
          rsp_num_bytes_rx, p_msg_rsp->len);
      nfc_status = NFC_STATUS_FAILED;
      GKI_freebuf(p_msg_rsp);
    } else {
      /* If this is the the final block, then set len to reflect only valid
       * bytes (do not include padding to 16-byte boundary) */
      if ((p_cb->flags & RW_T3T_FL_IS_FINAL_NDEF_SEGMENT) &&
          (p_cb->ndef_attrib.ln & 0x000F)) {
        rsp_num_bytes_rx -= (16 - (p_cb->ndef_attrib.ln & 0x000F));
      }

      p_msg_rsp->len = rsp_num_bytes_rx;
      tRW_DATA rw_data;
      rw_data.data.status = NFC_STATUS_OK;
      rw_data.data.p_data = p_msg_rsp;
      (*(rw_cb.p_cback))(RW_T3T_CHECK_EVT, &rw_data);

      /* Send CHECK cmd for next NDEF segment, if needed */
      if (!(p_cb->flags & RW_T3T_FL_IS_FINAL_NDEF_SEGMENT)) {
        nfc_status = rw_t3t_send_next_ndef_check_cmd(p_cb);
        if (nfc_status == NFC_STATUS_OK) {
          /* Still getting more segments. Don't send RW_T3T_CHECK_CPLT_EVT yet
           */
          check_complete = false;
        }
      }
    }
  } else {
    android_errorWriteLog(0x534e4554, "120502559");
    GKI_freebuf(p_msg_rsp);
    nfc_status = NFC_STATUS_FAILED;
    LOG(ERROR) << StringPrintf("Underflow in p_msg_rsp->len!");
  }

  /* Notify app of RW_T3T_CHECK_CPLT_EVT if entire NDEF has been read, or if
   * failure */
  if (check_complete) {
    p_cb->rw_state = RW_T3T_STATE_IDLE;
    tRW_DATA evt_data;
    evt_data.status = nfc_status;
    (*(rw_cb.p_cback))(RW_T3T_CHECK_CPLT_EVT, &evt_data);
  }
}

/*****************************************************************************
**
** Function         rw_t3t_act_handle_update_ndef_rsp
**
** Description      Handle response to NDEF write segment
**
** Returns          Nothing
**
*****************************************************************************/
void rw_t3t_act_handle_update_ndef_rsp(tRW_T3T_CB* p_cb, NFC_HDR* p_msg_rsp) {
  bool update_complete = true;
  tNFC_STATUS nfc_status = NFC_STATUS_OK;
  uint8_t* p_t3t_rsp = (uint8_t*)(p_msg_rsp + 1) + p_msg_rsp->offset;

  /* Check nfcid2 and status of response */
  if ((p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1] !=
       T3T_MSG_RSP_STATUS_OK) /* verify response status code */
      || (memcmp(p_cb->peer_nfcid2, &p_t3t_rsp[T3T_MSG_RSP_OFFSET_IDM],
                 NCI_NFCID2_LEN) != 0)) /* verify response IDm */
  {
    nfc_status = NFC_STATUS_FAILED;
  }
  /* Validate response opcode */
  else if (p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE] != T3T_MSG_OPC_UPDATE_RSP) {
    LOG(ERROR) << StringPrintf(
        "Response error: expecting rsp_code %02X, but got %02X",
        T3T_MSG_OPC_UPDATE_RSP, p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE]);
    nfc_status = NFC_STATUS_FAILED;
  }
  /* If this is response to final UPDATE, then update NDEF local size */
  else if (p_cb->flags & RW_T3T_FL_IS_FINAL_NDEF_SEGMENT) {
    /* If successful, update current NDEF size */
    p_cb->ndef_attrib.ln = p_cb->ndef_msg_len;
  }
  /*  If any more NDEF bytes to update, then send next UPDATE command */
  else if (p_cb->ndef_msg_bytes_sent < p_cb->ndef_msg_len) {
    /* Send UPDATE command for next segment of NDEF */
    nfc_status = rw_t3t_send_next_ndef_update_cmd(p_cb);
    if (nfc_status == NFC_STATUS_OK) {
      /* Wait for update response */
      update_complete = false;
    }
  }
  /*  Otherwise, no more NDEF bytes. Send final UPDATE for Attribute Information
     block */
  else {
    p_cb->flags |= RW_T3T_FL_IS_FINAL_NDEF_SEGMENT;
    nfc_status = rw_t3t_send_update_ndef_attribute_cmd(p_cb, false);
    if (nfc_status == NFC_STATUS_OK) {
      /* Wait for update response */
      update_complete = false;
    }
  }

  /* If update is completed, then notify app */
  if (update_complete) {
    p_cb->rw_state = RW_T3T_STATE_IDLE;
    tRW_DATA evt_data;
    evt_data.status = nfc_status;
    (*(rw_cb.p_cback))(RW_T3T_UPDATE_CPLT_EVT, &evt_data);
  }

  GKI_freebuf(p_msg_rsp);

  return;
}

/*****************************************************************************
**
** Function         rw_t3t_handle_get_sc_poll_rsp
**
** Description      Handle POLL response for getting system codes
**
** Returns          Nothing
**
*****************************************************************************/
static void rw_t3t_handle_get_sc_poll_rsp(tRW_T3T_CB* p_cb, uint8_t nci_status,
                                          uint8_t num_responses,
                                          uint8_t sensf_res_buf_size,
                                          uint8_t* p_sensf_res_buf) {
  uint8_t* p;
  uint16_t sc;

  /* Get the system code from the response */
  if ((nci_status == NCI_STATUS_OK) && (num_responses > 0) &&
      (sensf_res_buf_size >=
       (RW_T3T_SENSF_RES_RD_OFFSET + RW_T3T_SENSF_RES_RD_LEN))) {
    p = &p_sensf_res_buf[RW_T3T_SENSF_RES_RD_OFFSET];
    BE_STREAM_TO_UINT16(sc, p);

    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("FeliCa detected (RD, system code %04X)", sc);
    if (p_cb->num_system_codes < T3T_MAX_SYSTEM_CODES) {
      p_cb->system_codes[p_cb->num_system_codes++] = sc;
    } else {
      LOG(ERROR) << StringPrintf("Exceed T3T_MAX_SYSTEM_CODES!");
      android_errorWriteLog(0x534e4554, "120499324");
    }
  }

  rw_t3t_handle_get_system_codes_cplt();
}

/*****************************************************************************
**
** Function         rw_t3t_handle_ndef_detect_poll_rsp
**
** Description      Handle POLL response for getting system codes
**
** Returns          Nothing
**
*****************************************************************************/
static void rw_t3t_handle_ndef_detect_poll_rsp(tRW_T3T_CB* p_cb,
                                               uint8_t nci_status,
                                               uint8_t num_responses) {
  NFC_HDR* p_cmd_buf;
  uint8_t *p, *p_cmd_start;
  tRW_DATA evt_data;

  /* Validate response for NDEF poll */
  if ((nci_status == NCI_STATUS_OK) && (num_responses > 0)) {
    /* Tag responded for NDEF poll */

    /* Read NDEF attribute block */
    p_cmd_buf = rw_t3t_get_cmd_buf();
    if (p_cmd_buf != nullptr) {
      /* Construct T3T message */
      p = p_cmd_start = (uint8_t*)(p_cmd_buf + 1) + p_cmd_buf->offset;

      /* Add CHECK opcode to message  */
      UINT8_TO_STREAM(p, T3T_MSG_OPC_CHECK_CMD);

      /* Add IDm to message */
      ARRAY_TO_STREAM(p, p_cb->peer_nfcid2, NCI_NFCID2_LEN);

      /* Add Service code list */
      UINT8_TO_STREAM(p, 1); /* Number of services (only 1 service: NDEF) */
      UINT16_TO_STREAM(
          p, T3T_MSG_NDEF_SC_RO); /* Service code (little-endian format) */

      /* Number of blocks */
      UINT8_TO_STREAM(
          p,
          1); /* Number of blocks (only 1 block: NDEF Attribute Information ) */

      /* Block List element: the NDEF attribute information block (block 0) */
      UINT8_TO_STREAM(p, T3T_MSG_MASK_TWO_BYTE_BLOCK_DESC_FORMAT);
      UINT8_TO_STREAM(p, 0);

      /* Calculate length of message */
      p_cmd_buf->len = (uint16_t)(p - p_cmd_start);

      /* Send the T3T message */
      evt_data.status = rw_t3t_send_cmd(p_cb, RW_T3T_CMD_DETECT_NDEF, p_cmd_buf,
                                        rw_t3t_check_timeout(1));
      if (evt_data.status == NFC_STATUS_OK) {
        /* CHECK command sent. Wait for response */
        return;
      }
    }
    nci_status = NFC_STATUS_FAILED;
  }

  /* NDEF detection failed */
  p_cb->rw_state = RW_T3T_STATE_IDLE;
  evt_data.ndef.status = nci_status;
  evt_data.ndef.flags = RW_NDEF_FL_UNKNOWN;
  rw_t3t_update_ndef_flag(&evt_data.ndef.flags);
  (*(rw_cb.p_cback))(RW_T3T_NDEF_DETECT_EVT, &evt_data);
}

/*****************************************************************************
**
** Function         rw_t3t_update_block
**
** Description      Send UPDATE command for single block
**                  (for formatting/configuring read only)
**
** Returns          tNFC_STATUS
**
*****************************************************************************/
tNFC_STATUS rw_t3t_update_block(tRW_T3T_CB* p_cb, uint8_t block_id,
                                uint8_t* p_block_data) {
  uint8_t *p_dst, *p_cmd_start;
  NFC_HDR* p_cmd_buf;
  tNFC_STATUS status;

  p_cmd_buf = rw_t3t_get_cmd_buf();
  if (p_cmd_buf != nullptr) {
    p_dst = p_cmd_start = (uint8_t*)(p_cmd_buf + 1) + p_cmd_buf->offset;

    /* Add UPDATE opcode to message  */
    UINT8_TO_STREAM(p_dst, T3T_MSG_OPC_UPDATE_CMD);

    /* Add IDm to message */
    ARRAY_TO_STREAM(p_dst, p_cb->peer_nfcid2, NCI_NFCID2_LEN);

    /* Add Service code list */
    UINT8_TO_STREAM(p_dst, 1); /* Number of services (only 1 service: NDEF) */
    UINT16_TO_STREAM(
        p_dst, T3T_MSG_NDEF_SC_RW); /* Service code (little-endian format) */

    /* Number of blocks */
    UINT8_TO_STREAM(p_dst, 1);

    /* Add Block list element for MC */
    UINT8_TO_STREAM(p_dst, T3T_MSG_MASK_TWO_BYTE_BLOCK_DESC_FORMAT);
    UINT8_TO_STREAM(p_dst, block_id);

    /* Copy MC data to UPDATE message */
    ARRAY_TO_STREAM(p_dst, p_block_data, T3T_MSG_BLOCKSIZE);

    /* Calculate length of message */
    p_cmd_buf->len = (uint16_t)(p_dst - p_cmd_start);

    /* Send the T3T message */
    status = rw_t3t_send_cmd(p_cb, p_cb->cur_cmd, p_cmd_buf,
                             rw_t3t_update_timeout(1));
  } else {
    /* Unable to send UPDATE command */
    status = NFC_STATUS_NO_BUFFERS;
  }

  return (status);
}

/*****************************************************************************
**
** Function         rw_t3t_handle_fmt_poll_rsp
**
** Description      Handle POLL response for formatting felica-lite
**
** Returns          Nothing
**
*****************************************************************************/
static void rw_t3t_handle_fmt_poll_rsp(tRW_T3T_CB* p_cb, uint8_t nci_status,
                                       uint8_t num_responses) {
  tRW_DATA evt_data;

  evt_data.status = NFC_STATUS_OK;

  /* Validate response for poll response */
  if ((nci_status == NCI_STATUS_OK) && (num_responses > 0)) {
    /* Tag responded for Felica-Lite poll */
    /* Get MemoryControl block */
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "Felica-Lite tag detected...getting Memory Control block.");

    p_cb->rw_substate = RW_T3T_FMT_SST_CHECK_MC_BLK;

    /* Send command to check Memory Configuration block */
    evt_data.status = rw_t3t_check_mc_block(p_cb);
  } else {
    LOG(ERROR) << StringPrintf("Felica-Lite tag not detected");
    evt_data.status = NFC_STATUS_FAILED;
  }

  /* If error, notify upper layer */
  if (evt_data.status != NFC_STATUS_OK) {
    rw_t3t_format_cplt(evt_data.status);
  }
}

/*****************************************************************************
**
** Function         rw_t3t_act_handle_fmt_rsp
**
** Description      Handle response for formatting codes
**
** Returns          Nothing
**
*****************************************************************************/
void rw_t3t_act_handle_fmt_rsp(tRW_T3T_CB* p_cb, NFC_HDR* p_msg_rsp) {
  uint8_t* p_t3t_rsp = (uint8_t*)(p_msg_rsp + 1) + p_msg_rsp->offset;
  uint8_t* p_mc;
  tRW_DATA evt_data;

  evt_data.status = NFC_STATUS_OK;

  /* Check tags's response for reading MemoryControl block */
  if (p_cb->rw_substate == RW_T3T_FMT_SST_CHECK_MC_BLK) {
    /* Validate response opcode */
    if (p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE] != T3T_MSG_OPC_CHECK_RSP) {
      LOG(ERROR) << StringPrintf(
          "Response error: expecting rsp_code %02X, but got %02X",
          T3T_MSG_OPC_CHECK_RSP, p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE]);
      evt_data.status = NFC_STATUS_FAILED;
    }
    /* Validate status code and NFCID2 response from tag */
    else if ((p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1] !=
              T3T_MSG_RSP_STATUS_OK) /* verify response status code */
             || (memcmp(p_cb->peer_nfcid2, &p_t3t_rsp[T3T_MSG_RSP_OFFSET_IDM],
                        NCI_NFCID2_LEN) != 0)) /* verify response IDm */
    {
      evt_data.status = NFC_STATUS_FAILED;
    } else if (p_msg_rsp->len <
               (T3T_MSG_RSP_OFFSET_CHECK_DATA + T3T_MSG_BLOCKSIZE)) {
      evt_data.status = NFC_STATUS_FAILED;
      android_errorWriteLog(0x534e4554, "120506143");
    } else {
      /* Check if memory configuration (MC) block to see if SYS_OP=1 (NDEF
       * enabled) */
      p_mc = &p_t3t_rsp[T3T_MSG_RSP_OFFSET_CHECK_DATA]; /* Point to MC data of
                                                           CHECK response */

      if (p_mc[T3T_MSG_FELICALITE_MC_OFFSET_SYS_OP] != 0x01) {
        /* Tag is not currently enabled for NDEF. Indicate that we need to
         * update the MC block */

        /* Set SYS_OP field to 0x01 (enable NDEF) */
        p_mc[T3T_MSG_FELICALITE_MC_OFFSET_SYS_OP] = 0x01;

        /* Set RF_PRM field to 0x07 (procedure of issuance) */
        p_mc[T3T_MSG_FELICALITE_MC_OFFSET_RF_PRM] = 0x07;

        /* Construct and send UPDATE message to write MC block */
        p_cb->rw_substate = RW_T3T_FMT_SST_UPDATE_MC_BLK;
        evt_data.status =
            rw_t3t_update_block(p_cb, T3T_MSG_FELICALITE_BLOCK_ID_MC, p_mc);
      } else {
        /* SYS_OP=1: ndef already enabled. Just need to update attribute
         * information block */
        p_cb->rw_substate = RW_T3T_FMT_SST_UPDATE_NDEF_ATTRIB;
        evt_data.status =
            rw_t3t_update_block(p_cb, 0, (uint8_t*)rw_t3t_default_attrib_info);
      }
    }

    /* If error, notify upper layer */
    if (evt_data.status != NFC_STATUS_OK) {
      rw_t3t_format_cplt(evt_data.status);
    }
  } else if (p_cb->rw_substate == RW_T3T_FMT_SST_UPDATE_MC_BLK) {
    /* Validate response opcode */
    if ((p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE] != T3T_MSG_OPC_UPDATE_RSP) ||
        (p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1] != T3T_MSG_RSP_STATUS_OK))

    {
      LOG(ERROR) << StringPrintf("Response error: rsp_code=%02X, status=%02X",
                                 p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE],
                                 p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1]);
      evt_data.status = NFC_STATUS_FAILED;
    } else {
      /* SYS_OP=1: ndef already enabled. Just need to update attribute
       * information block */
      p_cb->rw_substate = RW_T3T_FMT_SST_UPDATE_NDEF_ATTRIB;
      evt_data.status =
          rw_t3t_update_block(p_cb, 0, (uint8_t*)rw_t3t_default_attrib_info);
    }

    /* If error, notify upper layer */
    if (evt_data.status != NFC_STATUS_OK) {
      rw_t3t_format_cplt(evt_data.status);
    }
  } else if (p_cb->rw_substate == RW_T3T_FMT_SST_UPDATE_NDEF_ATTRIB) {
    /* Validate response opcode */
    if ((p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE] != T3T_MSG_OPC_UPDATE_RSP) ||
        (p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1] != T3T_MSG_RSP_STATUS_OK))

    {
      LOG(ERROR) << StringPrintf("Response error: rsp_code=%02X, status=%02X",
                                 p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE],
                                 p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1]);
      evt_data.status = NFC_STATUS_FAILED;
    }

    rw_t3t_format_cplt(evt_data.status);
  }

  GKI_freebuf(p_msg_rsp);
}

/*****************************************************************************
**
** Function         rw_t3t_handle_sro_poll_rsp
**
** Description      Handle POLL response for configuring felica-lite read only
**
** Returns          Nothing
**
*****************************************************************************/
static void rw_t3t_handle_sro_poll_rsp(tRW_T3T_CB* p_cb, uint8_t nci_status,
                                       uint8_t num_responses) {
  tRW_DATA evt_data;
  uint8_t rw_t3t_ndef_attrib_info[T3T_MSG_BLOCKSIZE];
  uint8_t* p;
  uint8_t tempU8;
  uint16_t checksum, i;
  uint32_t tempU32 = 0;

  evt_data.status = NFC_STATUS_OK;

  /* Validate response for poll response */
  if ((nci_status == NCI_STATUS_OK) && (num_responses > 0)) {
    /* Tag responded for Felica-Lite poll */
    if (p_cb->ndef_attrib.rwflag != T3T_MSG_NDEF_RWFLAG_RO) {
      /* First update attribute information block */
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "Felica-Lite tag detected...update NDef attribution block.");

      p_cb->rw_substate = RW_T3T_SRO_SST_UPDATE_NDEF_ATTRIB;

      p = rw_t3t_ndef_attrib_info;

      UINT8_TO_STREAM(p, p_cb->ndef_attrib.version);

      /* Update NDEF info */
      UINT8_TO_STREAM(
          p, p_cb->ndef_attrib.nbr); /* NBr: number of blocks that can be read
                                        using one Check command */
      UINT8_TO_STREAM(p, p_cb->ndef_attrib.nbw); /* Nbw: number of blocks that
                                                    can be written using one
                                                    Update command */
      UINT16_TO_BE_STREAM(
          p, p_cb->ndef_attrib.nmaxb); /* Nmaxb: maximum number of blocks
                                          available for NDEF data */
      UINT32_TO_BE_STREAM(p, tempU32);
      UINT8_TO_STREAM(p,
                      p_cb->ndef_attrib.writef); /* WriteFlag: 00h if writing
                                                    data finished; 0Fh if
                                                    writing data in progress */
      UINT8_TO_STREAM(p, 0x00); /* RWFlag: 00h NDEF is read-only */

      tempU8 = (uint8_t)(p_cb->ndef_attrib.ln >> 16);
      /* Get length (3-byte, big-endian) */
      UINT8_TO_STREAM(p, tempU8);                   /* Ln: high-byte */
      UINT16_TO_BE_STREAM(p, p_cb->ndef_attrib.ln); /* Ln: lo-word */

      /* Calculate and append Checksum */
      checksum = 0;
      for (i = 0; i < T3T_MSG_NDEF_ATTR_INFO_SIZE; i++) {
        checksum += rw_t3t_ndef_attrib_info[i];
      }
      UINT16_TO_BE_STREAM(p, checksum);

      evt_data.status =
          rw_t3t_update_block(p_cb, 0, (uint8_t*)rw_t3t_ndef_attrib_info);
    } else if (p_cb->cur_cmd == RW_T3T_CMD_SET_READ_ONLY_HARD) {
      /* NDEF is already read only, Read and update MemoryControl block */
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "Felica-Lite tag detected...getting Memory Control block.");
      p_cb->rw_substate = RW_T3T_SRO_SST_CHECK_MC_BLK;

      /* Send command to check Memory Configuration block */
      evt_data.status = rw_t3t_check_mc_block(p_cb);
    }
  } else {
    LOG(ERROR) << StringPrintf("Felica-Lite tag not detected");
    evt_data.status = NFC_STATUS_FAILED;
  }

  /* If error, notify upper layer */
  if (evt_data.status != NFC_STATUS_OK) {
    rw_t3t_set_readonly_cplt(evt_data.status);
  }
}

/*****************************************************************************
**
** Function         rw_t3t_act_handle_sro_rsp
**
** Description      Handle response for setting read only codes
**
** Returns          Nothing
**
*****************************************************************************/
void rw_t3t_act_handle_sro_rsp(tRW_T3T_CB* p_cb, NFC_HDR* p_msg_rsp) {
  uint8_t* p_t3t_rsp = (uint8_t*)(p_msg_rsp + 1) + p_msg_rsp->offset;
  uint8_t* p_mc;
  tRW_DATA evt_data;

  evt_data.status = NFC_STATUS_OK;

  if (p_cb->rw_substate == RW_T3T_SRO_SST_UPDATE_NDEF_ATTRIB) {
    /* Validate response opcode */
    if ((p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE] != T3T_MSG_OPC_UPDATE_RSP) ||
        (p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1] != T3T_MSG_RSP_STATUS_OK))

    {
      LOG(ERROR) << StringPrintf("Response error: rsp_code=%02X, status=%02X",
                                 p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE],
                                 p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1]);
      evt_data.status = NFC_STATUS_FAILED;
    } else {
      p_cb->ndef_attrib.rwflag = T3T_MSG_NDEF_RWFLAG_RO;
      if (p_cb->cur_cmd == RW_T3T_CMD_SET_READ_ONLY_HARD) {
        p_cb->rw_substate = RW_T3T_SRO_SST_CHECK_MC_BLK;

        /* Send command to check Memory Configuration block */
        evt_data.status = rw_t3t_check_mc_block(p_cb);
      } else {
        rw_t3t_set_readonly_cplt(evt_data.status);
      }
    }
  } else if (p_cb->rw_substate == RW_T3T_SRO_SST_CHECK_MC_BLK) {
    /* Check tags's response for reading MemoryControl block, Validate response
     * opcode */
    if (p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE] != T3T_MSG_OPC_CHECK_RSP) {
      LOG(ERROR) << StringPrintf(
          "Response error: expecting rsp_code %02X, but got %02X",
          T3T_MSG_OPC_CHECK_RSP, p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE]);
      evt_data.status = NFC_STATUS_FAILED;
    }
    /* Validate status code and NFCID2 response from tag */
    else if ((p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1] !=
              T3T_MSG_RSP_STATUS_OK) /* verify response status code */
             || (memcmp(p_cb->peer_nfcid2, &p_t3t_rsp[T3T_MSG_RSP_OFFSET_IDM],
                        NCI_NFCID2_LEN) != 0)) /* verify response IDm */
    {
      evt_data.status = NFC_STATUS_FAILED;
    } else if (p_msg_rsp->len <
               (T3T_MSG_RSP_OFFSET_CHECK_DATA + T3T_MSG_BLOCKSIZE)) {
      evt_data.status = NFC_STATUS_FAILED;
      android_errorWriteLog(0x534e4554, "120506143");
    } else {
      /* Check if memory configuration (MC) block to see if SYS_OP=1 (NDEF
       * enabled) */
      p_mc = &p_t3t_rsp[T3T_MSG_RSP_OFFSET_CHECK_DATA]; /* Point to MC data of
                                                           CHECK response */

      evt_data.status = NFC_STATUS_FAILED;
      if (p_mc[T3T_MSG_FELICALITE_MC_OFFSET_SYS_OP] == 0x01) {
        /* Set MC_SP field with MC[0] = 0x00 & MC[1] = 0xC0 (Hardlock) to change
         * access permission from RW to RO */
        p_mc[T3T_MSG_FELICALITE_MC_OFFSET_MC_SP] = 0x00;
        /* Not changing the access permission of Subtraction Register and
         * MC[0:1] */
        p_mc[T3T_MSG_FELICALITE_MC_OFFSET_MC_SP + 1] = 0xC0;

        /* Set RF_PRM field to 0x07 (procedure of issuance) */
        p_mc[T3T_MSG_FELICALITE_MC_OFFSET_RF_PRM] = 0x07;

        /* Construct and send UPDATE message to write MC block */
        p_cb->rw_substate = RW_T3T_SRO_SST_UPDATE_MC_BLK;
        evt_data.status =
            rw_t3t_update_block(p_cb, T3T_MSG_FELICALITE_BLOCK_ID_MC, p_mc);
      }
    }
  } else if (p_cb->rw_substate == RW_T3T_SRO_SST_UPDATE_MC_BLK) {
    /* Validate response opcode */
    if ((p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE] != T3T_MSG_OPC_UPDATE_RSP) ||
        (p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1] != T3T_MSG_RSP_STATUS_OK))

    {
      LOG(ERROR) << StringPrintf("Response error: rsp_code=%02X, status=%02X",
                                 p_t3t_rsp[T3T_MSG_RSP_OFFSET_RSPCODE],
                                 p_t3t_rsp[T3T_MSG_RSP_OFFSET_STATUS1]);
      evt_data.status = NFC_STATUS_FAILED;
    } else {
      rw_t3t_set_readonly_cplt(evt_data.status);
    }
  }

  /* If error, notify upper layer */
  if (evt_data.status != NFC_STATUS_OK) {
    rw_t3t_set_readonly_cplt(evt_data.status);
  }

  GKI_freebuf(p_msg_rsp);
}

/*******************************************************************************
**
** Function         rw_t3t_data_cback
**
** Description      This callback function receives the data from NFCC.
**
** Returns          none
**
*******************************************************************************/
void rw_t3t_data_cback(__attribute__((unused)) uint8_t conn_id,
                       tNFC_DATA_CEVT* p_data) {
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;
  NFC_HDR* p_msg = p_data->p_data;
  bool free_msg = false; /* if TRUE, free msg buffer before returning */
  uint8_t *p, sod;

  /* Stop rsponse timer */
  nfc_stop_quick_timer(&p_cb->timer);

#if (RW_STATS_INCLUDED == TRUE)
  /* Update rx stats */
  rw_main_update_rx_stats(p_msg->len);
#endif /* RW_STATS_INCLUDED */

  /* Check if we are expecting a response */
  if (p_cb->rw_state != RW_T3T_STATE_COMMAND_PENDING) {
    /*
    **  This must be raw frame response
    **  send raw frame to app with SoD
    */
    rw_t3t_act_handle_raw_senddata_rsp(p_cb, p_data);
  }
  /* Sanity check: verify msg len is big enough to contain t3t header */
  else if (p_msg->len < T3T_MSG_RSP_COMMON_HDR_LEN) {
    LOG(ERROR) << StringPrintf(
        "T3T: invalid Type3 Tag Message (invalid len: %i)", p_msg->len);
    free_msg = true;
    rw_t3t_process_frame_error();
  } else {
    /* Check for RF frame error */
    p = (uint8_t*)(p_msg + 1) + p_msg->offset;
    sod = p[0];

    if (p_msg->len < sod || p[sod] != NCI_STATUS_OK) {
      LOG(ERROR) << "T3T: rf frame error";
      GKI_freebuf(p_msg);
      rw_t3t_process_frame_error();
      return;
    }

    /* Skip over SoD */
    p_msg->offset++;
    p_msg->len--;

    /* Get response code */
    switch (p_cb->cur_cmd) {
      case RW_T3T_CMD_DETECT_NDEF:
        rw_t3t_act_handle_ndef_detect_rsp(p_cb, p_msg);
        break;

      case RW_T3T_CMD_CHECK_NDEF:
        rw_t3t_act_handle_check_ndef_rsp(p_cb, p_msg);
        break;

      case RW_T3T_CMD_UPDATE_NDEF:
        rw_t3t_act_handle_update_ndef_rsp(p_cb, p_msg);
        break;

      case RW_T3T_CMD_CHECK:
        rw_t3t_act_handle_check_rsp(p_cb, p_msg);
        break;

      case RW_T3T_CMD_UPDATE:
        rw_t3t_act_handle_update_rsp(p_cb, p_msg);
        break;

      case RW_T3T_CMD_SEND_RAW_FRAME:
        rw_t3t_act_handle_raw_senddata_rsp(p_cb, p_data);
        break;

      case RW_T3T_CMD_FORMAT:
        rw_t3t_act_handle_fmt_rsp(p_cb, p_msg);
        break;

      case RW_T3T_CMD_SET_READ_ONLY_SOFT:
      case RW_T3T_CMD_SET_READ_ONLY_HARD:
        rw_t3t_act_handle_sro_rsp(p_cb, p_msg);
        break;

      default:
        GKI_freebuf(p_msg);
        break;
    }
  }

  if (free_msg) {
    GKI_freebuf(p_msg);
  }
}

/*******************************************************************************
**
** Function         rw_t3t_conn_cback
**
** Description      This callback function receives the events/data from NFCC.
**
** Returns          none
**
*******************************************************************************/
void rw_t3t_conn_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                       tNFC_CONN* p_data) {
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "rw_t3t_conn_cback: conn_id=%i, evt=0x%02x", conn_id, event);

  /* Only handle NFC_RF_CONN_ID conn_id */
  if (conn_id != NFC_RF_CONN_ID) {
    return;
  }

  switch (event) {
    case NFC_DEACTIVATE_CEVT:
      rw_t3t_unselect();
      break;

    case NFC_DATA_CEVT: /* check for status in tNFC_CONN */
      if ((p_data != nullptr) && ((p_data->data.status == NFC_STATUS_OK) ||
                               (p_data->data.status == NFC_STATUS_CONTINUE))) {
        rw_t3t_data_cback(conn_id, &(p_data->data));
        break;
      } else if (p_data->data.p_data != nullptr) {
        /* Free the response buffer in case of error response */
        GKI_freebuf((NFC_HDR*)(p_data->data.p_data));
        p_data->data.p_data = nullptr;
      }
      /* Data event with error status...fall through to NFC_ERROR_CEVT case */
      FALLTHROUGH_INTENDED;

    case NFC_ERROR_CEVT:
      nfc_stop_quick_timer(&p_cb->timer);

#if (RW_STATS_INCLUDED == TRUE)
      rw_main_update_trans_error_stats();
#endif /* RW_STATS_INCLUDED */

      if (event == NFC_ERROR_CEVT)
        rw_t3t_process_error(NFC_STATUS_TIMEOUT);
      else if (p_data)
        rw_t3t_process_error(p_data->status);
      break;

    default:
      break;
  }
}

/*******************************************************************************
**
** Function         rw_t3t_mrti_to_a_b
**
** Description      Converts the given MRTI (Maximum Response Time Information)
**                  to the base to calculate timeout value.
**                  (The timeout value is a + b * number_blocks)
**
** Returns          NFC_STATUS_OK
**
*******************************************************************************/
static void rw_t3t_mrti_to_a_b(uint8_t mrti, uint32_t* p_a, uint32_t* p_b) {
  uint8_t a, b, e;

  a = (mrti & 0x7) + 1; /* A is bit 0 ~ bit 2 */
  mrti >>= 3;
  b = (mrti & 0x7) + 1; /* B is bit 3 ~ bit 5 */
  mrti >>= 3;
  e = mrti & 0x3;                 /* E is bit 6 ~ bit 7 */
  *p_a = rw_t3t_mrti_base[e] * a; /* (A+1) * base (i.e T/t3t * 4^E) */
  *p_b = rw_t3t_mrti_base[e] * b; /* (B+1) * base (i.e T/t3t * 4^E) */
}

/*******************************************************************************
**
** Function         rw_t3t_select
**
** Description      Called by NFC manager when a Type3 tag has been activated
**
** Returns          NFC_STATUS_OK
**
*******************************************************************************/
tNFC_STATUS rw_t3t_select(uint8_t peer_nfcid2[NCI_RF_F_UID_LEN],
                          uint8_t mrti_check, uint8_t mrti_update) {
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  memcpy(p_cb->peer_nfcid2, peer_nfcid2,
         NCI_NFCID2_LEN); /* Store tag's NFCID2 */
  p_cb->ndef_attrib.status =
      NFC_STATUS_NOT_INITIALIZED; /* Indicate that NDEF detection has not been
                                     performed yet */
  p_cb->rw_state = RW_T3T_STATE_IDLE;
  p_cb->flags = 0;
  rw_t3t_mrti_to_a_b(mrti_check, &p_cb->check_tout_a, &p_cb->check_tout_b);
  rw_t3t_mrti_to_a_b(mrti_update, &p_cb->update_tout_a, &p_cb->update_tout_b);

  /* Alloc cmd buf for retransmissions */
  if (p_cb->p_cur_cmd_buf == nullptr) {
    p_cb->p_cur_cmd_buf = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);
    if (p_cb->p_cur_cmd_buf == nullptr) {
      LOG(ERROR) << StringPrintf(
          "rw_t3t_select: unable to allocate buffer for retransmission");
      p_cb->rw_state = RW_T3T_STATE_NOT_ACTIVATED;
      return (NFC_STATUS_FAILED);
    }
  }

  NFC_SetStaticRfCback(rw_t3t_conn_cback);

  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         rw_t3t_unselect
**
** Description      Called by NFC manager when a Type3 tag has been de-activated
**
** Returns          NFC_STATUS_OK
**
*******************************************************************************/
static tNFC_STATUS rw_t3t_unselect() {
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;

#if (RW_STATS_INCLUDED == TRUE)
  /* Display stats */
  rw_main_log_stats();
#endif /* RW_STATS_INCLUDED */

  /* Stop t3t timer (if started) */
  nfc_stop_quick_timer(&p_cb->timer);

  /* Free cmd buf for retransmissions */
  if (p_cb->p_cur_cmd_buf) {
    GKI_freebuf(p_cb->p_cur_cmd_buf);
    p_cb->p_cur_cmd_buf = nullptr;
  }

  p_cb->rw_state = RW_T3T_STATE_NOT_ACTIVATED;
  NFC_SetStaticRfCback(nullptr);

  return NFC_STATUS_OK;
}

/*******************************************************************************
**
** Function         rw_t3t_update_ndef_flag
**
** Description      set additional NDEF Flags for felica lite tag
**
** Returns          updated NDEF Flag value
**
*******************************************************************************/
static void rw_t3t_update_ndef_flag(uint8_t* p_flag) {
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;
  uint8_t xx;

  for (xx = 0; xx < p_cb->num_system_codes; xx++) {
    if (p_cb->system_codes[xx] == T3T_SYSTEM_CODE_FELICA_LITE) {
      *p_flag &= ~RW_NDEF_FL_UNKNOWN;
      *p_flag |= (RW_NDEF_FL_SUPPORTED | RW_NDEF_FL_FORMATABLE);
      break;
    }
  }
}

/*******************************************************************************
**
** Function         rw_t3t_cmd_str
**
** Description      Converts cmd_id to command string for logging
**
** Returns          command string
**
*******************************************************************************/
static std::string rw_t3t_cmd_str(uint8_t cmd_id) {
  switch (cmd_id) {
    case RW_T3T_CMD_DETECT_NDEF:
      return "RW_T3T_CMD_DETECT_NDEF";
    case RW_T3T_CMD_CHECK_NDEF:
      return "RW_T3T_CMD_CHECK_NDEF";
    case RW_T3T_CMD_UPDATE_NDEF:
      return "RW_T3T_CMD_UPDATE_NDEF";
    case RW_T3T_CMD_CHECK:
      return "RW_T3T_CMD_CHECK";
    case RW_T3T_CMD_UPDATE:
      return "RW_T3T_CMD_UPDATE";
    case RW_T3T_CMD_SEND_RAW_FRAME:
      return "RW_T3T_CMD_SEND_RAW_FRAME";
    case RW_T3T_CMD_GET_SYSTEM_CODES:
      return "RW_T3T_CMD_GET_SYSTEM_CODES";
    default:
      return "Unknown";
  }
}

/*******************************************************************************
**
** Function         rw_t3t_state_str
**
** Description      Converts state_id to command string for logging
**
** Returns          command string
**
*******************************************************************************/
static std::string rw_t3t_state_str(uint8_t state_id) {
  switch (state_id) {
    case RW_T3T_STATE_NOT_ACTIVATED:
      return "RW_T3T_STATE_NOT_ACTIVATED";
    case RW_T3T_STATE_IDLE:
      return "RW_T3T_STATE_IDLE";
    case RW_T3T_STATE_COMMAND_PENDING:
      return "RW_T3T_STATE_COMMAND_PENDING";
    default:
      return "Unknown";
  }
}

/*****************************************************************************
**  Type3 Tag API Functions
*****************************************************************************/

/*****************************************************************************
**
** Function         RW_T3tDetectNDef
**
** Description
**      This function is used to perform NDEF detection on a Type 3 tag, and
**      retrieve the tag's NDEF attribute information (block 0).
**
**      Before using this API, the application must call RW_SelectTagType to
**      indicate that a Type 3 tag has been activated, and to provide the
**      tag's Manufacture ID (IDm) .
**
** Returns
**      NFC_STATUS_OK: ndef detection procedure started
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_T3tDetectNDef(void) {
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;
  tNFC_STATUS retval = NFC_STATUS_OK;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* Check if we are in valid state to handle this API */
  if (p_cb->rw_state != RW_T3T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Error: invalid state to handle API (0x%x)",
                               p_cb->rw_state);
    return (NFC_STATUS_FAILED);
  }

  retval = (tNFC_STATUS)nci_snd_t3t_polling(T3T_SYSTEM_CODE_NDEF, 0, 0);
  if (retval == NCI_STATUS_OK) {
    p_cb->cur_cmd = RW_T3T_CMD_DETECT_NDEF;
    p_cb->cur_tout = RW_T3T_DEFAULT_CMD_TIMEOUT_TICKS;
    p_cb->cur_poll_rc = 0;
    p_cb->rw_state = RW_T3T_STATE_COMMAND_PENDING;
    p_cb->flags |= RW_T3T_FL_W4_NDEF_DETECT_POLL_RSP;

    /* start timer for waiting for responses */
    rw_t3t_start_poll_timer(p_cb);
  }

  return (retval);
}

/*****************************************************************************
**
** Function         RW_T3tCheckNDef
**
** Description
**      Retrieve NDEF contents from a Type3 tag.
**
**      The RW_T3T_CHECK_EVT event is used to notify the application for each
**      segment of NDEF data received. The RW_T3T_CHECK_CPLT_EVT event is used
**      to notify the application all segments have been received.
**
**      Before using this API, the RW_T3tDetectNDef function must be called to
**      verify that the tag contains NDEF data, and to retrieve the NDEF
**      attributes.
**
**      Internally, this command will be separated into multiple Tag 3 Check
**      commands (if necessary) - depending on the tag's Nbr (max number of
**      blocks per read) attribute.
**
** Returns
**      NFC_STATUS_OK: check command started
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_T3tCheckNDef(void) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* Check if we are in valid state to handle this API */
  if (p_cb->rw_state != RW_T3T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Error: invalid state to handle API (0x%x)",
                               p_cb->rw_state);
    return (NFC_STATUS_FAILED);
  } else if (p_cb->ndef_attrib.status !=
             NFC_STATUS_OK) /* NDEF detection not performed yet? */
  {
    LOG(ERROR) << StringPrintf("Error: NDEF detection not performed yet");
    return (NFC_STATUS_NOT_INITIALIZED);
  } else if (p_cb->ndef_attrib.ln == 0) {
    LOG(ERROR) << StringPrintf("Type 3 tag contains empty NDEF message");
    return (NFC_STATUS_FAILED);
  }

  /* Check number of blocks needed for this update */
  p_cb->flags &= ~RW_T3T_FL_IS_FINAL_NDEF_SEGMENT;
  p_cb->ndef_rx_offset = 0;
  retval = rw_t3t_send_next_ndef_check_cmd(p_cb);

  return (retval);
}

/*****************************************************************************
**
** Function         RW_T3tUpdateNDef
**
** Description
**      Write NDEF contents to a Type3 tag.
**
**      The RW_T3T_UPDATE_CPLT_EVT callback event will be used to notify the
**      application of the response.
**
**      Before using this API, the RW_T3tDetectNDef function must be called to
**      verify that the tag contains NDEF data, and to retrieve the NDEF
**      attributes.
**
**      Internally, this command will be separated into multiple Tag 3 Update
**      commands (if necessary) - depending on the tag's Nbw (max number of
**      blocks per write) attribute.
**
** Returns
**      NFC_STATUS_OK: check command started
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_REFUSED: tag is read-only
**      NFC_STATUS_BUFFER_FULL: len exceeds tag's maximum size
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_T3tUpdateNDef(uint32_t len, uint8_t* p_data) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("RW_T3tUpdateNDef (len=%i)", len);

  /* Check if we are in valid state to handle this API */
  if (p_cb->rw_state != RW_T3T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Error: invalid state to handle API (0x%x)",
                               p_cb->rw_state);
    return (NFC_STATUS_FAILED);
  } else if (p_cb->ndef_attrib.status !=
             NFC_STATUS_OK) /* NDEF detection not performed yet? */
  {
    LOG(ERROR) << StringPrintf("Error: NDEF detection not performed yet");
    return (NFC_STATUS_NOT_INITIALIZED);
  } else if (len > (((uint32_t)p_cb->ndef_attrib.nmaxb) *
                    16)) /* Len exceed's tag's NDEF memory? */
  {
    return (NFC_STATUS_BUFFER_FULL);
  } else if (p_cb->ndef_attrib.rwflag ==
             T3T_MSG_NDEF_RWFLAG_RO) /* Tag's NDEF memory is read-only? */
  {
    return (NFC_STATUS_REFUSED);
  }

  /* Check number of blocks needed for this update */
  p_cb->flags &= ~RW_T3T_FL_IS_FINAL_NDEF_SEGMENT;
  p_cb->ndef_msg_bytes_sent = 0;
  p_cb->ndef_msg_len = len;
  p_cb->ndef_msg = p_data;

  /* Send initial UPDATE command for NDEF Attribute Info */
  retval = rw_t3t_send_update_ndef_attribute_cmd(p_cb, true);

  return (retval);
}

/*****************************************************************************
**
** Function         RW_T3tCheck
**
** Description
**      Read (non-NDEF) contents from a Type3 tag.
**
**      The RW_READ_EVT event is used to notify the application for each
**      segment of NDEF data received. The RW_READ_CPLT_EVT event is used to
**      notify the application all segments have been received.
**
**      Before using this API, the application must call RW_SelectTagType to
**      indicate that a Type 3 tag has been activated, and to provide the
**      tag's Manufacture ID (IDm) .
**
** Returns
**      NFC_STATUS_OK: check command started
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_T3tCheck(uint8_t num_blocks, tT3T_BLOCK_DESC* t3t_blocks) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("RW_T3tCheck (num_blocks = %i)", num_blocks);

  /* Check if we are in valid state to handle this API */
  if (p_cb->rw_state != RW_T3T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Error: invalid state to handle API (0x%x)",
                               p_cb->rw_state);
    return (NFC_STATUS_FAILED);
  }

  /* Send the CHECK command */
  retval = rw_t3t_send_check_cmd(p_cb, num_blocks, t3t_blocks);

  return (retval);
}

/*****************************************************************************
**
** Function         RW_T3tUpdate
**
** Description
**      Write (non-NDEF) contents to a Type3 tag.
**
**      The RW_WRITE_CPLT_EVT event is used to notify the application all
**      segments have been received.
**
**      Before using this API, the application must call RW_SelectTagType to
**      indicate that a Type 3 tag has been activated, and to provide the tag's
**      Manufacture ID (IDm) .
**
** Returns
**      NFC_STATUS_OK: check command started
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_T3tUpdate(uint8_t num_blocks, tT3T_BLOCK_DESC* t3t_blocks,
                         uint8_t* p_data) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("RW_T3tUpdate (num_blocks = %i)", num_blocks);

  /* Check if we are in valid state to handle this API */
  if (p_cb->rw_state != RW_T3T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Error: invalid state to handle API (0x%x)",
                               p_cb->rw_state);
    return (NFC_STATUS_FAILED);
  }

  /* Send the UPDATE command */
  retval = rw_t3t_send_update_cmd(p_cb, num_blocks, t3t_blocks, p_data);

  return (retval);
}

/*****************************************************************************
**
** Function         RW_T3tPresenceCheck
**
** Description
**      Check if the tag is still in the field.
**
**      The RW_T3T_PRESENCE_CHECK_EVT w/ status is used to indicate presence
**      or non-presence.
**
** Returns
**      NFC_STATUS_OK, if raw data frame sent
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_T3tPresenceCheck(void) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_DATA evt_data;
  tRW_CB* p_rw_cb = &rw_cb;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* If RW_SelectTagType was not called (no conn_callback) return failure */
  if (!(p_rw_cb->p_cback)) {
    retval = NFC_STATUS_FAILED;
  }
  /* If we are not activated, then RW_T3T_PRESENCE_CHECK_EVT status=FAIL */
  else if (p_rw_cb->tcb.t3t.rw_state == RW_T3T_STATE_NOT_ACTIVATED) {
    evt_data.status = NFC_STATUS_FAILED;
    (*p_rw_cb->p_cback)(RW_T3T_PRESENCE_CHECK_EVT, &evt_data);
  }
  /* If command is pending */
  else if (p_rw_cb->tcb.t3t.rw_state == RW_T3T_STATE_COMMAND_PENDING) {
    /* If already performing presence check, return error */
    if (p_rw_cb->tcb.t3t.flags & RW_T3T_FL_W4_PRESENCE_CHECK_POLL_RSP) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("RW_T3tPresenceCheck already in progress");
      retval = NFC_STATUS_FAILED;
    }
    /* If busy with any other command, assume that the tag is present */
    else {
      evt_data.status = NFC_STATUS_OK;
      (*p_rw_cb->p_cback)(RW_T3T_PRESENCE_CHECK_EVT, &evt_data);
    }
  } else {
    /* IDLE state: send POLL command */
    retval = (tNFC_STATUS)nci_snd_t3t_polling(0xFFFF, T3T_POLL_RC_SC, 0);
    if (retval == NCI_STATUS_OK) {
      p_rw_cb->tcb.t3t.flags |= RW_T3T_FL_W4_PRESENCE_CHECK_POLL_RSP;
      p_rw_cb->tcb.t3t.rw_state = RW_T3T_STATE_COMMAND_PENDING;
      p_rw_cb->tcb.t3t.cur_poll_rc = 0;

      /* start timer for waiting for responses */
      rw_t3t_start_poll_timer(&p_rw_cb->tcb.t3t);
    } else {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "RW_T3tPresenceCheck error sending NCI_RF_T3T_POLLING cmd (status = "
          "0x%0x)",
          retval);
    }
  }

  return (retval);
}

/*****************************************************************************
**
** Function         RW_T3tPoll
**
** Description
**      Send POLL command
**
** Returns
**      NFC_STATUS_OK, if raw data frame sent
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_T3tPoll(uint16_t system_code, tT3T_POLL_RC rc, uint8_t tsn) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* Check if we are in valid state to handle this API */
  if (p_cb->rw_state != RW_T3T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Error: invalid state to handle API (0x%x)",
                               p_cb->rw_state);
    return (NFC_STATUS_FAILED);
  }

  retval = (tNFC_STATUS)nci_snd_t3t_polling(system_code, (uint8_t)rc, tsn);
  if (retval == NCI_STATUS_OK) {
    /* start timer for waiting for responses */
    p_cb->cur_poll_rc = rc;
    p_cb->rw_state = RW_T3T_STATE_COMMAND_PENDING;
    rw_t3t_start_poll_timer(p_cb);
  }

  return (retval);
}

/*****************************************************************************
**
** Function         RW_T3tSendRawFrame
**
** Description
**      This function is called to send a raw data frame to the peer device.
**      When type 3 tag receives response from peer, the callback function
**      will be called with a RW_T3T_RAW_FRAME_EVT [Table 6].
**
**      Before using this API, the application must call RW_SelectTagType to
**      indicate that a Type 3 tag has been activated.
**
**      The raw frame should be a properly formatted Type 3 tag message.
**
** Returns
**      NFC_STATUS_OK, if raw data frame sent
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_T3tSendRawFrame(uint16_t len, uint8_t* p_data) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("RW_T3tSendRawFrame (len = %i)", len);

  /* Check if we are in valid state to handle this API */
  if (p_cb->rw_state != RW_T3T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Error: invalid state to handle API (0x%x)",
                               p_cb->rw_state);
    return (NFC_STATUS_FAILED);
  }

  /* Send the UPDATE command */
  retval = rw_t3t_send_raw_frame(p_cb, len, p_data);

  return (retval);
}

/*****************************************************************************
**
** Function         RW_T3tGetSystemCodes
**
** Description
**      Get systems codes supported by the activated tag:
**              Poll for wildcard (FFFF, RC=1):
**
**      Before using this API, the application must call RW_SelectTagType to
**      indicate that a Type 3 tag has been activated.
**
** Returns
**      NFC_STATUS_OK, if raw data frame sent
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_T3tGetSystemCodes(void) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* Check if we are in valid state to handle this API */
  if (p_cb->rw_state != RW_T3T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Error: invalid state to handle API (0x%x)",
                               p_cb->rw_state);
    return (NFC_STATUS_FAILED);
  } else {
    retval = (tNFC_STATUS)nci_snd_t3t_polling(0xFFFF, T3T_POLL_RC_SC, 0);
    if (retval == NCI_STATUS_OK) {
      p_cb->cur_cmd = RW_T3T_CMD_GET_SYSTEM_CODES;
      p_cb->cur_tout = RW_T3T_DEFAULT_CMD_TIMEOUT_TICKS;
      p_cb->cur_poll_rc = T3T_POLL_RC_SC;
      p_cb->rw_state = RW_T3T_STATE_COMMAND_PENDING;
      p_cb->flags |= RW_T3T_FL_W4_GET_SC_POLL_RSP;
      p_cb->num_system_codes = 0;

      /* start timer for waiting for responses */
      rw_t3t_start_poll_timer(p_cb);
    }
  }

  return (retval);
}

/*****************************************************************************
**
** Function         RW_T3tFormatNDef
**
** Description
**      Format a type-3 tag for NDEF.
**
**      Only Felica-Lite tags are supported by this API. The
**      RW_T3T_FORMAT_CPLT_EVT is used to notify the status of the operation.
**
** Returns
**      NFC_STATUS_OK: ndef detection procedure started
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_T3tFormatNDef(void) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* Check if we are in valid state to handle this API */
  if (p_cb->rw_state != RW_T3T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Error: invalid state to handle API (0x%x)",
                               p_cb->rw_state);
    return (NFC_STATUS_FAILED);
  } else {
    /* Poll tag, to see if Felica-Lite system is supported */
    retval = (tNFC_STATUS)nci_snd_t3t_polling(T3T_SYSTEM_CODE_FELICA_LITE,
                                              T3T_POLL_RC_SC, 0);
    if (retval == NCI_STATUS_OK) {
      p_cb->cur_cmd = RW_T3T_CMD_FORMAT;
      p_cb->cur_tout = RW_T3T_DEFAULT_CMD_TIMEOUT_TICKS;
      p_cb->cur_poll_rc = T3T_POLL_RC_SC;
      p_cb->rw_state = RW_T3T_STATE_COMMAND_PENDING;
      p_cb->rw_substate = RW_T3T_FMT_SST_POLL_FELICA_LITE;
      p_cb->flags |= RW_T3T_FL_W4_FMT_FELICA_LITE_POLL_RSP;

      /* start timer for waiting for responses */
      rw_t3t_start_poll_timer(p_cb);
    }
  }

  return (retval);
}

/*****************************************************************************
**
** Function         RW_T3tSetReadOnly
**
** Description      This function performs NDEF read-only procedure
**                  Note: Only Felica-Lite tags are supported by this API.
**                        RW_T3tDetectNDef() must be called before using this
**
**                  The RW_T3T_SET_READ_ONLY_CPLT_EVT event will be returned.
**
** Returns          NFC_STATUS_OK if success
**                  NFC_STATUS_FAILED if T3T is busy or other error
**
*****************************************************************************/
tNFC_STATUS RW_T3tSetReadOnly(bool b_hard_lock) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_T3T_CB* p_cb = &rw_cb.tcb.t3t;
  tRW_DATA evt_data;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("b_hard_lock=%d", b_hard_lock);

  /* Check if we are in valid state to handle this API */
  if (p_cb->rw_state != RW_T3T_STATE_IDLE) {
    LOG(ERROR) << StringPrintf("Error: invalid state to handle API (0x%x)",
                               p_cb->rw_state);
    return (NFC_STATUS_FAILED);
  }

  if (p_cb->ndef_attrib.status !=
      NFC_STATUS_OK) /* NDEF detection not performed yet? */
  {
    LOG(ERROR) << StringPrintf("Error: NDEF detection not performed yet");
    return (NFC_STATUS_NOT_INITIALIZED);
  }

  if ((!b_hard_lock) &&
      (p_cb->ndef_attrib.rwflag ==
       T3T_MSG_NDEF_RWFLAG_RO)) /* Tag's NDEF memory is read-only already */
  {
    evt_data.status = NFC_STATUS_OK;
    (*(rw_cb.p_cback))(RW_T3T_SET_READ_ONLY_CPLT_EVT, &evt_data);
    return (retval);
  } else {
    /* Poll tag, to see if Felica-Lite system is supported */
    retval = (tNFC_STATUS)nci_snd_t3t_polling(T3T_SYSTEM_CODE_FELICA_LITE,
                                              T3T_POLL_RC_SC, 0);
    if (retval == NCI_STATUS_OK) {
      if (b_hard_lock)
        p_cb->cur_cmd = RW_T3T_CMD_SET_READ_ONLY_HARD;
      else
        p_cb->cur_cmd = RW_T3T_CMD_SET_READ_ONLY_SOFT;
      p_cb->cur_tout = RW_T3T_DEFAULT_CMD_TIMEOUT_TICKS;
      p_cb->cur_poll_rc = T3T_POLL_RC_SC;
      p_cb->rw_state = RW_T3T_STATE_COMMAND_PENDING;
      p_cb->rw_substate = RW_T3T_SRO_SST_POLL_FELICA_LITE;
      p_cb->flags |= RW_T3T_FL_W4_SRO_FELICA_LITE_POLL_RSP;

      /* start timer for waiting for responses */
      rw_t3t_start_poll_timer(p_cb);
    }
  }
  return (retval);
}
