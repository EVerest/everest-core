/******************************************************************************
 *
 *  Copyright (C) 1999-2014 Broadcom Corporation
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
#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <fcntl.h>
#include <log/log.h>
#include <sys/stat.h>

#include "nfc_target.h"
#ifndef LINUX
#include "include/debug_nfcsnoop.h"
#endif
#include "nci_defs.h"
#include "nci_hmsgs.h"
#include "nfc_api.h"
#include "nfc_int.h"
#include "rw_api.h"
#include "rw_int.h"

#include <statslog.h>
#include "metrics.h"
#include<unistd.h> // required only in Linux Raspberry for the close sytem call
#ifdef ANDROID
using android::base::StringPrintf;
#endif
#if (NFC_RW_ONLY == FALSE)
static const uint8_t nfc_mpl_code_to_size[] = {64, 128, 192, 254};

#endif /* NFC_RW_ONLY */
#if (APPL_DTA_MODE == TRUE)
// Global Structure varibale for FW Version
static tNFC_FW_VERSION nfc_fw_version;
#endif
#define NFC_PB_ATTRIB_REQ_FIXED_BYTES 1
#define NFC_LB_ATTRIB_REQ_FIXED_BYTES 8

extern unsigned char appl_dta_mode_flag;
extern bool nfc_debug_enabled;
extern std::string nfc_storage_path;

static struct timeval timer_start;
static struct timeval timer_end;

static const off_t NATIVE_CRASH_FILE_SIZE = (1024 * 1024);

void storeNativeCrashLogs(void) {
  std::string filename = "/native_crash_logs";
  std::string filepath = nfc_storage_path + filename;
  int fileStream;
  off_t fileSize;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: file=%s", __func__, filepath.c_str());
  // check file size
  struct stat st;
  if (stat(filepath.c_str(), &st) == 0) {
    fileSize = st.st_size;
  } else {
    fileSize = 0;
  }

  if (fileSize >= NATIVE_CRASH_FILE_SIZE) {
    fileStream =
        open(filepath.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  } else {
    fileStream =
        open(filepath.c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
  }

  if (fileStream >= 0) {
#ifndef LINUX
    debug_nfcsnoop_dump(fileStream);
#endif
    close(fileStream);
  } else {
    LOG(ERROR) << StringPrintf("%s: fail to create, error = %d", __func__,
                               errno);
  }
}

/*******************************************************************************
**
** Function         nfc_ncif_update_window
**
** Description      Update tx cmd window to indicate that NFCC can received
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_update_window(void) {
  /* Sanity check - see if we were expecting a update_window */
  if (nfc_cb.nci_cmd_window == NCI_MAX_CMD_WINDOW) {
    if (nfc_cb.nfc_state != NFC_STATE_W4_HAL_CLOSE) {
      LOG(ERROR) << StringPrintf("nfc_ncif_update_window: Unexpected call");
    }
    return;
  }

  /* Stop command-pending timer */
  nfc_stop_timer(&nfc_cb.nci_wait_rsp_timer);

  nfc_cb.p_vsc_cback = nullptr;
  nfc_cb.nci_cmd_window++;

  /* Check if there were any commands waiting to be sent */
  nfc_ncif_check_cmd_queue(nullptr);
}

/*******************************************************************************
**
** Function         nfc_ncif_cmd_timeout
**
** Description      Handle a command timeout
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_cmd_timeout(void) {
  LOG(ERROR) << StringPrintf("nfc_ncif_cmd_timeout");

  storeNativeCrashLogs();

  /* report an error */
  nfc_ncif_event_status(NFC_GEN_ERROR_REVT, NFC_STATUS_HW_TIMEOUT);
  nfc_ncif_event_status(NFC_NFCC_TIMEOUT_REVT, NFC_STATUS_HW_TIMEOUT);

  /* if enabling NFC, notify upper layer of failure */
  if (nfc_cb.nfc_state == NFC_STATE_CORE_INIT) {
    nfc_enabled(NFC_STATUS_FAILED, nullptr);
  }

  /* XXX maco since this failure is unrecoverable, abort the process */
#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
  // Do not abort if for fuzz testing -- this may have some undesired
  // effect but this is the best we can do.
#else
  abort();
#endif
}

/*******************************************************************************
**
** Function         nfc_wait_2_deactivate_timeout
**
** Description      Handle a command timeout
**
** Returns          void
**
*******************************************************************************/
void nfc_wait_2_deactivate_timeout(void) {
  LOG(ERROR) << StringPrintf("nfc_wait_2_deactivate_timeout");
  nfc_cb.flags &= ~NFC_FL_DEACTIVATING;
  nci_snd_deactivate_cmd((uint8_t)nfc_cb.deactivate_timer.param);
}

/*******************************************************************************
**
** Function         nfc_ncif_send_data
**
** Description      This function is called to add the NCI data header
**                  and send it to NCIT task for sending it to transport
**                  as credits are available.
**
** Returns          void
**
*******************************************************************************/
uint8_t nfc_ncif_send_data(tNFC_CONN_CB* p_cb, NFC_HDR* p_data) {
  uint8_t* pp;
  uint8_t* ps;
  uint8_t ulen = NCI_MAX_PAYLOAD_SIZE;
  NFC_HDR* p;
  uint8_t pbf = 1;
  uint8_t buffer_size = p_cb->buff_size;
  uint8_t hdr0 = p_cb->conn_id;
  bool fragmented = false;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfc_ncif_send_data :%d, num_buff:%d qc:%d",
                      p_cb->conn_id, p_cb->num_buff, p_cb->tx_q.count);
  if (p_cb->id == NFC_RF_CONN_ID) {
    if (nfc_cb.nfc_state != NFC_STATE_OPEN) {
      if (nfc_cb.nfc_state == NFC_STATE_CLOSING) {
        if ((p_data == nullptr) && /* called because credit from NFCC */
            (nfc_cb.flags & NFC_FL_DEACTIVATING)) {
          if (p_cb->init_credits == p_cb->num_buff) {
            /* all the credits are back */
            nfc_cb.flags &= ~NFC_FL_DEACTIVATING;
            DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
                "deactivating NFC-DEP init_credits:%d, num_buff:%d",
                p_cb->init_credits, p_cb->num_buff);
            nfc_stop_timer(&nfc_cb.deactivate_timer);
            nci_snd_deactivate_cmd((uint8_t)nfc_cb.deactivate_timer.param);
          }
        }
      }
      return NCI_STATUS_FAILED;
    }
  }

  if (p_data) {
    /* always enqueue the data to the tx queue */
    GKI_enqueue(&p_cb->tx_q, p_data);
  }

  /* try to send the first data packet in the tx queue  */
  p_data = (NFC_HDR*)GKI_getfirst(&p_cb->tx_q);

  /* post data fragment to NCIT task as credits are available */
  while (p_data && (p_cb->num_buff > 0)) {
    if (p_data->len <= buffer_size) {
      pbf = 0; /* last fragment */
      ulen = (uint8_t)(p_data->len);
      fragmented = false;
    } else {
      fragmented = true;
      ulen = buffer_size;
    }

    if (!fragmented) {
      /* if data packet is not fragmented, use the original buffer */
      p = p_data;
      p_data = (NFC_HDR*)GKI_dequeue(&p_cb->tx_q);
    } else {
      /* the data packet is too big and need to be fragmented
       * prepare a new GKI buffer
       * (even the last fragment to avoid issues) */
      p = NCI_GET_CMD_BUF(ulen);
      if (p == nullptr) return (NCI_STATUS_BUFFER_FULL);
      p->len = ulen;
      p->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE + 1;
      if (p->len) {
        pp = (uint8_t*)(p + 1) + p->offset;
        ps = (uint8_t*)(p_data + 1) + p_data->offset;
        memcpy(pp, ps, ulen);
      }
      /* adjust the NFC_HDR on the old fragment */
      p_data->len -= ulen;
      p_data->offset += ulen;
    }

    p->event = BT_EVT_TO_NFC_NCI;
    p->layer_specific = pbf;
    p->len += NCI_DATA_HDR_SIZE;
    p->offset -= NCI_DATA_HDR_SIZE;
    pp = (uint8_t*)(p + 1) + p->offset;
    /* build NCI Data packet header */
    NCI_DATA_PBLD_HDR(pp, pbf, hdr0, ulen);

    if (p_cb->num_buff != NFC_CONN_NO_FC) p_cb->num_buff--;

    /* send to HAL */
#ifndef LINUX
    nfcsnoop_capture(p, false);
#endif
    HAL_WRITE(p);

    if (!fragmented) {
      /* check if there are more data to send */
      p_data = (NFC_HDR*)GKI_getfirst(&p_cb->tx_q);
    }
  }

  // log duration for the first hce data response
  if (timer_start.tv_sec != 0 || timer_start.tv_usec != 0) {
    gettimeofday(&timer_end, nullptr);
    uint32_t delta_time_ms = (timer_end.tv_sec - timer_start.tv_sec) * 1000 +
                             (timer_end.tv_usec - timer_start.tv_usec) / 1000;
    memset(&timer_start, 0, sizeof(timer_start));
    memset(&timer_end, 0, sizeof(timer_end));
#ifdef ANDROID
    android::util::stats_write(android::util::NFC_HCE_TRANSACTION_OCCURRED,
                               (int32_t)delta_time_ms);
#endif
  }
  return (NCI_STATUS_OK);
}

/*******************************************************************************
**
** Function         nfc_ncif_check_cmd_queue
**
** Description      Send NCI command to the transport
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_check_cmd_queue(NFC_HDR* p_buf) {
  uint8_t* ps;
  /* If there are commands waiting in the xmit queue, or if the controller
   * cannot accept any more commands, */
  /* then enqueue this command */
  if (p_buf) {
    if ((nfc_cb.nci_cmd_xmit_q.count) || (nfc_cb.nci_cmd_window == 0)) {
      GKI_enqueue(&nfc_cb.nci_cmd_xmit_q, p_buf);
      p_buf = nullptr;
    }
  }

  /* If controller can accept another command, then send the next command */
  if (nfc_cb.nci_cmd_window > 0) {
    /* If no command was provided, or if older commands were in the queue, then
     * get cmd from the queue */
    if (!p_buf) p_buf = (NFC_HDR*)GKI_dequeue(&nfc_cb.nci_cmd_xmit_q);

    if (p_buf) {
      /* save the message header to double check the response */
      ps = (uint8_t*)(p_buf + 1) + p_buf->offset;
      memcpy(nfc_cb.last_hdr, ps, NFC_SAVED_HDR_SIZE);
      memcpy(nfc_cb.last_cmd, ps + NCI_MSG_HDR_SIZE, NFC_SAVED_CMD_SIZE);
      if (p_buf->layer_specific == NFC_WAIT_RSP_VSC) {
        /* save the callback for NCI VSCs)  */
        nfc_cb.p_vsc_cback = (void*)((tNFC_NCI_VS_MSG*)p_buf)->p_cback;
      } else if (p_buf->layer_specific == NFC_WAIT_RSP_RAW_VS) {
        /* save the callback for RAW VS */
        nfc_cb.p_vsc_cback = (void*)((tNFC_NCI_VS_MSG*)p_buf)->p_cback;
        nfc_cb.rawVsCbflag = true;
      }

      /* Indicate command is pending */
      nfc_cb.nci_cmd_window--;

      /* send to HAL */
      HAL_WRITE(p_buf);
      /* start NFC command-timeout timer */
      nfc_start_timer(&nfc_cb.nci_wait_rsp_timer,
                      (uint16_t)(NFC_TTYPE_NCI_WAIT_RSP),
                      nfc_cb.nci_wait_rsp_tout);
    }
  }

  if (nfc_cb.nci_cmd_window == NCI_MAX_CMD_WINDOW) {
    /* the command queue must be empty now */
    if (nfc_cb.flags & NFC_FL_CONTROL_REQUESTED) {
      /* HAL requested control or stack needs to handle pre-discover */
      nfc_cb.flags &= ~NFC_FL_CONTROL_REQUESTED;
      if (nfc_cb.flags & NFC_FL_DISCOVER_PENDING) {
        if (nfc_cb.p_hal->prediscover()) {
          /* HAL has the command window now */
          nfc_cb.flags |= NFC_FL_CONTROL_GRANTED;
          nfc_cb.nci_cmd_window = 0;
        } else {
          /* HAL does not need to send command,
           * - restore the command window and issue the discovery command now */
          nfc_cb.flags &= ~NFC_FL_DISCOVER_PENDING;
          ps = (uint8_t*)nfc_cb.p_disc_pending;
          nci_snd_discover_cmd(*ps, (tNFC_DISCOVER_PARAMS*)(ps + 1));
          GKI_freebuf(nfc_cb.p_disc_pending);
          nfc_cb.p_disc_pending = nullptr;
        }
      } else if (nfc_cb.flags & NFC_FL_HAL_REQUESTED) {
        /* grant the control to HAL */
        nfc_cb.flags &= ~NFC_FL_HAL_REQUESTED;
        nfc_cb.flags |= NFC_FL_CONTROL_GRANTED;
        nfc_cb.nci_cmd_window = 0;
        nfc_cb.p_hal->control_granted();
      }
    }
  }
}

#if (APPL_DTA_MODE == TRUE)
/*******************************************************************************
**
** Function         nfc_ncif_getFWVersion
**
** Description      This function is called to fet the FW Version
**
** Returns          tNFC_FW_VERSION
**
*******************************************************************************/
tNFC_FW_VERSION nfc_ncif_getFWVersion() { return nfc_fw_version; }
#endif

/*******************************************************************************
**
** Function         nfc_ncif_send_cmd
**
** Description      Send NCI command to the NCIT task
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_send_cmd(NFC_HDR* p_buf) {
  /* post the p_buf to NCIT task */
  p_buf->event = BT_EVT_TO_NFC_NCI;
  p_buf->layer_specific = 0;
#ifndef LINUX
  nfcsnoop_capture(p_buf, false);
#endif
  nfc_ncif_check_cmd_queue(p_buf);
}

/*******************************************************************************
**
** Function         nfc_ncif_process_event
**
** Description      This function is called to process the
**                  data/response/notification from NFCC
**
** Returns          TRUE if need to free buffer
**
*******************************************************************************/
bool nfc_ncif_process_event(NFC_HDR* p_msg) {
  uint8_t mt, pbf, gid, *p;
  bool free = true;
  uint8_t oid;
  uint16_t len;
  uint8_t *p_old, old_gid, old_oid, old_mt;

  p = (uint8_t*)(p_msg + 1) + p_msg->offset;

  if (p_msg->len < 3) {
    // Per NCI spec, every packets should have at least 3 bytes: HDR0, HDR1, and
    // LEN field.
    LOG(ERROR) << StringPrintf("Invalid NCI packet: p_msg->len: %d",
                               p_msg->len);
    return free;
  }

  // LEN field contains the size of the payload, not including the 3-byte packet
  // header.
  len = p[2] + 3;
  if (p_msg->len < len) {
    // Making sure the packet holds enough data than it claims.
    LOG(ERROR) << StringPrintf("Invalid NCI packet: p_msg->len (%d) < len (%d)",
                               p_msg->len, len);
    return free;
  }

  NCI_MSG_PRS_HDR0(p, mt, pbf, gid);
  oid = ((*p) & NCI_OID_MASK);
  if (nfc_cb.rawVsCbflag == true &&
      nfc_ncif_proc_proprietary_rsp(mt, gid, oid) == true) {
    nci_proc_prop_raw_vs_rsp(p_msg);
    nfc_cb.rawVsCbflag = false;
    return free;
  }

#ifndef LINUX
  nfcsnoop_capture(p_msg, true);
#endif
  switch (mt) {
    case NCI_MT_DATA:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("NFC received data");
      nfc_ncif_proc_data(p_msg);
      free = false;
      break;

    case NCI_MT_RSP:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("NFC received rsp gid:%d", gid);
      oid = ((*p) & NCI_OID_MASK);
      p_old = nfc_cb.last_hdr;
      NCI_MSG_PRS_HDR0(p_old, old_mt, pbf, old_gid);
      old_oid = ((*p_old) & NCI_OID_MASK);
      /* make sure this is the RSP we are waiting for before updating the
       * command window */
      if ((old_gid != gid) || (old_oid != oid)) {
        LOG(ERROR) << StringPrintf(
            "nfc_ncif_process_event unexpected rsp: gid:0x%x, oid:0x%x", gid,
            oid);
        return true;
      }

      switch (gid) {
        case NCI_GID_CORE: /* 0000b NCI Core group */
          free = nci_proc_core_rsp(p_msg);
          break;
        case NCI_GID_RF_MANAGE: /* 0001b NCI Discovery group */
          nci_proc_rf_management_rsp(p_msg);
          break;
#if (NFC_NFCEE_INCLUDED == TRUE)
#if (NFC_RW_ONLY == FALSE)
        case NCI_GID_EE_MANAGE: /* 0x02 0010b NFCEE Discovery group */
          nci_proc_ee_management_rsp(p_msg);
          break;
#endif
#endif
        case NCI_GID_PROP: /* 1111b Proprietary */
          nci_proc_prop_rsp(p_msg);
          break;
        default:
          LOG(ERROR) << StringPrintf("NFC: Unknown gid:%d", gid);
          break;
      }

      nfc_ncif_update_window();
      break;

    case NCI_MT_NTF:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("NFC received ntf gid:%d", gid);
      switch (gid) {
        case NCI_GID_CORE: /* 0000b NCI Core group */
          nci_proc_core_ntf(p_msg);
          break;
        case NCI_GID_RF_MANAGE: /* 0001b NCI Discovery group */
          nci_proc_rf_management_ntf(p_msg);
          break;
#if (NFC_NFCEE_INCLUDED == TRUE)
#if (NFC_RW_ONLY == FALSE)
        case NCI_GID_EE_MANAGE: /* 0x02 0010b NFCEE Discovery group */
          nci_proc_ee_management_ntf(p_msg);
          break;
#endif
#endif
        case NCI_GID_PROP: /* 1111b Proprietary */
          nci_proc_prop_ntf(p_msg);
          break;
        default:
          LOG(ERROR) << StringPrintf("NFC: Unknown gid:%d", gid);
          break;
      }
      break;

    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("NFC received unknown mt:0x%x, gid:%d", mt, gid);
  }

  return (free);
}

/*******************************************************************************
**
** Function         nfc_ncif_rf_management_status
**
** Description      This function is called to report an event
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_rf_management_status(tNFC_DISCOVER_EVT event, uint8_t status) {
  tNFC_DISCOVER evt_data;
  if (nfc_cb.p_discv_cback) {
    evt_data.status = (tNFC_STATUS)status;
    (*nfc_cb.p_discv_cback)(event, &evt_data);
  }
}

/*******************************************************************************
**
** Function         nfc_ncif_set_config_status
**
** Description      This function is called to report NFC_SET_CONFIG_REVT
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_set_config_status(uint8_t* p, uint8_t len) {
  tNFC_RESPONSE evt_data;
  if (nfc_cb.p_resp_cback) {
    evt_data.set_config.num_param_id = 0;
    if (len == 0) {
      LOG(ERROR) << StringPrintf("Insufficient RSP length");
      evt_data.set_config.status = NFC_STATUS_SYNTAX_ERROR;
      (*nfc_cb.p_resp_cback)(NFC_SET_CONFIG_REVT, &evt_data);
      return;
    }
    evt_data.set_config.status = (tNFC_STATUS)*p++;
    if (evt_data.set_config.status != NFC_STATUS_OK && len > 1) {
      evt_data.set_config.num_param_id = *p++;
      if (evt_data.set_config.num_param_id > NFC_MAX_NUM_IDS) {
        android_errorWriteLog(0x534e4554, "114047681");
        LOG(ERROR) << StringPrintf("OOB write num_param_id %d",
                                   evt_data.set_config.num_param_id);
        evt_data.set_config.num_param_id = 0;
      } else if (evt_data.set_config.num_param_id <= len - 2) {
        STREAM_TO_ARRAY(evt_data.set_config.param_ids, p,
                        evt_data.set_config.num_param_id);
      } else {
        LOG(ERROR) << StringPrintf("Insufficient RSP length %d,num_param_id %d",
                                   len, evt_data.set_config.num_param_id);
        evt_data.set_config.num_param_id = 0;
      }
    }
    (*nfc_cb.p_resp_cback)(NFC_SET_CONFIG_REVT, &evt_data);
  }
}

/*******************************************************************************
**
** Function         nfc_ncif_event_status
**
** Description      This function is called to report an event
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_event_status(tNFC_RESPONSE_EVT event, uint8_t status) {
  tNFC_RESPONSE evt_data;
  if (event == NFC_NFCC_TIMEOUT_REVT && status == NFC_STATUS_HW_TIMEOUT) {
    uint32_t cmd_hdr = (nfc_cb.last_hdr[0] << 8) | nfc_cb.last_hdr[1];
#ifdef ANDROID
    android::util::stats_write(android::util::NFC_ERROR_OCCURRED,
                               (int32_t)NCI_TIMEOUT, (int32_t)cmd_hdr,
                               (int32_t)status);
#endif
  }
  if (nfc_cb.p_resp_cback) {
    evt_data.status = (tNFC_STATUS)status;
    (*nfc_cb.p_resp_cback)(event, &evt_data);
  }
}

/*******************************************************************************
**
** Function         nfc_ncif_error_status
**
** Description      This function is called to report an error event to data
**                  cback
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_error_status(uint8_t conn_id, uint8_t status) {
  tNFC_CONN_CB* p_cb = nfc_find_conn_cb_by_conn_id(conn_id);
  if (p_cb && p_cb->p_cback) {
    tNFC_CONN nfc_conn;
    nfc_conn.status = status;
    (*p_cb->p_cback)(conn_id, NFC_ERROR_CEVT, &nfc_conn);
  }
#ifdef ANDROID
  android::util::stats_write(android::util::NFC_ERROR_OCCURRED,
                             (int32_t)ERROR_NTF, (int32_t)0, (int32_t)status);
#endif
}

/*******************************************************************************
**
** Function         nfc_ncif_proc_rf_field_ntf
**
** Description      This function is called to process RF field notification
**
** Returns          void
**
*******************************************************************************/
#if (NFC_RW_ONLY == FALSE)
void nfc_ncif_proc_rf_field_ntf(uint8_t rf_status) {
  tNFC_RESPONSE evt_data;
  if (nfc_cb.p_resp_cback) {
    evt_data.status = (tNFC_STATUS)NFC_STATUS_OK;
    evt_data.rf_field.rf_field = rf_status;
    (*nfc_cb.p_resp_cback)(NFC_RF_FIELD_REVT, &evt_data);
  }
}
#endif

/*******************************************************************************
**
** Function         nfc_ncif_proc_credits
**
** Description      This function is called to process data credits
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_proc_credits(uint8_t* p, uint16_t plen) {
  uint8_t num, xx;
  tNFC_CONN_CB* p_cb;

  if (plen != 0) {
    num = *p++;
    plen--;
    if (num * 2 > plen) {
      android_errorWriteLog(0x534e4554, "118148142");
      return;
    }
    for (xx = 0; xx < num; xx++) {
      p_cb = nfc_find_conn_cb_by_conn_id(*p++);
      if (p_cb && p_cb->num_buff != NFC_CONN_NO_FC) {
        p_cb->num_buff += (*p);
#if (BT_USE_TRACES == TRUE)
        if (p_cb->num_buff > p_cb->init_credits) {
          if (nfc_cb.nfc_state == NFC_STATE_OPEN) {
            /* if this happens in activated state, it's very likely that our
             * NFCC has issues */
            /* However, credit may be returned after deactivation */
            LOG(ERROR) << StringPrintf("num_buff:0x%x, init_credits:0x%x",
                                       p_cb->num_buff, p_cb->init_credits);
          }
          p_cb->num_buff = p_cb->init_credits;
        }
#endif
        /* check if there's nay data in tx q to be sent */
        nfc_ncif_send_data(p_cb, nullptr);
      }
      p++;
    }
  }
}
/*******************************************************************************
**
** Function         nfc_ncif_decode_rf_params
**
** Description      This function is called to process the detected technology
**                  and mode and the associated parameters for DISCOVER_NTF and
**                  ACTIVATE_NTF
**
** Returns          void
**
*******************************************************************************/
uint8_t* nfc_ncif_decode_rf_params(tNFC_RF_TECH_PARAMS* p_param, uint8_t* p) {
  tNFC_RF_PA_PARAMS* p_pa;
  uint8_t len, *p_start, u8;
  tNFC_RF_PB_PARAMS* p_pb;
  tNFC_RF_LF_PARAMS* p_lf;
  tNFC_RF_PF_PARAMS* p_pf;
  tNFC_RF_PISO15693_PARAMS* p_i93;
  tNFC_RF_ACM_P_PARAMS* acm_p;
  uint8_t mpl_idx = 0;
  uint8_t gb_idx = 0, mpl;
  uint8_t plen;
  plen = len = *p++;
  p_start = p;
  memset(&p_param->param, 0, sizeof(tNFC_RF_TECH_PARAMU));

  if (NCI_DISCOVERY_TYPE_POLL_A == p_param->mode ||
      (NCI_DISCOVERY_TYPE_POLL_A_ACTIVE == p_param->mode &&
       NFC_GetNCIVersion() != NCI_VERSION_2_0)) {
    p_pa = &p_param->param.pa;
    /*
SENS_RES Response   2 bytes Defined in [DIGPROT] Available after Technology
Detection
NFCID1 length   1 byte  Length of NFCID1 Available after Collision Resolution
NFCID1  4, 7, or 10 bytes   Defined in [DIGPROT]Available after Collision
Resolution
SEL_RES Response    1 byte  Defined in [DIGPROT]Available after Collision
Resolution
HRx Length  1 Octets    Length of HRx Parameters collected from the response to
the T1T RID command.
HRx 0 or 2 Octets   If present, the first byte SHALL contain HR0 and the second
byte SHALL contain HR1 as defined in [DIGITAL].
    */
    if (plen < 3) {
      goto invalid_packet;
    }
    plen -= 3;
    STREAM_TO_ARRAY(p_pa->sens_res, p, 2);
    p_pa->nfcid1_len = *p++;
    if (p_pa->nfcid1_len > NCI_NFCID1_MAX_LEN)
      p_pa->nfcid1_len = NCI_NFCID1_MAX_LEN;

    if (plen < p_pa->nfcid1_len + 1) {
      goto invalid_packet;
    }
    plen -= (p_pa->nfcid1_len + 1);
    STREAM_TO_ARRAY(p_pa->nfcid1, p, p_pa->nfcid1_len);
    u8 = *p++;

    if (u8) {
      if (plen < 1) {
        goto invalid_packet;
      }
      plen--;
      p_pa->sel_rsp = *p++;
    }

    if (len ==
        (7 + p_pa->nfcid1_len + u8)) /* 2(sens_res) + 1(len) +
                                        p_pa->nfcid1_len + 1(len) + u8 + hr
                                        (1:len + 2) */
    {
      p_pa->hr_len = *p++;
      if (p_pa->hr_len == NCI_T1T_HR_LEN) {
        p_pa->hr[0] = *p++;
        p_pa->hr[1] = *p;
      }
    }
  } else if (NCI_DISCOVERY_TYPE_POLL_B == p_param->mode) {
    /*
SENSB_RES Response length (n)   1 byte  Length of SENSB_RES Response (Byte 2 -
Byte 12 or 13)Available after Technology Detection
SENSB_RES Response Byte 2 - Byte 12 or 13   11 or 12 bytes  Defined in [DIGPROT]
Available after Technology Detection
    */
    p_pb = &p_param->param.pb;

    if (plen < 1) {
      goto invalid_packet;
    }
    plen--;
    p_pb->sensb_res_len = *p++;
    if (p_pb->sensb_res_len > NCI_MAX_SENSB_RES_LEN)
      p_pb->sensb_res_len = NCI_MAX_SENSB_RES_LEN;

    if (plen < p_pb->sensb_res_len) {
      goto invalid_packet;
    }
    plen -= p_pb->sensb_res_len;
    STREAM_TO_ARRAY(p_pb->sensb_res, p, p_pb->sensb_res_len);
    memcpy(p_pb->nfcid0, p_pb->sensb_res, NFC_NFCID0_MAX_LEN);
  } else if (NCI_DISCOVERY_TYPE_POLL_F == p_param->mode ||
             (NCI_DISCOVERY_TYPE_POLL_F_ACTIVE == p_param->mode &&
              NFC_GetNCIVersion() != NCI_VERSION_2_0)) {
    /*
Bit Rate    1 byte  1   212 kbps/2   424 kbps/0 and 3 to 255  RFU
SENSF_RES Response length.(n) 1 byte  Length of SENSF_RES (Byte 2 - Byte 17 or
19).Available after Technology Detection
SENSF_RES Response Byte 2 - Byte 17 or 19  n bytes Defined in [DIGPROT]
Available after Technology Detection
    */
    p_pf = &p_param->param.pf;

    if (plen < 2) {
      goto invalid_packet;
    }
    plen -= 2;
    p_pf->bit_rate = *p++;
    p_pf->sensf_res_len = *p++;
    if (p_pf->sensf_res_len > NCI_MAX_SENSF_RES_LEN)
      p_pf->sensf_res_len = NCI_MAX_SENSF_RES_LEN;

    if (plen < p_pf->sensf_res_len) {
      goto invalid_packet;
    }
    plen -= p_pf->sensf_res_len;
    STREAM_TO_ARRAY(p_pf->sensf_res, p, p_pf->sensf_res_len);

    if (p_pf->sensf_res_len < NCI_MRTI_UPDATE_INDEX + 1) {
      goto invalid_packet;
    }
    memcpy(p_pf->nfcid2, p_pf->sensf_res, NCI_NFCID2_LEN);
    p_pf->mrti_check = p_pf->sensf_res[NCI_MRTI_CHECK_INDEX];
    p_pf->mrti_update = p_pf->sensf_res[NCI_MRTI_UPDATE_INDEX];
  } else if (NCI_DISCOVERY_TYPE_LISTEN_F == p_param->mode ||
             (NCI_DISCOVERY_TYPE_LISTEN_F_ACTIVE == p_param->mode &&
              NFC_GetNCIVersion() != NCI_VERSION_2_0)) {
    p_lf = &p_param->param.lf;

    if (plen < 1) {
      goto invalid_packet;
    }
    plen--;
    u8 = *p++;
    if (u8) {
      if (plen < NCI_NFCID2_LEN) {
        goto invalid_packet;
      }
      plen -= NCI_NFCID2_LEN;
      STREAM_TO_ARRAY(p_lf->nfcid2, p, NCI_NFCID2_LEN);
    }
  } else if (NCI_DISCOVERY_TYPE_POLL_V == p_param->mode) {
    p_i93 = &p_param->param.pi93;

    if (plen < 2) {
      goto invalid_packet;
    }
    plen -= 2;
    p_i93->flag = *p++;
    p_i93->dsfid = *p++;

    if (plen < NFC_ISO15693_UID_LEN) {
      goto invalid_packet;
    }
    plen -= NFC_ISO15693_UID_LEN;
    STREAM_TO_ARRAY(p_i93->uid, p, NFC_ISO15693_UID_LEN);
  } else if (NCI_DISCOVERY_TYPE_POLL_KOVIO == p_param->mode) {
    p_param->param.pk.uid_len = len;
    if (p_param->param.pk.uid_len > NFC_KOVIO_MAX_LEN) {
      LOG(ERROR) << StringPrintf("Kovio UID len:0x%x exceeds max(0x%x)",
                                 p_param->param.pk.uid_len, NFC_KOVIO_MAX_LEN);
      p_param->param.pk.uid_len = NFC_KOVIO_MAX_LEN;
    }
    STREAM_TO_ARRAY(p_param->param.pk.uid, p, p_param->param.pk.uid_len);
  } else if (NCI_DISCOVERY_TYPE_POLL_ACTIVE == p_param->mode) {
    acm_p = &p_param->param.acm_p;

    if (plen < 1) {
      goto invalid_packet;
    }
    plen--;
    acm_p->atr_res_len = *p++;
    if (acm_p->atr_res_len > 0) {
      if (acm_p->atr_res_len > NFC_MAX_ATS_LEN)
        acm_p->atr_res_len = NFC_MAX_ATS_LEN;

      if (plen < acm_p->atr_res_len) {
        goto invalid_packet;
      }
      plen -= acm_p->atr_res_len;
      STREAM_TO_ARRAY(acm_p->atr_res, p, acm_p->atr_res_len);
      /* ATR_RES
      Byte 3~12 Byte 13 Byte 14 Byte 15 Byte 16 Byte 17 Byte 18~18+n
      NFCID3T   DIDT    BST     BRT     TO      PPT     [GT0 ... GTn] */
      mpl_idx = 14;
      gb_idx = NCI_P_GEN_BYTE_INDEX;

      if (acm_p->atr_res_len < mpl_idx + 1) {
        goto invalid_packet;
      }
      acm_p->waiting_time = acm_p->atr_res[NCI_L_NFC_DEP_TO_INDEX] & 0x0F;
      mpl = ((acm_p->atr_res[mpl_idx]) >> 4) & 0x03;
      acm_p->max_payload_size = nfc_mpl_code_to_size[mpl];
      if (acm_p->atr_res_len > gb_idx) {
        acm_p->gen_bytes_len = acm_p->atr_res_len - gb_idx;
        if (acm_p->gen_bytes_len > NFC_MAX_GEN_BYTES_LEN)
          acm_p->gen_bytes_len = NFC_MAX_GEN_BYTES_LEN;
        memcpy(acm_p->gen_bytes, &acm_p->atr_res[gb_idx], acm_p->gen_bytes_len);
      }
    }
  } else if (NCI_DISCOVERY_TYPE_LISTEN_ACTIVE == p_param->mode) {
    acm_p = &p_param->param.acm_p;

    if (plen < 1) {
      goto invalid_packet;
    }
    plen--;
    acm_p->atr_res_len = *p++;
    if (acm_p->atr_res_len > 0) {
      if (acm_p->atr_res_len > NFC_MAX_ATS_LEN)
        acm_p->atr_res_len = NFC_MAX_ATS_LEN;

      if (plen < acm_p->atr_res_len) {
        goto invalid_packet;
      }
      plen -= acm_p->atr_res_len;
      STREAM_TO_ARRAY(acm_p->atr_res, p, acm_p->atr_res_len);
      /* ATR_REQ
      Byte 3~12 Byte 13 Byte 14 Byte 15 Byte 16 Byte 17~17+n
      NFCID3I   DIDI    BSI     BRI     PPI     [GI0 ... GIn] */
      mpl_idx = 13;
      gb_idx = NCI_L_GEN_BYTE_INDEX;

      if (acm_p->atr_res_len < mpl_idx + 1) {
        goto invalid_packet;
      }
      mpl = ((acm_p->atr_res[mpl_idx]) >> 4) & 0x03;
      acm_p->max_payload_size = nfc_mpl_code_to_size[mpl];
      if (acm_p->atr_res_len > gb_idx) {
        acm_p->gen_bytes_len = acm_p->atr_res_len - gb_idx;
        if (acm_p->gen_bytes_len > NFC_MAX_GEN_BYTES_LEN)
          acm_p->gen_bytes_len = NFC_MAX_GEN_BYTES_LEN;
        memcpy(acm_p->gen_bytes, &acm_p->atr_res[gb_idx], acm_p->gen_bytes_len);
      }
    }
  }
invalid_packet:
  return (p_start + len);
}

/*******************************************************************************
**
** Function         nfc_ncif_proc_discover_ntf
**
** Description      This function is called to process discover notification
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_proc_discover_ntf(uint8_t* p, uint16_t plen) {
  tNFC_DISCOVER evt_data;

  if (nfc_cb.p_discv_cback) {
    // validate packet length should be larger than (NCI header + rf_disc_id +
    // protocol + mode + length of rf parameters).
    if (plen < NCI_MSG_HDR_SIZE + 4) {
      evt_data.status = NCI_STATUS_FAILED;
      goto invalid_packet;
    }
    plen -= (NCI_MSG_HDR_SIZE + 4);
    p += NCI_MSG_HDR_SIZE;
    evt_data.status = NCI_STATUS_OK;
    evt_data.result.rf_disc_id = *p++;
    evt_data.result.protocol = *p++;

    /* fill in tNFC_RESULT_DEVT */
    evt_data.result.rf_tech_param.mode = *p++;

    // validate packet length should be larger than (rf_tech_param + ntf_type)
    if (plen < *p + 1) {
      evt_data.status = NCI_STATUS_FAILED;
      goto invalid_packet;
    }
    plen -= (*p + 1);
    p = nfc_ncif_decode_rf_params(&evt_data.result.rf_tech_param, p);

    evt_data.result.more = *p++;

  invalid_packet:
    (*nfc_cb.p_discv_cback)(NFC_RESULT_DEVT, &evt_data);
  }
}

/*******************************************************************************
**
** Function         nfc_ncif_proc_isodep_nak_presence_check_status
**
** Description      This function is called to handle response and notification
**                  for presence check nak command
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_proc_isodep_nak_presence_check_status(uint8_t status,
                                                    bool is_ntf) {
  rw_t4t_handle_isodep_nak_rsp(status, is_ntf);
}
/*******************************************************************************
**
** Function         nfc_ncif_proc_activate
**
** Description      This function is called to process de-activate
**                  response and notification
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_proc_activate(uint8_t* p, uint8_t len) {
  tNFC_DISCOVER evt_data;
  tNFC_INTF_PARAMS* p_intf = &evt_data.activate.intf_param;
  tNFC_INTF_PA_ISO_DEP* p_pa_iso;
  tNFC_INTF_LB_ISO_DEP* p_lb_iso;
  tNFC_INTF_PB_ISO_DEP* p_pb_iso;
#if (NFC_RW_ONLY == FALSE)
  tNFC_INTF_PA_NFC_DEP* p_pa_nfc;
  int mpl_idx = 0;
  uint8_t gb_idx = 0, mpl;
#endif
  uint8_t t0;
  tNCI_DISCOVERY_TYPE mode;
  tNFC_CONN_CB* p_cb = &nfc_cb.conn_cb[NFC_RF_CONN_ID];
  uint8_t *pp, len_act;
  uint8_t buff_size, num_buff;
  tNFC_RF_PA_PARAMS* p_pa;
  uint8_t plen = len, pplen = 0;

  nfc_set_state(NFC_STATE_OPEN);

  memset(p_intf, 0, sizeof(tNFC_INTF_PARAMS));
  // validate packet length should be larger than (rf_disc_id + type +
  // protocol + mode + buff_size + num_buff + length of rf parameters).
  if (plen < 7) {
    evt_data.status = NCI_STATUS_FAILED;
    goto invalid_packet;
  }
  plen -= 7;

  evt_data.activate.rf_disc_id = *p++;
  p_intf->type = *p++;
  evt_data.activate.protocol = *p++;

  if (evt_data.activate.protocol == NCI_PROTOCOL_18092_ACTIVE)
    evt_data.activate.protocol = NCI_PROTOCOL_NFC_DEP;

  evt_data.activate.rf_tech_param.mode = *p++;
  buff_size = *p++;
  num_buff = *p++;
  /* fill in tNFC_activate_DEVT */
  // validate remaining packet length should be larger than (rf_tech_param +
  // data_mode + tx_bitrate + rx_bitrte + len_act).
  if (plen < *p + 4) {
    evt_data.status = NCI_STATUS_FAILED;
    goto invalid_packet;
  }
  plen -= (*p + 4);
  p = nfc_ncif_decode_rf_params(&evt_data.activate.rf_tech_param, p);

  evt_data.activate.data_mode = *p++;
  evt_data.activate.tx_bitrate = *p++;
  evt_data.activate.rx_bitrate = *p++;
  mode = evt_data.activate.rf_tech_param.mode;
  len_act = *p++;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nfc_ncif_proc_activate:%d %d, mode:0x%02x", len, len_act, mode);
  /* just in case the interface reports activation parameters not defined in the
   * NCI spec */
  p_intf->intf_param.frame.param_len = len_act;
  if (p_intf->intf_param.frame.param_len > NFC_MAX_RAW_PARAMS)
    p_intf->intf_param.frame.param_len = NFC_MAX_RAW_PARAMS;
  pp = p;

  if (plen < p_intf->intf_param.frame.param_len) {
    evt_data.status = NCI_STATUS_FAILED;
    goto invalid_packet;
  }
  STREAM_TO_ARRAY(p_intf->intf_param.frame.param, pp,
                  p_intf->intf_param.frame.param_len);
  if (evt_data.activate.intf_param.type == NCI_INTERFACE_ISO_DEP) {
    /* Make max payload of NCI aligned to max payload of ISO-DEP for better
     * performance */
    if (buff_size > NCI_ISO_DEP_MAX_INFO) buff_size = NCI_ISO_DEP_MAX_INFO;

    switch (mode) {
      case NCI_DISCOVERY_TYPE_POLL_A:
        p_pa_iso = &p_intf->intf_param.pa_iso;

        if (plen < 1) {
          evt_data.status = NCI_STATUS_FAILED;
          goto invalid_packet;
        }
        plen--;
        p_pa_iso->ats_res_len = *p++;

        if (p_pa_iso->ats_res_len == 0) break;

        if (p_pa_iso->ats_res_len > NFC_MAX_ATS_LEN)
          p_pa_iso->ats_res_len = NFC_MAX_ATS_LEN;

        if (plen < p_pa_iso->ats_res_len) {
          evt_data.status = NCI_STATUS_FAILED;
          goto invalid_packet;
        }
        plen -= p_pa_iso->ats_res_len;
        STREAM_TO_ARRAY(p_pa_iso->ats_res, p, p_pa_iso->ats_res_len);

        pplen = p_pa_iso->ats_res_len;
        pp = &p_pa_iso->ats_res[NCI_ATS_T0_INDEX];
        t0 = p_pa_iso->ats_res[NCI_ATS_T0_INDEX];
        pp++;                           /* T0 */
        pplen--;
        if (t0 & NCI_ATS_TA_MASK) {
          if (pplen < 1) {
            evt_data.status = NCI_STATUS_FAILED;
            goto invalid_packet;
          }
          pplen--;
          pp++; /* TA */
        }
        if (t0 & NCI_ATS_TB_MASK) {
          /* FWI (Frame Waiting time Integer) & SPGI (Start-up Frame Guard time
           * Integer) */
          if (pplen < 1) {
            evt_data.status = NCI_STATUS_FAILED;
            goto invalid_packet;
          }
          pplen--;
          p_pa_iso->fwi = (((*pp) >> 4) & 0x0F);
          p_pa_iso->sfgi = ((*pp) & 0x0F);
          pp++; /* TB */
        }
        if (t0 & NCI_ATS_TC_MASK) {
          if (pplen < 1) {
            evt_data.status = NCI_STATUS_FAILED;
            goto invalid_packet;
          }
          pplen--;
          p_pa_iso->nad_used = ((*pp) & 0x01);
          pp++; /* TC */
        }
        p_pa_iso->his_byte_len =
            (uint8_t)(p_pa_iso->ats_res_len - (pp - p_pa_iso->ats_res));
        if (p_pa_iso->his_byte_len > NFC_MAX_HIS_BYTES_LEN)
          p_pa_iso->his_byte_len = NFC_MAX_HIS_BYTES_LEN;
        if (pplen < p_pa_iso->his_byte_len) {
          evt_data.status = NCI_STATUS_FAILED;
          goto invalid_packet;
        }
        memcpy(p_pa_iso->his_byte, pp, p_pa_iso->his_byte_len);
        break;

      case NCI_DISCOVERY_TYPE_LISTEN_A:
        if (plen < 1) {
          evt_data.status = NCI_STATUS_FAILED;
          goto invalid_packet;
        }
        plen--;
        p_intf->intf_param.la_iso.rats = *p++;
        gettimeofday(&timer_start, nullptr);
        break;

      case NCI_DISCOVERY_TYPE_POLL_B:
        /* ATTRIB RSP
        Byte 1   Byte 2 ~ 2+n-1
        MBLI/DID Higher layer - Response
        */
        p_pb_iso = &p_intf->intf_param.pb_iso;

        if (plen < 1) {
          evt_data.status = NCI_STATUS_FAILED;
          goto invalid_packet;
        }
        plen--;
        p_pb_iso->attrib_res_len = *p++;

        if (p_pb_iso->attrib_res_len == 0) break;

        if (p_pb_iso->attrib_res_len > NFC_MAX_ATTRIB_LEN)
          p_pb_iso->attrib_res_len = NFC_MAX_ATTRIB_LEN;

        if (plen < p_pb_iso->attrib_res_len) {
          evt_data.status = NCI_STATUS_FAILED;
          goto invalid_packet;
        }
        plen -= p_pb_iso->attrib_res_len;
        STREAM_TO_ARRAY(p_pb_iso->attrib_res, p, p_pb_iso->attrib_res_len);
        p_pb_iso->mbli = (p_pb_iso->attrib_res[0]) >> 4;
        if (p_pb_iso->attrib_res_len > NFC_PB_ATTRIB_REQ_FIXED_BYTES) {
          p_pb_iso->hi_info_len =
              p_pb_iso->attrib_res_len - NFC_PB_ATTRIB_REQ_FIXED_BYTES;
          if (p_pb_iso->hi_info_len > NFC_MAX_GEN_BYTES_LEN)
            p_pb_iso->hi_info_len = NFC_MAX_GEN_BYTES_LEN;
          memcpy(p_pb_iso->hi_info,
                 &p_pb_iso->attrib_res[NFC_PB_ATTRIB_REQ_FIXED_BYTES],
                 p_pb_iso->hi_info_len);
        }
        break;

      case NCI_DISCOVERY_TYPE_LISTEN_B:
        /* ATTRIB CMD
        Byte 2~5 Byte 6  Byte 7  Byte 8  Byte 9  Byte 10 ~ 10+k-1
        NFCID0   Param 1 Param 2 Param 3 Param 4 Higher layer - INF
        */
        p_lb_iso = &p_intf->intf_param.lb_iso;

        if (plen < 1) {
          evt_data.status = NCI_STATUS_FAILED;
          goto invalid_packet;
        }
        plen--;
        p_lb_iso->attrib_req_len = *p++;

        if (p_lb_iso->attrib_req_len == 0) break;

        if (p_lb_iso->attrib_req_len > NFC_MAX_ATTRIB_LEN)
          p_lb_iso->attrib_req_len = NFC_MAX_ATTRIB_LEN;

        if (plen < p_lb_iso->attrib_req_len) {
          evt_data.status = NCI_STATUS_FAILED;
          goto invalid_packet;
        }
        plen -= p_lb_iso->attrib_req_len;
        STREAM_TO_ARRAY(p_lb_iso->attrib_req, p, p_lb_iso->attrib_req_len);

        if (p_lb_iso->attrib_req_len < NFC_NFCID0_MAX_LEN) {
          evt_data.status = NCI_STATUS_FAILED;
          goto invalid_packet;
        }
        memcpy(p_lb_iso->nfcid0, p_lb_iso->attrib_req, NFC_NFCID0_MAX_LEN);
        if (p_lb_iso->attrib_req_len > NFC_LB_ATTRIB_REQ_FIXED_BYTES) {
          p_lb_iso->hi_info_len =
              p_lb_iso->attrib_req_len - NFC_LB_ATTRIB_REQ_FIXED_BYTES;
          if (p_lb_iso->hi_info_len > NFC_MAX_GEN_BYTES_LEN)
            p_lb_iso->hi_info_len = NFC_MAX_GEN_BYTES_LEN;
          memcpy(p_lb_iso->hi_info,
                 &p_lb_iso->attrib_req[NFC_LB_ATTRIB_REQ_FIXED_BYTES],
                 p_lb_iso->hi_info_len);
        }
        gettimeofday(&timer_start, nullptr);
        break;
    }

  }
#if (NFC_RW_ONLY == FALSE)
  else if (evt_data.activate.intf_param.type == NCI_INTERFACE_NFC_DEP) {
    /* Make max payload of NCI aligned to max payload of NFC-DEP for better
     * performance */
    if (buff_size > NCI_NFC_DEP_MAX_DATA) buff_size = NCI_NFC_DEP_MAX_DATA;

    p_pa_nfc = &p_intf->intf_param.pa_nfc;

    /* Active mode, no info in activation parameters (NCI 2.0) */
    if ((NFC_GetNCIVersion() == NCI_VERSION_2_0) &&
        ((mode == NCI_DISCOVERY_TYPE_POLL_ACTIVE) ||
         (mode == NCI_DISCOVERY_TYPE_LISTEN_ACTIVE))) {
        p_pa_nfc->atr_res_len =
                  evt_data.activate.rf_tech_param.param.acm_p.atr_res_len;
    } else {
      if (plen < 1) {
        evt_data.status = NCI_STATUS_FAILED;
        goto invalid_packet;
      }
      plen--;
      p_pa_nfc->atr_res_len = *p++;
    }

    if (p_pa_nfc->atr_res_len > 0) {
      if (p_pa_nfc->atr_res_len > NFC_MAX_ATS_LEN)
        p_pa_nfc->atr_res_len = NFC_MAX_ATS_LEN;

      if ((NFC_GetNCIVersion() == NCI_VERSION_2_0) &&
          ((mode == NCI_DISCOVERY_TYPE_POLL_ACTIVE) ||
           (mode == NCI_DISCOVERY_TYPE_LISTEN_ACTIVE))) {
         /* NCI 2.0 : ATR_RES is included in RF technology parameters in active mode */
         memcpy(p_pa_nfc->atr_res,
                evt_data.activate.rf_tech_param.param.acm_p.atr_res,
                p_pa_nfc->atr_res_len);
      } else {
        if (plen < p_pa_nfc->atr_res_len) {
          evt_data.status = NCI_STATUS_FAILED;
          goto invalid_packet;
        }
        plen -= p_pa_nfc->atr_res_len;
        STREAM_TO_ARRAY(p_pa_nfc->atr_res, p, p_pa_nfc->atr_res_len);
      }

      if ((mode == NCI_DISCOVERY_TYPE_POLL_A) ||
          (mode == NCI_DISCOVERY_TYPE_POLL_F) ||
          ((mode == NCI_DISCOVERY_TYPE_POLL_A_ACTIVE ||
            mode == NCI_DISCOVERY_TYPE_POLL_F_ACTIVE) &&
           NFC_GetNCIVersion() != NCI_VERSION_2_0) ||
          (NFC_GetNCIVersion() == NCI_VERSION_2_0 &&
           mode == NCI_DISCOVERY_TYPE_POLL_ACTIVE)) {
        /* ATR_RES
        Byte 3~12 Byte 13 Byte 14 Byte 15 Byte 16 Byte 17 Byte 18~18+n
        NFCID3T   DIDT    BST     BRT     TO      PPT     [GT0 ... GTn] */
        mpl_idx = 14;
        gb_idx = NCI_P_GEN_BYTE_INDEX;

        if (p_pa_nfc->atr_res_len < NCI_L_NFC_DEP_TO_INDEX + 1) {
          evt_data.status = NCI_STATUS_FAILED;
          goto invalid_packet;
        }
        p_pa_nfc->waiting_time =
            p_pa_nfc->atr_res[NCI_L_NFC_DEP_TO_INDEX] & 0x0F;
      } else if ((mode == NCI_DISCOVERY_TYPE_LISTEN_A) ||
                 (mode == NCI_DISCOVERY_TYPE_LISTEN_F) ||
                 (NFC_GetNCIVersion() != NCI_VERSION_2_0 &&
                  (mode == NCI_DISCOVERY_TYPE_LISTEN_A_ACTIVE ||
                   mode == NCI_DISCOVERY_TYPE_LISTEN_F_ACTIVE)) ||
                 (NFC_GetNCIVersion() == NCI_VERSION_2_0 &&
                  mode == NCI_DISCOVERY_TYPE_LISTEN_ACTIVE)) {
        /* ATR_REQ
        Byte 3~12 Byte 13 Byte 14 Byte 15 Byte 16 Byte 17~17+n
        NFCID3I   DIDI    BSI     BRI     PPI     [GI0 ... GIn] */
        mpl_idx = 13;
        gb_idx = NCI_L_GEN_BYTE_INDEX;
      }

      if (p_pa_nfc->atr_res_len < mpl_idx + 1) {
        evt_data.status = NCI_STATUS_FAILED;
        goto invalid_packet;
      }
      mpl = ((p_pa_nfc->atr_res[mpl_idx]) >> 4) & 0x03;
      p_pa_nfc->max_payload_size = nfc_mpl_code_to_size[mpl];
      if (p_pa_nfc->atr_res_len > gb_idx) {
        p_pa_nfc->gen_bytes_len = p_pa_nfc->atr_res_len - gb_idx;
        if (p_pa_nfc->gen_bytes_len > NFC_MAX_GEN_BYTES_LEN)
          p_pa_nfc->gen_bytes_len = NFC_MAX_GEN_BYTES_LEN;
        memcpy(p_pa_nfc->gen_bytes, &p_pa_nfc->atr_res[gb_idx],
               p_pa_nfc->gen_bytes_len);
      }
    }
  }
#endif
  else if ((evt_data.activate.intf_param.type == NCI_INTERFACE_FRAME) &&
           (evt_data.activate.protocol == NCI_PROTOCOL_T1T)) {
    p_pa = &evt_data.activate.rf_tech_param.param.pa;
    if ((len_act == NCI_T1T_HR_LEN) && (p_pa->hr_len == 0)) {
      p_pa->hr_len = NCI_T1T_HR_LEN;

      if (plen < 2) {
        evt_data.status = NCI_STATUS_FAILED;
        goto invalid_packet;
      }
      plen -= 2;
      p_pa->hr[0] = *p++;
      p_pa->hr[1] = *p++;
    }
  }

  p_cb->act_protocol = evt_data.activate.protocol;
  p_cb->act_interface = evt_data.activate.intf_param.type;
  p_cb->buff_size = buff_size;
  p_cb->num_buff = num_buff;
  p_cb->init_credits = num_buff;

invalid_packet:
  if (nfc_cb.p_discv_cback) {
    (*nfc_cb.p_discv_cback)(NFC_ACTIVATE_DEVT, &evt_data);
  }
}

/*******************************************************************************
**
** Function         nfc_ncif_proc_deactivate
**
** Description      This function is called to process de-activate
**                  response and notification
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_proc_deactivate(uint8_t status, uint8_t deact_type, bool is_ntf) {
  tNFC_DISCOVER evt_data;
  tNFC_CONN_CB* p_cb = &nfc_cb.conn_cb[NFC_RF_CONN_ID];
  void* p_data;

  nfc_set_state(NFC_STATE_IDLE);
  evt_data.deactivate.status = status;
  evt_data.deactivate.type = deact_type;
  evt_data.deactivate.is_ntf = is_ntf;
  if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
    evt_data.deactivate.reason = nfc_cb.deact_reason;
  }

  while ((p_data = GKI_dequeue(&p_cb->rx_q)) != nullptr) {
    GKI_freebuf(p_data);
  }

  while ((p_data = GKI_dequeue(&p_cb->tx_q)) != nullptr) {
    GKI_freebuf(p_data);
  }

  if (p_cb->p_cback) {
    tNFC_CONN nfc_conn;
    nfc_conn.deactivate = evt_data.deactivate;
    (*p_cb->p_cback)(NFC_RF_CONN_ID, NFC_DEACTIVATE_CEVT, &nfc_conn);
  }

  if (nfc_cb.p_discv_cback) {
    (*nfc_cb.p_discv_cback)(NFC_DEACTIVATE_DEVT, &evt_data);
  }

  // clear previous stored tick count if not comsumed
  if (timer_start.tv_sec != 0 || timer_start.tv_usec != 0) {
    memset(&timer_start, 0, sizeof(timer_start));
  }
}
/*******************************************************************************
**
** Function         nfc_ncif_proc_ee_action
**
** Description      This function is called to process NFCEE ACTION NTF
**
** Returns          void
**
*******************************************************************************/
#if (NFC_NFCEE_INCLUDED == TRUE && NFC_RW_ONLY == FALSE)
void nfc_ncif_proc_ee_action(uint8_t* p, uint16_t plen) {
  tNFC_EE_ACTION_REVT evt_data;
  tNFC_RESPONSE_CBACK* p_cback = nfc_cb.p_resp_cback;
  uint8_t data_len, ulen, tag, *p_data;
  uint8_t max_len;

  if (p_cback) {
    memset(&evt_data.act_data, 0, sizeof(tNFC_ACTION_DATA));
    evt_data.status = NFC_STATUS_OK;
    evt_data.nfcee_id = *p++;
    evt_data.act_data.trigger = *p++;
    data_len = *p++;
    if (plen >= 3) plen -= 3;
    if (data_len > plen) data_len = (uint8_t)plen;

    switch (evt_data.act_data.trigger) {
      case NCI_EE_TRIG_7816_SELECT:
        if (data_len > NFC_MAX_AID_LEN) data_len = NFC_MAX_AID_LEN;
        evt_data.act_data.param.aid.len_aid = data_len;
        STREAM_TO_ARRAY(evt_data.act_data.param.aid.aid, p, data_len);
        break;
      case NCI_EE_TRIG_RF_PROTOCOL:
        evt_data.act_data.param.protocol = *p++;
        break;
      case NCI_EE_TRIG_RF_TECHNOLOGY:
        evt_data.act_data.param.technology = *p++;
        break;
      case NCI_EE_TRIG_APP_INIT:
        while (data_len > NFC_TL_SIZE) {
          data_len -= NFC_TL_SIZE;
          tag = *p++;
          ulen = *p++;
          if (ulen > data_len) ulen = data_len;
          p_data = nullptr;
          max_len = ulen;
          switch (tag) {
            case NCI_EE_ACT_TAG_AID: /* AID                 */
              if (max_len > NFC_MAX_AID_LEN) max_len = NFC_MAX_AID_LEN;
              evt_data.act_data.param.app_init.len_aid = max_len;
              p_data = evt_data.act_data.param.app_init.aid;
              break;
            case NCI_EE_ACT_TAG_DATA: /* hex data for app    */
              if (max_len > NFC_MAX_APP_DATA_LEN)
                max_len = NFC_MAX_APP_DATA_LEN;
              evt_data.act_data.param.app_init.len_data = max_len;
              p_data = evt_data.act_data.param.app_init.data;
              break;
          }
          if (p_data) {
            STREAM_TO_ARRAY(p_data, p, max_len);
          }
          data_len -= ulen;
        }
        break;
    }
    tNFC_RESPONSE nfc_response;
    nfc_response.ee_action = evt_data;
    (*p_cback)(NFC_EE_ACTION_REVT, &nfc_response);
  }
}

/*******************************************************************************
**
** Function         nfc_ncif_proc_ee_discover_req
**
** Description      This function is called to process NFCEE DISCOVER REQ NTF
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_proc_ee_discover_req(uint8_t* p, uint16_t plen) {
  tNFC_RESPONSE_CBACK* p_cback = nfc_cb.p_resp_cback;
  tNFC_EE_DISCOVER_REQ_REVT ee_disc_req;
  tNFC_EE_DISCOVER_INFO* p_info;
  uint8_t u8;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfc_ncif_proc_ee_discover_req %d len:%d", *p, plen);

  if (*p > NFC_MAX_EE_DISC_ENTRIES) {
    android_errorWriteLog(0x534e4554, "122361874");
    LOG(ERROR) << __func__ << "Exceed NFC_MAX_EE_DISC_ENTRIES";
    return;
  }

  if (p_cback) {
    u8 = *p;
    ee_disc_req.status = NFC_STATUS_OK;
    ee_disc_req.num_info = *p++;
    p_info = ee_disc_req.info;
    if (plen) plen--;
    while ((u8 > 0) && (plen >= NFC_EE_DISCOVER_ENTRY_LEN)) {
      p_info->op = *p++;                  /* T */
      if (*p != NFC_EE_DISCOVER_INFO_LEN) /* L */
      {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("bad entry len:%d", *p);
        return;
      }
      p++;
      /* V */
      p_info->nfcee_id = *p++;
      p_info->tech_n_mode = *p++;
      p_info->protocol = *p++;
      u8--;
      plen -= NFC_EE_DISCOVER_ENTRY_LEN;
      p_info++;
    }
    tNFC_RESPONSE nfc_response;
    nfc_response.ee_discover_req = ee_disc_req;
    (*p_cback)(NFC_EE_DISCOVER_REQ_REVT, &nfc_response);
  }
}

/*******************************************************************************
**
** Function         nfc_ncif_proc_get_routing
**
** Description      This function is called to process get routing notification
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_proc_get_routing(uint8_t* p,
                               __attribute__((unused)) uint8_t len) {
  tNFC_GET_ROUTING_REVT evt_data;
  uint8_t more, num_entries, xx, yy, *pn, tl;
  tNFC_STATUS status = NFC_STATUS_CONTINUE;

  if (nfc_cb.p_resp_cback) {
    more = *p++;
    num_entries = *p++;
    for (xx = 0; xx < num_entries; xx++) {
      if ((more == false) && (xx == (num_entries - 1))) status = NFC_STATUS_OK;
      evt_data.status = (tNFC_STATUS)status;
      evt_data.nfcee_id = *p++;
      evt_data.num_tlvs = *p++;
      evt_data.tlv_size = 0;
      pn = evt_data.param_tlvs;
      for (yy = 0; yy < evt_data.num_tlvs; yy++) {
        tl = *(p + 1);
        tl += NFC_TL_SIZE;
        evt_data.tlv_size += tl;
        if (evt_data.tlv_size > NFC_MAX_EE_TLV_SIZE) {
          android_errorWriteLog(0x534e4554, "117554809");
          LOG(ERROR) << __func__ << "Invalid data format";
          return;
        }
        STREAM_TO_ARRAY(pn, p, tl);
        pn += tl;
      }
      tNFC_RESPONSE nfc_response;
      nfc_response.get_routing = evt_data;
      (*nfc_cb.p_resp_cback)(NFC_GET_ROUTING_REVT, &nfc_response);
    }
  }
}
#endif

/*******************************************************************************
**
** Function         nfc_ncif_proc_conn_create_rsp
**
** Description      This function is called to process connection create
**                  response
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_proc_conn_create_rsp(uint8_t* p,
                                   __attribute__((unused)) uint16_t plen,
                                   uint8_t dest_type) {
  tNFC_CONN_CB* p_cb;
  tNFC_STATUS status;
  tNFC_CONN_CBACK* p_cback;
  tNFC_CONN evt_data;
  uint8_t conn_id;

  /* find the pending connection control block */
  p_cb = nfc_find_conn_cb_by_conn_id(NFC_PEND_CONN_ID);
  if (p_cb) {
    p += NCI_MSG_HDR_SIZE;
    status = *p++;
    p_cb->buff_size = *p++;
    p_cb->num_buff = p_cb->init_credits = *p++;
    conn_id = *p++;
    if (conn_id > NFC_MAX_CONN_ID) {
      status = NCI_STATUS_FAILED;
      conn_id = NFC_ILLEGAL_CONN_ID;
    }
    evt_data.conn_create.status = status;
    evt_data.conn_create.dest_type = dest_type;
    evt_data.conn_create.id = p_cb->id;
    evt_data.conn_create.buff_size = p_cb->buff_size;
    evt_data.conn_create.num_buffs = p_cb->num_buff;
    p_cback = p_cb->p_cback;
    if (status == NCI_STATUS_OK) {
      nfc_set_conn_id(p_cb, conn_id);
    } else {
      nfc_free_conn_cb(p_cb);
    }

    if (p_cback) (*p_cback)(conn_id, NFC_CONN_CREATE_CEVT, &evt_data);
  }
}

/*******************************************************************************
**
** Function         nfc_ncif_report_conn_close_evt
**
** Description      This function is called to report connection close event
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_report_conn_close_evt(uint8_t conn_id, tNFC_STATUS status) {
  tNFC_CONN evt_data;
  tNFC_CONN_CBACK* p_cback;
  tNFC_CONN_CB* p_cb;

  p_cb = nfc_find_conn_cb_by_conn_id(conn_id);
  if (p_cb) {
    p_cback = p_cb->p_cback;
    nfc_free_conn_cb(p_cb);
    evt_data.status = status;
    if (p_cback) (*p_cback)(conn_id, NFC_CONN_CLOSE_CEVT, &evt_data);
  }
}

/*******************************************************************************
**
** Function         nfc_ncif_proc_reset_rsp
**
** Description      This function is called to process reset
**                  response/notification
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_proc_reset_rsp(uint8_t* p, bool is_ntf) {
  uint8_t* p_len = p - 1;
  uint8_t status = NCI_STATUS_FAILED;
  uint8_t wait_for_ntf = FALSE;

  status = *p_len > 0 ? *p++ : NCI_STATUS_FAILED;
  if (*p_len > 2 && is_ntf) {
    LOG(ERROR) << StringPrintf("reset notification!!:0x%x ", status);
    /* clean up, if the state is OPEN
     * FW does not report reset ntf right now */
    if (status == NCI2_0_RESET_TRIGGER_TYPE_CORE_RESET_CMD_RECEIVED ||
        status == NCI2_0_RESET_TRIGGER_TYPE_POWERED_ON) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "CORE_RESET_NTF Received status nfc_state : 0x%x : 0x%x", status,
          nfc_cb.nfc_state);
      nfc_stop_timer(&nfc_cb.nci_wait_rsp_timer);
      p++;
      STREAM_TO_UINT8(nfc_cb.nci_version, p);
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf(" CORE_RESET_NTF nci_version%x", nfc_cb.nci_version);
      status = NCI_STATUS_OK;
    } else {
      /* CORE_RESET_NTF received error case , trigger recovery*/
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "CORE_RESET_NTF Received status nfc_state : 0x%x : 0x%x", status,
          nfc_cb.nfc_state);
      nfc_ncif_cmd_timeout();
      status = NCI_STATUS_FAILED;
    }
    if (nfc_cb.nfc_state == NFC_STATE_OPEN) {
      /*if any conn_cb is connected, close it.
        if any pending outgoing packets are dropped.*/
      nfc_reset_all_conn_cbs();
    }
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("CORE_RESET_RSP len :0x%x ", *p_len);
    if ((*p_len) == NCI_CORE_RESET_RSP_LEN(NCI_VERSION_2_0)) {
      wait_for_ntf = TRUE;
    } else if ((*p_len) == NCI_CORE_RESET_RSP_LEN(NCI_VERSION_1_0)) {
      nfc_cb.nci_version = NCI_VERSION_1_0;
    }
  }

  if (nfc_cb.flags & (NFC_FL_RESTARTING | NFC_FL_POWER_CYCLE_NFCC)) {
    nfc_reset_all_conn_cbs();
  }

  if (status == NCI_STATUS_OK) {
    if (wait_for_ntf == TRUE) {
      /* reset version reported by NFCC is NCI2.0 , start a timer for 2000ms to
       * wait for NTF*/
      nfc_start_timer(&nfc_cb.nci_wait_rsp_timer,
                      (uint16_t)(NFC_TTYPE_NCI_WAIT_RSP),
                      nfc_cb.nci_wait_rsp_tout);
    } else {
      if (nfc_cb.nci_version == NCI_VERSION_1_0)
        nci_snd_core_init(NCI_VERSION_1_0);
      else
        nci_snd_core_init(NCI_VERSION_2_0);
    }
  } else {
    LOG(ERROR) << StringPrintf("Failed to reset NFCC");
    nfc_enabled(status, nullptr);
  }
}

/*******************************************************************************
**
** Function         nfc_ncif_proc_init_rsp
**
** Description      This function is called to process init response
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_proc_init_rsp(NFC_HDR* p_msg) {
  uint8_t *p, status;
  tNFC_CONN_CB* p_cb = &nfc_cb.conn_cb[NFC_RF_CONN_ID];

  p = (uint8_t*)(p_msg + 1) + p_msg->offset;

  /* handle init params in nfc_enabled */
  status = *(p + NCI_MSG_HDR_SIZE);
  if (status == NCI_STATUS_OK) {
    if (nfc_cb.nci_version == NCI_VERSION_UNKNOWN) {
      nci_snd_core_reset(NCI_RESET_TYPE_RESET_CFG);
    } else {
      p_cb->id = NFC_RF_CONN_ID;
      // check scbr bit as per NCI 2.0 spec
      nfc_cb.isScbrSupported = p[5] & NCI_SCBR_MASK;
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("scbr support: 0x%x", nfc_cb.isScbrSupported);
      p_cb->act_protocol = NCI_PROTOCOL_UNKNOWN;

      nfc_set_state(NFC_STATE_W4_POST_INIT_CPLT);

      nfc_cb.p_nci_init_rsp = p_msg;
      nfc_cb.p_hal->core_initialized(p_msg->len, p);
    }
  } else {
    if (nfc_cb.nci_version == NCI_VERSION_UNKNOWN) {
      nfc_cb.nci_version = NCI_VERSION_1_0;
      nci_snd_core_reset(NCI_RESET_TYPE_RESET_CFG);
    } else {
      nfc_enabled(status, nullptr);
      GKI_freebuf(p_msg);
    }
  }
}

/*******************************************************************************
**
** Function         nfc_ncif_proc_get_config_rsp
**
** Description      This function is called to process get config response
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_proc_get_config_rsp(NFC_HDR* p_evt) {
  uint8_t* p;
  tNFC_RESPONSE_CBACK* p_cback = nfc_cb.p_resp_cback;
  tNFC_RESPONSE evt_data;

  p_evt->offset += NCI_MSG_HDR_SIZE;
  p_evt->len -= NCI_MSG_HDR_SIZE;
  if (p_cback) {
    p = (uint8_t*)(p_evt + 1) + p_evt->offset;
    evt_data.get_config.status = *p++;
    evt_data.get_config.tlv_size = p_evt->len;
    evt_data.get_config.p_param_tlvs = p;
    (*p_cback)(NFC_GET_CONFIG_REVT, &evt_data);
  }
}

/*******************************************************************************
**
** Function         nfc_ncif_proc_t3t_polling_ntf
**
** Description      Handle NCI_MSG_RF_T3T_POLLING NTF
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_proc_t3t_polling_ntf(uint8_t* p, uint16_t plen) {
  uint8_t status;
  uint8_t num_responses;

  if (plen < NFC_TL_SIZE) {
    return;
  }

  /* Pass result to RW_T3T for processing */
  STREAM_TO_UINT8(status, p);
  STREAM_TO_UINT8(num_responses, p);
  plen -= NFC_TL_SIZE;
  rw_t3t_handle_nci_poll_ntf(status, num_responses, (uint8_t)plen, p);
}

/*******************************************************************************
**
** Function         nfc_data_event
**
** Description      Report Data event on the given connection control block
**
** Returns          void
**
*******************************************************************************/
void nfc_data_event(tNFC_CONN_CB* p_cb) {
  NFC_HDR* p_evt;
  tNFC_DATA_CEVT data_cevt;
  uint8_t* p;

  if (p_cb->p_cback) {
    while ((p_evt = (NFC_HDR*)GKI_getfirst(&p_cb->rx_q)) != nullptr) {
      if (p_evt->layer_specific & NFC_RAS_FRAGMENTED) {
        /* Not the last fragment */
        if (!(p_evt->layer_specific & NFC_RAS_TOO_BIG)) {
          /* buffer can hold more */
          if ((p_cb->conn_id != NFC_RF_CONN_ID) || (nfc_cb.reassembly)) {
            /* If not rf connection or If rf connection and reassembly
             * requested,
             * try to Reassemble next packet */
            break;
          }
        }
      }

      p_evt = (NFC_HDR*)GKI_dequeue(&p_cb->rx_q);
      /* report data event */
      p_evt->offset += NCI_MSG_HDR_SIZE;
      p_evt->len -= NCI_MSG_HDR_SIZE;

      if (p_evt->layer_specific)
        data_cevt.status = NFC_STATUS_CONTINUE;
      else {
        nfc_cb.reassembly = true;
        data_cevt.status = NFC_STATUS_OK;
      }

      data_cevt.p_data = p_evt;
      /* adjust payload, if needed */
      if (p_cb->conn_id == NFC_RF_CONN_ID) {
        /* if NCI_PROTOCOL_T1T/NCI_PROTOCOL_T2T/NCI_PROTOCOL_T3T, the status
         * byte needs to be removed
         */
        if ((p_cb->act_protocol >= NCI_PROTOCOL_T1T) &&
            (p_cb->act_protocol <= NCI_PROTOCOL_T3T)) {
          if (p_evt->len) p_evt->len--;
          p = (uint8_t*)(p_evt + 1);
          data_cevt.status = *(p + p_evt->offset + p_evt->len);
          if ((NFC_GetNCIVersion() == NCI_VERSION_2_0) &&
              (p_cb->act_protocol == NCI_PROTOCOL_T2T) &&
              (p_cb->act_interface == NCI_INTERFACE_FRAME)) {
            if ((data_cevt.status != NFC_STATUS_OK) &&
                ((data_cevt.status >= T2T_STATUS_OK_1_BIT) &&
                 (data_cevt.status <= T2T_STATUS_OK_7_BIT))) {
              DLOG_IF(INFO, nfc_debug_enabled)
                  << StringPrintf("%s: T2T tag data xchange", __func__);
              data_cevt.status = NFC_STATUS_OK;
            }
          }
        }
        if ((NFC_GetNCIVersion() == NCI_VERSION_2_0) &&
            (p_cb->act_protocol == NCI_PROTOCOL_T5T)) {
          if (p_evt->len) p_evt->len--;
          p = (uint8_t*)(p_evt + 1);
          data_cevt.status = *(p + p_evt->offset + p_evt->len);
        }
      }
      tNFC_CONN nfc_conn;
      nfc_conn.data = data_cevt;
      (*p_cb->p_cback)(p_cb->conn_id, NFC_DATA_CEVT, &nfc_conn);
      p_evt = nullptr;
    }
  }
}

/*******************************************************************************
**
** Function         nfc_ncif_proc_data
**
** Description      Find the connection control block associated with the data
**                  packet. Assemble the data packet, if needed.
**                  Report the Data event.
**
** Returns          void
**
*******************************************************************************/
void nfc_ncif_proc_data(NFC_HDR* p_msg) {
  uint8_t *pp, cid;
  tNFC_CONN_CB* p_cb;
  uint8_t pbf;
  NFC_HDR* p_last;
  uint8_t *ps, *pd;
  uint16_t size;
  NFC_HDR* p_max = nullptr;
  uint16_t len;

  pp = (uint8_t*)(p_msg + 1) + p_msg->offset;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfc_ncif_proc_data 0x%02x%02x%02x", pp[0], pp[1], pp[2]);
  NCI_DATA_PRS_HDR(pp, pbf, cid, len);
  p_cb = nfc_find_conn_cb_by_conn_id(cid);
  if (p_cb && (p_msg->len >= NCI_DATA_HDR_SIZE)) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("nfc_ncif_proc_data len:%d", len);

    len = p_msg->len - NCI_MSG_HDR_SIZE;
    p_msg->layer_specific = 0;
    if (pbf) {
      NFC_SetReassemblyFlag(true);
      p_msg->layer_specific = NFC_RAS_FRAGMENTED;
    }
    p_last = (NFC_HDR*)GKI_getlast(&p_cb->rx_q);
    if (p_last && (p_last->layer_specific & NFC_RAS_FRAGMENTED)) {
      /* last data buffer is not last fragment, append this new packet to the
       * last */
      size = GKI_get_buf_size(p_last);
      if (size < (NFC_HDR_SIZE + p_last->len + p_last->offset + len)) {
        /* the current size of p_last is not big enough to hold the new
         * fragment, p_msg */
        if (size != GKI_MAX_BUF_SIZE) {
          /* try the biggest GKI pool */
          p_max = (NFC_HDR*)GKI_getpoolbuf(GKI_MAX_BUF_SIZE_POOL_ID);
          if (p_max) {
            /* copy the content of last buffer to the new buffer */
            memcpy(p_max, p_last, NFC_HDR_SIZE);
            pd = (uint8_t*)(p_max + 1) + p_max->offset;
            ps = (uint8_t*)(p_last + 1) + p_last->offset;
            memcpy(pd, ps, p_last->len);

            /* place the new buffer in the queue instead */
            GKI_remove_from_queue(&p_cb->rx_q, p_last);
            GKI_freebuf(p_last);
            GKI_enqueue(&p_cb->rx_q, p_max);
            p_last = p_max;
          }
        }
        if (p_max == nullptr) {
          /* Biggest GKI Pool not available (or)
           * Biggest available GKI Pool is not big enough to hold the new
           * fragment, p_msg */
          p_last->layer_specific |= NFC_RAS_TOO_BIG;
        }
      }

      ps = (uint8_t*)(p_msg + 1) + p_msg->offset + NCI_MSG_HDR_SIZE;

      if (!(p_last->layer_specific & NFC_RAS_TOO_BIG)) {
        pd = (uint8_t*)(p_last + 1) + p_last->offset + p_last->len;
        memcpy(pd, ps, len);
        p_last->len += len;
        /* do not need to update pbf and len in NCI header.
         * They are stripped off at NFC_DATA_CEVT and len may exceed 255 */
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("nfc_ncif_proc_data len:%d", p_last->len);
        p_last->layer_specific = p_msg->layer_specific;
        GKI_freebuf(p_msg);
        nfc_data_event(p_cb);
      } else {
        /* Not enough memory to add new buffer
         * Send data already in queue first with status Continue */
        nfc_data_event(p_cb);
        /* now enqueue the new buffer to the rx queue */
        GKI_enqueue(&p_cb->rx_q, p_msg);
      }
    } else {
      /* if this is the first fragment on RF link */
      if ((p_msg->layer_specific & NFC_RAS_FRAGMENTED) &&
          (p_cb->conn_id == NFC_RF_CONN_ID) && (p_cb->p_cback)) {
        /* Indicate upper layer that local device started receiving data */
        (*p_cb->p_cback)(p_cb->conn_id, NFC_DATA_START_CEVT, nullptr);
      }
      /* enqueue the new buffer to the rx queue */
      GKI_enqueue(&p_cb->rx_q, p_msg);
      nfc_data_event(p_cb);
    }
    return;
  }
  GKI_freebuf(p_msg);
}

/*******************************************************************************
**
** Function         nfc_ncif_process_proprietary_rsp
**
** Description      Process the response to avoid collision
**                  while rawVsCbflag is set
**
** Returns          true if proprietary response else false
**
*******************************************************************************/
bool nfc_ncif_proc_proprietary_rsp(uint8_t mt, uint8_t gid, uint8_t oid) {
  bool stat = FALSE;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: mt=%u, gid=%u, oid=%u", __func__, mt, gid, oid);

  switch (mt) {
    case NCI_MT_DATA:
      /* check for Data Response */
      if (gid != 0x03 && oid != 0x00) stat = TRUE;
      break;

    case NCI_MT_NTF:
      switch (gid) {
        case NCI_GID_CORE:
          /* check for CORE_RESET_NTF or CORE_CONN_CREDITS_NTF */
          if (oid != 0x00 && oid != 0x06) stat = TRUE;
          break;
        case NCI_GID_RF_MANAGE:
          /* check for CORE_CONN_CREDITS_NTF or NFA_EE_ACTION_NTF or
           * NFA_EE_DISCOVERY_REQ_NTF */
          if (oid != 0x06 && oid != 0x09 && oid != 0x0A) stat = TRUE;
          break;
        case NCI_GID_EE_MANAGE:
          if (oid != 0x00) stat = TRUE;
          break;
        default:
          stat = TRUE;
          break;
      }
      break;

    default:
      stat = TRUE;
      break;
  }
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit status=%u", __func__, stat);
  return stat;
}

/*******************************************************************************
** Function         nfc_mode_set_ntf_timeout
**
** Description      This function is invoked on mode set ntf timeout
**
** Returns          void
**
*******************************************************************************/
void nfc_mode_set_ntf_timeout() {
  LOG(ERROR) << StringPrintf("%s", __func__);
  tNFC_RESPONSE nfc_response;
  nfc_response.mode_set.status = NCI_STATUS_FAILED;
  nfc_response.mode_set.nfcee_id = *nfc_cb.last_cmd;
  nfc_response.mode_set.mode = NCI_NFCEE_MD_DEACTIVATE;

  tNFC_RESPONSE_CBACK* p_cback = nfc_cb.p_resp_cback;
  tNFC_RESPONSE_EVT event = NFC_NFCEE_MODE_SET_REVT;
  if (p_cback) (*p_cback)(event, &nfc_response);
}
