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
 *  This file contains the action functions for NFA-EE
 *
 ******************************************************************************/
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nfa_api.h"
#include "nfa_dm_int.h"
#include "nfa_ee_int.h"
#include "nfa_hci_int.h"

#if (NXP_EXTNS == TRUE)
#include "nfa_nfcee_int.h"
#include "nfc_config.h"
#endif

#include <statslog.h>
#include "metrics.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/* the de-bounce timer:
 * The NFA-EE API functions are called to set the routing and VS configuration.
 * When this timer expires, the configuration is sent to NFCC all at once.
 * This is the timeout value for the de-bounce timer. */
#ifndef NFA_EE_ROUT_TIMEOUT_VAL
#define NFA_EE_ROUT_TIMEOUT_VAL 1000
#endif

#define NFA_EE_ROUT_BUF_SIZE 540
#define NFA_EE_ROUT_MAX_TLV_SIZE 0xFD

/* the following 2 tables convert the technology mask in API and control block
 * to the command for NFCC */
#define NFA_EE_NUM_TECH 3
const uint8_t nfa_ee_tech_mask_list[NFA_EE_NUM_TECH] = {
    NFA_TECHNOLOGY_MASK_A, NFA_TECHNOLOGY_MASK_B, NFA_TECHNOLOGY_MASK_F};

const uint8_t nfa_ee_tech_list[NFA_EE_NUM_TECH] = {
    NFC_RF_TECHNOLOGY_A, NFC_RF_TECHNOLOGY_B, NFC_RF_TECHNOLOGY_F};

/* the following 2 tables convert the protocol mask in API and control block to
 * the command for NFCC */
#define NFA_EE_NUM_PROTO 5

static void add_route_tech_proto_tlv(uint8_t** pp, uint8_t tlv_type,
                                     uint8_t nfcee_id, uint8_t pwr_cfg,
                                     uint8_t tech_proto) {
  *(*pp)++ = tlv_type;
  *(*pp)++ = 3;
  *(*pp)++ = nfcee_id;
  *(*pp)++ = pwr_cfg;
  *(*pp)++ = tech_proto;
}

static void add_route_aid_tlv(uint8_t** pp, uint8_t* pa, uint8_t nfcee_id,
                              uint8_t pwr_cfg, uint8_t tag) {
  pa++;                /* EMV tag */
  uint8_t len = *pa++; /* aid_len */
  *(*pp)++ = tag;
  *(*pp)++ = len + 2;
  *(*pp)++ = nfcee_id;
  *(*pp)++ = pwr_cfg;
  /* copy the AID */
  memcpy(*pp, pa, len);
  *pp += len;
}

static void add_route_sys_code_tlv(uint8_t** p_buff, uint8_t* p_sys_code_cfg,
                                   uint8_t sys_code_rt_loc,
                                   uint8_t sys_code_pwr_cfg) {
  *(*p_buff)++ = NFC_ROUTE_TAG_SYSCODE | nfa_ee_cb.route_block_control;
  *(*p_buff)++ = NFA_EE_SYSTEM_CODE_LEN + 2;
  *(*p_buff)++ = sys_code_rt_loc;
  *(*p_buff)++ = sys_code_pwr_cfg;
  /* copy the system code */
  memcpy(*p_buff, p_sys_code_cfg, NFA_EE_SYSTEM_CODE_LEN);
  *p_buff += NFA_EE_SYSTEM_CODE_LEN;
}

const uint8_t nfa_ee_proto_mask_list[NFA_EE_NUM_PROTO] = {
    NFA_PROTOCOL_MASK_T1T, NFA_PROTOCOL_MASK_T2T, NFA_PROTOCOL_MASK_T3T,
    NFA_PROTOCOL_MASK_ISO_DEP, NFA_PROTOCOL_MASK_NFC_DEP};

const uint8_t nfa_ee_proto_list[NFA_EE_NUM_PROTO] = {
    NFC_PROTOCOL_T1T, NFC_PROTOCOL_T2T, NFC_PROTOCOL_T3T, NFC_PROTOCOL_ISO_DEP,
    NFC_PROTOCOL_NFC_DEP};

static void nfa_ee_report_discover_req_evt(void);
static void nfa_ee_build_discover_req_evt(tNFA_EE_DISCOVER_REQ* p_evt_data);
void nfa_ee_check_set_routing(uint16_t new_size, int* p_max_len, uint8_t* p,
                              int* p_cur_offset);

#if (NXP_EXTNS == TRUE)
static void nfa_ee_add_t4tnfcee_aid(uint8_t* p, int* cur_offset);
#endif

/*******************************************************************************
**
** Function         nfa_ee_trace_aid
**
** Description      trace AID
**
** Returns          void
**
*******************************************************************************/
static void nfa_ee_trace_aid(std::string p_str, uint8_t id, uint8_t aid_len,
                             uint8_t* p) {
  int len = aid_len;
  int xx, yy = 0;
  const uint8_t MAX_BUFF_SIZE = 100;
  char buff[MAX_BUFF_SIZE];

  buff[0] = 0;
  if (aid_len > NFA_MAX_AID_LEN) {
    LOG(ERROR) << StringPrintf("aid_len: %d exceeds max(%d)", aid_len,
                               NFA_MAX_AID_LEN);
    len = NFA_MAX_AID_LEN;
  }
  for (xx = 0; xx < len; xx++) {
    yy += snprintf(&buff[yy], MAX_BUFF_SIZE - yy, "%02x ", *p);
    p++;
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s id:0x%x len=%d aid:%s", p_str.c_str(), id, aid_len, buff);
}

/*******************************************************************************
**
** Function         nfa_ee_update_route_size
**
** Description      Update the size required for technology and protocol routing
**                  of the given NFCEE ID.
**
** Returns          void
**
*******************************************************************************/
static void nfa_ee_update_route_size(tNFA_EE_ECB* p_cb) {
  int xx;
  uint8_t power_cfg = 0;

  p_cb->size_mask_proto = 0;
  p_cb->size_mask_tech = 0;
  /* add the Technology based routing */
  for (xx = 0; xx < NFA_EE_NUM_TECH; xx++) {
    power_cfg = 0;
    if (p_cb->tech_switch_on & nfa_ee_tech_mask_list[xx])
      power_cfg |= NCI_ROUTE_PWR_STATE_ON;
    if (p_cb->tech_switch_off & nfa_ee_tech_mask_list[xx])
      power_cfg |= NCI_ROUTE_PWR_STATE_SWITCH_OFF;
    if (p_cb->tech_battery_off & nfa_ee_tech_mask_list[xx])
      power_cfg |= NCI_ROUTE_PWR_STATE_BATT_OFF;
    if ((power_cfg & NCI_ROUTE_PWR_STATE_ON) &&
        (NFC_GetNCIVersion() == NCI_VERSION_2_0)) {
      if (p_cb->tech_screen_lock & nfa_ee_tech_mask_list[xx])
        power_cfg |= NCI_ROUTE_PWR_STATE_SCREEN_ON_LOCK();
      if (p_cb->tech_screen_off & nfa_ee_tech_mask_list[xx])
        power_cfg |= NCI_ROUTE_PWR_STATE_SCREEN_OFF_UNLOCK();
      if (p_cb->tech_screen_off_lock & nfa_ee_tech_mask_list[xx])
        power_cfg |= NCI_ROUTE_PWR_STATE_SCREEN_OFF_LOCK();
    }
    if (power_cfg) {
      /* 5 = 1 (tag) + 1 (len) + 1(nfcee_id) + 1(power cfg) + 1 (technology) */
      p_cb->size_mask_tech += 5;
    }
  }

  /* add the Protocol based routing */
  for (xx = 0; xx < NFA_EE_NUM_PROTO; xx++) {
    power_cfg = 0;
    if (p_cb->proto_switch_on & nfa_ee_proto_mask_list[xx])
      power_cfg |= NCI_ROUTE_PWR_STATE_ON;
    if (p_cb->proto_switch_off & nfa_ee_proto_mask_list[xx])
      power_cfg |= NCI_ROUTE_PWR_STATE_SWITCH_OFF;
    if (p_cb->proto_battery_off & nfa_ee_proto_mask_list[xx])
      power_cfg |= NCI_ROUTE_PWR_STATE_BATT_OFF;
    if ((power_cfg & NCI_ROUTE_PWR_STATE_ON) &&
        (NFC_GetNCIVersion() == NCI_VERSION_2_0)) {
      if (p_cb->proto_screen_lock & nfa_ee_proto_mask_list[xx])
        power_cfg |= NCI_ROUTE_PWR_STATE_SCREEN_ON_LOCK();
      if (p_cb->proto_screen_off & nfa_ee_proto_mask_list[xx])
        power_cfg |= NCI_ROUTE_PWR_STATE_SCREEN_OFF_UNLOCK();
      if (p_cb->proto_screen_off_lock & nfa_ee_proto_mask_list[xx])
        power_cfg |= NCI_ROUTE_PWR_STATE_SCREEN_OFF_LOCK();
    }

    // NFC-DEP must route to HOST
    if (power_cfg ||
        (p_cb->nfcee_id == NFC_DH_ID &&
         nfa_ee_proto_mask_list[xx] == NFA_PROTOCOL_MASK_NFC_DEP)) {
      /* 5 = 1 (tag) + 1 (len) + 1(nfcee_id) + 1(power cfg) + 1 (protocol) */
      p_cb->size_mask_proto += 5;
    }
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nfa_ee_update_route_size nfcee_id:0x%x size_mask_proto:%d "
      "size_mask_tech:%d",
      p_cb->nfcee_id, p_cb->size_mask_proto, p_cb->size_mask_tech);
}

/*******************************************************************************
**
** Function         nfa_ee_update_route_aid_size
**
** Description      Update the size required for AID routing
**                  of the given NFCEE ID.
**
** Returns          void
**
*******************************************************************************/
static void nfa_ee_update_route_aid_size(tNFA_EE_ECB* p_cb) {
  uint8_t *pa, len;
  int start_offset;
  int xx;

  p_cb->size_aid = 0;
  if (p_cb->aid_entries) {
    start_offset = 0;
    for (xx = 0; xx < p_cb->aid_entries; xx++) {
      /* add one AID entry */
      if (p_cb->aid_rt_info[xx] & NFA_EE_AE_ROUTE) {
        pa = &p_cb->aid_cfg[start_offset];
        pa++;        /* EMV tag */
        len = *pa++; /* aid_len */
        /* 4 = 1 (tag) + 1 (len) + 1(nfcee_id) + 1(power cfg) */
        p_cb->size_aid += 4;
        p_cb->size_aid += len;
      }
      start_offset += p_cb->aid_len[xx];
    }
  }
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_ee_update_route_aid_size nfcee_id:0x%x size_aid:%d",
                      p_cb->nfcee_id, p_cb->size_aid);
}

/*******************************************************************************
**
** Function         nfa_ee_update_route_sys_code_size
**
** Description      Update the size required for system code routing
**                  of the given NFCEE ID.
**
** Returns          void
**
*******************************************************************************/
static void nfa_ee_update_route_sys_code_size(tNFA_EE_ECB* p_cb) {
  p_cb->size_sys_code = 0;
  if (p_cb->sys_code_cfg_entries) {
    for (uint8_t xx = 0; xx < p_cb->sys_code_cfg_entries; xx++) {
      if (p_cb->sys_code_rt_loc_vs_info[xx] & NFA_EE_AE_ROUTE) {
        /* 4 = 1 (tag) + 1 (len) + 1(nfcee_id) + 1(power cfg) */
        p_cb->size_sys_code += 4;
        p_cb->size_sys_code += NFA_EE_SYSTEM_CODE_LEN;
      }
    }
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nfa_ee_update_route_sys_code_size nfcee_id:0x%x size_sys_code:%d",
      p_cb->nfcee_id, p_cb->size_sys_code);
}

/*******************************************************************************
**
** Function         nfa_ee_total_lmrt_size
**
** Description      the total listen mode routing table size
**
** Returns          uint16_t
**
*******************************************************************************/
static uint16_t nfa_ee_total_lmrt_size(void) {
  int xx;
  uint16_t lmrt_size = 0;
  tNFA_EE_ECB* p_cb;

  p_cb = &nfa_ee_cb.ecb[NFA_EE_CB_4_DH];
  lmrt_size += p_cb->size_mask_proto;
  lmrt_size += p_cb->size_mask_tech;
  lmrt_size += p_cb->size_aid;
  lmrt_size += p_cb->size_sys_code;
  if (nfa_ee_cb.cur_ee > 0) p_cb = &nfa_ee_cb.ecb[nfa_ee_cb.cur_ee - 1];
  for (xx = 0; xx < nfa_ee_cb.cur_ee; xx++, p_cb--) {
    if (p_cb->ee_status == NFC_NFCEE_STATUS_ACTIVE) {
      lmrt_size += p_cb->size_mask_proto;
      lmrt_size += p_cb->size_mask_tech;
      lmrt_size += p_cb->size_aid;
      lmrt_size += p_cb->size_sys_code;
    }
  }
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_ee_total_lmrt_size size:%d", lmrt_size);
  return lmrt_size;
}

static void nfa_ee_add_tech_route_to_ecb(tNFA_EE_ECB* p_cb, uint8_t* pp,
                                         uint8_t* p, uint8_t* ps,
                                         int* p_cur_offset) {
  uint8_t num_tlv = *ps;

  /* add the Technology based routing */
  for (int xx = 0; xx < NFA_EE_NUM_TECH; xx++) {
    uint8_t power_cfg = 0;
    if (p_cb->tech_switch_on & nfa_ee_tech_mask_list[xx])
      power_cfg |= NCI_ROUTE_PWR_STATE_ON;
    if (p_cb->tech_switch_off & nfa_ee_tech_mask_list[xx])
      power_cfg |= NCI_ROUTE_PWR_STATE_SWITCH_OFF;
    if (p_cb->tech_battery_off & nfa_ee_tech_mask_list[xx])
      power_cfg |= NCI_ROUTE_PWR_STATE_BATT_OFF;
    if ((power_cfg & NCI_ROUTE_PWR_STATE_ON) &&
        (NFC_GetNCIVersion() == NCI_VERSION_2_0)) {
      if (p_cb->tech_screen_lock & nfa_ee_tech_mask_list[xx])
        power_cfg |= NCI_ROUTE_PWR_STATE_SCREEN_ON_LOCK();
      if (p_cb->tech_screen_off & nfa_ee_tech_mask_list[xx])
        power_cfg |= NCI_ROUTE_PWR_STATE_SCREEN_OFF_UNLOCK();
      if (p_cb->tech_screen_off_lock & nfa_ee_tech_mask_list[xx])
        power_cfg |= NCI_ROUTE_PWR_STATE_SCREEN_OFF_LOCK();
    }
    if (power_cfg) {
      add_route_tech_proto_tlv(&pp, NFC_ROUTE_TAG_TECH, p_cb->nfcee_id,
                               power_cfg, nfa_ee_tech_list[xx]);
      num_tlv++;
      if (power_cfg != NCI_ROUTE_PWR_STATE_ON)
        nfa_ee_cb.ee_cfged |= NFA_EE_CFGED_OFF_ROUTING;
    }
  }

  /* update the num_tlv and current offset */
  uint8_t entry_size = (uint8_t)(pp - p);
  *p_cur_offset += entry_size;
  *ps = num_tlv;
}

static void nfa_ee_add_proto_route_to_ecb(tNFA_EE_ECB* p_cb, uint8_t* pp,
                                          uint8_t* p, uint8_t* ps,
                                          int* p_cur_offset) {
  uint8_t num_tlv = *ps;

  /* add the Protocol based routing */
  for (int xx = 0; xx < NFA_EE_NUM_PROTO; xx++) {
    uint8_t power_cfg = 0, proto_tag = 0;
    if (p_cb->proto_switch_on & nfa_ee_proto_mask_list[xx])
      power_cfg |= NCI_ROUTE_PWR_STATE_ON;
    if (p_cb->proto_switch_off & nfa_ee_proto_mask_list[xx])
      power_cfg |= NCI_ROUTE_PWR_STATE_SWITCH_OFF;
    if (p_cb->proto_battery_off & nfa_ee_proto_mask_list[xx])
      power_cfg |= NCI_ROUTE_PWR_STATE_BATT_OFF;
    if (power_cfg ||
        (p_cb->nfcee_id == NFC_DH_ID &&
         nfa_ee_proto_mask_list[xx] == NFA_PROTOCOL_MASK_NFC_DEP)) {
      /* Applying Route Block for ISO DEP Protocol, so that AIDs
       * which are not in the routing table can also be blocked */
      if (nfa_ee_proto_mask_list[xx] == NFA_PROTOCOL_MASK_ISO_DEP) {
        proto_tag = NFC_ROUTE_TAG_PROTO | nfa_ee_cb.route_block_control;

        /* Enable screen on lock power state for ISO-DEP protocol to
           enable HCE screen lock */
        if ((power_cfg & NCI_ROUTE_PWR_STATE_ON) &&
            (NFC_GetNCIVersion() == NCI_VERSION_2_0)) {
          if (p_cb->proto_screen_lock & nfa_ee_proto_mask_list[xx])
            power_cfg |= NCI_ROUTE_PWR_STATE_SCREEN_ON_LOCK();
          if (p_cb->proto_screen_off & nfa_ee_proto_mask_list[xx])
            power_cfg |= NCI_ROUTE_PWR_STATE_SCREEN_OFF_UNLOCK();
          if (p_cb->proto_screen_off_lock & nfa_ee_proto_mask_list[xx])
            power_cfg |= NCI_ROUTE_PWR_STATE_SCREEN_OFF_LOCK();
        }
      } else {
        proto_tag = NFC_ROUTE_TAG_PROTO;
      }
      if (p_cb->nfcee_id == NFC_DH_ID &&
          nfa_ee_proto_mask_list[xx] == NFA_PROTOCOL_MASK_NFC_DEP) {
        /* add NFC-DEP routing to HOST */
        add_route_tech_proto_tlv(&pp, NFC_ROUTE_TAG_PROTO, NFC_DH_ID,
                                 NCI_ROUTE_PWR_STATE_ON, NFC_PROTOCOL_NFC_DEP);
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s - NFC DEP added for DH!!!", __func__);
      } else {
        add_route_tech_proto_tlv(&pp, proto_tag, p_cb->nfcee_id, power_cfg,
                                 nfa_ee_proto_list[xx]);
      }
      num_tlv++;
      if (power_cfg != NCI_ROUTE_PWR_STATE_ON)
        nfa_ee_cb.ee_cfged |= NFA_EE_CFGED_OFF_ROUTING;
    }
  }

  /* update the num_tlv and current offset */
  uint8_t entry_size = (uint8_t)(pp - p);
  *p_cur_offset += entry_size;
  *ps = num_tlv;
}

static void nfa_ee_add_aid_route_to_ecb(tNFA_EE_ECB* p_cb, uint8_t* pp,
                                        uint8_t* p, uint8_t* ps,
                                        int* p_cur_offset, int* p_max_len) {
  uint8_t num_tlv = *ps;

  /* add the AID routing */
  if (p_cb->aid_entries) {
    int start_offset = 0;
    for (int xx = 0; xx < p_cb->aid_entries; xx++) {
      /* remember the beginning of this AID routing entry, just in case we
       * need to put it in next command */
      uint8_t route_qual = 0;
      uint8_t* p_start = pp;
      /* add one AID entry */
      if (p_cb->aid_rt_info[xx] & NFA_EE_AE_ROUTE) {
        num_tlv++;
        uint8_t* pa = &p_cb->aid_cfg[start_offset];

        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s -  p_cb->aid_info%x", __func__, p_cb->aid_info[xx]);
        if (p_cb->aid_info[xx] & NCI_ROUTE_QUAL_LONG_SELECT) {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s - %x", __func__,
                              p_cb->aid_info[xx] & NCI_ROUTE_QUAL_LONG_SELECT);
          route_qual |= NCI_ROUTE_QUAL_LONG_SELECT;
        }
        if (p_cb->aid_info[xx] & NCI_ROUTE_QUAL_SHORT_SELECT) {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s - %x", __func__,
                              p_cb->aid_info[xx] & NCI_ROUTE_QUAL_SHORT_SELECT);
          route_qual |= NCI_ROUTE_QUAL_SHORT_SELECT;
        }

        uint8_t tag =
            NFC_ROUTE_TAG_AID | nfa_ee_cb.route_block_control | route_qual;

        add_route_aid_tlv(&pp, pa, p_cb->nfcee_id, p_cb->aid_pwr_cfg[xx], tag);
      }
      start_offset += p_cb->aid_len[xx];
      uint8_t new_size = (uint8_t)(pp - p_start);
      nfa_ee_check_set_routing(new_size, p_max_len, ps, p_cur_offset);
      if (*ps == 0) {
        /* just sent routing command, update local */
        *ps = 1;
        num_tlv = *ps;
        *p_cur_offset = new_size;
        pp = ps + 1;
        p = pp;
        memcpy(p, p_start, new_size);
        pp += new_size;
      } else {
        /* add the new entry */
        *ps = num_tlv;
        *p_cur_offset += new_size;
      }
    }
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s - No AID entries available", __func__);
  }
}

static void nfa_ee_add_sys_code_route_to_ecb(tNFA_EE_ECB* p_cb, uint8_t* pp,
                                             uint8_t* p, uint8_t* p_buff,
                                             int* p_cur_offset,
                                             int* p_max_len) {
  uint8_t num_tlv = *p_buff;

  /* add the SC routing */
  if (p_cb->sys_code_cfg_entries) {
    int start_offset = 0;
    for (int xx = 0; xx < p_cb->sys_code_cfg_entries; xx++) {
      /* remember the beginning of this SC routing entry, just in case we
       * need to put it in next command */
      uint8_t* p_start = pp;
      /* add one SC entry */
      if (p_cb->sys_code_rt_loc_vs_info[xx] & NFA_EE_AE_ROUTE) {
        uint8_t* p_sys_code_cfg = &p_cb->sys_code_cfg[start_offset];
        if (nfa_ee_is_active(p_cb->sys_code_rt_loc[xx] | NFA_HANDLE_GROUP_EE)) {
          add_route_sys_code_tlv(&pp, p_sys_code_cfg, p_cb->sys_code_rt_loc[xx],
                                 p_cb->sys_code_pwr_cfg[xx]);
          p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_ROUTING;
          num_tlv++;
        } else {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s -  ignoring route loc%x", __func__,
                              p_cb->sys_code_rt_loc[xx]);
        }
      }
      start_offset += NFA_EE_SYSTEM_CODE_LEN;
      uint8_t new_size = (uint8_t)(pp - p_start);
      nfa_ee_check_set_routing(new_size, p_max_len, p_buff, p_cur_offset);
      if (*p_buff == 0 && (num_tlv > 0x00)) {
        /* just sent routing command, update local */
        *p_buff = 1;
        num_tlv = *p_buff;
        *p_cur_offset = new_size;
        pp = p_buff + 1;
        p = pp;
        memcpy(p, p_start, new_size);
        pp += new_size;
      } else {
        /* add the new entry */
        *p_buff = num_tlv;
        *p_cur_offset += new_size;
      }
    }
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "nfa_ee_route_add_one_ecb_by_route_order --num_tlv:- %d", num_tlv);
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s - No SC entries available", __func__);
  }
}

/*******************************************************************************
**
** Function         nfa_ee_conn_cback
**
** Description      process connection callback event from stack
**
** Returns          void
**
*******************************************************************************/
static void nfa_ee_conn_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                              tNFC_CONN* p_data) {
  tNFA_EE_NCI_CONN cbk;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nfa_ee_conn_cback: conn_id: %d, event=0x%02x", conn_id, event);

  cbk.hdr.event = NFA_EE_NCI_CONN_EVT;
  if (event == NFC_DATA_CEVT) {
    /* Treat data event specially to avoid potential memory leak */
    cbk.hdr.event = NFA_EE_NCI_DATA_EVT;
  }
  cbk.conn_id = conn_id;
  cbk.event = event;
  cbk.p_data = p_data;
  tNFA_EE_MSG nfa_ee_msg;
  nfa_ee_msg.conn = cbk;

  nfa_ee_evt_hdlr(&nfa_ee_msg.hdr);
}

/*******************************************************************************
**
** Function         nfa_ee_find_max_aid_cfg_len
**
** Description      Find the max len for aid_cfg
**
** Returns          max length
**
*******************************************************************************/
int nfa_ee_find_max_aid_cfg_len(void) {
  int max_lmrt_size = NFC_GetLmrtSize();
  if (max_lmrt_size) {
    return max_lmrt_size - NFA_EE_MAX_PROTO_TECH_EXT_ROUTE_LEN;
  } else {
    return NFA_EE_MAX_AID_CFG_LEN;
  }
}

/*******************************************************************************
**
** Function         nfa_ee_find_total_aid_len
**
** Description      Find the total len in aid_cfg from start_entry to the last
**
** Returns          void
**
*******************************************************************************/
int nfa_ee_find_total_aid_len(tNFA_EE_ECB* p_cb, int start_entry) {
  int len = 0, xx;

  if (p_cb->aid_entries > start_entry) {
    for (xx = start_entry; xx < p_cb->aid_entries; xx++) {
      len += p_cb->aid_len[xx];
    }
  }
  return len;
}

/*******************************************************************************
**
** Function         nfa_ee_find_total_sys_code_len
**
** Description      Find the total len in sys_code_cfg from start_entry to the
**                  last in the given ecb.
**
** Returns          void
**
*******************************************************************************/
int nfa_ee_find_total_sys_code_len(tNFA_EE_ECB* p_cb, int start_entry) {
  int len = 0;
  if (p_cb->sys_code_cfg_entries > start_entry) {
    for (int xx = start_entry; xx < p_cb->sys_code_cfg_entries; xx++) {
      len += NFA_EE_SYSTEM_CODE_LEN;
    }
  }
  return len;
}

/*******************************************************************************
**
** Function         nfa_all_ee_find_total_sys_code_len
**
** Description      Find the total len in sys_code_cfg from start_entry to the
**                  last for all EE and DH.
**
** Returns          total length
**
*******************************************************************************/
int nfa_all_ee_find_total_sys_code_len() {
  int total_len = 0;
  for (int32_t xx = 0; xx < NFA_EE_NUM_ECBS; xx++) {
    tNFA_EE_ECB* p_cb = &nfa_ee_cb.ecb[xx];
    total_len += nfa_ee_find_total_sys_code_len(p_cb, 0);
  }
  return total_len;
}

/*******************************************************************************
**
** Function         nfa_ee_find_aid_offset
**
** Description      Given the AID, find the associated tNFA_EE_ECB and the
**                  offset in aid_cfg[]. *p_entry is the index.
**
** Returns          void
**
*******************************************************************************/
tNFA_EE_ECB* nfa_ee_find_aid_offset(uint8_t aid_len, uint8_t* p_aid,
                                    int* p_offset, int* p_entry) {
  int xx, yy, aid_len_offset, offset;
  tNFA_EE_ECB *p_ret = nullptr, *p_ecb;

  p_ecb = &nfa_ee_cb.ecb[NFA_EE_CB_4_DH];
  aid_len_offset = 1; /* skip the tag */
  for (yy = 0; yy <= nfa_ee_cb.cur_ee; yy++) {
    if (p_ecb->aid_entries) {
      offset = 0;
      for (xx = 0; xx < p_ecb->aid_entries; xx++) {
        if ((p_ecb->aid_cfg[offset + aid_len_offset] == aid_len) &&
            (memcmp(&p_ecb->aid_cfg[offset + aid_len_offset + 1], p_aid,
                    aid_len) == 0)) {
          p_ret = p_ecb;
          if (p_offset) *p_offset = offset;
          if (p_entry) *p_entry = xx;
          break;
        }
        offset += p_ecb->aid_len[xx];
      }

      if (p_ret) {
        /* found the entry already */
        break;
      }
    }
    p_ecb = &nfa_ee_cb.ecb[yy];
  }

  return p_ret;
}

/*******************************************************************************
 **
 ** Function         nfa_ee_find_sys_code_offset
 **
 ** Description      Given the System Code, find the associated tNFA_EE_ECB and
 *the
 **                  offset in sys_code_cfg[]. *p_entry is the index.
 **
 ** Returns          void
 **
 *******************************************************************************/
tNFA_EE_ECB* nfa_ee_find_sys_code_offset(uint16_t sys_code, int* p_offset,
                                         int* p_entry) {
  tNFA_EE_ECB* p_ret = nullptr;

  for (uint8_t xx = 0; xx < NFA_EE_NUM_ECBS; xx++) {
    tNFA_EE_ECB* p_ecb = &nfa_ee_cb.ecb[xx];
    uint8_t mask = nfa_ee_ecb_to_mask(p_ecb);
    if ((nfa_ee_cb.ee_cfged & mask) == 0 || p_ecb->sys_code_cfg_entries == 0) {
      continue; /*try next ecb*/
    }
    if (p_ecb->sys_code_cfg_entries) {
      uint8_t offset = 0;
      for (uint8_t yy = 0; yy < p_ecb->sys_code_cfg_entries; yy++) {
        if ((memcmp(&p_ecb->sys_code_cfg[offset], &sys_code,
                    NFA_EE_SYSTEM_CODE_LEN) == 0)) {
          p_ret = p_ecb;
          if (p_offset) *p_offset = offset;
          if (p_entry) *p_entry = yy;
          break;
        }
        offset += NFA_EE_SYSTEM_CODE_LEN;
      }

      if (p_ret) {
        /* found the entry already */
        return p_ret;
      }
    }
  }
  return p_ret;
}

/*******************************************************************************
**
** Function         nfa_ee_report_event
**
** Description      report the given event to the callback
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_report_event(tNFA_EE_CBACK* p_cback, tNFA_EE_EVT event,
                         tNFA_EE_CBACK_DATA* p_data) {
  int xx;

  /* use the given callback, if not NULL */
  if (p_cback) {
    (*p_cback)(event, p_data);
    return;
  }
  /* if the given is NULL, report to all registered ones */
  for (xx = 0; xx < NFA_EE_MAX_CBACKS; xx++) {
    if (nfa_ee_cb.p_ee_cback[xx] != nullptr) {
      (*nfa_ee_cb.p_ee_cback[xx])(event, p_data);
    }
  }
}
/*******************************************************************************
**
** Function         nfa_ee_start_timer
**
** Description      start the de-bounce timer
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_start_timer(void) {
#if(NXP_EXTNS != TRUE)
  if (nfa_dm_is_active())
    nfa_sys_start_timer(&nfa_ee_cb.timer, NFA_EE_ROUT_TIMEOUT_EVT,
                        NFA_EE_ROUT_TIMEOUT_VAL);
#endif
}

/*******************************************************************************
**
** Function         nfa_ee_api_discover
**
** Description      process discover command from user
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_discover(tNFA_EE_MSG* p_data) {
  tNFA_EE_CBACK* p_cback = p_data->ee_discover.p_cback;
  tNFA_EE_CBACK_DATA evt_data = {0};

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("in_use:%d", nfa_ee_cb.discv_timer.in_use);
  if (nfa_ee_cb.discv_timer.in_use) {
    nfa_sys_stop_timer(&nfa_ee_cb.discv_timer);
    if (NFA_GetNCIVersion() != NCI_VERSION_2_0) NFC_NfceeDiscover(false);
  }
  if (nfa_ee_cb.p_ee_disc_cback == nullptr &&
      NFC_NfceeDiscover(true) == NFC_STATUS_OK) {
    nfa_ee_cb.p_ee_disc_cback = p_cback;
  } else {
    evt_data.status = NFA_STATUS_FAILED;
    nfa_ee_report_event(p_cback, NFA_EE_DISCOVER_EVT, &evt_data);
  }
}

/*******************************************************************************
**
** Function         nfa_ee_api_register
**
** Description      process register command from user
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_register(tNFA_EE_MSG* p_data) {
  int xx;
  tNFA_EE_CBACK* p_cback = p_data->ee_register.p_cback;
  tNFA_EE_CBACK_DATA evt_data = {0};
  bool found = false;

  evt_data.ee_register = NFA_STATUS_FAILED;
  /* loop through all entries to see if there's a matching callback */
  for (xx = 0; xx < NFA_EE_MAX_CBACKS; xx++) {
    if (nfa_ee_cb.p_ee_cback[xx] == p_cback) {
      evt_data.ee_register = NFA_STATUS_OK;
      found = true;
      break;
    }
  }

  /* If no matching callback, allocated an entry */
  if (!found) {
    for (xx = 0; xx < NFA_EE_MAX_CBACKS; xx++) {
      if (nfa_ee_cb.p_ee_cback[xx] == nullptr) {
        nfa_ee_cb.p_ee_cback[xx] = p_cback;
        evt_data.ee_register = NFA_STATUS_OK;
        break;
      }
    }
  }

  int max_aid_cfg_length = nfa_ee_find_max_aid_cfg_len();
  int max_aid_entries = max_aid_cfg_length / NFA_MIN_AID_LEN + 1;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("max_aid_cfg_length: %d and max_aid_entries: %d",
                      max_aid_cfg_length, max_aid_entries);

  for (xx = 0; xx < NFA_EE_NUM_ECBS; xx++) {
    nfa_ee_cb.ecb[xx].aid_len = (uint8_t*)GKI_getbuf(max_aid_entries);
    nfa_ee_cb.ecb[xx].aid_pwr_cfg = (uint8_t*)GKI_getbuf(max_aid_entries);
    nfa_ee_cb.ecb[xx].aid_rt_info = (uint8_t*)GKI_getbuf(max_aid_entries);
    nfa_ee_cb.ecb[xx].aid_info = (uint8_t*)GKI_getbuf(max_aid_entries);
    nfa_ee_cb.ecb[xx].aid_cfg = (uint8_t*)GKI_getbuf(max_aid_cfg_length);
    if ((NULL != nfa_ee_cb.ecb[xx].aid_len) &&
        (NULL != nfa_ee_cb.ecb[xx].aid_pwr_cfg) &&
        (NULL != nfa_ee_cb.ecb[xx].aid_info) &&
        (NULL != nfa_ee_cb.ecb[xx].aid_cfg)) {
      memset(nfa_ee_cb.ecb[xx].aid_len, 0, max_aid_entries);
      memset(nfa_ee_cb.ecb[xx].aid_pwr_cfg, 0, max_aid_entries);
      memset(nfa_ee_cb.ecb[xx].aid_rt_info, 0, max_aid_entries);
      memset(nfa_ee_cb.ecb[xx].aid_info, 0, max_aid_entries);
      memset(nfa_ee_cb.ecb[xx].aid_cfg, 0, max_aid_cfg_length);
    } else {
      LOG(ERROR) << StringPrintf("GKI_getbuf allocation for ECB failed !");
    }
  }

  /* This callback is verified (not NULL) in NFA_EeRegister() */
  (*p_cback)(NFA_EE_REGISTER_EVT, &evt_data);

  /* report NFCEE Discovery Request collected during booting up */
  nfa_ee_build_discover_req_evt(&evt_data.discover_req);
  (*p_cback)(NFA_EE_DISCOVER_REQ_EVT, &evt_data);
}

/*******************************************************************************
**
** Function         nfa_ee_api_deregister
**
** Description      process de-register command from user
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_deregister(tNFA_EE_MSG* p_data) {
  tNFA_EE_CBACK* p_cback = nullptr;
  int index = p_data->deregister.index;
  tNFA_EE_CBACK_DATA evt_data = {0};

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("nfa_ee_api_deregister");

  for (int xx = 0; xx < NFA_EE_NUM_ECBS; xx++) {
    GKI_freebuf(nfa_ee_cb.ecb[xx].aid_len);
    GKI_freebuf(nfa_ee_cb.ecb[xx].aid_pwr_cfg);
    GKI_freebuf(nfa_ee_cb.ecb[xx].aid_rt_info);
    GKI_freebuf(nfa_ee_cb.ecb[xx].aid_info);
    GKI_freebuf(nfa_ee_cb.ecb[xx].aid_cfg);
  }

  p_cback = nfa_ee_cb.p_ee_cback[index];
  nfa_ee_cb.p_ee_cback[index] = nullptr;
  if (p_cback) (*p_cback)(NFA_EE_DEREGISTER_EVT, &evt_data);
}

/*******************************************************************************
**
** Function         nfa_ee_api_mode_set
**
** Description      process mode set command from user
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_mode_set(tNFA_EE_MSG* p_data) {
  tNFA_EE_ECB* p_cb = p_data->cfg_hdr.p_cb;
  tNFA_EE_MODE_SET mode_set;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "handle:0x%02x mode:%d", p_cb->nfcee_id, p_data->mode_set.mode);
  mode_set.status = NFC_NfceeModeSet(p_cb->nfcee_id, p_data->mode_set.mode);
  if (mode_set.status != NFC_STATUS_OK) {
    /* the api is rejected at NFC layer, report the failure status right away */
    mode_set.ee_handle = (tNFA_HANDLE)p_cb->nfcee_id | NFA_HANDLE_GROUP_EE;
    mode_set.ee_status = p_data->mode_set.mode;
    tNFA_EE_CBACK_DATA nfa_ee_cback_data;
    nfa_ee_cback_data.mode_set = mode_set;
    nfa_ee_report_event(nullptr, NFA_EE_MODE_SET_EVT, &nfa_ee_cback_data);
    return;
  }
  /* set the NFA_EE_STATUS_PENDING bit to indicate the status is not exactly
   * active */
  if (p_data->mode_set.mode == NFC_MODE_ACTIVATE)
    p_cb->ee_status = NFA_EE_STATUS_PENDING | NFA_EE_STATUS_ACTIVE;
  else {
    p_cb->ee_status = NFA_EE_STATUS_INACTIVE;
    /* DH should release the NCI connection before deactivate the NFCEE */
    if (p_cb->conn_st == NFA_EE_CONN_ST_CONN) {
      p_cb->conn_st = NFA_EE_CONN_ST_DISC;
      NFC_ConnClose(p_cb->conn_id);
    }
  }
  /* report the NFA_EE_MODE_SET_EVT status on the response from NFCC */
}

/*******************************************************************************
**
** Function         nfa_ee_api_set_tech_cfg
**
** Description      process set technology routing configuration from user
**                  start a 1 second timer. When the timer expires,
**                  the configuration collected in control block is sent to NFCC
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_set_tech_cfg(tNFA_EE_MSG* p_data) {
  tNFA_EE_ECB* p_cb = p_data->cfg_hdr.p_cb;
  tNFA_EE_CBACK_DATA evt_data = {0};
  tNFA_TECHNOLOGY_MASK old_tech_switch_on = p_cb->tech_switch_on;
  tNFA_TECHNOLOGY_MASK old_tech_switch_off = p_cb->tech_switch_off;
  tNFA_TECHNOLOGY_MASK old_tech_battery_off = p_cb->tech_battery_off;
  tNFA_TECHNOLOGY_MASK old_tech_screen_lock = p_cb->tech_screen_lock;
  tNFA_TECHNOLOGY_MASK old_tech_screen_off = p_cb->tech_screen_off;
  tNFA_TECHNOLOGY_MASK old_tech_screen_off_lock = p_cb->tech_screen_off_lock;
  uint8_t old_size_mask_tech = p_cb->size_mask_tech;

  if ((p_cb->tech_switch_on == p_data->set_tech.technologies_switch_on) &&
      (p_cb->tech_switch_off == p_data->set_tech.technologies_switch_off) &&
      (p_cb->tech_battery_off == p_data->set_tech.technologies_battery_off) &&
      (p_cb->tech_screen_lock == p_data->set_tech.technologies_screen_lock) &&
      (p_cb->tech_screen_off == p_data->set_tech.technologies_screen_off) &&
      (p_cb->tech_screen_off_lock ==
       p_data->set_tech.technologies_screen_off_lock)) {
    /* nothing to change */
    evt_data.status = NFA_STATUS_OK;
    nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_SET_TECH_CFG_EVT, &evt_data);
    return;
  }

  p_cb->tech_switch_on |= p_data->set_tech.technologies_switch_on;
  p_cb->tech_switch_off |= p_data->set_tech.technologies_switch_off;
  p_cb->tech_battery_off |= p_data->set_tech.technologies_battery_off;
  p_cb->tech_screen_lock |= p_data->set_tech.technologies_screen_lock;
  p_cb->tech_screen_off |= p_data->set_tech.technologies_screen_off;
  p_cb->tech_screen_off_lock |= p_data->set_tech.technologies_screen_off_lock;
  nfa_ee_update_route_size(p_cb);
  if (nfa_ee_total_lmrt_size() > NFC_GetLmrtSize()) {
    LOG(ERROR) << StringPrintf("nfa_ee_api_set_tech_cfg Exceed LMRT size");
    evt_data.status = NFA_STATUS_BUFFER_FULL;
    p_cb->tech_switch_on = old_tech_switch_on;
    p_cb->tech_switch_off = old_tech_switch_off;
    p_cb->tech_battery_off = old_tech_battery_off;
    p_cb->tech_screen_lock = old_tech_screen_lock;
    p_cb->tech_screen_off = old_tech_screen_off;
    p_cb->tech_screen_off_lock = old_tech_screen_off_lock;
    p_cb->size_mask_tech = old_size_mask_tech;
  } else {
    p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_TECH;
    if (p_cb->tech_switch_on | p_cb->tech_switch_off | p_cb->tech_battery_off |
        p_cb->tech_screen_lock | p_cb->tech_screen_off |
        p_cb->tech_screen_off_lock) {
      /* if any technology in any power mode is configured, mark this entry as
       * configured */
      nfa_ee_cb.ee_cfged |= nfa_ee_ecb_to_mask(p_cb);
    }
    nfa_ee_start_timer();
  }
  nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_SET_TECH_CFG_EVT, &evt_data);
}

/*******************************************************************************
**
** Function         nfa_ee_api_clear_tech_cfg
**
** Description      process clear technology routing configuration from user
**                  start a 1 second timer. When the timer expires,
**                  the configuration collected in control block is sent to NFCC
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_clear_tech_cfg(tNFA_EE_MSG* p_data) {
  tNFA_EE_ECB* p_cb = p_data->cfg_hdr.p_cb;
  tNFA_EE_CBACK_DATA evt_data = {0};

  tNFA_TECHNOLOGY_MASK old_tech_switch_on = p_cb->tech_switch_on;
  tNFA_TECHNOLOGY_MASK old_tech_switch_off = p_cb->tech_switch_off;
  tNFA_TECHNOLOGY_MASK old_tech_battery_off = p_cb->tech_battery_off;
  tNFA_TECHNOLOGY_MASK old_tech_screen_lock = p_cb->tech_screen_lock;
  tNFA_TECHNOLOGY_MASK old_tech_screen_off = p_cb->tech_screen_off;
  tNFA_TECHNOLOGY_MASK old_tech_screen_off_lock = p_cb->tech_screen_off_lock;

  p_cb->tech_switch_on &= ~p_data->clear_tech.technologies_switch_on;
  p_cb->tech_switch_off &= ~p_data->clear_tech.technologies_switch_off;
  p_cb->tech_battery_off &= ~p_data->clear_tech.technologies_battery_off;
  p_cb->tech_screen_lock &= ~p_data->clear_tech.technologies_screen_lock;
  p_cb->tech_screen_off &= ~p_data->clear_tech.technologies_screen_off;
  p_cb->tech_screen_off_lock &=
      ~p_data->clear_tech.technologies_screen_off_lock;

  if ((p_cb->tech_switch_on == old_tech_switch_on) &&
      (p_cb->tech_switch_off == old_tech_switch_off) &&
      (p_cb->tech_battery_off == old_tech_battery_off) &&
      (p_cb->tech_screen_lock == old_tech_screen_lock) &&
      (p_cb->tech_screen_off == old_tech_screen_off) &&
      (p_cb->tech_screen_off_lock == old_tech_screen_off_lock)) {
    /* nothing to change */
    evt_data.status = NFA_STATUS_OK;
    nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_CLEAR_TECH_CFG_EVT, &evt_data);
    return;
  }
  nfa_ee_update_route_size(p_cb);
  p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_TECH;

  nfa_ee_start_timer();
  nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_CLEAR_TECH_CFG_EVT, &evt_data);
}

/*******************************************************************************
**
** Function         nfa_ee_api_set_proto_cfg
**
** Description      process set protocol routing configuration from user
**                  start a 1 second timer. When the timer expires,
**                  the configuration collected in control block is sent to NFCC
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_set_proto_cfg(tNFA_EE_MSG* p_data) {
  tNFA_EE_ECB* p_cb = p_data->cfg_hdr.p_cb;
  tNFA_EE_CBACK_DATA evt_data = {0};
  tNFA_PROTOCOL_MASK old_proto_switch_on = p_cb->proto_switch_on;
  tNFA_PROTOCOL_MASK old_proto_switch_off = p_cb->proto_switch_off;
  tNFA_PROTOCOL_MASK old_proto_battery_off = p_cb->proto_battery_off;
  tNFA_PROTOCOL_MASK old_proto_screen_lock = p_cb->proto_screen_lock;
  tNFA_PROTOCOL_MASK old_proto_screen_off = p_cb->proto_screen_off;
  tNFA_PROTOCOL_MASK old_proto_screen_off_lock = p_cb->proto_screen_off_lock;
  uint8_t old_size_mask_proto = p_cb->size_mask_proto;

  if ((p_cb->proto_switch_on == p_data->set_proto.protocols_switch_on) &&
      (p_cb->proto_switch_off == p_data->set_proto.protocols_switch_off) &&
      (p_cb->proto_battery_off == p_data->set_proto.protocols_battery_off) &&
      (p_cb->proto_screen_lock == p_data->set_proto.protocols_screen_lock) &&
      (p_cb->proto_screen_off == p_data->set_proto.protocols_screen_off) &&
      (p_cb->proto_screen_off_lock ==
       p_data->set_proto.protocols_screen_off_lock)) {
    /* nothing to change */
    evt_data.status = NFA_STATUS_OK;
    nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_SET_PROTO_CFG_EVT, &evt_data);
    return;
  }

  p_cb->proto_switch_on |= p_data->set_proto.protocols_switch_on;
  p_cb->proto_switch_off |= p_data->set_proto.protocols_switch_off;
  p_cb->proto_battery_off |= p_data->set_proto.protocols_battery_off;
  p_cb->proto_screen_lock |= p_data->set_proto.protocols_screen_lock;
  p_cb->proto_screen_off |= p_data->set_proto.protocols_screen_off;
  p_cb->proto_screen_off_lock |= p_data->set_proto.protocols_screen_off_lock;
  nfa_ee_update_route_size(p_cb);
  if (nfa_ee_total_lmrt_size() > NFC_GetLmrtSize()) {
    LOG(ERROR) << StringPrintf("nfa_ee_api_set_proto_cfg Exceed LMRT size");
    evt_data.status = NFA_STATUS_BUFFER_FULL;
    p_cb->proto_switch_on = old_proto_switch_on;
    p_cb->proto_switch_off = old_proto_switch_off;
    p_cb->proto_battery_off = old_proto_battery_off;
    p_cb->proto_screen_lock = old_proto_screen_lock;
    p_cb->proto_screen_off = old_proto_screen_off;
    p_cb->proto_screen_off_lock = old_proto_screen_off_lock;
    p_cb->size_mask_proto = old_size_mask_proto;
  } else {
    p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_PROTO;
    if (p_cb->proto_switch_on | p_cb->proto_switch_off |
        p_cb->proto_battery_off | p_cb->proto_screen_lock |
        p_cb->proto_screen_off | p_cb->proto_screen_off_lock) {
      /* if any protocol in any power mode is configured, mark this entry as
       * configured */
      nfa_ee_cb.ee_cfged |= nfa_ee_ecb_to_mask(p_cb);
    }
    nfa_ee_start_timer();
  }
  nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_SET_PROTO_CFG_EVT, &evt_data);
}

/*******************************************************************************
**
** Function         nfa_ee_api_clear_proto_cfg
**
** Description      process clear protocol routing configuration from user
**                  start a 1 second timer. When the timer expires,
**                  the configuration collected in control block is sent to NFCC
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_clear_proto_cfg(tNFA_EE_MSG* p_data) {
  tNFA_EE_ECB* p_cb = p_data->cfg_hdr.p_cb;
  tNFA_EE_CBACK_DATA evt_data = {0};

  tNFA_TECHNOLOGY_MASK old_proto_switch_on = p_cb->proto_switch_on;
  tNFA_TECHNOLOGY_MASK old_proto_switch_off = p_cb->proto_switch_off;
  tNFA_TECHNOLOGY_MASK old_proto_battery_off = p_cb->proto_battery_off;
  tNFA_TECHNOLOGY_MASK old_proto_screen_lock = p_cb->proto_screen_lock;
  tNFA_TECHNOLOGY_MASK old_proto_screen_off = p_cb->proto_screen_off;
  tNFA_TECHNOLOGY_MASK old_proto_screen_off_lock = p_cb->proto_screen_off_lock;

  p_cb->proto_switch_on &= ~p_data->clear_proto.protocols_switch_on;
  p_cb->proto_switch_off &= ~p_data->clear_proto.protocols_switch_off;
  p_cb->proto_battery_off &= ~p_data->clear_proto.protocols_battery_off;
  p_cb->proto_screen_lock &= ~p_data->clear_proto.protocols_screen_lock;
  p_cb->proto_screen_off &= ~p_data->clear_proto.protocols_screen_off;
  p_cb->proto_screen_off_lock &= ~p_data->clear_proto.protocols_screen_off_lock;

  if ((p_cb->proto_switch_on == old_proto_switch_on) &&
      (p_cb->proto_switch_off == old_proto_switch_off) &&
      (p_cb->proto_battery_off == old_proto_battery_off) &&
      (p_cb->proto_screen_lock == old_proto_screen_lock) &&
      (p_cb->proto_screen_off == old_proto_screen_off) &&
      (p_cb->proto_screen_off_lock == old_proto_screen_off_lock)) {
    /* nothing to change */
    evt_data.status = NFA_STATUS_OK;
    nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_CLEAR_PROTO_CFG_EVT,
                        &evt_data);
    return;
  }
  nfa_ee_update_route_size(p_cb);
  p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_PROTO;

  nfa_ee_start_timer();
  nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_CLEAR_PROTO_CFG_EVT, &evt_data);
}

/*******************************************************************************
**
** Function         nfa_ee_api_add_aid
**
** Description      process add an AID routing configuration from user
**                  start a 1 second timer. When the timer expires,
**                  the configuration collected in control block is sent to NFCC
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_add_aid(tNFA_EE_MSG* p_data) {
  tNFA_EE_API_ADD_AID* p_add = &p_data->add_aid;
  tNFA_EE_ECB* p_cb = p_data->cfg_hdr.p_cb;
  tNFA_EE_ECB* p_chk_cb;
  uint8_t *p, *p_start;
  int len, len_needed;
  tNFA_EE_CBACK_DATA evt_data = {0};
  int offset = 0, entry = 0;
  uint16_t new_size;

  nfa_ee_trace_aid("nfa_ee_api_add_aid", p_cb->nfcee_id, p_add->aid_len,
                   p_add->p_aid);
  int max_aid_cfg_length = nfa_ee_find_max_aid_cfg_len();
  int max_aid_entries = max_aid_cfg_length / NFA_MIN_AID_LEN + 1;

  p_chk_cb =
      nfa_ee_find_aid_offset(p_add->aid_len, p_add->p_aid, &offset, &entry);
  if (p_chk_cb) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "nfa_ee_api_add_aid The AID entry is already in the database");
    if (p_chk_cb == p_cb) {
      p_cb->aid_rt_info[entry] |= NFA_EE_AE_ROUTE;
      p_cb->aid_info[entry] = p_add->aidInfo;
      new_size = nfa_ee_total_lmrt_size();
      if (new_size > NFC_GetLmrtSize()) {
        LOG(ERROR) << StringPrintf("Exceed LMRT size:%d (add ROUTE)", new_size);
        evt_data.status = NFA_STATUS_BUFFER_FULL;
        p_cb->aid_rt_info[entry] &= ~NFA_EE_AE_ROUTE;
      } else {
        p_cb->aid_pwr_cfg[entry] = p_add->power_state;
      }
    } else {
      LOG(ERROR) << StringPrintf(
          "The AID entry is already in the database for different NFCEE "
          "ID:0x%02x",
          p_chk_cb->nfcee_id);
      evt_data.status = NFA_STATUS_SEMANTIC_ERROR;
    }
  } else {
    /* Find the total length so far */
    len = nfa_ee_find_total_aid_len(p_cb, 0);

    /* make sure the control block has enough room to hold this entry */
    len_needed = p_add->aid_len + 2; /* tag/len */

    if ((len_needed + len) > max_aid_cfg_length) {
      LOG(ERROR) << StringPrintf(
          "Exceed capacity: (len_needed:%d + len:%d) > "
          "NFA_EE_MAX_AID_CFG_LEN:%d",
          len_needed, len, max_aid_cfg_length);
      evt_data.status = NFA_STATUS_BUFFER_FULL;
    } else if (p_cb->aid_entries < max_aid_entries) {
      /* 4 = 1 (tag) + 1 (len) + 1(nfcee_id) + 1(power cfg) */
      new_size = nfa_ee_total_lmrt_size() + 4 + p_add->aid_len;
      if (new_size > NFC_GetLmrtSize()) {
        LOG(ERROR) << StringPrintf("Exceed LMRT size:%d", new_size);
        evt_data.status = NFA_STATUS_BUFFER_FULL;
      } else {
        /* add AID */
        p_cb->aid_pwr_cfg[p_cb->aid_entries] = p_add->power_state;
        p_cb->aid_info[p_cb->aid_entries] = p_add->aidInfo;
        p_cb->aid_rt_info[p_cb->aid_entries] = NFA_EE_AE_ROUTE;
        p = p_cb->aid_cfg + len;
        p_start = p;
        *p++ = NFA_EE_AID_CFG_TAG_NAME;
        *p++ = p_add->aid_len;
        memcpy(p, p_add->p_aid, p_add->aid_len);
        p += p_add->aid_len;

        p_cb->aid_len[p_cb->aid_entries++] = (uint8_t)(p - p_start);
      }
    } else {
      LOG(ERROR) << StringPrintf("Exceed NFA_EE_MAX_AID_ENTRIES:%d",
                                 max_aid_entries);
      evt_data.status = NFA_STATUS_BUFFER_FULL;
    }
  }

  if (evt_data.status == NFA_STATUS_OK) {
    /* mark AID changed */
    p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_AID;
    nfa_ee_cb.ee_cfged |= nfa_ee_ecb_to_mask(p_cb);
    nfa_ee_update_route_aid_size(p_cb);
    nfa_ee_start_timer();
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "status:%d ee_cfged:0x%02x ", evt_data.status, nfa_ee_cb.ee_cfged);
  if (evt_data.status == NFA_STATUS_BUFFER_FULL)
    //android::util::stats_write(android::util::NFC_ERROR_OCCURRED,
    //                           (int32_t)AID_OVERFLOW, 0, 0);
    LOG(ERROR) << StringPrintf("\nNFC ERROR occured -> Buffer FULL\n");
  /* report the status of this operation */
  nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_ADD_AID_EVT, &evt_data);
}

/*******************************************************************************
**
** Function         nfa_ee_api_remove_aid
**
** Description      process remove an AID routing configuration from user
**                  start a 1 second timer. When the timer expires,
**                  the configuration collected in control block is sent to NFCC
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_remove_aid(tNFA_EE_MSG* p_data) {
  tNFA_EE_ECB* p_cb;
  tNFA_EE_CBACK_DATA evt_data = {0};
  int offset = 0, entry = 0, len;
  int rest_len;
  tNFA_EE_CBACK* p_cback = nullptr;

  nfa_ee_trace_aid("nfa_ee_api_remove_aid", 0, p_data->rm_aid.aid_len,
                   p_data->rm_aid.p_aid);
  p_cb = nfa_ee_find_aid_offset(p_data->rm_aid.aid_len, p_data->rm_aid.p_aid,
                                &offset, &entry);
  if (p_cb && p_cb->aid_entries) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "aid_rt_info[%d]: 0x%02x", entry, p_cb->aid_rt_info[entry]);
    /* mark routing and VS changed */
    if (p_cb->aid_rt_info[entry] & NFA_EE_AE_ROUTE)
      p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_AID;

    if (p_cb->aid_rt_info[entry] & NFA_EE_AE_VS)
      p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_VS;

    /* remove the aid */
    if ((entry + 1) < p_cb->aid_entries) {
      /* not the last entry, move the aid entries in control block */
      /* Find the total len from the next entry to the last one */
      rest_len = nfa_ee_find_total_aid_len(p_cb, entry + 1);

      len = p_cb->aid_len[entry];
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "nfa_ee_api_remove_aid len:%d, rest_len:%d", len, rest_len);
      GKI_shiftup(&p_cb->aid_cfg[offset], &p_cb->aid_cfg[offset + len],
                  rest_len);
      rest_len = p_cb->aid_entries - entry;
      GKI_shiftup(&p_cb->aid_len[entry], &p_cb->aid_len[entry + 1], rest_len);
      GKI_shiftup(&p_cb->aid_pwr_cfg[entry], &p_cb->aid_pwr_cfg[entry + 1],
                  rest_len);
      GKI_shiftup(&p_cb->aid_rt_info[entry], &p_cb->aid_rt_info[entry + 1],
                  rest_len);
    }
    /* else the last entry, just reduce the aid_entries by 1 */
    p_cb->aid_entries--;
    nfa_ee_cb.ee_cfged |= nfa_ee_ecb_to_mask(p_cb);
    nfa_ee_update_route_aid_size(p_cb);
    nfa_ee_start_timer();
    /* report NFA_EE_REMOVE_AID_EVT to the callback associated the NFCEE */
    p_cback = p_cb->p_ee_cback;
  } else {
    LOG(ERROR) << StringPrintf(
        "nfa_ee_api_remove_aid The AID entry is not in the database");
    evt_data.status = NFA_STATUS_INVALID_PARAM;
  }
  nfa_ee_report_event(p_cback, NFA_EE_REMOVE_AID_EVT, &evt_data);
}

/*******************************************************************************
 **
 ** Function         nfa_ee_api_add_sys_code
 **
 ** Description      Adds System Code routing configuration from user. When the
 **                  timer expires, the configuration collected in control block
 **                  is sent to NFCC
 **
 ** Returns          void
 **
 *******************************************************************************/
void nfa_ee_api_add_sys_code(tNFA_EE_MSG* p_data) {
  tNFA_EE_CBACK_DATA evt_data = {0};
  tNFA_EE_API_ADD_SYSCODE* p_add = &p_data->add_syscode;
  tNFA_EE_ECB* p_cb = p_data->cfg_hdr.p_cb;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s id:0x%x SC:0x%X ", __func__, p_add->nfcee_id, p_add->syscode);

  int offset = 0, entry = 0;
  tNFA_EE_ECB* p_chk_cb =
      nfa_ee_find_sys_code_offset(p_add->syscode, &offset, &entry);

  if (p_chk_cb) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: The SC entry already registered "
        "for this NFCEE id:0x%02x",
        __func__, p_add->nfcee_id);

    if (p_chk_cb == p_cb) {
      p_cb->sys_code_rt_loc_vs_info[entry] |= NFA_EE_AE_ROUTE;
      uint16_t new_size = nfa_ee_total_lmrt_size();
      if (new_size > NFC_GetLmrtSize()) {
        LOG(ERROR) << StringPrintf("Exceeded LMRT size:%d (add SYSCODE)",
                                   new_size);
        evt_data.status = NFA_STATUS_BUFFER_FULL;
        p_cb->sys_code_rt_loc_vs_info[entry] &= ~NFA_EE_AE_ROUTE;
      } else {
        p_cb->sys_code_pwr_cfg[entry] = p_add->power_state;
      }
    } else {
      LOG(ERROR) << StringPrintf(
          "%s: SystemCode entry already registered for different "
          "NFCEE id:0x%02x",
          __func__, p_chk_cb->nfcee_id);
      evt_data.status = NFA_STATUS_REJECTED;
    }
  } else {
    /* Find the total length so far in sys_code_cfg */
    int total_sc_len = nfa_all_ee_find_total_sys_code_len();
    /* make sure the control block has enough room to hold this entry */
    if ((NFA_EE_SYSTEM_CODE_LEN + total_sc_len) >
        NFA_EE_MAX_SYSTEM_CODE_CFG_LEN) {
      LOG(ERROR) << StringPrintf(
          "Exceeded capacity: (NFA_EE_SYSTEM_CODE_LEN:%d + total_sc_len:%d) > "
          "NFA_EE_MAX_SYSTEM_CODE_CFG_LEN:%d",
          NFA_EE_SYSTEM_CODE_LEN, total_sc_len, NFA_EE_MAX_SYSTEM_CODE_CFG_LEN);
      evt_data.status = NFA_STATUS_BUFFER_FULL;
    } else if (p_cb->sys_code_cfg_entries < NFA_EE_MAX_SYSTEM_CODE_ENTRIES) {
      /* 6 = 1 (tag) + 1 (len) + 1(nfcee_id) + 1(power cfg) + 2(system code)*/
      uint16_t new_size =
          nfa_ee_total_lmrt_size() + NFA_EE_SYSTEM_CODE_TLV_SIZE;
      if (new_size > NFC_GetLmrtSize()) {
        LOG(ERROR) << StringPrintf("Exceeded LMRT size:%d", new_size);
        evt_data.status = NFA_STATUS_BUFFER_FULL;
      } else {
        /* add SC entry*/
        uint32_t p_cb_sc_len = nfa_ee_find_total_sys_code_len(p_cb, 0);
        p_cb->sys_code_pwr_cfg[p_cb->sys_code_cfg_entries] = p_add->power_state;
        p_cb->sys_code_rt_loc[p_cb->sys_code_cfg_entries] = p_add->nfcee_id;
        p_cb->sys_code_rt_loc_vs_info[p_cb->sys_code_cfg_entries] =
            NFA_EE_AE_ROUTE;

        uint8_t* p = p_cb->sys_code_cfg + p_cb_sc_len;
        memcpy(p, &p_add->syscode, NFA_EE_SYSTEM_CODE_LEN);
        p += NFA_EE_SYSTEM_CODE_LEN;

        p_cb->sys_code_cfg_entries++;
      }
    } else {
      LOG(ERROR) << StringPrintf("Exceeded NFA_EE_MAX_SYSTEM_CODE_ENTRIES:%d",
                                 NFA_EE_MAX_SYSTEM_CODE_ENTRIES);
      evt_data.status = NFA_STATUS_BUFFER_FULL;
    }
  }

  if (evt_data.status == NFA_STATUS_OK) {
    /* mark SC changed */
    p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_SYSCODE;
    nfa_ee_cb.ee_cfged |= nfa_ee_ecb_to_mask(p_cb);
    nfa_ee_update_route_sys_code_size(p_cb);
    nfa_ee_start_timer();
  }
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: status:%d ee_cfged:0x%02x ", __func__,
                      evt_data.status, nfa_ee_cb.ee_cfged);

  /* report the status of this operation */
  nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_ADD_SYSCODE_EVT, &evt_data);
}

/*******************************************************************************
**
** Function         nfa_ee_api_remove_sys_code
**
** Description      process remove an System Code routing configuration from
**                  user start a 1 second timer. When the timer expires,
**                  the configuration collected in control block is sent to NFCC
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_remove_sys_code(tNFA_EE_MSG* p_data) {
  tNFA_EE_CBACK_DATA evt_data = {0};
  tNFA_EE_API_REMOVE_SYSCODE* p_remove = &p_data->rm_syscode;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s SC:0x%x", __func__, p_remove->syscode);

  int offset = 0, entry = 0;
  tNFA_EE_ECB* p_cb =
      nfa_ee_find_sys_code_offset(p_data->rm_syscode.syscode, &offset, &entry);

  if (p_cb && p_cb->sys_code_cfg_entries) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("sys_code_rt_loc_vs_info[%d]: 0x%02x", entry,
                        p_cb->sys_code_rt_loc_vs_info[entry]);
    /* mark routing and VS changed */
    if (p_cb->sys_code_rt_loc_vs_info[entry] & NFA_EE_AE_ROUTE)
      p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_SYSCODE;

    if (p_cb->sys_code_rt_loc_vs_info[entry] & NFA_EE_AE_VS)
      p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_VS;

    /* remove the system code */
    if ((entry + 1) < p_cb->sys_code_cfg_entries) {
      /* not the last entry, move the SC entries in control block */
      /* Find the total len from the next entry to the last one */
      int total_len = nfa_ee_find_total_sys_code_len(p_cb, entry + 1);

      int rm_len = NFA_EE_SYSTEM_CODE_LEN;

      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("nfa_ee_api_remove_sys_code: rm_len:%d, total_len:%d",
                          rm_len, total_len);

      GKI_shiftup(&p_cb->sys_code_cfg[offset],
                  &p_cb->sys_code_cfg[offset + rm_len], total_len);

      total_len = p_cb->sys_code_cfg_entries - entry;

      GKI_shiftup(&p_cb->sys_code_pwr_cfg[entry],
                  &p_cb->sys_code_pwr_cfg[entry + 1], total_len);

      GKI_shiftup(&p_cb->sys_code_rt_loc_vs_info[entry],
                  &p_cb->sys_code_rt_loc_vs_info[entry + 1], total_len);

      GKI_shiftup(&p_cb->sys_code_rt_loc[entry],
                  &p_cb->sys_code_rt_loc[entry + 1], total_len);
    }
    /* else the last entry, just reduce the aid_entries by 1 */
    p_cb->sys_code_cfg_entries--;
    nfa_ee_cb.ee_cfged |= nfa_ee_ecb_to_mask(p_cb);
    nfa_ee_update_route_sys_code_size(p_cb);
    nfa_ee_start_timer();
  } else {
    LOG(ERROR) << StringPrintf(
        "nfa_ee_api_remove_sys_code: The SC entry is not in the database");
    evt_data.status = NFA_STATUS_INVALID_PARAM;
  }
  /* report the status of this operation */
  if (p_cb) {
    nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_REMOVE_SYSCODE_EVT, &evt_data);
  } else {
    nfa_ee_report_event(NULL, NFA_EE_REMOVE_SYSCODE_EVT, &evt_data);
  }
}

/*******************************************************************************
**
** Function         nfa_ee_api_lmrt_size
**
** Description      Reports the remaining size in the Listen Mode Routing Table
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_lmrt_size(__attribute__((unused)) tNFA_EE_MSG* p_data) {
  tNFA_EE_CBACK_DATA evt_data = {0};
  uint16_t total_size = NFC_GetLmrtSize();

  evt_data.size = total_size - nfa_ee_total_lmrt_size();
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_ee_api_lmrt_size total size:%d remaining size:%d",
                      total_size, evt_data.size);

  nfa_ee_report_event(nullptr, NFA_EE_REMAINING_SIZE_EVT, &evt_data);
}

/*******************************************************************************
**
** Function         nfa_ee_api_update_now
**
** Description      Initiates connection creation process to the given NFCEE
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_update_now(tNFA_EE_MSG* p_data) {
  tNFA_EE_CBACK_DATA evt_data;

  if (nfa_ee_cb.ee_wait_evt & NFA_EE_WAIT_UPDATE_ALL) {
    LOG(ERROR) << StringPrintf(
        "nfa_ee_api_update_now still waiting for update complete "
        "ee_wait_evt:0x%x wait_rsp:%d",
        nfa_ee_cb.ee_wait_evt, nfa_ee_cb.wait_rsp);
    evt_data.status = NFA_STATUS_SEMANTIC_ERROR;
    nfa_ee_report_event(nullptr, NFA_EE_UPDATED_EVT, &evt_data);
    return;
  }
  nfa_sys_stop_timer(&nfa_ee_cb.timer);
  nfa_ee_cb.ee_cfged |= NFA_EE_CFGED_UPDATE_NOW;
  nfa_ee_rout_timeout(p_data);
}

/*******************************************************************************
**
** Function         nfa_ee_api_connect
**
** Description      Initiates connection creation process to the given NFCEE
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_connect(tNFA_EE_MSG* p_data) {
  tNFA_EE_ECB* p_cb = p_data->connect.p_cb;
  int xx;
  tNFA_EE_CBACK_DATA evt_data = {0};

  evt_data.connect.status = NFA_STATUS_FAILED;
  if (p_cb->conn_st == NFA_EE_CONN_ST_NONE) {
    for (xx = 0; xx < p_cb->num_interface; xx++) {
      if (p_data->connect.ee_interface == p_cb->ee_interface[xx]) {
        p_cb->p_ee_cback = p_data->connect.p_cback;
        p_cb->conn_st = NFA_EE_CONN_ST_WAIT;
        p_cb->use_interface = p_data->connect.ee_interface;
        evt_data.connect.status =
            NFC_ConnCreate(NCI_DEST_TYPE_NFCEE, p_data->connect.nfcee_id,
                           p_data->connect.ee_interface, nfa_ee_conn_cback);
        /* report the NFA_EE_CONNECT_EVT status on the response from NFCC */
        break;
      }
    }
  }

  if (evt_data.connect.status != NCI_STATUS_OK) {
    evt_data.connect.ee_handle =
        (tNFA_HANDLE)p_data->connect.nfcee_id | NFA_HANDLE_GROUP_EE;
    evt_data.connect.status = NFA_STATUS_INVALID_PARAM;
    evt_data.connect.ee_interface = p_data->connect.ee_interface;
    nfa_ee_report_event(p_data->connect.p_cback, NFA_EE_CONNECT_EVT, &evt_data);
  }
}

/*******************************************************************************
**
** Function         nfa_ee_api_send_data
**
** Description      Send the given data packet to the given NFCEE
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_send_data(tNFA_EE_MSG* p_data) {
  tNFA_EE_ECB* p_cb = p_data->send_data.p_cb;
  NFC_HDR* p_pkt;
  uint16_t size = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE +
                  p_data->send_data.data_len + NFC_HDR_SIZE;
  uint8_t* p;
  tNFA_STATUS status = NFA_STATUS_FAILED;

  if (p_cb->conn_st == NFA_EE_CONN_ST_CONN) {
    p_pkt = (NFC_HDR*)GKI_getbuf(size);
    if (p_pkt) {
      p_pkt->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
      p_pkt->len = p_data->send_data.data_len;
      p = (uint8_t*)(p_pkt + 1) + p_pkt->offset;
      memcpy(p, p_data->send_data.p_data, p_pkt->len);
      NFC_SendData(p_cb->conn_id, p_pkt);
    } else {
      tNFA_EE_CBACK_DATA nfa_ee_cback_data;
      nfa_ee_cback_data.status = status;
      nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_NO_MEM_ERR_EVT,
                          &nfa_ee_cback_data);
    }
  } else {
    tNFA_EE_CBACK_DATA nfa_ee_cback_data;
    nfa_ee_cback_data.status = status;
    nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_NO_CB_ERR_EVT,
                        &nfa_ee_cback_data);
  }
}

/*******************************************************************************
**
** Function         nfa_ee_api_disconnect
**
** Description      Initiates closing of the connection to the given NFCEE
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_api_disconnect(tNFA_EE_MSG* p_data) {
  tNFA_EE_ECB* p_cb = p_data->disconnect.p_cb;
  tNFA_EE_CBACK_DATA evt_data = {0};

  if (p_cb->conn_st == NFA_EE_CONN_ST_CONN) {
    p_cb->conn_st = NFA_EE_CONN_ST_DISC;
    NFC_ConnClose(p_cb->conn_id);
  }
  evt_data.handle = (tNFA_HANDLE)p_cb->nfcee_id | NFA_HANDLE_GROUP_EE;
  nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_DISCONNECT_EVT, &evt_data);
}

/*******************************************************************************
**
** Function         nfa_ee_report_disc_done
**
** Description      Process the callback for NFCEE discovery response
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_report_disc_done(bool notify_enable_done) {
  tNFA_EE_CBACK* p_cback;
  tNFA_EE_CBACK_DATA evt_data = {0};

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "em_state:%d num_ee_expecting:%d "
      "notify_enable_done:%d",
      nfa_ee_cb.em_state, nfa_ee_cb.num_ee_expecting, notify_enable_done);
  if (nfa_ee_cb.num_ee_expecting == 0) {
    if (notify_enable_done) {
      if (nfa_ee_cb.em_state == NFA_EE_EM_STATE_INIT_DONE) {
        nfa_sys_cback_notify_enable_complete(NFA_ID_EE);
        if (nfa_ee_cb.p_enable_cback)
          (*nfa_ee_cb.p_enable_cback)(NFA_EE_DISC_STS_ON);
      } else if ((nfa_ee_cb.em_state == NFA_EE_EM_STATE_RESTORING) &&
                 (nfa_ee_cb.ee_flags & NFA_EE_FLAG_NOTIFY_HCI)) {
        nfa_ee_cb.ee_flags &= ~NFA_EE_FLAG_NOTIFY_HCI;
        if (nfa_ee_cb.p_enable_cback)
          (*nfa_ee_cb.p_enable_cback)(NFA_EE_DISC_STS_ON);
      }
    }

    if (nfa_ee_cb.p_ee_disc_cback) {
      /* notify API callback */
      p_cback = nfa_ee_cb.p_ee_disc_cback;
      nfa_ee_cb.p_ee_disc_cback = nullptr;
      evt_data.status = NFA_STATUS_OK;
      evt_data.ee_discover.num_ee = NFA_EE_MAX_EE_SUPPORTED;
      NFA_EeGetInfo(&evt_data.ee_discover.num_ee, evt_data.ee_discover.ee_info);
      nfa_ee_report_event(p_cback, NFA_EE_DISCOVER_EVT, &evt_data);
    }
    if ((nfa_hci_cb.hci_state == NFA_HCI_STATE_EE_RECOVERY) &&
        nfa_ee_cb.p_enable_cback)
      (*nfa_ee_cb.p_enable_cback)(NFA_EE_RECOVERY_REDISCOVERED);
  }
}

/*******************************************************************************
**
** Function         nfa_ee_restore_ntf_done
**
** Description      check if any ee_status still has NFA_EE_STATUS_PENDING bit
**
** Returns          TRUE, if all NFA_EE_STATUS_PENDING bits are removed
**
*******************************************************************************/
bool nfa_ee_restore_ntf_done(void) {
  tNFA_EE_ECB* p_cb;
  bool is_done = true;
  int xx;

  p_cb = nfa_ee_cb.ecb;
  for (xx = 0; xx < nfa_ee_cb.cur_ee; xx++, p_cb++) {
    if ((p_cb->nfcee_id != NFA_EE_INVALID) &&
        (p_cb->ee_old_status & NFA_EE_STATUS_RESTORING)) {
      is_done = false;
      break;
    }
  }
  return is_done;
}

/*******************************************************************************
**
** Function         nfa_ee_remove_pending
**
** Description      check if any ee_status still has NFA_EE_STATUS_RESTORING bit
**
** Returns          TRUE, if all NFA_EE_STATUS_RESTORING bits are removed
**
*******************************************************************************/
static void nfa_ee_remove_pending(void) {
  tNFA_EE_ECB* p_cb;
  tNFA_EE_ECB *p_cb_n, *p_cb_end;
  int xx, num_removed = 0;
  int first_removed = NFA_EE_MAX_EE_SUPPORTED;

  p_cb = nfa_ee_cb.ecb;
  for (xx = 0; xx < nfa_ee_cb.cur_ee; xx++, p_cb++) {
    if ((p_cb->nfcee_id != NFA_EE_INVALID) &&
        (p_cb->ee_status & NFA_EE_STATUS_RESTORING)) {
      p_cb->nfcee_id = NFA_EE_INVALID;
      num_removed++;
      if (first_removed == NFA_EE_MAX_EE_SUPPORTED) first_removed = xx;
    }
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("cur_ee:%d, num_removed:%d first_removed:%d",
                      nfa_ee_cb.cur_ee, num_removed, first_removed);
  if (num_removed && (first_removed != (nfa_ee_cb.cur_ee - num_removed))) {
    /* if the removes ECB entried are not at the end, move the entries up */
    p_cb_end = nullptr;
    if (nfa_ee_cb.cur_ee > 0) p_cb_end = &nfa_ee_cb.ecb[nfa_ee_cb.cur_ee - 1];
    p_cb = &nfa_ee_cb.ecb[first_removed];
    for (p_cb_n = p_cb + 1; p_cb_n <= p_cb_end;) {
      while ((p_cb_n->nfcee_id == NFA_EE_INVALID) && (p_cb_n <= p_cb_end)) {
        p_cb_n++;
      }

      if (p_cb_n <= p_cb_end) {
        memcpy(p_cb, p_cb_n, sizeof(tNFA_EE_ECB));
        p_cb_n->nfcee_id = NFA_EE_INVALID;
      }
      p_cb++;
      p_cb_n++;
    }
  }
  nfa_ee_cb.cur_ee -= (uint8_t)num_removed;
}

/*******************************************************************************
**
** Function         nfa_ee_nci_disc_rsp
**
** Description      Process the callback for NFCEE discovery response
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_nci_disc_rsp(tNFA_EE_MSG* p_data) {
  tNFC_NFCEE_DISCOVER_REVT* p_evt = p_data->disc_rsp.p_data;
  tNFA_EE_ECB* p_cb;
  uint8_t xx;
  uint8_t num_nfcee = p_evt->num_nfcee;
  bool notify_enable_done = false;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("em_state:%d cur_ee:%d, num_nfcee:%d", nfa_ee_cb.em_state,
                      nfa_ee_cb.cur_ee, num_nfcee);
  switch (nfa_ee_cb.em_state) {
    case NFA_EE_EM_STATE_INIT:
      nfa_ee_cb.cur_ee = 0;
      nfa_ee_cb.num_ee_expecting = 0;
      if (num_nfcee == 0) {
        nfa_ee_cb.em_state = NFA_EE_EM_STATE_INIT_DONE;
        notify_enable_done = true;
        if (p_evt->status != NFC_STATUS_OK) {
          nfa_sys_stop_timer(&nfa_ee_cb.discv_timer);
        }
      }
      break;

    case NFA_EE_EM_STATE_INIT_DONE:
      if (num_nfcee) {
        /* if this is initiated by api function,
         * check if the number of NFCEE expected is more than what's currently
         * in CB */
        if (num_nfcee > NFA_EE_MAX_EE_SUPPORTED)
          num_nfcee = NFA_EE_MAX_EE_SUPPORTED;
        if (nfa_ee_cb.cur_ee < num_nfcee) {
          p_cb = &nfa_ee_cb.ecb[nfa_ee_cb.cur_ee];
          for (xx = nfa_ee_cb.cur_ee; xx < num_nfcee; xx++, p_cb++) {
            /* mark the new entries as a new one */
            p_cb->nfcee_id = NFA_EE_INVALID;
          }
        }
        nfa_ee_cb.cur_ee = num_nfcee;
      }
      break;

    case NFA_EE_EM_STATE_RESTORING:
      if (num_nfcee == 0) {
        nfa_ee_cb.em_state = NFA_EE_EM_STATE_INIT_DONE;
        nfa_ee_remove_pending();
        nfa_ee_check_restore_complete();
        if (p_evt->status != NFC_STATUS_OK) {
          nfa_sys_stop_timer(&nfa_ee_cb.discv_timer);
        }
      }
      break;
  }

  if (p_evt->status == NFC_STATUS_OK) {
    nfa_ee_cb.num_ee_expecting = p_evt->num_nfcee;
    if (nfa_ee_cb.num_ee_expecting > NFA_EE_MAX_EE_SUPPORTED) {
      LOG(ERROR) << StringPrintf("NFA-EE num_ee_expecting:%d > max:%d",
                                 nfa_ee_cb.num_ee_expecting,
                                 NFA_EE_MAX_EE_SUPPORTED);
    }
  }
  nfa_ee_report_disc_done(notify_enable_done);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "em_state:%d cur_ee:%d num_ee_expecting:%d", nfa_ee_cb.em_state,
      nfa_ee_cb.cur_ee, nfa_ee_cb.num_ee_expecting);
}

/*******************************************************************************
**
** Function         nfa_ee_nci_disc_ntf
**
** Description      Process the callback for NFCEE discovery notification
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_nci_disc_ntf(tNFA_EE_MSG* p_data) {
  tNFC_NFCEE_INFO_REVT* p_ee = p_data->disc_ntf.p_data;
  tNFA_EE_ECB* p_cb = nullptr;
  bool notify_enable_done = false;
  bool notify_new_ee = false;
  tNFA_EE_CBACK_DATA evt_data = {0};
  tNFA_EE_INFO* p_info;
  tNFA_EE_EM_STATE new_em_state = NFA_EE_EM_STATE_MAX;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "em_state:%d ee_flags:0x%x cur_ee:%d "
      "num_ee_expecting:%d",
      nfa_ee_cb.em_state, nfa_ee_cb.ee_flags, nfa_ee_cb.cur_ee,
      nfa_ee_cb.num_ee_expecting);
  if (nfa_ee_cb.num_ee_expecting) {
    nfa_ee_cb.num_ee_expecting--;
    if ((nfa_ee_cb.num_ee_expecting == 0) &&
        (nfa_ee_cb.p_ee_disc_cback != nullptr)) {
      /* Discovery triggered by API function */
      if (NFA_GetNCIVersion() != NCI_VERSION_2_0) NFC_NfceeDiscover(false);
    }
  }
  switch (nfa_ee_cb.em_state) {
    case NFA_EE_EM_STATE_INIT:
      if (nfa_ee_cb.cur_ee < NFA_EE_MAX_EE_SUPPORTED) {
        /* the cb can collect up to NFA_EE_MAX_EE_SUPPORTED ee_info */
        p_cb = &nfa_ee_cb.ecb[nfa_ee_cb.cur_ee++];
      }

      if (nfa_ee_cb.num_ee_expecting == 0) {
        /* notify init_done callback */
        nfa_ee_cb.em_state = NFA_EE_EM_STATE_INIT_DONE;
        notify_enable_done = true;
      }
      break;

    case NFA_EE_EM_STATE_INIT_DONE:
      p_cb = nfa_ee_find_ecb(p_ee->nfcee_id);
      if (p_cb == nullptr) {
        /* the NFCEE ID is not in the last NFCEE discovery
         * maybe it's a new one */
        p_cb = nfa_ee_find_ecb(NFA_EE_INVALID);
        if (p_cb) {
          nfa_ee_cb.cur_ee++;
          notify_new_ee = true;
        }
      } else if (p_cb->ecb_flags & NFA_EE_ECB_FLAGS_ORDER) {
        nfa_ee_cb.cur_ee++;
        notify_new_ee = true;
      } else {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("cur_ee:%d ecb_flags=0x%02x  ee_status=0x%x",
                            nfa_ee_cb.cur_ee, p_cb->ecb_flags, p_cb->ee_status);
      }
      break;

    case NFA_EE_EM_STATE_RESTORING:
      p_cb = nfa_ee_find_ecb(p_ee->nfcee_id);
      if (p_cb == nullptr) {
        /* the NFCEE ID is not in the last NFCEE discovery
         * maybe it's a new one */
        p_cb = nfa_ee_find_ecb(NFA_EE_INVALID);
        if (p_cb) {
          nfa_ee_cb.cur_ee++;
          notify_new_ee = true;
        }
      }
      if (nfa_ee_cb.num_ee_expecting == 0) {
        /* notify init_done callback */
        notify_enable_done = true;
        if (nfa_ee_restore_ntf_done()) {
          new_em_state = NFA_EE_EM_STATE_INIT_DONE;
        }
      }
      break;
  }
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_ee_nci_disc_ntf cur_ee:%d", nfa_ee_cb.cur_ee);

  if (p_cb) {
    p_cb->nfcee_id = p_ee->nfcee_id;
    p_cb->ee_status = p_ee->ee_status;
    p_cb->num_interface = p_ee->num_interface;
    memcpy(p_cb->ee_interface, p_ee->ee_interface, p_ee->num_interface);
    p_cb->num_tlvs = p_ee->num_tlvs;
    memcpy(p_cb->ee_tlv, p_ee->ee_tlv, p_ee->num_tlvs * sizeof(tNFA_EE_TLV));
    if (NFA_GetNCIVersion() == NCI_VERSION_2_0)
      p_cb->ee_power_supply_status = p_ee->nfcee_power_ctrl;
    if (nfa_ee_cb.em_state == NFA_EE_EM_STATE_RESTORING) {
      /* NCI spec says: An NFCEE_DISCOVER_NTF that contains a Protocol type of
       * "HCI Access"
       * SHALL NOT contain any other additional Protocol
       * i.e. check only first supported NFCEE interface is HCI access */
      /* NFA_HCI module handles restoring configurations for HCI access */
      if (p_cb->ee_interface[0] != NFC_NFCEE_INTERFACE_HCI_ACCESS) {
        if ((nfa_ee_cb.ee_flags & NFA_EE_FLAG_WAIT_HCI) == 0) {
          nfa_ee_restore_one_ecb(p_cb);
        }
        /* else wait for NFA-HCI module to restore the HCI network information
         * before enabling the NFCEE */
      }
    }

    if (p_ee->nfcee_id == T4TNFCEE_TARGET_HANDLE) {
      nfa_t4tnfcee_set_ee_cback(p_cb);
      p_info = &evt_data.new_ee;
      p_info->ee_handle = (tNFA_HANDLE)p_cb->nfcee_id;
      p_info->ee_status = p_cb->ee_status;
      nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_DISCOVER_EVT, &evt_data);
    }

    if ((nfa_ee_cb.p_ee_disc_cback == nullptr) && (notify_new_ee == true)) {
      if (nfa_dm_is_active() && (p_cb->ee_status != NFA_EE_STATUS_REMOVED)) {
        /* report this NFA_EE_NEW_EE_EVT only after NFA_DM_ENABLE_EVT is
         * reported */
        p_info = &evt_data.new_ee;
        p_info->ee_handle = NFA_HANDLE_GROUP_EE | (tNFA_HANDLE)p_cb->nfcee_id;
        p_info->ee_status = p_cb->ee_status;
        p_info->num_interface = p_cb->num_interface;
        p_info->num_tlvs = p_cb->num_tlvs;
        memcpy(p_info->ee_interface, p_cb->ee_interface, p_cb->num_interface);
        memcpy(p_info->ee_tlv, p_cb->ee_tlv,
               p_cb->num_tlvs * sizeof(tNFA_EE_TLV));
        if (NFA_GetNCIVersion() == NCI_VERSION_2_0)
          p_info->ee_power_supply_status = p_cb->ee_power_supply_status;
        nfa_ee_report_event(nullptr, NFA_EE_NEW_EE_EVT, &evt_data);
      }
    } else
      nfa_ee_report_disc_done(notify_enable_done);

    if (p_cb->ecb_flags & NFA_EE_ECB_FLAGS_ORDER) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("NFA_EE_ECB_FLAGS_ORDER");
      p_cb->ecb_flags &= ~NFA_EE_ECB_FLAGS_ORDER;
      nfa_ee_report_discover_req_evt();
    }
  }

  if (new_em_state != NFA_EE_EM_STATE_MAX) {
    nfa_ee_cb.em_state = new_em_state;
    nfa_ee_check_restore_complete();
  }

  if ((nfa_ee_cb.cur_ee == nfa_ee_max_ee_cfg) &&
      (nfa_ee_cb.em_state == NFA_EE_EM_STATE_INIT_DONE)) {
    if (nfa_ee_cb.discv_timer.in_use) {
      nfa_sys_stop_timer(&nfa_ee_cb.discv_timer);
      p_data->hdr.event = NFA_EE_DISCV_TIMEOUT_EVT;
      nfa_ee_evt_hdlr(&p_data->hdr);
    }
  }
}

/*******************************************************************************
**
** Function         nfa_ee_nci_nfcee_status_ntf
**
** Description      Process the callback for NFCEE status notification
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_nci_nfcee_status_ntf(tNFA_EE_MSG* p_data) {
  if (p_data != nullptr) {
    tNFC_NFCEE_STATUS_REVT* p_ee_data = p_data->nfcee_status_ntf.p_data;
    if ((NFA_GetNCIVersion() == NCI_VERSION_2_0) &&
        (p_ee_data->nfcee_status == NFC_NFCEE_STATUS_UNRECOVERABLE_ERROR)) {
      tNFA_EE_ECB* p_cb = nfa_ee_find_ecb(p_ee_data->nfcee_id);
      if (p_cb && nfa_ee_cb.p_enable_cback) {
        (*nfa_ee_cb.p_enable_cback)(NFA_EE_RECOVERY_INIT);
        NFC_NfceeDiscover(true);
      }
    }
  }
}

/*******************************************************************************
**
** Function         nfa_ee_check_restore_complete
**
** Description      Check if restore the NFA-EE related configuration to the
**                  state prior to low power mode is complete.
**                  If complete, notify sys.
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_check_restore_complete(void) {
  uint32_t xx;
  tNFA_EE_ECB* p_cb;
  bool proc_complete = true;

  p_cb = nfa_ee_cb.ecb;
  for (xx = 0; xx < nfa_ee_cb.cur_ee; xx++, p_cb++) {
    if (p_cb->ecb_flags & NFA_EE_ECB_FLAGS_RESTORE) {
      /* NFA_HCI module handles restoring configurations for HCI access.
       * ignore the restoring status for HCI Access */
      if (p_cb->ee_interface[0] != NFC_NFCEE_INTERFACE_HCI_ACCESS) {
        proc_complete = false;
        break;
      }
    }
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nfa_ee_check_restore_complete nfa_ee_cb.ee_cfg_sts:0x%02x "
      "proc_complete:%d",
      nfa_ee_cb.ee_cfg_sts, proc_complete);
  if (proc_complete) {
    /* update routing table when NFA_EE_ROUT_TIMEOUT_EVT is received */
    if (nfa_ee_cb.ee_cfg_sts & NFA_EE_STS_PREV_ROUTING)
      nfa_ee_api_update_now(nullptr);

    nfa_ee_cb.em_state = NFA_EE_EM_STATE_INIT_DONE;
    nfa_sys_cback_notify_nfcc_power_mode_proc_complete(NFA_ID_EE);
  }
}

/*******************************************************************************
**
** Function         nfa_ee_build_discover_req_evt
**
** Description      Build NFA_EE_DISCOVER_REQ_EVT for all active NFCEE
**
** Returns          void
**
*******************************************************************************/
static void nfa_ee_build_discover_req_evt(tNFA_EE_DISCOVER_REQ* p_evt_data) {
  tNFA_EE_ECB* p_cb;
  tNFA_EE_DISCOVER_INFO* p_info;
  uint8_t xx;

  if (!p_evt_data) return;

  p_evt_data->num_ee = 0;
  p_cb = nfa_ee_cb.ecb;
  p_info = p_evt_data->ee_disc_info;

  for (xx = 0; xx < nfa_ee_cb.cur_ee; xx++, p_cb++) {
    if ((p_cb->ee_status & NFA_EE_STATUS_INT_MASK) ||
        (p_cb->ee_status != NFA_EE_STATUS_ACTIVE)) {
      continue;
    }
    p_info->ee_handle = (tNFA_HANDLE)p_cb->nfcee_id | NFA_HANDLE_GROUP_EE;
    p_info->la_protocol = p_cb->la_protocol;
    p_info->lb_protocol = p_cb->lb_protocol;
    p_info->lf_protocol = p_cb->lf_protocol;
    p_info->lbp_protocol = p_cb->lbp_protocol;
    p_evt_data->num_ee++;
    p_info++;

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "[%d] ee_handle:0x%x, listen protocol A:%d, B:%d, F:%d, BP:%d",
        p_evt_data->num_ee, p_cb->nfcee_id, p_cb->la_protocol,
        p_cb->lb_protocol, p_cb->lf_protocol, p_cb->lbp_protocol);
  }

  p_evt_data->status = NFA_STATUS_OK;
}

/*******************************************************************************
**
** Function         nfa_ee_report_discover_req_evt
**
** Description      Report NFA_EE_DISCOVER_REQ_EVT for all active NFCEE
**
** Returns          void
**
*******************************************************************************/
static void nfa_ee_report_discover_req_evt(void) {
  if (nfa_ee_cb.p_enable_cback)
    (*nfa_ee_cb.p_enable_cback)(NFA_EE_DISC_STS_REQ);

  /* if this is restoring NFCC */
  if (!nfa_dm_is_active()) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("nfa_ee_report_discover_req_evt DM is not active");
    return;
  }

  tNFA_EE_CBACK_DATA nfa_ee_cback_data;
  nfa_ee_build_discover_req_evt(&nfa_ee_cback_data.discover_req);
  nfa_ee_report_event(nullptr, NFA_EE_DISCOVER_REQ_EVT, &nfa_ee_cback_data);
}

/*******************************************************************************
**
** Function         nfa_ee_nci_mode_set_rsp
**
** Description      Process the result for NFCEE ModeSet response
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_nci_mode_set_rsp(tNFA_EE_MSG* p_data) {
  tNFA_EE_ECB* p_cb;
  tNFA_EE_MODE_SET mode_set;
  tNFC_NFCEE_MODE_SET_REVT* p_rsp = p_data->mode_set_rsp.p_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s handle:0x%02x mode:%d", __func__, p_rsp->nfcee_id, p_rsp->mode);
  p_cb = nfa_ee_find_ecb(p_rsp->nfcee_id);
  if (p_cb == nullptr) {
    LOG(ERROR) << StringPrintf("%s Can not find cb for handle:0x%02x", __func__,
                               p_rsp->nfcee_id);
    return;
  }

  /* Do not update routing table in EE_RECOVERY state */
  if (nfa_hci_cb.hci_state != NFA_HCI_STATE_EE_RECOVERY) {
    /* Start routing table update debounce timer */
    nfa_ee_start_timer();
  }
  LOG(ERROR) << StringPrintf("%s p_rsp->status:0x%02x", __func__,
                             p_rsp->status);
  if (p_rsp->status == NFA_STATUS_OK) {
    if (p_rsp->mode == NFA_EE_MD_ACTIVATE) {
      p_cb->ee_status = NFC_NFCEE_STATUS_ACTIVE;
    } else {
      if (p_cb->tech_switch_on | p_cb->tech_switch_off |
          p_cb->tech_battery_off | p_cb->proto_switch_on |
          p_cb->proto_switch_off | p_cb->proto_battery_off |
          p_cb->aid_entries) {
        /* this NFCEE still has configuration when deactivated. clear the
         * configuration */
        nfa_ee_cb.ee_cfged &= ~nfa_ee_ecb_to_mask(p_cb);
        nfa_ee_cb.ee_cfg_sts |= NFA_EE_STS_CHANGED_ROUTING;
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("deactivating/still configured. Force update");
      }
      p_cb->tech_switch_on = p_cb->tech_switch_off = p_cb->tech_battery_off = 0;
      p_cb->proto_switch_on = p_cb->proto_switch_off = p_cb->proto_battery_off =
          0;
      p_cb->aid_entries = 0;
      p_cb->ee_status = NFC_NFCEE_STATUS_INACTIVE;
    }
  } else if (p_rsp->mode == NFA_EE_MD_ACTIVATE) {
    p_cb->ee_status = NFC_NFCEE_STATUS_REMOVED;
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "status:%d ecb_flags  :0x%02x ee_cfged:0x%02x ee_status:%d",
      p_rsp->status, p_cb->ecb_flags, nfa_ee_cb.ee_cfged, p_cb->ee_status);
  if (p_cb->ecb_flags & NFA_EE_ECB_FLAGS_RESTORE) {
    if (p_cb->conn_st == NFA_EE_CONN_ST_CONN) {
      /* NFA_HCI module handles restoring configurations for HCI access */
      if (p_cb->ee_interface[0] != NFC_NFCEE_INTERFACE_HCI_ACCESS) {
        NFC_ConnCreate(NCI_DEST_TYPE_NFCEE, p_cb->nfcee_id, p_cb->use_interface,
                       nfa_ee_conn_cback);
      }
    } else {
      p_cb->ecb_flags &= ~NFA_EE_ECB_FLAGS_RESTORE;
      nfa_ee_check_restore_complete();
    }
  } else {
    mode_set.status = p_rsp->status;
    mode_set.ee_handle = (tNFA_HANDLE)p_rsp->nfcee_id | NFA_HANDLE_GROUP_EE;
    mode_set.ee_status = p_cb->ee_status;

    tNFA_EE_CBACK_DATA nfa_ee_cback_data;
    nfa_ee_cback_data.mode_set = mode_set;
    nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_MODE_SET_EVT,
                        &nfa_ee_cback_data);

    if ((p_cb->ee_status == NFC_NFCEE_STATUS_INACTIVE) ||
        (p_cb->ee_status == NFC_NFCEE_STATUS_ACTIVE)) {
      /* Report NFA_EE_DISCOVER_REQ_EVT for all active NFCEE */
      nfa_ee_report_discover_req_evt();
    }
  }
  if (nfa_ee_cb.p_enable_cback)
    (*nfa_ee_cb.p_enable_cback)(NFA_EE_MODE_SET_COMPLETE);
}

/*******************************************************************************
**
** Function         nfa_ee_report_update_evt
**
** Description      Check if need to report NFA_EE_UPDATED_EVT
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_report_update_evt(void) {
  tNFA_EE_CBACK_DATA evt_data;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_ee_report_update_evt ee_wait_evt:0x%x wait_rsp:%d",
                      nfa_ee_cb.ee_wait_evt, nfa_ee_cb.wait_rsp);
  if (nfa_ee_cb.wait_rsp == 0) {
    nfa_ee_cb.ee_wait_evt &= ~NFA_EE_WAIT_UPDATE_RSP;

    if (nfa_ee_cb.ee_wait_evt & NFA_EE_WAIT_UPDATE) {
      nfa_ee_cb.ee_wait_evt &= ~NFA_EE_WAIT_UPDATE;
      /* finished updating NFCC; report NFA_EE_UPDATED_EVT now */
      evt_data.status = NFA_STATUS_OK;
      nfa_ee_report_event(nullptr, NFA_EE_UPDATED_EVT, &evt_data);
    }
  }
}

/*******************************************************************************
**
** Function         nfa_ee_nci_wait_rsp
**
** Description      Process the result for NCI response
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_nci_wait_rsp(tNFA_EE_MSG* p_data) {
  tNFA_EE_NCI_WAIT_RSP* p_rsp = &p_data->wait_rsp;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("ee_wait_evt:0x%x wait_rsp:%d", nfa_ee_cb.ee_wait_evt,
                      nfa_ee_cb.wait_rsp);
  if (nfa_ee_cb.wait_rsp) {
    if (p_rsp->opcode == NCI_MSG_RF_SET_ROUTING) nfa_ee_cb.wait_rsp--;
  }
  nfa_ee_report_update_evt();
}

/*******************************************************************************
**
** Function         nfa_ee_nci_conn
**
** Description      process the connection callback events
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_nci_conn(tNFA_EE_MSG* p_data) {
  tNFA_EE_ECB* p_cb;
  tNFA_EE_NCI_CONN* p_cbk = &p_data->conn;
  tNFC_CONN* p_conn = p_data->conn.p_data;
  NFC_HDR* p_pkt = nullptr;
  tNFA_EE_CBACK_DATA evt_data = {0};
  tNFA_EE_EVT event = NFA_EE_INVALID;
  tNFA_EE_CBACK* p_cback = nullptr;

  if (p_cbk->event == NFC_CONN_CREATE_CEVT) {
    p_cb = nfa_ee_find_ecb(p_cbk->p_data->conn_create.id);
  } else {
    p_cb = nfa_ee_find_ecb_by_conn_id(p_cbk->conn_id);
    if (p_cbk->event == NFC_DATA_CEVT) p_pkt = p_conn->data.p_data;
  }

  if (p_cb) {
    p_cback = p_cb->p_ee_cback;
    evt_data.handle = (tNFA_HANDLE)p_cb->nfcee_id | NFA_HANDLE_GROUP_EE;
    switch (p_cbk->event) {
      case NFC_CONN_CREATE_CEVT:
        if (p_conn->conn_create.status == NFC_STATUS_OK) {
          p_cb->conn_id = p_cbk->conn_id;
          p_cb->conn_st = NFA_EE_CONN_ST_CONN;
        } else {
          p_cb->conn_st = NFA_EE_CONN_ST_NONE;
        }
        if (p_cb->ecb_flags & NFA_EE_ECB_FLAGS_RESTORE) {
          p_cb->ecb_flags &= ~NFA_EE_ECB_FLAGS_RESTORE;
          nfa_ee_check_restore_complete();
        } else {
          evt_data.connect.status = p_conn->conn_create.status;
          evt_data.connect.ee_interface = p_cb->use_interface;
          event = NFA_EE_CONNECT_EVT;
        }
        break;

      case NFC_CONN_CLOSE_CEVT:
        if (p_cb->conn_st != NFA_EE_CONN_ST_DISC) event = NFA_EE_DISCONNECT_EVT;
        p_cb->conn_st = NFA_EE_CONN_ST_NONE;
        p_cb->p_ee_cback = nullptr;
        p_cb->conn_id = 0;
        if (nfa_ee_cb.em_state == NFA_EE_EM_STATE_DISABLING) {
          if (nfa_ee_cb.ee_flags & NFA_EE_FLAG_WAIT_DISCONN) {
            if (nfa_ee_cb.num_ee_expecting) {
              nfa_ee_cb.num_ee_expecting--;
            }
          }
          if (nfa_ee_cb.num_ee_expecting == 0) {
            nfa_ee_cb.ee_flags &= ~NFA_EE_FLAG_WAIT_DISCONN;
            nfa_ee_check_disable();
          }
        }
        break;

      case NFC_DATA_CEVT:
        if (p_cb->conn_st == NFA_EE_CONN_ST_CONN) {
          /* report data event only in connected state */
          if (p_cb->p_ee_cback && p_pkt) {
            evt_data.data.len = p_pkt->len;
            evt_data.data.p_buf = (uint8_t*)(p_pkt + 1) + p_pkt->offset;
            event = NFA_EE_DATA_EVT;
            p_pkt = nullptr; /* so this function does not free this GKI buffer */
          }
        }
        break;
    }

    if ((event != NFA_EE_INVALID) && (p_cback)) (*p_cback)(event, &evt_data);
  }
  if (p_pkt) GKI_freebuf(p_pkt);
}

/*******************************************************************************
**
** Function         nfa_ee_nci_action_ntf
**
** Description      process the NFCEE action callback event
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_nci_action_ntf(tNFA_EE_MSG* p_data) {
  tNFC_EE_ACTION_REVT* p_cbk = p_data->act.p_data;
  tNFA_EE_ACTION evt_data;

  evt_data.ee_handle = (tNFA_HANDLE)p_cbk->nfcee_id | NFA_HANDLE_GROUP_EE;
  evt_data.trigger = p_cbk->act_data.trigger;
  memcpy(&(evt_data.param), &(p_cbk->act_data.param),
         sizeof(tNFA_EE_ACTION_PARAM));
  tNFA_EE_CBACK_DATA nfa_ee_cback_data;
  nfa_ee_cback_data.action = evt_data;
  nfa_ee_report_event(nullptr, NFA_EE_ACTION_EVT, &nfa_ee_cback_data);
}

/*******************************************************************************
**
** Function         nfa_ee_nci_disc_req_ntf
**
** Description      process the NFCEE discover request callback event
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_nci_disc_req_ntf(tNFA_EE_MSG* p_data) {
  tNFC_EE_DISCOVER_REQ_REVT* p_cbk = p_data->disc_req.p_data;
  tNFA_HANDLE ee_handle;
  tNFA_EE_ECB* p_cb = nullptr;
  uint8_t report_ntf = 0;
  uint8_t xx;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "num_info: %d cur_ee:%d", p_cbk->num_info, nfa_ee_cb.cur_ee);

  for (xx = 0; xx < p_cbk->num_info; xx++) {
    ee_handle = NFA_HANDLE_GROUP_EE | p_cbk->info[xx].nfcee_id;

    p_cb = nfa_ee_find_ecb(p_cbk->info[xx].nfcee_id);
    if (!p_cb) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "Cannot find cb for NFCEE: 0x%x", p_cbk->info[xx].nfcee_id);
      p_cb = nfa_ee_find_ecb(NFA_EE_INVALID);
      if (p_cb) {
        p_cb->nfcee_id = p_cbk->info[xx].nfcee_id;
        p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_ORDER;
      } else {
        LOG(ERROR) << StringPrintf("Cannot allocate cb for NFCEE: 0x%x",
                                   p_cbk->info[xx].nfcee_id);
        continue;
      }
    } else {
      report_ntf |= nfa_ee_ecb_to_mask(p_cb);
    }

    p_cb->ecb_flags |= NFA_EE_ECB_FLAGS_DISC_REQ;
    if (p_cbk->info[xx].op == NFC_EE_DISC_OP_ADD) {
      if (p_cbk->info[xx].tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_A) {
        p_cb->la_protocol = p_cbk->info[xx].protocol;
      } else if (p_cbk->info[xx].tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_B) {
        p_cb->lb_protocol = p_cbk->info[xx].protocol;
      } else if (p_cbk->info[xx].tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_F) {
        p_cb->lf_protocol = p_cbk->info[xx].protocol;
      } else if (p_cbk->info[xx].tech_n_mode ==
                 NFC_DISCOVERY_TYPE_LISTEN_B_PRIME) {
        p_cb->lbp_protocol = p_cbk->info[xx].protocol;
      }
      #if (NXP_EXTNS == TRUE)
      if (p_cb->nfcee_id == T4TNFCEE_TARGET_HANDLE) {
        tNFA_EE_CBACK_DATA nfa_ee_cback_data = {0};
        nfa_ee_report_event(p_cb->p_ee_cback, NFA_EE_DISCOVER_REQ_EVT,
                            &nfa_ee_cback_data);
      }
      #endif
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "nfcee_id=0x%x ee_status=0x%x ecb_flags=0x%x la_protocol=0x%x "
          "la_protocol=0x%x la_protocol=0x%x",
          p_cb->nfcee_id, p_cb->ee_status, p_cb->ecb_flags, p_cb->la_protocol,
          p_cb->lb_protocol, p_cb->lf_protocol);
    } else {
      if (p_cbk->info[xx].tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_A) {
        p_cb->la_protocol = 0;
      } else if (p_cbk->info[xx].tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_B) {
        p_cb->lb_protocol = 0;
      } else if (p_cbk->info[xx].tech_n_mode == NFC_DISCOVERY_TYPE_LISTEN_F) {
        p_cb->lf_protocol = 0;
      } else if (p_cbk->info[xx].tech_n_mode ==
                 NFC_DISCOVERY_TYPE_LISTEN_B_PRIME) {
        p_cb->lbp_protocol = 0;
      }
    }
  }

  /* Report NFA_EE_DISCOVER_REQ_EVT for all active NFCEE */
  if (report_ntf) nfa_ee_report_discover_req_evt();
}

/*******************************************************************************
**
** Function         nfa_ee_is_active
**
** Description      Check if the given NFCEE is active
**
** Returns          TRUE if the given NFCEE is active
**
*******************************************************************************/
bool nfa_ee_is_active(tNFA_HANDLE nfcee_id) {
  bool is_active = false;
  int xx;
  tNFA_EE_ECB* p_cb = nfa_ee_cb.ecb;

  if ((NFA_HANDLE_GROUP_MASK & nfcee_id) == NFA_HANDLE_GROUP_EE)
    nfcee_id &= NFA_HANDLE_MASK;

  if (nfcee_id == NFC_DH_ID) return true;

  /* compose output */
  for (xx = 0; xx < nfa_ee_cb.cur_ee; xx++, p_cb++) {
    if ((tNFA_HANDLE)p_cb->nfcee_id == nfcee_id) {
      if (p_cb->ee_status == NFA_EE_STATUS_ACTIVE) {
        is_active = true;
      }
      break;
    }
  }
  return is_active;
}

/*******************************************************************************
**
** Function         nfa_ee_get_tech_route
**
** Description      Given a power state, find the technology routing
**                  destination. The result is filled in the given p_handles
**                  in the order of A, B, F, Bprime
**
** Returns          None
**
*******************************************************************************/
void nfa_ee_get_tech_route(uint8_t power_state, uint8_t* p_handles) {
  int xx, yy;
  tNFA_EE_ECB* p_cb;
  uint8_t tech_mask_list[NFA_EE_MAX_TECH_ROUTE] = {
      NFA_TECHNOLOGY_MASK_A, NFA_TECHNOLOGY_MASK_B, NFA_TECHNOLOGY_MASK_F,
      NFA_TECHNOLOGY_MASK_B_PRIME};

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%d", power_state);

  for (xx = 0; xx < NFA_EE_MAX_TECH_ROUTE; xx++) {
    p_handles[xx] = NFC_DH_ID;
    if (nfa_ee_cb.cur_ee > 0) p_cb = &nfa_ee_cb.ecb[nfa_ee_cb.cur_ee - 1];
    for (yy = 0; yy < nfa_ee_cb.cur_ee; yy++, p_cb--) {
      if (p_cb->ee_status == NFC_NFCEE_STATUS_ACTIVE) {
        switch (power_state) {
          case NFA_EE_PWR_STATE_ON:
            if (p_cb->tech_switch_on & tech_mask_list[xx])
              p_handles[xx] = p_cb->nfcee_id;
            break;
          case NFA_EE_PWR_STATE_SWITCH_OFF:
            if (p_cb->tech_switch_off & tech_mask_list[xx])
              p_handles[xx] = p_cb->nfcee_id;
            break;
          case NFA_EE_PWR_STATE_BATT_OFF:
            if (p_cb->tech_battery_off & tech_mask_list[xx])
              p_handles[xx] = p_cb->nfcee_id;
            break;
        }
      }
    }
  }
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("0x%x, 0x%x, 0x%x, 0x%x", p_handles[0], p_handles[1],
                      p_handles[2], p_handles[3]);
}

/*******************************************************************************
**
** Function         nfa_ee_check_set_routing
**
** Description      If the new size exceeds the capacity of next block,
**                  send the routing command now and reset the related
**                  parameters.
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_check_set_routing(uint16_t new_size, int* p_max_len, uint8_t* p,
                              int* p_cur_offset) {
  uint8_t max_tlv = (uint8_t)((*p_max_len > NFA_EE_ROUT_MAX_TLV_SIZE)
                                  ? NFA_EE_ROUT_MAX_TLV_SIZE
                                  : *p_max_len);

  if (new_size + *p_cur_offset > max_tlv) {
    if (NFC_SetRouting(true, *p, *p_cur_offset, p + 1) == NFA_STATUS_OK) {
      nfa_ee_cb.wait_rsp++;
    }
    /* after the routing command is sent, re-use the same buffer to send the
     * next routing command.
     * reset the related parameters */
    if (*p_max_len > *p_cur_offset)
      *p_max_len -= *p_cur_offset; /* the max is reduced */
    else
      *p_max_len = 0;
    *p_cur_offset = 0; /* nothing is in queue any more */
    *p = 0;            /* num_tlv=0 */
  }
}

/*******************************************************************************
**
** Function         nfa_ee_route_add_one_ecb_order
**
** Description      Add the routing entries for NFCEE/DH in order defined
**
** Returns          NFA_STATUS_OK, if ok to continue
**
*******************************************************************************/
void nfa_ee_route_add_one_ecb_by_route_order(tNFA_EE_ECB* p_cb, int rout_type,
                                             int* p_max_len, bool more,
                                             uint8_t* ps, int* p_cur_offset) {
  /* use the first byte of the buffer (ps) to keep the num_tlv */
  uint8_t num_tlv = *ps;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s - max_len:%d, cur_offset:%d, more:%d, num_tlv:%d,rout_type:- %d",
      __func__, *p_max_len, *p_cur_offset, more, num_tlv, rout_type);
  uint8_t* pp = ps + 1 + *p_cur_offset;
  uint8_t* p = pp;
  uint16_t tlv_size = (uint8_t)*p_cur_offset;

  switch (rout_type) {
    case NCI_ROUTE_ORDER_TECHNOLOGY: {
      nfa_ee_check_set_routing(p_cb->size_mask_tech, p_max_len, ps,
                               p_cur_offset);
      pp = ps + 1 + *p_cur_offset;
      p = pp;
      nfa_ee_add_tech_route_to_ecb(p_cb, pp, p, ps, p_cur_offset);
    } break;

    case NCI_ROUTE_ORDER_PROTOCOL: {
      nfa_ee_check_set_routing(p_cb->size_mask_proto, p_max_len, ps,
                               p_cur_offset);
      pp = ps + 1 + *p_cur_offset;
      p = pp;
      nfa_ee_add_proto_route_to_ecb(p_cb, pp, p, ps, p_cur_offset);
    } break;
    case NCI_ROUTE_ORDER_AID: {
      nfa_ee_add_aid_route_to_ecb(p_cb, pp, p, ps, p_cur_offset, p_max_len);
    } break;
    case NCI_ROUTE_ORDER_SYS_CODE: {
      nfa_ee_add_sys_code_route_to_ecb(p_cb, pp, p, ps, p_cur_offset,
                                       p_max_len);
    } break;
    default: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s -  Route type - NA:- %d", __func__, rout_type);
    }
  }

  /* update the total number of entries */
  num_tlv = *ps;

  tlv_size = nfa_ee_total_lmrt_size();
  if (tlv_size) {
    nfa_ee_cb.ee_cfged |= nfa_ee_ecb_to_mask(p_cb);
  }
  if (p_cb->ecb_flags & NFA_EE_ECB_FLAGS_ROUTING) {
    nfa_ee_cb.ee_cfg_sts |= NFA_EE_STS_CHANGED_ROUTING;
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "ee_cfg_sts:0x%02x lmrt_size:%d", nfa_ee_cb.ee_cfg_sts, tlv_size);

  if (more == false) {
    /* last entry. update routing table now */
    if (nfa_ee_cb.ee_cfg_sts & NFA_EE_STS_CHANGED_ROUTING) {
      if (tlv_size) {
        nfa_ee_cb.ee_cfg_sts |= NFA_EE_STS_PREV_ROUTING;
      } else {
        nfa_ee_cb.ee_cfg_sts &= ~NFA_EE_STS_PREV_ROUTING;
      }
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s : set routing num_tlv:%d tlv_size:%d", __func__,
                          num_tlv, tlv_size);
      if (NFC_SetRouting(more, num_tlv, (uint8_t)(*p_cur_offset), ps + 1) ==
          NFA_STATUS_OK) {
        nfa_ee_cb.wait_rsp++;
      }
    } else if (nfa_ee_cb.ee_cfg_sts & NFA_EE_STS_PREV_ROUTING) {
      if (tlv_size == 0) {
        nfa_ee_cb.ee_cfg_sts &= ~NFA_EE_STS_PREV_ROUTING;
        /* indicated routing is configured to NFCC */
        nfa_ee_cb.ee_cfg_sts |= NFA_EE_STS_CHANGED_ROUTING;
        if (NFC_SetRouting(more, 0, 0, ps + 1) == NFA_STATUS_OK) {
          nfa_ee_cb.wait_rsp++;
        }
      }
    }
  }
}

/*******************************************************************************
**
** Function         nfa_ee_need_recfg
**
** Description      Check if any API function to configure the routing table or
**                  VS is called since last update
**
**                  The algorithm for the NFCEE configuration handling is as
**                  follows:
**
**                  Each NFCEE_ID/DH has its own control block - tNFA_EE_ECB
**                  Each control block uses ecb_flags to keep track if an API
**                  that changes routing/VS is invoked. This ecb_flags is
**                  cleared at the end of nfa_ee_update_rout().
**
**                  nfa_ee_cb.ee_cfged is the bitmask of the control blocks with
**                  routing/VS configuration and NFA_EE_CFGED_UPDATE_NOW.
**                  nfa_ee_cb.ee_cfged is cleared and re-calculated at the end
**                  of nfa_ee_update_rout().
**
**                  nfa_ee_cb.ee_cfg_sts is used to check is any status is
**                  changed and the associated command is issued to NFCC.
**                  nfa_ee_cb.ee_cfg_sts is AND with NFA_EE_STS_PREV at the end
**                  of nfa_ee_update_rout() to clear the NFA_EE_STS_CHANGED bits
**                  (except NFA_EE_STS_CHANGED_CANNED_VS is cleared in
**                  nfa_ee_vs_cback)
**
** Returns          TRUE if any configuration is changed
**
*******************************************************************************/
static bool nfa_ee_need_recfg(void) {
  bool needed = false;
  uint32_t xx;
  tNFA_EE_ECB* p_cb;
  uint8_t mask;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("ee_cfged: 0x%02x ee_cfg_sts: 0x%02x", nfa_ee_cb.ee_cfged,
                      nfa_ee_cb.ee_cfg_sts);
  /* if no routing/vs is configured, do not need to send the info to NFCC */
  if (nfa_ee_cb.ee_cfged || nfa_ee_cb.ee_cfg_sts) {
    if (nfa_ee_cb.ee_cfg_sts & NFA_EE_STS_CHANGED) {
      needed = true;
    } else {
      p_cb = &nfa_ee_cb.ecb[NFA_EE_CB_4_DH];
      mask = 1 << NFA_EE_CB_4_DH;
      for (xx = 0; xx <= nfa_ee_cb.cur_ee; xx++) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%d: ecb_flags  : 0x%02x, mask: 0x%02x", xx, p_cb->ecb_flags, mask);
        if ((p_cb->ecb_flags) && (nfa_ee_cb.ee_cfged & mask)) {
          needed = true;
          break;
        }
        p_cb = &nfa_ee_cb.ecb[xx];
        mask = 1 << xx;
      }
    }
  }

  return needed;
}

/*******************************************************************************
**
** Function         nfa_ee_rout_timeout
**
** Description      Anytime VS or routing entries are changed,
**                  a 1 second timer is started. This function is called when
**                  the timer expires or NFA_EeUpdateNow() is called.
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_rout_timeout(__attribute__((unused)) tNFA_EE_MSG* p_data) {
  uint8_t ee_cfged = nfa_ee_cb.ee_cfged;

  DLOG_IF(INFO, nfc_debug_enabled) << __func__;
  if (nfa_ee_need_recfg()) {
    /* discovery is not started */
    nfa_ee_update_rout();
  }

  if (nfa_ee_cb.wait_rsp) nfa_ee_cb.ee_wait_evt |= NFA_EE_WAIT_UPDATE_RSP;
  if (ee_cfged & NFA_EE_CFGED_UPDATE_NOW) {
    /* need to report NFA_EE_UPDATED_EVT when done updating NFCC */
    nfa_ee_cb.ee_wait_evt |= NFA_EE_WAIT_UPDATE;
    if (!nfa_ee_cb.wait_rsp) {
      nfa_ee_report_update_evt();
    }
  }
}

/*******************************************************************************
**
** Function         nfa_ee_discv_timeout
**
** Description
**
**
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_discv_timeout(__attribute__((unused)) tNFA_EE_MSG* p_data) {
  if (NFA_GetNCIVersion() != NCI_VERSION_2_0) NFC_NfceeDiscover(false);
  if (nfa_ee_cb.p_enable_cback)
    (*nfa_ee_cb.p_enable_cback)(NFA_EE_DISC_STS_OFF);
}

/*******************************************************************************
**
** Function         nfa_ee_lmrt_to_nfcc
**
** Description      This function would set the listen mode routing table
**                  to NFCC.
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_lmrt_to_nfcc(__attribute__((unused)) tNFA_EE_MSG* p_data) {
  int xx;
  tNFA_EE_ECB* p_cb;
  uint8_t* p = nullptr;
  bool more = true;
  bool check = true;
  uint8_t last_active = NFA_EE_INVALID;
  int max_len;
  tNFA_STATUS status = NFA_STATUS_FAILED;
  int cur_offset;
  uint8_t max_tlv;

  /* update routing table: DH and the activated NFCEEs */
  max_len = (NFC_GetLmrtSize() > NFA_EE_ROUT_BUF_SIZE) ? NFC_GetLmrtSize()
                                                       : NFA_EE_ROUT_BUF_SIZE;
  p = (uint8_t*)GKI_getbuf(max_len);
  #if (NXP_EXTNS == TRUE)
  if (nfa_t4tnfcee_is_enabled()) nfa_ee_add_t4tnfcee_aid(p, &cur_offset);
  #endif
  if (p == nullptr) {
    LOG(ERROR) << StringPrintf("no buffer to send routing info.");
    tNFA_EE_CBACK_DATA nfa_ee_cback_data;
    nfa_ee_cback_data.status = status;
    nfa_ee_report_event(nullptr, NFA_EE_NO_MEM_ERR_EVT, &nfa_ee_cback_data);
    return;
  }

  /* find the last active NFCEE. */
  if (nfa_ee_cb.cur_ee > 0) p_cb = &nfa_ee_cb.ecb[nfa_ee_cb.cur_ee - 1];

  for (xx = 0; xx < nfa_ee_cb.cur_ee; xx++, p_cb--) {
    if (p_cb->ee_status == NFC_NFCEE_STATUS_ACTIVE) {
      if (last_active == NFA_EE_INVALID) {
        last_active = p_cb->nfcee_id;
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("last_active: 0x%x", last_active);
      }
    }
  }
  if (last_active == NFA_EE_INVALID) {
    check = false;
  }

  max_tlv =
      (uint8_t)((max_len > NFA_EE_ROUT_MAX_TLV_SIZE) ? NFA_EE_ROUT_MAX_TLV_SIZE
                                                     : max_len);
  cur_offset = 0;
  /* use the first byte of the buffer (p) to keep the num_tlv */
  *p = 0;
  for (int rt = NCI_ROUTE_ORDER_AID; rt <= NCI_ROUTE_ORDER_TECHNOLOGY; rt++) {
    /* add the routing entries for NFCEEs */
    p_cb = &nfa_ee_cb.ecb[0];

    for (xx = 0; (xx < nfa_ee_cb.cur_ee) && check; xx++, p_cb++) {
      if (p_cb->ee_status == NFC_NFCEE_STATUS_ACTIVE) {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s --add the routing for NFCEEs!!", __func__);
        nfa_ee_route_add_one_ecb_by_route_order(p_cb, rt, &max_len, more, p,
                                                &cur_offset);
      }
    }
    if (rt == NCI_ROUTE_ORDER_TECHNOLOGY) more = false;
    /* add the routing entries for DH */
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s --add the routing for DH!!", __func__);
    nfa_ee_route_add_one_ecb_by_route_order(&nfa_ee_cb.ecb[NFA_EE_CB_4_DH], rt,
                                            &max_len, more, p, &cur_offset);
  }

  GKI_freebuf(p);
}

/*******************************************************************************
**
** Function         nfa_ee_update_rout
**
** Description      This function would set the VS and listen mode routing table
**                  to NFCC.
**
** Returns          void
**
*******************************************************************************/
void nfa_ee_update_rout(void) {
  int xx;
  tNFA_EE_ECB* p_cb;
  uint8_t mask;
  tNFA_EE_MSG nfa_ee_msg;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nfa_ee_update_rout ee_cfg_sts:0x%02x", nfa_ee_cb.ee_cfg_sts);

  /* use action function to send routing and VS configuration to NFCC */
  nfa_ee_msg.hdr.event = NFA_EE_CFG_TO_NFCC_EVT;
  nfa_ee_evt_hdlr(&nfa_ee_msg.hdr);

  /* all configuration is updated to NFCC, clear the status mask */
  nfa_ee_cb.ee_cfg_sts &= NFA_EE_STS_PREV;
  nfa_ee_cb.ee_cfged = 0;
  p_cb = &nfa_ee_cb.ecb[0];
  for (xx = 0; xx < NFA_EE_NUM_ECBS; xx++, p_cb++) {
    p_cb->ecb_flags = 0;
    mask = (1 << xx);
    if (p_cb->tech_switch_on | p_cb->tech_switch_off | p_cb->tech_battery_off |
        p_cb->proto_switch_on | p_cb->proto_switch_off |
        p_cb->proto_battery_off | p_cb->aid_entries |
        p_cb->sys_code_cfg_entries) {
      /* this entry has routing configuration. mark it configured */
      nfa_ee_cb.ee_cfged |= mask;
    }
  }
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfa_ee_update_rout ee_cfg_sts:0x%02x ee_cfged:0x%02x",
                      nfa_ee_cb.ee_cfg_sts, nfa_ee_cb.ee_cfged);
}

#if(NXP_EXTNS == TRUE)
/*******************************************************************************
**
** Function         nfa_ee_add_t4tnfcee_aid
**
** Description      Adds t4t Nfcee AID at the beginning top of routing table
**
** Returns          none
**
*******************************************************************************/
static void nfa_ee_add_t4tnfcee_aid(uint8_t* p, int* cur_offset) {
  const uint8_t t4tNfcee[] = {0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};
  int t4tNfceeRoute = T4TNFCEE_TARGET_HANDLE;
  unsigned long t4tNfceePower = 0x00;
  uint8_t* pp;
  t4tNfceePower =
      NfcConfig::getUnsigned(NAME_DEFAULT_T4TNFCEE_AID_POWER_STATE, 0x00);
  if (!t4tNfceePower) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("t4tNfceePower not found; taking default value");
    t4tNfceePower = (NCI_ROUTE_PWR_STATE_ON | NCI_ROUTE_PWR_STATE_SWITCH_OFF);
    t4tNfceePower |= NCI_ROUTE_PWR_STATE_SCREEN_ON_LOCK();
    t4tNfceePower |= NCI_ROUTE_PWR_STATE_SCREEN_OFF_UNLOCK();
    t4tNfceePower |= NCI_ROUTE_PWR_STATE_SCREEN_OFF_LOCK();
  } else {
    t4tNfceePower = T4TNFCEE_AID_POWER_STATE;
  }

  /*Number of Entries. Current Entry 1.
   *Later same values will be incremented with required number of entries
   */
  *p = 0x01;
  pp = p + 1;
  *pp++ = NFC_ROUTE_TAG_AID;
  *pp++ = sizeof(t4tNfcee) + 2;  // sizeof(t4tNfcee) + size(t4tNfceeRoute):1byte
                                 // + size(t4tNfceePower):1byte
  *pp++ = t4tNfceeRoute;
  *pp++ = (uint8_t)t4tNfceePower;
  memcpy(pp, t4tNfcee, sizeof(t4tNfcee));

  *cur_offset = (uint8_t)(pp - (p + 1)) + sizeof(t4tNfcee);
}
#endif
