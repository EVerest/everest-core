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
 *  This file contains functions that interface with the NFC NCI transport.
 *  On the receive side, it routes events to the appropriate handler
 *  (callback). On the transmit side, it manages the command transmission.
 *
 ******************************************************************************/
#include <string.h>

#include <android-base/stringprintf.h>
#include <android/hardware/nfc/1.1/types.h>
#include <base/logging.h>

#include "nfc_target.h"

#include "bt_types.h"
#include "ce_int.h"
#include "gki.h"
#include "nci_hmsgs.h"
#include "nfc_int.h"
#include "rw_int.h"
#include "nfc.h"
#if (NFC_RW_ONLY == FALSE)

#include "llcp_int.h"

/* NFC mandates support for at least one logical connection;
 * Update max_conn to the NFCC capability on InitRsp */
#if (NXP_EXTNS == TRUE)
#define NFC_SET_MAX_CONN_DEFAULT() \
  { nfc_cb.max_conn = 2; }
#else
#define NFC_SET_MAX_CONN_DEFAULT() \
  { nfc_cb.max_conn = 1; }
#endif
#else /* NFC_RW_ONLY */
#define ce_init()
#define llcp_init()

#define NFC_SET_MAX_CONN_DEFAULT()

#endif /* NFC_RW_ONLY */
#ifdef ANDROID
using android::base::StringPrintf;
#endif
using android::hardware::nfc::V1_1::NfcEvent;

extern bool nfc_debug_enabled;
extern void delete_stack_non_volatile_store(bool forceDelete);

/****************************************************************************
** Declarations
****************************************************************************/
tNFC_CB nfc_cb;

#if (NFC_RW_ONLY == FALSE)
#define NFC_NUM_INTERFACE_MAP 2
#else
#define NFC_NUM_INTERFACE_MAP 1
#endif

static const tNCI_DISCOVER_MAPS nfc_interface_mapping[NFC_NUM_INTERFACE_MAP] = {
    /* Protocols that use Frame Interface do not need to be included in the
       interface mapping */
    {NCI_PROTOCOL_ISO_DEP, NCI_INTERFACE_MODE_POLL_N_LISTEN,
     NCI_INTERFACE_ISO_DEP}
#if (NFC_RW_ONLY == FALSE)
    ,
    /* this can not be set here due to 2079xB0 NFCC issues */
    {NCI_PROTOCOL_NFC_DEP, NCI_INTERFACE_MODE_POLL_N_LISTEN,
     NCI_INTERFACE_NFC_DEP}
#endif
};

/*******************************************************************************
**
** Function         nfc_state_name
**
** Description      This function returns the state name.
**
** NOTE             conditionally compiled to save memory.
**
** Returns          pointer to the name
**
*******************************************************************************/
static std::string nfc_state_name(uint8_t state) {
  switch (state) {
    case NFC_STATE_NONE:
      return "NONE";
    case NFC_STATE_W4_HAL_OPEN:
      return "W4_HAL_OPEN";
    case NFC_STATE_CORE_INIT:
      return "CORE_INIT";
    case NFC_STATE_W4_POST_INIT_CPLT:
      return "W4_POST_INIT_CPLT";
    case NFC_STATE_IDLE:
      return "IDLE";
    case NFC_STATE_OPEN:
      return "OPEN";
    case NFC_STATE_CLOSING:
      return "CLOSING";
    case NFC_STATE_W4_HAL_CLOSE:
      return "W4_HAL_CLOSE";
    case NFC_STATE_NFCC_POWER_OFF_SLEEP:
      return "NFCC_POWER_OFF_SLEEP";
    default:
      return "???? UNKNOWN STATE";
  }
}

/*******************************************************************************
**
** Function         nfc_hal_event_name
**
** Description      This function returns the HAL event name.
**
** NOTE             conditionally compiled to save memory.
**
** Returns          pointer to the name
**
*******************************************************************************/
static std::string nfc_hal_event_name(uint8_t event) {
  switch (event) {
    case HAL_NFC_OPEN_CPLT_EVT:
      return "HAL_NFC_OPEN_CPLT_EVT";
    case HAL_NFC_CLOSE_CPLT_EVT:
      return "HAL_NFC_CLOSE_CPLT_EVT";
    case HAL_NFC_POST_INIT_CPLT_EVT:
      return "HAL_NFC_POST_INIT_CPLT_EVT";
    case HAL_NFC_PRE_DISCOVER_CPLT_EVT:
      return "HAL_NFC_PRE_DISCOVER_CPLT_EVT";
    case HAL_NFC_REQUEST_CONTROL_EVT:
      return "HAL_NFC_REQUEST_CONTROL_EVT";
    case HAL_NFC_RELEASE_CONTROL_EVT:
      return "HAL_NFC_RELEASE_CONTROL_EVT";
    case HAL_NFC_ERROR_EVT:
      return "HAL_NFC_ERROR_EVT";
    case (uint32_t)NfcEvent::HCI_NETWORK_RESET:
      return "HCI_NETWORK_RESET";
    default:
      return "???? UNKNOWN EVENT";
  }
}

/*******************************************************************************
**
** Function         nfc_main_notify_enable_status
**
** Description      Notify status of Enable/PowerOffSleep/PowerCycle
**
*******************************************************************************/
static void nfc_main_notify_enable_status(tNFC_STATUS nfc_status) {
  tNFC_RESPONSE evt_data;

  evt_data.status = nfc_status;

  if (nfc_cb.p_resp_cback) {
    /* if getting out of PowerOffSleep mode or restarting NFCC */
    if (nfc_cb.flags & (NFC_FL_RESTARTING | NFC_FL_POWER_CYCLE_NFCC)) {
      nfc_cb.flags &= ~(NFC_FL_RESTARTING | NFC_FL_POWER_CYCLE_NFCC);
      if (nfc_status != NFC_STATUS_OK) {
        nfc_cb.flags |= NFC_FL_POWER_OFF_SLEEP;
      }
      (*nfc_cb.p_resp_cback)(NFC_NFCC_RESTART_REVT, &evt_data);
    } else {
      (*nfc_cb.p_resp_cback)(NFC_ENABLE_REVT, &evt_data);
    }
  }
}

/*******************************************************************************
**
** Function         nfc_enabled
**
** Description      NFCC enabled, proceed with stack start up.
**
** Returns          void
**
*******************************************************************************/
void nfc_enabled(tNFC_STATUS nfc_status, NFC_HDR* p_init_rsp_msg) {
  tNFC_RESPONSE evt_data;
  tNFC_CONN_CB* p_cb = &nfc_cb.conn_cb[NFC_RF_CONN_ID];
  int16_t lremain = 0;
  uint8_t* p;
  uint8_t num_interfaces = 0, xx;
  uint8_t num_interface_extensions = 0, zz;
  uint8_t interface_type;
  int yy = 0;
  memset(&evt_data, 0, sizeof(tNFC_RESPONSE));

  if (nfc_status == NCI_STATUS_OK) {
    nfc_set_state(NFC_STATE_IDLE);

    p = (uint8_t*)(p_init_rsp_msg + 1) + p_init_rsp_msg->offset +
        NCI_MSG_HDR_SIZE + 1;

    lremain = p_init_rsp_msg->len - NCI_MSG_HDR_SIZE - 1 - sizeof(uint32_t) - 5;
    if (lremain < 0) {
      nfc_status = NCI_STATUS_FAILED;
      goto plen_err;
    }
    /* we currently only support NCI of the same version.
    * We may need to change this, when we support multiple version of NFCC */

    evt_data.enable.nci_version = nfc_cb.nci_version;
    STREAM_TO_UINT32(evt_data.enable.nci_features, p);
    if (nfc_cb.nci_version == NCI_VERSION_1_0) {
      /* this byte is consumed in the top expression */
      STREAM_TO_UINT8(num_interfaces, p);
      lremain -= num_interfaces;
      if (lremain < 0) {
        nfc_status = NCI_STATUS_FAILED;
        goto plen_err;
      }
      evt_data.enable.nci_interfaces = 0;
      for (xx = 0; xx < num_interfaces; xx++) {
        if ((*p) <= NCI_INTERFACE_MAX)
          evt_data.enable.nci_interfaces |= (1 << (*p));
        else if (((*p) >= NCI_INTERFACE_FIRST_VS) &&
                 (yy < NFC_NFCC_MAX_NUM_VS_INTERFACE)) {
          /* save the VS RF interface in control block, if there's still room */
          nfc_cb.vs_interface[yy++] = *p;
        }
        p++;
      }
      nfc_cb.nci_interfaces = evt_data.enable.nci_interfaces;
      memcpy(evt_data.enable.vs_interface, nfc_cb.vs_interface,
             NFC_NFCC_MAX_NUM_VS_INTERFACE);
    }
    /* four bytes below are consumed in the top expression */
    evt_data.enable.max_conn = *p++;
    STREAM_TO_UINT16(evt_data.enable.max_ce_table, p);
#if (NFC_RW_ONLY == FALSE)
    nfc_cb.max_ce_table = evt_data.enable.max_ce_table;
    nfc_cb.nci_features = evt_data.enable.nci_features;
    nfc_cb.max_conn = evt_data.enable.max_conn;
#endif
    nfc_cb.nci_ctrl_size = *p++; /* Max Control Packet Payload Length */
    p_cb->init_credits = p_cb->num_buff = 0;
    nfc_set_conn_id(p_cb, NFC_RF_CONN_ID);
    if (nfc_cb.nci_version == NCI_VERSION_2_0) {
      /* one byte is consumed in the top expression and
       * 3 bytes from uit16+uint8 below */
      lremain -= 4;
      if (lremain < 0) {
        nfc_status = NCI_STATUS_FAILED;
        goto plen_err;
      }
      if (evt_data.enable.nci_features & NCI_FEAT_HCI_NETWORK) {
        p_cb = &nfc_cb.conn_cb[NFC_HCI_CONN_ID];
        nfc_set_conn_id(p_cb, NFC_HCI_CONN_ID);
        p_cb->id = NFC_HCI_CONN_ID;
        STREAM_TO_UINT8(p_cb->buff_size, p);
        STREAM_TO_UINT8(p_cb->num_buff, p);
        p_cb->init_credits = p_cb->num_buff;
        evt_data.enable.hci_packet_size = p_cb->buff_size;
        evt_data.enable.hci_conn_credits = p_cb->init_credits;
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "hci num_buf=%d buf_size=%d", p_cb->num_buff, p_cb->buff_size);
      } else {
        /*HCI n/w not enabled skip data buff size and data credit HCI conn */
        p += 2;
      }
      STREAM_TO_UINT16(evt_data.enable.max_nfc_v_size, p);
      STREAM_TO_UINT8(num_interfaces, p);
#if (NFC_RW_ONLY == FALSE)
      nfc_cb.hci_packet_size = evt_data.enable.hci_packet_size;
      nfc_cb.hci_conn_credits = evt_data.enable.hci_conn_credits;
      nfc_cb.nci_max_v_size = evt_data.enable.max_nfc_v_size;
#endif
      evt_data.enable.nci_interfaces = 0;

      for (xx = 0; xx < num_interfaces; xx++) {
        lremain -= 2;
        if (lremain < 0) {
          nfc_status = NCI_STATUS_FAILED;
          goto plen_err;
        }
        if ((*p) <= NCI_INTERFACE_MAX)
          evt_data.enable.nci_interfaces |= (1 << (*p));
        else if (((*p) >= NCI_INTERFACE_FIRST_VS) &&
                 (yy < NFC_NFCC_MAX_NUM_VS_INTERFACE)) {
          /* save the VS RF interface in control block, if there's still room */
          nfc_cb.vs_interface[yy++] = *p;
        }
        interface_type = *p++;
        num_interface_extensions = *p++;
        lremain -= num_interface_extensions;
        if (lremain < 0) {
          nfc_status = NCI_STATUS_FAILED;
          goto plen_err;
        }
        for (zz = 0; zz < num_interface_extensions; zz++) {
          if (((*p) < NCI_INTERFACE_EXTENSION_MAX) &&
              (interface_type <= NCI_INTERFACE_MAX)) {
            nfc_cb.nci_intf_extensions |= (1 << (*p));
            nfc_cb.nci_intf_extension_map[*p] = (1 << interface_type);
          }
          p++;
        }
      }

      nfc_cb.nci_interfaces = evt_data.enable.nci_interfaces;
      memcpy(evt_data.enable.vs_interface, nfc_cb.vs_interface,
             NFC_NFCC_MAX_NUM_VS_INTERFACE);
    } else {
      /* one byte is consumed in the top expression */
      lremain -= sizeof(uint16_t) + NFC_NFCC_INFO_LEN;
      if (lremain < 0) {
        nfc_status = NCI_STATUS_FAILED;
        goto plen_err;
      }
      STREAM_TO_UINT16(evt_data.enable.max_param_size, p);
      evt_data.enable.manufacture_id = *p++;
      STREAM_TO_ARRAY(evt_data.enable.nfcc_info, p, NFC_NFCC_INFO_LEN);
    }
    NFC_DiscoveryMap(nfc_cb.num_disc_maps,
                     (tNCI_DISCOVER_MAPS*)nfc_cb.p_disc_maps, nullptr);
  }
  /* else not successful. the buffers will be freed in nfc_free_conn_cb () */
  else {
  plen_err:
    if (nfc_cb.flags & NFC_FL_RESTARTING) {
      nfc_set_state(NFC_STATE_NFCC_POWER_OFF_SLEEP);
    } else {
      nfc_free_conn_cb(p_cb);

      /* if NFCC didn't respond to CORE_RESET or CORE_INIT */
      if (nfc_cb.nfc_state == NFC_STATE_CORE_INIT) {
        /* report status after closing HAL */
        nfc_cb.p_hal->close();
        return;
      } else
        nfc_set_state(NFC_STATE_NONE);
    }
  }

  nfc_main_notify_enable_status(nfc_status);
}

/*******************************************************************************
**
** Function         nfc_set_state
**
** Description      Set the state of NFC stack
**
** Returns          void
**
*******************************************************************************/
void nfc_set_state(tNFC_STATE nfc_state) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfc_set_state %d (%s)->%d (%s)", nfc_cb.nfc_state,
                      nfc_state_name(nfc_cb.nfc_state).c_str(), nfc_state,
                      nfc_state_name(nfc_state).c_str());
  nfc_cb.nfc_state = nfc_state;
}

/*******************************************************************************
**
** Function         nfc_gen_cleanup
**
** Description      Clean up for both going into low power mode and disabling
**                  NFC
**
*******************************************************************************/
void nfc_gen_cleanup(void) {
  nfc_cb.flags &= ~NFC_FL_DEACTIVATING;

  /* the HAL pre-discover is still active - clear the pending flag/free the
   * buffer */
  if (nfc_cb.flags & NFC_FL_DISCOVER_PENDING) {
    nfc_cb.flags &= ~NFC_FL_DISCOVER_PENDING;
    GKI_freebuf(nfc_cb.p_disc_pending);
    nfc_cb.p_disc_pending = nullptr;
  }

  nfc_cb.flags &= ~(NFC_FL_CONTROL_REQUESTED | NFC_FL_CONTROL_GRANTED |
                    NFC_FL_HAL_REQUESTED);

  nfc_stop_timer(&nfc_cb.deactivate_timer);

  /* Reset the connection control blocks */
  nfc_reset_all_conn_cbs();

  if (nfc_cb.p_nci_init_rsp) {
    GKI_freebuf(nfc_cb.p_nci_init_rsp);
    nfc_cb.p_nci_init_rsp = nullptr;
  }

  /* clear any pending CMD/RSP */
  nfc_main_flush_cmd_queue();
}

/*******************************************************************************
**
** Function         nfc_main_handle_hal_evt
**
** Description      Handle BT_EVT_TO_NFC_MSGS
**
*******************************************************************************/
void nfc_main_handle_hal_evt(tNFC_HAL_EVT_MSG* p_msg) {
  uint8_t* ps;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("HAL event=0x%x", p_msg->hal_evt);

  switch (p_msg->hal_evt) {
    case HAL_NFC_OPEN_CPLT_EVT: /* only for failure case */
      nfc_enabled(NFC_STATUS_FAILED, nullptr);
      break;

    case HAL_NFC_CLOSE_CPLT_EVT:
      if (nfc_cb.p_resp_cback) {
        if (nfc_cb.nfc_state == NFC_STATE_W4_HAL_CLOSE) {
          if (nfc_cb.flags & NFC_FL_POWER_OFF_SLEEP) {
            nfc_cb.flags &= ~NFC_FL_POWER_OFF_SLEEP;
            nfc_set_state(NFC_STATE_NFCC_POWER_OFF_SLEEP);
            (*nfc_cb.p_resp_cback)(NFC_NFCC_POWER_OFF_REVT, nullptr);
          } else {
            nfc_set_state(NFC_STATE_NONE);
            (*nfc_cb.p_resp_cback)(NFC_DISABLE_REVT, nullptr);
            nfc_cb.p_resp_cback = nullptr;
          }
        } else {
          /* found error during initialization */
          nfc_set_state(NFC_STATE_NONE);
          nfc_main_notify_enable_status(NFC_STATUS_FAILED);
        }
      }
      break;

    case HAL_NFC_POST_INIT_CPLT_EVT:
      if (nfc_cb.p_nci_init_rsp) {
        /*
        ** if NFC_Disable() is called before receiving
        ** HAL_NFC_POST_INIT_CPLT_EVT, then wait for HAL_NFC_CLOSE_CPLT_EVT.
        */
        if (nfc_cb.nfc_state == NFC_STATE_W4_POST_INIT_CPLT) {
          if (p_msg->status == HAL_NFC_STATUS_OK) {
            nfc_enabled(NCI_STATUS_OK, nfc_cb.p_nci_init_rsp);
          } else /* if post initailization failed */
          {
            nfc_enabled(NCI_STATUS_FAILED, nullptr);
          }
        }

        GKI_freebuf(nfc_cb.p_nci_init_rsp);
        nfc_cb.p_nci_init_rsp = nullptr;
      }
      break;

    case HAL_NFC_PRE_DISCOVER_CPLT_EVT:
      /* restore the command window, no matter if the discover command is still
       * pending */
      nfc_cb.nci_cmd_window = NCI_MAX_CMD_WINDOW;
      nfc_cb.flags &= ~NFC_FL_CONTROL_GRANTED;
      if (nfc_cb.flags & NFC_FL_DISCOVER_PENDING) {
        /* issue the discovery command now, if it is still pending */
        nfc_cb.flags &= ~NFC_FL_DISCOVER_PENDING;
        ps = (uint8_t*)nfc_cb.p_disc_pending;
        nci_snd_discover_cmd(*ps, (tNFC_DISCOVER_PARAMS*)(ps + 1));
        GKI_freebuf(nfc_cb.p_disc_pending);
        nfc_cb.p_disc_pending = nullptr;
      } else {
        /* check if there's other pending commands */
        nfc_ncif_check_cmd_queue(nullptr);
      }

      if (p_msg->status == HAL_NFC_STATUS_ERR_CMD_TIMEOUT)
        nfc_ncif_event_status(NFC_NFCC_TIMEOUT_REVT, NFC_STATUS_HW_TIMEOUT);
      break;

    case HAL_NFC_REQUEST_CONTROL_EVT:
      nfc_cb.flags |= NFC_FL_CONTROL_REQUESTED;
      nfc_cb.flags |= NFC_FL_HAL_REQUESTED;
      nfc_ncif_check_cmd_queue(nullptr);
      break;

    case HAL_NFC_RELEASE_CONTROL_EVT:
      if (nfc_cb.flags & NFC_FL_CONTROL_GRANTED) {
        nfc_cb.flags &= ~NFC_FL_CONTROL_GRANTED;
        nfc_cb.nci_cmd_window = NCI_MAX_CMD_WINDOW;
        nfc_ncif_check_cmd_queue(nullptr);

        if (p_msg->status == HAL_NFC_STATUS_ERR_CMD_TIMEOUT)
          nfc_ncif_event_status(NFC_NFCC_TIMEOUT_REVT, NFC_STATUS_HW_TIMEOUT);
      }
      break;

    case HAL_NFC_ERROR_EVT:
      switch (p_msg->status) {
        case HAL_NFC_STATUS_ERR_TRANSPORT:
          /* Notify app of transport error */
          if (nfc_cb.p_resp_cback) {
            (*nfc_cb.p_resp_cback)(NFC_NFCC_TRANSPORT_ERR_REVT, nullptr);

            /* if enabling NFC, notify upper layer of failure after closing HAL
             */
            if (nfc_cb.nfc_state < NFC_STATE_IDLE) {
              nfc_enabled(NFC_STATUS_FAILED, nullptr);
            }
          }
          break;

        case HAL_NFC_STATUS_ERR_CMD_TIMEOUT:
          nfc_ncif_event_status(NFC_NFCC_TIMEOUT_REVT, NFC_STATUS_HW_TIMEOUT);

          /* if enabling NFC, notify upper layer of failure after closing HAL */
          if (nfc_cb.nfc_state < NFC_STATE_IDLE) {
            nfc_enabled(NFC_STATUS_FAILED, nullptr);
            return;
          }
          break;

        case (uint32_t)NfcEvent::HCI_NETWORK_RESET:
          delete_stack_non_volatile_store(true);
          break;

        default:
          break;
      }
      break;

    default:
      LOG(ERROR) << StringPrintf("unhandled event (0x%x).", p_msg->hal_evt);
      break;
  }
}

/*******************************************************************************
**
** Function         nfc_main_flush_cmd_queue
**
** Description      This function is called when setting power off sleep state.
**
** Returns          void
**
*******************************************************************************/
void nfc_main_flush_cmd_queue(void) {
  NFC_HDR* p_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* initialize command window */
  nfc_cb.nci_cmd_window = NCI_MAX_CMD_WINDOW;

  /* Stop command-pending timer */
  nfc_stop_timer(&nfc_cb.nci_wait_rsp_timer);

  /* dequeue and free buffer */
  while ((p_msg = (NFC_HDR*)GKI_dequeue(&nfc_cb.nci_cmd_xmit_q)) != nullptr) {
    GKI_freebuf(p_msg);
  }
}

/*******************************************************************************
**
** Function         nfc_main_post_hal_evt
**
** Description      This function posts HAL event to NFC_TASK
**
** Returns          void
**
*******************************************************************************/
void nfc_main_post_hal_evt(uint8_t hal_evt, tHAL_NFC_STATUS status) {
  tNFC_HAL_EVT_MSG* p_msg;

  p_msg = (tNFC_HAL_EVT_MSG*)GKI_getbuf(sizeof(tNFC_HAL_EVT_MSG));
  if (p_msg != nullptr) {
    /* Initialize NFC_HDR */
    p_msg->hdr.len = 0;
    p_msg->hdr.event = BT_EVT_TO_NFC_MSGS;
    p_msg->hdr.offset = 0;
    p_msg->hdr.layer_specific = 0;
    p_msg->hal_evt = hal_evt;
    p_msg->status = status;
    GKI_send_msg(NFC_TASK, NFC_MBOX_ID, p_msg);
  } else {
    LOG(ERROR) << StringPrintf("No buffer");
  }
}

/*******************************************************************************
**
** Function         nfc_main_hal_cback
**
** Description      HAL event handler
**
** Returns          void
**
*******************************************************************************/
static void nfc_main_hal_cback(uint8_t event, tHAL_NFC_STATUS status) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfc_main_hal_cback event: %s(0x%x), status=%d",
                      nfc_hal_event_name(event).c_str(), event, status);

  switch (event) {
    case HAL_NFC_OPEN_CPLT_EVT:
      /*
      ** if NFC_Disable() is called before receiving HAL_NFC_OPEN_CPLT_EVT,
      ** then wait for HAL_NFC_CLOSE_CPLT_EVT.
      */
      if (nfc_cb.nfc_state == NFC_STATE_W4_HAL_OPEN) {
        if (status == HAL_NFC_STATUS_OK) {
          /* Notify NFC_TASK that NCI tranport is initialized */
          GKI_send_event(NFC_TASK, NFC_TASK_EVT_TRANSPORT_READY);
        } else {
          nfc_main_post_hal_evt(event, status);
        }
      }
      break;

    case HAL_NFC_CLOSE_CPLT_EVT:
    case HAL_NFC_POST_INIT_CPLT_EVT:
    case HAL_NFC_PRE_DISCOVER_CPLT_EVT:
    case HAL_NFC_REQUEST_CONTROL_EVT:
    case HAL_NFC_RELEASE_CONTROL_EVT:
    case HAL_NFC_ERROR_EVT:
    case (uint32_t)NfcEvent::HCI_NETWORK_RESET:
      nfc_main_post_hal_evt(event, status);
      break;

    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("nfc_main_hal_cback unhandled event %x", event);
      break;
  }
}

/*******************************************************************************
**
** Function         nfc_main_hal_data_cback
**
** Description      HAL data event handler
**
** Returns          void
**
*******************************************************************************/
static void nfc_main_hal_data_cback(uint16_t data_len, uint8_t* p_data) {
  NFC_HDR* p_msg;

  /* ignore all data while shutting down NFCC */
  if (nfc_cb.nfc_state == NFC_STATE_W4_HAL_CLOSE) {
    return;
  }

  if (p_data) {
#ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
    // GKI_getpoolbuf returns a fixed size of memory buffer, which is usually
    // bigger than NFC packets. This may hide OOB issues.
    p_msg = (NFC_HDR*)GKI_getbuf(sizeof(NFC_HDR) + NFC_RECEIVE_MSGS_OFFSET +
                                 data_len);
#else
    p_msg = (NFC_HDR*)GKI_getpoolbuf(NFC_NCI_POOL_ID);
#endif
    if (p_msg != nullptr) {
      /* Initialize NFC_HDR */
      p_msg->len = data_len;
      p_msg->event = BT_EVT_TO_NFC_NCI;
      p_msg->offset = NFC_RECEIVE_MSGS_OFFSET;

      /* no need to check length, it always less than pool size */
      memcpy((uint8_t*)(p_msg + 1) + p_msg->offset, p_data, p_msg->len);

      GKI_send_msg(NFC_TASK, NFC_MBOX_ID, p_msg);
    } else {
      LOG(ERROR) << StringPrintf("No buffer");
    }
  }
}

/*******************************************************************************
**
** Function         NFC_Enable
**
** Description      This function enables NFC. Prior to calling NFC_Enable:
**                  - the NFCC must be powered up, and ready to receive
**                    commands.
**                  - GKI must be enabled
**                  - NFC_TASK must be started
**                  - NCIT_TASK must be started (if using dedicated NCI
**                    transport)
**
**                  This function opens the NCI transport (if applicable),
**                  resets the NFC controller, and initializes the NFC
**                  subsystems.
**
**                  When the NFC startup procedure is completed, an
**                  NFC_ENABLE_REVT is returned to the application using the
**                  tNFC_RESPONSE_CBACK.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_Enable(tNFC_RESPONSE_CBACK* p_cback) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* Validate callback */
  if (!p_cback) {
    return (NFC_STATUS_INVALID_PARAM);
  }
  nfc_cb.p_resp_cback = p_cback;

  /* Open HAL transport. */
  nfc_set_state(NFC_STATE_W4_HAL_OPEN);
  nfc_cb.p_hal->open(nfc_main_hal_cback, nfc_main_hal_data_cback);

  return (NFC_STATUS_OK);
}

/*******************************************************************************
**
** Function         NFC_Disable
**
** Description      This function performs clean up routines for shutting down
**                  NFC and closes the NCI transport (if using dedicated NCI
**                  transport).
**
**                  When the NFC shutdown procedure is completed, an
**                  NFC_DISABLED_REVT is returned to the application using the
**                  tNFC_RESPONSE_CBACK.
**
** Returns          nothing
**
*******************************************************************************/
void NFC_Disable(void) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfc_state = %d", nfc_cb.nfc_state);

  if ((nfc_cb.nfc_state == NFC_STATE_NONE) ||
      (nfc_cb.nfc_state == NFC_STATE_NFCC_POWER_OFF_SLEEP)) {
    nfc_set_state(NFC_STATE_NONE);
    if (nfc_cb.p_resp_cback) {
      (*nfc_cb.p_resp_cback)(NFC_DISABLE_REVT, nullptr);
      nfc_cb.p_resp_cback = nullptr;
    }
    return;
  }

  /* Close transport and clean up */
  nfc_task_shutdown_nfcc();
}

/*******************************************************************************
**
** Function         NFC_Init
**
** Description      This function initializes control block for NFC
**
** Returns          nothing
**
*******************************************************************************/
void NFC_Init(tHAL_NFC_ENTRY* p_hal_entry_tbl) {
  int xx;

  /* Clear nfc control block */
  memset(&nfc_cb, 0, sizeof(tNFC_CB));

  /* Reset the nfc control block */
  for (xx = 0; xx < NCI_MAX_CONN_CBS; xx++) {
    nfc_cb.conn_cb[xx].conn_id = NFC_ILLEGAL_CONN_ID;
  }

  /* NCI init */
  nfc_cb.p_hal = p_hal_entry_tbl;
  nfc_cb.nfc_state = NFC_STATE_NONE;
  nfc_cb.nci_cmd_window = NCI_MAX_CMD_WINDOW;
  nfc_cb.nci_wait_rsp_tout = NFC_CMD_CMPL_TIMEOUT;
  nfc_cb.p_disc_maps = nfc_interface_mapping;
  nfc_cb.num_disc_maps = NFC_NUM_INTERFACE_MAP;
  nfc_cb.nci_ctrl_size = NCI_CTRL_INIT_SIZE;
  nfc_cb.reassembly = true;
  nfc_cb.nci_version = NCI_VERSION_UNKNOWN;
  rw_init();
  ce_init();
  llcp_init();
  NFC_SET_MAX_CONN_DEFAULT();
}

/*******************************************************************************
**
** Function         NFC_GetLmrtSize
**
** Description      Called by application wto query the Listen Mode Routing
**                  Table size supported by NFCC
**
** Returns          Listen Mode Routing Table size
**
*******************************************************************************/
uint16_t NFC_GetLmrtSize(void) {
  uint16_t size = 0;
#if (NFC_RW_ONLY == FALSE)
  size = nfc_cb.max_ce_table;
#endif
  return size;
}

/*******************************************************************************
**
** Function         NFC_SetConfig
**
** Description      This function is called to send the configuration parameter
**                  TLV to NFCC. The response from NFCC is reported by
**                  tNFC_RESPONSE_CBACK as NFC_SET_CONFIG_REVT.
**
** Parameters       tlv_size - the length of p_param_tlvs.
**                  p_param_tlvs - the parameter ID/Len/Value list
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_SetConfig(uint8_t tlv_size, uint8_t* p_param_tlvs) {
  return nci_snd_core_set_config(p_param_tlvs, tlv_size);
}

/*******************************************************************************
**
** Function         NFC_GetConfig
**
** Description      This function is called to retrieve the parameter TLV from
**                  NFCC. The response from NFCC is reported by
**                  tNFC_RESPONSE_CBACK as NFC_GET_CONFIG_REVT.
**
** Parameters       num_ids - the number of parameter IDs
**                  p_param_ids - the parameter ID list.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_GetConfig(uint8_t num_ids, uint8_t* p_param_ids) {
  return nci_snd_core_get_config(p_param_ids, num_ids);
}

/*******************************************************************************
**
** Function         NFC_DiscoveryMap
**
** Description      This function is called to set the discovery interface
**                  mapping. The response from NFCC is reported by
**                  tNFC_DISCOVER_CBACK as NFC_MAP_DEVT.
**
** Parameters       num - the number of items in p_params.
**                  p_maps - the discovery interface mappings
**                  p_cback - the discovery callback function
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_DiscoveryMap(uint8_t num, tNFC_DISCOVER_MAPS* p_maps,
                             tNFC_DISCOVER_CBACK* p_cback) {
  uint8_t num_disc_maps = num;
  uint8_t xx, yy, num_intf, intf_mask;
  tNFC_DISCOVER_MAPS
      max_maps[NFC_NFCC_MAX_NUM_VS_INTERFACE + NCI_INTERFACE_MAX];
  bool is_supported;

  nfc_cb.p_discv_cback = p_cback;
  num_intf = 0;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nci_interfaces supported by NFCC: 0x%x", nfc_cb.nci_interfaces);

  for (xx = 0; xx < NFC_NFCC_MAX_NUM_VS_INTERFACE + NCI_INTERFACE_MAX; xx++) {
    memset(&max_maps[xx], 0x00, sizeof(tNFC_DISCOVER_MAPS));
  }

  for (xx = 0; xx < num_disc_maps; xx++) {
    is_supported = false;
    if (p_maps[xx].intf_type > NCI_INTERFACE_MAX) {
      for (yy = 0; yy < NFC_NFCC_MAX_NUM_VS_INTERFACE; yy++) {
        if (nfc_cb.vs_interface[yy] == p_maps[xx].intf_type)
          is_supported = true;
      }
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("[%d]: vs intf_type:0x%x is_supported:%d", xx,
                          p_maps[xx].intf_type, is_supported);
    } else {
      intf_mask = (1 << (p_maps[xx].intf_type));
      if (intf_mask & nfc_cb.nci_interfaces) {
        is_supported = true;
      }
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("[%d]: intf_type:%d intf_mask: 0x%x is_supported:%d",
                          xx, p_maps[xx].intf_type, intf_mask, is_supported);
    }
    if (is_supported)
      memcpy(&max_maps[num_intf++], &p_maps[xx], sizeof(tNFC_DISCOVER_MAPS));
    else {
      LOG(WARNING) << StringPrintf(
          "NFC_DiscoveryMap interface=0x%x is not supported by NFCC",
          p_maps[xx].intf_type);
    }
  }

  return nci_snd_discover_map_cmd(num_intf, (tNCI_DISCOVER_MAPS*)max_maps);
}

/*******************************************************************************
**
** Function         NFC_DiscoveryStart
**
** Description      This function is called to start Polling and/or Listening.
**                  The response from NFCC is reported by tNFC_DISCOVER_CBACK as
**                  NFC_START_DEVT. The notification from NFCC is reported by
**                  tNFC_DISCOVER_CBACK as NFC_RESULT_DEVT.
**
** Parameters       num_params - the number of items in p_params.
**                  p_params - the discovery parameters
**                  p_cback - the discovery callback function
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_DiscoveryStart(uint8_t num_params,
                               tNFC_DISCOVER_PARAMS* p_params,
                               tNFC_DISCOVER_CBACK* p_cback) {
  uint8_t* p;
  int params_size;
  tNFC_STATUS status = NFC_STATUS_NO_BUFFERS;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;
  if (nfc_cb.p_disc_pending) {
    LOG(ERROR) << StringPrintf("There's pending NFC_DiscoveryStart");
    status = NFC_STATUS_BUSY;
  } else {
    nfc_cb.p_discv_cback = p_cback;
    nfc_cb.flags |= NFC_FL_DISCOVER_PENDING;
    nfc_cb.flags |= NFC_FL_CONTROL_REQUESTED;
    params_size = sizeof(tNFC_DISCOVER_PARAMS) * num_params;
    nfc_cb.p_disc_pending =
        GKI_getbuf((uint16_t)(NFC_HDR_SIZE + 1 + params_size));
    if (nfc_cb.p_disc_pending) {
      p = (uint8_t*)nfc_cb.p_disc_pending;
      *p++ = num_params;
      memcpy(p, p_params, params_size);
      status = NFC_STATUS_CMD_STARTED;
      nfc_ncif_check_cmd_queue(nullptr);
    }
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("NFC_DiscoveryStart status: 0x%x", status);
  return status;
}

/*******************************************************************************
**
** Function         NFC_DiscoverySelect
**
** Description      If tNFC_DISCOVER_CBACK reports status=NFC_MULTIPLE_PROT,
**                  the application needs to use this function to select the
**                  the logical endpoint to continue. The response from NFCC is
**                  reported by tNFC_DISCOVER_CBACK as NFC_SELECT_DEVT.
**
** Parameters       rf_disc_id - The ID identifies the remote device.
**                  protocol - the logical endpoint on the remote devide
**                  rf_interface - the RF interface to communicate with NFCC
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_DiscoverySelect(uint8_t rf_disc_id, uint8_t protocol,
                                uint8_t rf_interface) {
  return nci_snd_discover_select_cmd(rf_disc_id, protocol, rf_interface);
}

/*******************************************************************************
**
** Function         NFC_ConnCreate
**
** Description      This function is called to create a logical connection with
**                  NFCC for data exchange.
**
** Parameters       dest_type - the destination type
**                  id   - the NFCEE ID or RF Discovery ID .
**                  protocol   - the protocol.
**                  p_cback - the connection callback function
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_ConnCreate(uint8_t dest_type, uint8_t id, uint8_t protocol,
                           tNFC_CONN_CBACK* p_cback) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tNFC_CONN_CB* p_cb;
  uint8_t num_tlv = 0, tlv_size = 0;
  uint8_t param_tlvs[4], *pp;

  p_cb = nfc_alloc_conn_cb(p_cback);
  if (p_cb) {
    p_cb->id = id;
    pp = param_tlvs;
    if (dest_type == NCI_DEST_TYPE_NFCEE) {
      num_tlv = 1;
      UINT8_TO_STREAM(pp, NCI_CON_CREATE_TAG_NFCEE_VAL);
      UINT8_TO_STREAM(pp, 2);
      UINT8_TO_STREAM(pp, id);
      UINT8_TO_STREAM(pp, protocol);
      tlv_size = 4;
    } else if (dest_type == NCI_DEST_TYPE_REMOTE) {
      num_tlv = 1;
      UINT8_TO_STREAM(pp, NCI_CON_CREATE_TAG_RF_DISC_ID);
      UINT8_TO_STREAM(pp, 1);
      UINT8_TO_STREAM(pp, id);
      tlv_size = 3;
    } else if (dest_type == NCI_DEST_TYPE_NFCC) {
      p_cb->id = NFC_TEST_ID;
    }
    /* Add handling of NCI_DEST_TYPE_REMOTE when more RF interface definitions
     * are added */
    p_cb->act_protocol = protocol;
    p_cb->p_cback = p_cback;
    status = nci_snd_core_conn_create(dest_type, num_tlv, tlv_size, param_tlvs);
    if (status == NFC_STATUS_FAILED) nfc_free_conn_cb(p_cb);
  }
  return status;
}

/*******************************************************************************
**
** Function         NFC_ConnClose
**
** Description      This function is called to close a logical connection with
**                  NFCC.
**
** Parameters       conn_id - the connection id.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_ConnClose(uint8_t conn_id) {
  tNFC_CONN_CB* p_cb = nfc_find_conn_cb_by_conn_id(conn_id);
  tNFC_STATUS status = NFC_STATUS_FAILED;

  if (p_cb) {
    status = nci_snd_core_conn_close(conn_id);
  }
  return status;
}

/*******************************************************************************
**
** Function         NFC_SetStaticRfCback
**
** Description      This function is called to update the data callback function
**                  to receive the data for the given connection id.
**
** Parameters       p_cback - the connection callback function
**
** Returns          Nothing
**
*******************************************************************************/
void NFC_SetStaticRfCback(tNFC_CONN_CBACK* p_cback) {
  tNFC_CONN_CB* p_cb = &nfc_cb.conn_cb[NFC_RF_CONN_ID];

  p_cb->p_cback = p_cback;
  /* just in case DH has received NCI data before the data callback is set
   * check if there's any data event to report on this connection id */
  nfc_data_event(p_cb);
}

#if (NXP_EXTNS == TRUE)
/*******************************************************************************
**
** Function         NFC_SetStaticT4tNfceeCback
**
** Description      This function is called to update the data callback function
**                  to receive the data for the given connection id.
**
** Parameters       p_cback - the connection callback function
**
** Returns          Nothing
**
*******************************************************************************/
void NFC_SetStaticT4tNfceeCback(tNFC_CONN_CBACK* p_cback) {
  // tNFC_CONN_CB * p_cb = &nfc_cb.conn_cb[];
  tNFC_CONN_CB* p_cb = nfc_find_conn_cb_by_conn_id(NFC_T4TNFCEE_CONN_ID);
  if (p_cb != NULL) {
    p_cb->p_cback = p_cback;
    /* just in case DH has received NCI data before the data callback is set
     * check if there's any data event to report on this connection id */
    nfc_data_event(p_cb);
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s = %p, p_cb->p_cback = %p", __func__, p_cb, p_cb->p_cback);
}
#endif

/*******************************************************************************
**
** Function         NFC_SetReassemblyFlag
**
** Description      This function is called to set if nfc will reassemble
**                  nci packet as much as its buffer can hold or it should not
**                  reassemble but forward the fragmented nci packet to layer
**                  above. If nci data pkt is fragmented, nfc may send multiple
**                  NFC_DATA_CEVT with status NFC_STATUS_CONTINUE before sending
**                  NFC_DATA_CEVT with status NFC_STATUS_OK based on reassembly
**                  configuration and reassembly buffer size
**
** Parameters       reassembly - flag to indicate if nfc may reassemble or not
**
** Returns          Nothing
**
*******************************************************************************/
void NFC_SetReassemblyFlag(bool reassembly) { nfc_cb.reassembly = reassembly; }

/*******************************************************************************
**
** Function         NFC_SendData
**
** Description      This function is called to send the given data packet
**                  to the connection identified by the given connection id.
**
** Parameters       conn_id - the connection id.
**                  p_data - the data packet.
**                  p_data->offset must be >= NCI_MSG_OFFSET_SIZE +
**                  NCI_DATA_HDR_SIZE
**                  The data payload starts at
**                  ((uint8_t *) (p_data + 1) + p_data->offset)
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_SendData(uint8_t conn_id, NFC_HDR* p_data) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tNFC_CONN_CB* p_cb = nfc_find_conn_cb_by_conn_id(conn_id);

  if (p_cb && p_data &&
      p_data->offset >= NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE) {
    status = nfc_ncif_send_data(p_cb, p_data);
  }

  if (status != NFC_STATUS_OK) GKI_freebuf(p_data);

  return status;
}

/*******************************************************************************
**
** Function         NFC_FlushData
**
** Description      This function is called to discard the tx data queue of
**                  the given connection id.
**
** Parameters       conn_id - the connection id.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_FlushData(uint8_t conn_id) {
  tNFC_STATUS status = NFC_STATUS_FAILED;
  tNFC_CONN_CB* p_cb = nfc_find_conn_cb_by_conn_id(conn_id);
  void* p_buf;

  if (p_cb) {
    status = NFC_STATUS_OK;
    while ((p_buf = GKI_dequeue(&p_cb->tx_q)) != nullptr) GKI_freebuf(p_buf);
  }

  return status;
}

/*******************************************************************************
**
** Function         NFC_Deactivate
**
** Description      This function is called to stop the discovery process or
**                  put the listen device in sleep mode or terminate the NFC
**                  link.
**
**                  The response from NFCC is reported by tNFC_DISCOVER_CBACK
**                  as NFC_DEACTIVATE_DEVT.
**
** Parameters       deactivate_type - NFC_DEACTIVATE_TYPE_IDLE, to IDLE mode.
**                                    NFC_DEACTIVATE_TYPE_SLEEP to SLEEP mode.
**                                    NFC_DEACTIVATE_TYPE_SLEEP_AF to SLEEP_AF
**                                    mode.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_Deactivate(tNFC_DEACT_TYPE deactivate_type) {
  tNFC_CONN_CB* p_cb = &nfc_cb.conn_cb[NFC_RF_CONN_ID];
  tNFC_STATUS status = NFC_STATUS_OK;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "NFC_Deactivate %d (%s) deactivate_type:%d", nfc_cb.nfc_state,
      nfc_state_name(nfc_cb.nfc_state).c_str(), deactivate_type);

  if (nfc_cb.flags & NFC_FL_DISCOVER_PENDING) {
    /* the HAL pre-discover is still active - clear the pending flag */
    nfc_cb.flags &= ~NFC_FL_DISCOVER_PENDING;
    if (!(nfc_cb.flags & NFC_FL_HAL_REQUESTED)) {
      /* if HAL did not request for control, clear this bit now */
      nfc_cb.flags &= ~NFC_FL_CONTROL_REQUESTED;
    }
    GKI_freebuf(nfc_cb.p_disc_pending);
    nfc_cb.p_disc_pending = nullptr;
    return NFC_STATUS_OK;
  }

  if (nfc_cb.nfc_state == NFC_STATE_OPEN) {
    nfc_set_state(NFC_STATE_CLOSING);
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("act_protocol %d credits:%d/%d", p_cb->act_protocol,
                        p_cb->init_credits, p_cb->num_buff);
    if ((p_cb->act_protocol == NCI_PROTOCOL_NFC_DEP) &&
        (p_cb->init_credits != p_cb->num_buff)) {
      nfc_cb.flags |= NFC_FL_DEACTIVATING;
      nfc_cb.deactivate_timer.param = (uintptr_t)deactivate_type;
      nfc_start_timer(&nfc_cb.deactivate_timer,
                      (uint16_t)(NFC_TTYPE_WAIT_2_DEACTIVATE),
                      NFC_DEACTIVATE_TIMEOUT);
      return status;
    }
  }

  status = nci_snd_deactivate_cmd(deactivate_type);
  return status;
}
/*******************************************************************************
**
** Function         NFC_SetPowerSubState
**
** Description      This function is called to send the power sub state (screen
**                  state) to NFCC. The response from NFCC is reported by
**                  tNFC_RESPONSE_CBACK as NFC_SET_POWER_STATE_REVT.
**
** Parameters       scree_state
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_SetPowerSubState(uint8_t screen_state) {
  return nci_snd_core_set_power_sub_state(screen_state);
}
/*******************************************************************************
**
** Function         NFC_UpdateRFCommParams
**
** Description      This function is called to update RF Communication
**                  parameters once the Frame RF Interface has been activated.
**
**                  The response from NFCC is reported by tNFC_RESPONSE_CBACK
**                  as NFC_RF_COMM_PARAMS_UPDATE_REVT.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_UpdateRFCommParams(tNFC_RF_COMM_PARAMS* p_params) {
  uint8_t tlvs[12];
  uint8_t* p = tlvs;
  uint8_t data_exch_config;

  /* RF Technology and Mode */
  if (p_params->include_rf_tech_mode) {
    UINT8_TO_STREAM(p, NCI_RF_PARAM_ID_TECH_N_MODE);
    UINT8_TO_STREAM(p, 1);
    UINT8_TO_STREAM(p, p_params->rf_tech_n_mode);
  }

  /* Transmit Bit Rate */
  if (p_params->include_tx_bit_rate) {
    UINT8_TO_STREAM(p, NCI_RF_PARAM_ID_TX_BIT_RATE);
    UINT8_TO_STREAM(p, 1);
    UINT8_TO_STREAM(p, p_params->tx_bit_rate);
  }

  /* Receive Bit Rate */
  if (p_params->include_tx_bit_rate) {
    UINT8_TO_STREAM(p, NCI_RF_PARAM_ID_RX_BIT_RATE);
    UINT8_TO_STREAM(p, 1);
    UINT8_TO_STREAM(p, p_params->rx_bit_rate);
  }

  /* NFC-B Data Exchange Configuration */
  if (p_params->include_nfc_b_config) {
    UINT8_TO_STREAM(p, NCI_RF_PARAM_ID_B_DATA_EX_PARAM);
    UINT8_TO_STREAM(p, 1);

    data_exch_config = (p_params->min_tr0 & 0x03) << 6; /* b7b6 : Mininum TR0 */
    data_exch_config |= (p_params->min_tr1 & 0x03)
                        << 4; /* b5b4 : Mininum TR1 */
    data_exch_config |= (p_params->suppression_eos & 0x01)
                        << 3; /* b3 :   Suppression of EoS */
    data_exch_config |= (p_params->suppression_sos & 0x01)
                        << 2; /* b2 :   Suppression of SoS */
    data_exch_config |= (p_params->min_tr2 & 0x03); /* b1b0 : Mininum TR2 */

    UINT8_TO_STREAM(p, data_exch_config);
  }

  return nci_snd_parameter_update_cmd(tlvs, (uint8_t)(p - tlvs));
}

/*******************************************************************************
**
** Function         NFC_SetPowerOffSleep
**
** Description      This function closes/opens transport and turns off/on NFCC.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_SetPowerOffSleep(bool enable) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("enable = %d", enable);

  if ((enable == false) &&
      (nfc_cb.nfc_state == NFC_STATE_NFCC_POWER_OFF_SLEEP)) {
    nfc_cb.flags |= NFC_FL_RESTARTING;

    /* open transport */
    nfc_set_state(NFC_STATE_W4_HAL_OPEN);
    nfc_cb.p_hal->open(nfc_main_hal_cback, nfc_main_hal_data_cback);

    return NFC_STATUS_OK;
  } else if ((enable == true) && (nfc_cb.nfc_state == NFC_STATE_IDLE)) {
    /* close transport to turn off NFCC and clean up */
    nfc_cb.flags |= NFC_FL_POWER_OFF_SLEEP;
    nfc_task_shutdown_nfcc();

    return NFC_STATUS_OK;
  }

  LOG(ERROR) << StringPrintf("invalid state = %d", nfc_cb.nfc_state);
  return NFC_STATUS_FAILED;
}

/*******************************************************************************
**
** Function         NFC_PowerCycleNFCC
**
** Description      This function turns off and then on NFCC.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_PowerCycleNFCC(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  if (nfc_cb.nfc_state == NFC_STATE_IDLE) {
    /* power cycle NFCC */
    nfc_cb.flags |= NFC_FL_POWER_CYCLE_NFCC;
    nfc_task_shutdown_nfcc();

    return NFC_STATUS_OK;
  }

  LOG(ERROR) << StringPrintf("invalid state = %d", nfc_cb.nfc_state);
  return NFC_STATUS_FAILED;
}

/*******************************************************************************
**
** Function         NFC_SetTraceLevel
**
** Description      This function sets the trace level for NFC.  If called with
**                  a value of 0xFF, it simply returns the current trace level.
**
** Returns          The new or current trace level
**
*******************************************************************************/
UINT8 NFC_SetTraceLevel (UINT8 new_level)
{
    NFC_TRACE_API1 ("NFC_SetTraceLevel () new_level = %d", new_level);

    if (new_level != 0xFF)
        nfc_cb.trace_level = new_level;

    return (nfc_cb.trace_level);
}

/*******************************************************************************
**
** Function         NFC_GetNCIVersion
**
** Description      Called by higher layer to get the current nci
**                  version of nfc.
**
** Returns          NCI version NCI2.0 / NCI1.0
**
*******************************************************************************/
uint8_t NFC_GetNCIVersion() { return nfc_cb.nci_version; }

/*******************************************************************************
**
** Function         NFC_ISODEPNakPresCheck
**
** Description      This function is called to send the ISO DEP nak presenc
**                  check cmd to check that the remote end point in RF field.
**
**                  The response from NFCC is reported by call back.The ntf
**                  indicates success if card is present in field or failed
**                  if card is lost.
**
** Returns          tNFC_STATUS
**
*******************************************************************************/
tNFC_STATUS NFC_ISODEPNakPresCheck() {
  return nci_snd_iso_dep_nak_presence_check_cmd();
}

/*******************************************************************************
**
** Function         NFC_SetStaticHciCback
**
** Description      This function is called to update the data callback function
**                  to receive the data for the static Hci connection id.
**
** Parameters       p_cback - the connection callback function
**
** Returns          Nothing
**
*******************************************************************************/
void NFC_SetStaticHciCback(tNFC_CONN_CBACK* p_cback) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s dest: %d", __func__, NCI_DEST_TYPE_NFCEE);
  tNFC_CONN_CB* p_cb = &nfc_cb.conn_cb[NFC_HCI_CONN_ID];
  tNFC_CONN evt_data;

  p_cb->p_cback = p_cback;
  if (p_cback && p_cb->buff_size && p_cb->num_buff) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s dest: %d", __func__, NCI_DEST_TYPE_NFCEE);
    evt_data.conn_create.status = NFC_STATUS_OK;
    evt_data.conn_create.dest_type = NCI_DEST_TYPE_NFCEE;
    evt_data.conn_create.id = p_cb->id;
    evt_data.conn_create.buff_size = p_cb->buff_size;
    evt_data.conn_create.num_buffs = p_cb->num_buff;
    (*p_cback)(NFC_HCI_CONN_ID, NFC_CONN_CREATE_CEVT, &evt_data);
  }
}

/*******************************************************************************
**
** Function         NFC_GetStatusName
**
** Description      This function returns the status name.
**
** NOTE             conditionally compiled to save memory.
**
** Returns          pointer to the name
**
*******************************************************************************/
std::string NFC_GetStatusName(tNFC_STATUS status) {
  switch (status) {
    case NFC_STATUS_OK:
      return "OK";
    case NFC_STATUS_REJECTED:
      return "REJECTED";
    case NFC_STATUS_MSG_CORRUPTED:
      return "CORRUPTED";
    case NFC_STATUS_BUFFER_FULL:
      return "BUFFER_FULL";
    case NFC_STATUS_FAILED:
      return "FAILED";
    case NFC_STATUS_NOT_INITIALIZED:
      return "NOT_INITIALIZED";
    case NFC_STATUS_SYNTAX_ERROR:
      return "SYNTAX_ERROR";
    case NFC_STATUS_SEMANTIC_ERROR:
      return "SEMANTIC_ERROR";
    case NFC_STATUS_UNKNOWN_GID:
      return "UNKNOWN_GID";
    case NFC_STATUS_UNKNOWN_OID:
      return "UNKNOWN_OID";
    case NFC_STATUS_INVALID_PARAM:
      return "INVALID_PARAM";
    case NFC_STATUS_MSG_SIZE_TOO_BIG:
      return "MSG_SIZE_TOO_BIG";
    case NFC_STATUS_ALREADY_STARTED:
      return "ALREADY_STARTED";
    case NFC_STATUS_ACTIVATION_FAILED:
      return "ACTIVATION_FAILED";
    case NFC_STATUS_TEAR_DOWN:
      return "TEAR_DOWN";
    case NFC_STATUS_RF_TRANSMISSION_ERR:
      return "RF_TRANSMISSION_ERR";
    case NFC_STATUS_RF_PROTOCOL_ERR:
      return "RF_PROTOCOL_ERR";
    case NFC_STATUS_TIMEOUT:
      return "TIMEOUT";
    case NFC_STATUS_EE_INTF_ACTIVE_FAIL:
      return "EE_INTF_ACTIVE_FAIL";
    case NFC_STATUS_EE_TRANSMISSION_ERR:
      return "EE_TRANSMISSION_ERR";
    case NFC_STATUS_EE_PROTOCOL_ERR:
      return "EE_PROTOCOL_ERR";
    case NFC_STATUS_EE_TIMEOUT:
      return "EE_TIMEOUT";
    case NFC_STATUS_CMD_STARTED:
      return "CMD_STARTED";
    case NFC_STATUS_HW_TIMEOUT:
      return "HW_TIMEOUT";
    case NFC_STATUS_CONTINUE:
      return "CONTINUE";
    case NFC_STATUS_REFUSED:
      return "REFUSED";
    case NFC_STATUS_BAD_RESP:
      return "BAD_RESP";
    case NFC_STATUS_CMD_NOT_CMPLTD:
      return "CMD_NOT_CMPLTD";
    case NFC_STATUS_NO_BUFFERS:
      return "NO_BUFFERS";
    case NFC_STATUS_WRONG_PROTOCOL:
      return "WRONG_PROTOCOL";
    case NFC_STATUS_BUSY:
      return "BUSY";
    case NFC_STATUS_LINK_LOSS:
      return "LINK_LOSS";
    case NFC_STATUS_BAD_LENGTH:
      return "BAD_LENGTH";
    case NFC_STATUS_BAD_HANDLE:
      return "BAD_HANDLE";
    case NFC_STATUS_CONGESTED:
      return "CONGESTED";
    default:
      return "UNKNOWN";
  }
}
