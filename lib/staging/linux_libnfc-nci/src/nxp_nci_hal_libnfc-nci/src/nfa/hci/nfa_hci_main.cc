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
 *  This is the main implementation file for the NFA HCI.
 *
 ******************************************************************************/
#include <string.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nfa_dm_int.h"
#include "nfa_ee_api.h"
#include "nfa_ee_int.h"
#include "nfa_hci_api.h"
#include "nfa_hci_defs.h"
#include "nfa_hci_int.h"
#include "nfa_nv_co.h"
#include "nfa_sys_int.h"

////using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/*****************************************************************************
**  Global Variables
*****************************************************************************/

tNFA_HCI_CB nfa_hci_cb;

#ifndef NFA_HCI_NV_READ_TIMEOUT_VAL
#define NFA_HCI_NV_READ_TIMEOUT_VAL 1000
#endif

#ifndef NFA_HCI_CON_CREATE_TIMEOUT_VAL
#define NFA_HCI_CON_CREATE_TIMEOUT_VAL 1000
#endif

/*****************************************************************************
**  Static Functions
*****************************************************************************/

/* event handler function type */
static bool nfa_hci_evt_hdlr(NFC_HDR* p_msg);

static void nfa_hci_sys_enable(void);
static void nfa_hci_sys_disable(void);
static void nfa_hci_rsp_timeout (tNFA_HCI_EVENT_DATA *p_evt_data);
static void nfa_hci_conn_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                               tNFC_CONN* p_data);
static void nfa_hci_set_receive_buf(uint8_t pipe);
static void nfa_hci_assemble_msg(uint8_t* p_data, uint16_t data_len);
static void nfa_hci_handle_nv_read(uint8_t block, tNFA_STATUS status);

/*****************************************************************************
**  Constants
*****************************************************************************/
static const tNFA_SYS_REG nfa_hci_sys_reg = {
    nfa_hci_sys_enable, nfa_hci_evt_hdlr, nfa_hci_sys_disable,
    nfa_hci_proc_nfcc_power_mode};

/*******************************************************************************
**
** Function         nfa_hci_ee_info_cback
**
** Description      Callback function
**
** Returns          None
**
*******************************************************************************/
void nfa_hci_ee_info_cback(tNFA_EE_DISC_STS status) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%d", status);

  switch (status) {
    case NFA_EE_DISC_STS_ON:
      if ((!nfa_hci_cb.ee_disc_cmplt) &&
          ((nfa_hci_cb.hci_state == NFA_HCI_STATE_STARTUP) ||
           (nfa_hci_cb.hci_state == NFA_HCI_STATE_RESTORE))) {
        /* NFCEE Discovery is in progress */
        nfa_hci_cb.ee_disc_cmplt = true;
        nfa_hci_cb.num_ee_dis_req_ntf = 0;
        nfa_hci_cb.num_hot_plug_evts = 0;
        nfa_hci_cb.conn_id = 0;
        nfa_hci_startup();
      }
      break;

    case NFA_EE_DISC_STS_OFF:
      if (nfa_hci_cb.ee_disable_disc) break;
      nfa_hci_cb.ee_disable_disc = true;

      if ((nfa_hci_cb.hci_state == NFA_HCI_STATE_WAIT_NETWK_ENABLE) ||
          (nfa_hci_cb.hci_state == NFA_HCI_STATE_RESTORE_NETWK_ENABLE)) {
        if ((nfa_hci_cb.num_nfcee <= 1) ||
            (nfa_hci_cb.num_ee_dis_req_ntf == (nfa_hci_cb.num_nfcee - 1)) ||
            (nfa_hci_cb.num_hot_plug_evts == (nfa_hci_cb.num_nfcee - 1))) {
          /* No UICC Host is detected or
           * HOT_PLUG_EVT(s) and or EE DISC REQ Ntf(s) are already received
           * Get Host list and notify SYS on Initialization complete */
          nfa_sys_stop_timer(&nfa_hci_cb.timer);
          if ((nfa_hci_cb.num_nfcee > 1) &&
              (nfa_hci_cb.num_ee_dis_req_ntf != (nfa_hci_cb.num_nfcee - 1))) {
            /* Received HOT PLUG EVT, we will also wait for EE DISC REQ Ntf(s)
             */
            nfa_sys_start_timer(&nfa_hci_cb.timer, NFA_HCI_RSP_TIMEOUT_EVT,
                                p_nfa_hci_cfg->hci_netwk_enable_timeout);
          } else {
            nfa_hci_cb.w4_hci_netwk_init = false;
            nfa_hciu_send_get_param_cmd(NFA_HCI_ADMIN_PIPE,
                                        NFA_HCI_HOST_LIST_INDEX);
          }
        }
      } else if (nfa_hci_cb.num_nfcee <= 1) {
        /* No UICC Host is detected, HCI NETWORK is enabled */
        nfa_hci_cb.w4_hci_netwk_init = false;
      }
      break;

    case NFA_EE_DISC_STS_REQ:
      nfa_hci_cb.num_ee_dis_req_ntf++;

      if (nfa_hci_cb.ee_disable_disc) {
        /* Already received Discovery Ntf */
        if ((nfa_hci_cb.hci_state == NFA_HCI_STATE_WAIT_NETWK_ENABLE) ||
            (nfa_hci_cb.hci_state == NFA_HCI_STATE_RESTORE_NETWK_ENABLE)) {
          /* Received DISC REQ Ntf while waiting for other Host in the network
           * to bootup after DH host bootup is complete */
          if ((nfa_hci_cb.num_ee_dis_req_ntf == (nfa_hci_cb.num_nfcee - 1)) &&
              NFC_GetNCIVersion() != NCI_VERSION_2_0) {
            /* Received expected number of EE DISC REQ Ntf(s) */
            nfa_sys_stop_timer(&nfa_hci_cb.timer);
            nfa_hci_cb.w4_hci_netwk_init = false;
            nfa_hciu_send_get_param_cmd(NFA_HCI_ADMIN_PIPE,
                                        NFA_HCI_HOST_LIST_INDEX);
          }
        } else if ((nfa_hci_cb.hci_state == NFA_HCI_STATE_STARTUP) ||
                   (nfa_hci_cb.hci_state == NFA_HCI_STATE_RESTORE)) {
          /* Received DISC REQ Ntf during DH host bootup */
          if (nfa_hci_cb.num_ee_dis_req_ntf == (nfa_hci_cb.num_nfcee - 1)) {
            /* Received expected number of EE DISC REQ Ntf(s) */
            nfa_hci_cb.w4_hci_netwk_init = false;
          }
        }
      }
      break;
    case NFA_EE_RECOVERY_REDISCOVERED:
    case NFA_EE_MODE_SET_COMPLETE:
      /*received mode set Ntf */
      if ((nfa_hci_cb.hci_state == NFA_HCI_STATE_WAIT_NETWK_ENABLE) ||
                (nfa_hci_cb.hci_state == NFA_HCI_STATE_RESTORE_NETWK_ENABLE)) {
              /* Discovery operation is complete, retrieve discovery result */
          NFA_EeGetInfo(&nfa_hci_cb.num_nfcee, nfa_hci_cb.ee_info);
          nfa_hci_enable_one_nfcee();
        }
      break;
    case NFA_EE_RECOVERY_INIT:
      /*NFCEE recovery in progress*/
      nfa_ee_cb.isDiscoveryStopped = nfa_dm_act_stop_rf_discovery(nullptr);
      nfa_hci_cb.hci_state = NFA_HCI_STATE_EE_RECOVERY;
      break;
  }
}

/*******************************************************************************
**
** Function         nfa_hci_init
**
** Description      Initialize NFA HCI
**
** Returns          None
**
*******************************************************************************/
void nfa_hci_init(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  /* initialize control block */
  memset(&nfa_hci_cb, 0, sizeof(tNFA_HCI_CB));

  nfa_hci_cb.hci_state = NFA_HCI_STATE_STARTUP;
  nfa_hci_cb.num_nfcee = NFA_HCI_MAX_HOST_IN_NETWORK;
  /* register message handler on NFA SYS */
  nfa_sys_register(NFA_ID_HCI, &nfa_hci_sys_reg);
}

/*******************************************************************************
**
** Function         nfa_hci_is_valid_cfg
**
** Description      Validate hci control block config parameters
**
** Returns          None
**
*******************************************************************************/
bool nfa_hci_is_valid_cfg(void) {
  uint8_t xx, yy, zz;
  tNFA_HANDLE reg_app[NFA_HCI_MAX_APP_CB];
  uint8_t valid_gate[NFA_HCI_MAX_GATE_CB];
  uint8_t app_count = 0;
  uint8_t gate_count = 0;
  uint32_t pipe_inx_mask = 0;

  /* First, see if valid values are stored in app names, send connectivity
   * events flag */
  for (xx = 0; xx < NFA_HCI_MAX_APP_CB; xx++) {
    /* Check if app name is valid with null terminated string */
    if (strlen(&nfa_hci_cb.cfg.reg_app_names[xx][0]) > NFA_MAX_HCI_APP_NAME_LEN)
      return false;

    /* Send Connectivity event flag can be either TRUE or FALSE */
    if ((nfa_hci_cb.cfg.b_send_conn_evts[xx] != true) &&
        (nfa_hci_cb.cfg.b_send_conn_evts[xx] != false))
      return false;

    if (nfa_hci_cb.cfg.reg_app_names[xx][0] != 0) {
      /* Check if the app name is present more than one time in the control
       * block */
      for (yy = xx + 1; yy < NFA_HCI_MAX_APP_CB; yy++) {
        if ((nfa_hci_cb.cfg.reg_app_names[yy][0] != 0) &&
            (!strncmp(&nfa_hci_cb.cfg.reg_app_names[xx][0],
                      &nfa_hci_cb.cfg.reg_app_names[yy][0],
                      strlen(nfa_hci_cb.cfg.reg_app_names[xx])))) {
          /* Two app cannot have the same name , NVRAM is corrupted */
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("nfa_hci_is_valid_cfg (%s)  Reusing: %u",
                              &nfa_hci_cb.cfg.reg_app_names[xx][0], xx);
          return false;
        }
      }
      /* Collect list of hci handle */
      reg_app[app_count++] = (tNFA_HANDLE)(xx | NFA_HANDLE_GROUP_HCI);
    }
  }

  /* Validate Gate Control block */
  for (xx = 0; xx < NFA_HCI_MAX_GATE_CB; xx++) {
    if (nfa_hci_cb.cfg.dyn_gates[xx].gate_id != 0) {
      if (((nfa_hci_cb.cfg.dyn_gates[xx].gate_id != NFA_HCI_LOOP_BACK_GATE) &&
           (nfa_hci_cb.cfg.dyn_gates[xx].gate_id !=
            NFA_HCI_IDENTITY_MANAGEMENT_GATE) &&
           (nfa_hci_cb.cfg.dyn_gates[xx].gate_id <
            NFA_HCI_FIRST_HOST_SPECIFIC_GENERIC_GATE)) ||
          (nfa_hci_cb.cfg.dyn_gates[xx].gate_id > NFA_HCI_LAST_PROP_GATE))
        return false;

      /* Check if the same gate id is present more than once in the control
       * block */
      for (yy = xx + 1; yy < NFA_HCI_MAX_GATE_CB; yy++) {
        if ((nfa_hci_cb.cfg.dyn_gates[yy].gate_id != 0) &&
            (nfa_hci_cb.cfg.dyn_gates[xx].gate_id ==
             nfa_hci_cb.cfg.dyn_gates[yy].gate_id)) {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("nfa_hci_is_valid_cfg  Reusing: %u",
                              nfa_hci_cb.cfg.dyn_gates[xx].gate_id);
          return false;
        }
      }
      if ((nfa_hci_cb.cfg.dyn_gates[xx].gate_owner & (~NFA_HANDLE_GROUP_HCI)) >=
          NFA_HCI_MAX_APP_CB) {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("nfa_hci_is_valid_cfg  Invalid Gate owner: %u",
                            nfa_hci_cb.cfg.dyn_gates[xx].gate_owner);
        return false;
      }
      if (!((nfa_hci_cb.cfg.dyn_gates[xx].gate_id ==
             NFA_HCI_CONNECTIVITY_GATE) ||
            ((nfa_hci_cb.cfg.dyn_gates[xx].gate_id >=
              NFA_HCI_PROP_GATE_FIRST) ||
             (nfa_hci_cb.cfg.dyn_gates[xx].gate_id <=
              NFA_HCI_PROP_GATE_LAST)))) {
        /* The gate owner should be one of the registered application */
        for (zz = 0; zz < app_count; zz++) {
          if (nfa_hci_cb.cfg.dyn_gates[xx].gate_owner == reg_app[zz]) break;
        }
        if (zz == app_count) {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("nfa_hci_is_valid_cfg  Invalid Gate owner: %u",
                              nfa_hci_cb.cfg.dyn_gates[xx].gate_owner);
          return false;
        }
      }
      /* Collect list of allocated gates */
      valid_gate[gate_count++] = nfa_hci_cb.cfg.dyn_gates[xx].gate_id;

      /* No two gates can own a same pipe */
      if ((pipe_inx_mask & nfa_hci_cb.cfg.dyn_gates[xx].pipe_inx_mask) != 0)
        return false;
      /* Collect the list of pipes on this gate */
      pipe_inx_mask |= nfa_hci_cb.cfg.dyn_gates[xx].pipe_inx_mask;
    }
  }

  for (xx = 0; (pipe_inx_mask && (xx < NFA_HCI_MAX_PIPE_CB));
       xx++, pipe_inx_mask >>= 1) {
    /* Every bit set in pipe increment mask indicates a valid pipe */
    if (pipe_inx_mask & 1) {
      /* Check if the pipe is valid one */
      if (nfa_hci_cb.cfg.dyn_pipes[xx].pipe_id < NFA_HCI_FIRST_DYNAMIC_PIPE)
        return false;
    }
  }

  if (xx == NFA_HCI_MAX_PIPE_CB) return false;

  /* Validate Gate Control block */
  for (xx = 0; xx < NFA_HCI_MAX_PIPE_CB; xx++) {
    if (nfa_hci_cb.cfg.dyn_pipes[xx].pipe_id != 0) {
      /* Check if pipe id is valid */
      if (nfa_hci_cb.cfg.dyn_pipes[xx].pipe_id < NFA_HCI_FIRST_DYNAMIC_PIPE)
        return false;

      /* Check if pipe state is valid */
      if ((nfa_hci_cb.cfg.dyn_pipes[xx].pipe_state != NFA_HCI_PIPE_OPENED) &&
          (nfa_hci_cb.cfg.dyn_pipes[xx].pipe_state != NFA_HCI_PIPE_CLOSED))
        return false;

      /* Check if local gate on which the pipe is created is valid */
      if ((((nfa_hci_cb.cfg.dyn_pipes[xx].local_gate !=
             NFA_HCI_LOOP_BACK_GATE) &&
            (nfa_hci_cb.cfg.dyn_pipes[xx].local_gate !=
             NFA_HCI_IDENTITY_MANAGEMENT_GATE)) &&
           (nfa_hci_cb.cfg.dyn_pipes[xx].local_gate <
            NFA_HCI_FIRST_HOST_SPECIFIC_GENERIC_GATE)) ||
          (nfa_hci_cb.cfg.dyn_pipes[xx].local_gate > NFA_HCI_LAST_PROP_GATE))
        return false;

      /* Check if the peer gate on which the pipe is created is valid */
      if ((((nfa_hci_cb.cfg.dyn_pipes[xx].dest_gate !=
             NFA_HCI_LOOP_BACK_GATE) &&
            (nfa_hci_cb.cfg.dyn_pipes[xx].dest_gate !=
             NFA_HCI_IDENTITY_MANAGEMENT_GATE)) &&
           (nfa_hci_cb.cfg.dyn_pipes[xx].dest_gate <
            NFA_HCI_FIRST_HOST_SPECIFIC_GENERIC_GATE)) ||
          (nfa_hci_cb.cfg.dyn_pipes[xx].dest_gate > NFA_HCI_LAST_PROP_GATE))
        return false;

      /* Check if the same pipe is present more than once in the control block
       */
      for (yy = xx + 1; yy < NFA_HCI_MAX_PIPE_CB; yy++) {
        if ((nfa_hci_cb.cfg.dyn_pipes[yy].pipe_id != 0) &&
            (nfa_hci_cb.cfg.dyn_pipes[xx].pipe_id ==
             nfa_hci_cb.cfg.dyn_pipes[yy].pipe_id)) {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("nfa_hci_is_valid_cfg  Reusing: %u",
                              nfa_hci_cb.cfg.dyn_pipes[xx].pipe_id);
          return false;
        }
      }
      /* The local gate should be one of the element in gate control block */
      for (zz = 0; zz < gate_count; zz++) {
        if (nfa_hci_cb.cfg.dyn_pipes[xx].local_gate == valid_gate[zz]) break;
      }
      if (zz == gate_count) {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("nfa_hci_is_valid_cfg  Invalid Gate: %u",
                            nfa_hci_cb.cfg.dyn_pipes[xx].local_gate);
        return false;
      }
    }
  }

  /* Check if admin pipe state is valid */
  if ((nfa_hci_cb.cfg.admin_gate.pipe01_state != NFA_HCI_PIPE_OPENED) &&
      (nfa_hci_cb.cfg.admin_gate.pipe01_state != NFA_HCI_PIPE_CLOSED))
    return false;

  /* Check if link management pipe state is valid */
  if ((nfa_hci_cb.cfg.link_mgmt_gate.pipe00_state != NFA_HCI_PIPE_OPENED) &&
      (nfa_hci_cb.cfg.link_mgmt_gate.pipe00_state != NFA_HCI_PIPE_CLOSED))
    return false;

  pipe_inx_mask = nfa_hci_cb.cfg.id_mgmt_gate.pipe_inx_mask;
  for (xx = 0; (pipe_inx_mask && (xx < NFA_HCI_MAX_PIPE_CB));
       xx++, pipe_inx_mask >>= 1) {
    /* Every bit set in pipe increment mask indicates a valid pipe */
    if (pipe_inx_mask & 1) {
      /* Check if the pipe is valid one */
      if (nfa_hci_cb.cfg.dyn_pipes[xx].pipe_id < NFA_HCI_FIRST_DYNAMIC_PIPE)
        return false;
      /* Check if the pipe is connected to Identity management gate */
      if (nfa_hci_cb.cfg.dyn_pipes[xx].local_gate !=
          NFA_HCI_IDENTITY_MANAGEMENT_GATE)
        return false;
    }
  }
  if (xx == NFA_HCI_MAX_PIPE_CB) return false;

  return true;
}

/*******************************************************************************
**
** Function         nfa_hci_cfg_default
**
** Description      Configure default values for hci control block
**
** Returns          None
**
*******************************************************************************/
void nfa_hci_restore_default_config(uint8_t* p_session_id) {
  memset(&nfa_hci_cb.cfg, 0, sizeof(nfa_hci_cb.cfg));
  memcpy(nfa_hci_cb.cfg.admin_gate.session_id, p_session_id,
         NFA_HCI_SESSION_ID_LEN);
  nfa_hci_cb.nv_write_needed = true;
}

/*******************************************************************************
**
** Function         nfa_hci_proc_nfcc_power_mode
**
** Description      Restore NFA HCI sub-module
**
** Returns          None
**
*******************************************************************************/
void nfa_hci_proc_nfcc_power_mode(uint8_t nfcc_power_mode) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("nfcc_power_mode=%d", nfcc_power_mode);

  /* if NFCC power mode is change to full power */
  if (nfcc_power_mode == NFA_DM_PWR_MODE_FULL) {
    nfa_hci_cb.b_low_power_mode = false;
    if (nfa_hci_cb.hci_state == NFA_HCI_STATE_IDLE) {
      nfa_hci_cb.hci_state = NFA_HCI_STATE_RESTORE;
      nfa_hci_cb.ee_disc_cmplt = false;
      nfa_hci_cb.ee_disable_disc = true;
      if (nfa_hci_cb.num_nfcee > 1)
        nfa_hci_cb.w4_hci_netwk_init = true;
      else
        nfa_hci_cb.w4_hci_netwk_init = false;
      nfa_hci_cb.conn_id = 0;
      nfa_hci_cb.num_ee_dis_req_ntf = 0;
      nfa_hci_cb.num_hot_plug_evts = 0;
    } else {
      LOG(ERROR) << StringPrintf("Cannot restore now");
      nfa_sys_cback_notify_nfcc_power_mode_proc_complete(NFA_ID_HCI);
    }
  } else {
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
    nfa_hci_cb.w4_rsp_evt = false;
    nfa_hci_cb.conn_id = 0;
    nfa_sys_stop_timer(&nfa_hci_cb.timer);
    nfa_hci_cb.b_low_power_mode = true;
    nfa_sys_cback_notify_nfcc_power_mode_proc_complete(NFA_ID_HCI);
  }
}

/*******************************************************************************
**
** Function         nfa_hci_dh_startup_complete
**
** Description      Initialization of terminal host in HCI Network is completed
**                  Wait for other host in the network to initialize
**
** Returns          None
**
*******************************************************************************/
void nfa_hci_dh_startup_complete(void) {
  if (nfa_hci_cb.w4_hci_netwk_init) {
    if (nfa_hci_cb.hci_state == NFA_HCI_STATE_STARTUP) {
      nfa_hci_cb.hci_state = NFA_HCI_STATE_WAIT_NETWK_ENABLE;
      /* Wait for EE Discovery to complete */
      nfa_sys_start_timer(&nfa_hci_cb.timer, NFA_HCI_RSP_TIMEOUT_EVT,
                          NFA_EE_DISCV_TIMEOUT_VAL);
    } else if (nfa_hci_cb.hci_state == NFA_HCI_STATE_RESTORE) {
      nfa_hci_cb.hci_state = NFA_HCI_STATE_RESTORE_NETWK_ENABLE;
      /* No HCP packet to DH for a specified period of time indicates all host
       * in the network is initialized */
      nfa_sys_start_timer(&nfa_hci_cb.timer, NFA_HCI_RSP_TIMEOUT_EVT,
                          p_nfa_hci_cfg->hci_netwk_enable_timeout);
    }
  } else if ((nfa_hci_cb.num_nfcee > 1) &&
             (nfa_hci_cb.num_ee_dis_req_ntf != (nfa_hci_cb.num_nfcee - 1))) {
    if (nfa_hci_cb.hci_state == NFA_HCI_STATE_RESTORE)
      nfa_hci_cb.ee_disable_disc = true;
    /* Received HOT PLUG EVT, we will also wait for EE DISC REQ Ntf(s) */
    nfa_sys_start_timer(&nfa_hci_cb.timer, NFA_HCI_RSP_TIMEOUT_EVT,
                        p_nfa_hci_cfg->hci_netwk_enable_timeout);
  } else {
    /* Received EE DISC REQ Ntf(s) */
    nfa_hciu_send_get_param_cmd(NFA_HCI_ADMIN_PIPE, NFA_HCI_HOST_LIST_INDEX);
  }
}

/*******************************************************************************
**
** Function         nfa_hci_startup_complete
**
** Description      HCI network initialization is completed
**
** Returns          None
**
*******************************************************************************/
void nfa_hci_startup_complete(tNFA_STATUS status) {
  tNFA_HCI_EVT_DATA evt_data;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("Status: %u", status);

  nfa_sys_stop_timer(&nfa_hci_cb.timer);

  if ((nfa_hci_cb.hci_state == NFA_HCI_STATE_RESTORE) ||
      (nfa_hci_cb.hci_state == NFA_HCI_STATE_RESTORE_NETWK_ENABLE)) {
    nfa_ee_proc_hci_info_cback();
    nfa_sys_cback_notify_nfcc_power_mode_proc_complete(NFA_ID_HCI);
  } else {
    evt_data.hci_init.status = status;

    nfa_hciu_send_to_all_apps(NFA_HCI_INIT_EVT, &evt_data);
    nfa_sys_cback_notify_enable_complete(NFA_ID_HCI);
  }

  if (status == NFA_STATUS_OK)
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;

  else
    nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
}

/*******************************************************************************
**
** Function         nfa_hci_enable_one_nfcee
**
** Description      Enable NFCEE Hosts which are discovered.
**
** Returns          None
**
*******************************************************************************/
void nfa_hci_enable_one_nfcee(void) {
  uint8_t xx;
  uint8_t nfceeid = 0;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%d", nfa_hci_cb.num_nfcee);

  for (xx = 0; xx < nfa_hci_cb.num_nfcee; xx++) {
    nfceeid = nfa_hci_cb.ee_info[xx].ee_handle & ~NFA_HANDLE_GROUP_EE;
    if (nfa_hci_cb.ee_info[xx].ee_status == NFA_EE_STATUS_INACTIVE) {
      NFC_NfceeModeSet(nfceeid, NFC_MODE_ACTIVATE);
      return;
    }
  }

  if (xx == nfa_hci_cb.num_nfcee) {
    if ((nfa_hci_cb.hci_state == NFA_HCI_STATE_WAIT_NETWK_ENABLE) ||
        (nfa_hci_cb.hci_state == NFA_HCI_STATE_RESTORE_NETWK_ENABLE)) {
      nfa_hciu_send_get_param_cmd(NFA_HCI_ADMIN_PIPE, NFA_HCI_HOST_LIST_INDEX);
    } else if (nfa_hci_cb.hci_state == NFA_HCI_STATE_EE_RECOVERY) {
      nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
      if (nfa_ee_cb.isDiscoveryStopped == true) {
        nfa_dm_act_start_rf_discovery(nullptr);
        nfa_ee_cb.isDiscoveryStopped = false;
      }
    }
  }
}

/*******************************************************************************
**
** Function         nfa_hci_startup
**
** Description      Perform HCI startup
**
** Returns          None
**
*******************************************************************************/
void nfa_hci_startup(void) {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  uint8_t target_handle;
  uint8_t count = 0;
  bool found = false;

  if (HCI_LOOPBACK_DEBUG == NFA_HCI_DEBUG_ON) {
    /* First step in initialization is to open the admin pipe */
    nfa_hciu_send_open_pipe_cmd(NFA_HCI_ADMIN_PIPE);
    return;
  }

  /* We can only start up if NV Ram is read and EE discovery is complete */
  if (nfa_hci_cb.nv_read_cmplt && nfa_hci_cb.ee_disc_cmplt &&
      (nfa_hci_cb.conn_id == 0)) {
    if (NFC_GetNCIVersion() == NCI_VERSION_2_0) {
      NFC_SetStaticHciCback(nfa_hci_conn_cback);
    } else {
      NFA_EeGetInfo(&nfa_hci_cb.num_nfcee, nfa_hci_cb.ee_info);

      while ((count < nfa_hci_cb.num_nfcee) && (!found)) {
        target_handle = (uint8_t)nfa_hci_cb.ee_info[count].ee_handle;

        if (nfa_hci_cb.ee_info[count].ee_interface[0] ==
            NFA_EE_INTERFACE_HCI_ACCESS) {
          found = true;

          if (nfa_hci_cb.ee_info[count].ee_status == NFA_EE_STATUS_INACTIVE) {
            NFC_NfceeModeSet(target_handle, NFC_MODE_ACTIVATE);
          }
          if ((status = NFC_ConnCreate(NCI_DEST_TYPE_NFCEE, target_handle,
                                       NFA_EE_INTERFACE_HCI_ACCESS,
                                       nfa_hci_conn_cback)) == NFA_STATUS_OK)
            nfa_sys_start_timer(&nfa_hci_cb.timer, NFA_HCI_RSP_TIMEOUT_EVT,
                                NFA_HCI_CON_CREATE_TIMEOUT_VAL);
          else {
            nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
            LOG(ERROR) << StringPrintf(
                "nfa_hci_startup - Failed to Create Logical connection. HCI "
                "Initialization/Restore failed");
            nfa_hci_startup_complete(NFA_STATUS_FAILED);
          }
        }
        count++;
      }
      if (!found) {
        LOG(ERROR) << StringPrintf(
            "nfa_hci_startup - HCI ACCESS Interface not discovered. HCI "
            "Initialization/Restore failed");
        nfa_hci_startup_complete(NFA_STATUS_FAILED);
      }
    }
  }
}

/*******************************************************************************
**
** Function         nfa_hci_sys_enable
**
** Description      Enable NFA HCI
**
** Returns          None
**
*******************************************************************************/
static void nfa_hci_sys_enable(void) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;
  nfa_ee_reg_cback_enable_done(&nfa_hci_ee_info_cback);

  nfa_nv_co_read((uint8_t*)&nfa_hci_cb.cfg, sizeof(nfa_hci_cb.cfg),
                 DH_NV_BLOCK);
  nfa_sys_start_timer(&nfa_hci_cb.timer, NFA_HCI_RSP_TIMEOUT_EVT,
                      NFA_HCI_NV_READ_TIMEOUT_VAL);
}

/*******************************************************************************
**
** Function         nfa_hci_sys_disable
**
** Description      Disable NFA HCI
**
** Returns          None
**
*******************************************************************************/
static void nfa_hci_sys_disable(void) {
  tNFA_HCI_EVT_DATA evt_data;

  nfa_sys_stop_timer(&nfa_hci_cb.timer);

  if (nfa_hci_cb.conn_id) {
    if (nfa_sys_is_graceful_disable()) {
      /* Tell all applications stack is down */
      if (NFC_GetNCIVersion() == NCI_VERSION_1_0) {
        nfa_hciu_send_to_all_apps(NFA_HCI_EXIT_EVT, &evt_data);
        NFC_ConnClose(nfa_hci_cb.conn_id);
        return;
      }
    }
    nfa_hci_cb.conn_id = 0;
  }

  nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
  /* deregister message handler on NFA SYS */
  nfa_sys_deregister(NFA_ID_HCI);
}

/*******************************************************************************
**
** Function         nfa_hci_conn_cback
**
** Description      This function Process event from NCI
**
** Returns          None
**
*******************************************************************************/
static void nfa_hci_conn_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                               tNFC_CONN* p_data) {
  uint8_t* p;
  NFC_HDR* p_pkt = (NFC_HDR*)p_data->data.p_data;
  uint8_t chaining_bit;
  uint8_t pipe;
  uint16_t pkt_len;
  const uint8_t MAX_BUFF_SIZE = 100;
  char buff[MAX_BUFF_SIZE];
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s State: %u  Cmd: %u", __func__, nfa_hci_cb.hci_state, event);
  if (event == NFC_CONN_CREATE_CEVT) {
    nfa_hci_cb.conn_id = conn_id;
    nfa_hci_cb.buff_size = p_data->conn_create.buff_size;

    if (nfa_hci_cb.hci_state == NFA_HCI_STATE_STARTUP) {
      nfa_hci_cb.w4_hci_netwk_init = true;
      nfa_hciu_alloc_gate(NFA_HCI_CONNECTIVITY_GATE, 0);
    }

    if (nfa_hci_cb.cfg.admin_gate.pipe01_state == NFA_HCI_PIPE_CLOSED) {
      /* First step in initialization/restore is to open the admin pipe */
      nfa_hciu_send_open_pipe_cmd(NFA_HCI_ADMIN_PIPE);
    } else {
      /* Read session id, to know DH session id is correct */
      nfa_hciu_send_get_param_cmd(NFA_HCI_ADMIN_PIPE,
                                  NFA_HCI_SESSION_IDENTITY_INDEX);
    }
  } else if (event == NFC_CONN_CLOSE_CEVT) {
    nfa_hci_cb.conn_id = 0;
    nfa_hci_cb.hci_state = NFA_HCI_STATE_DISABLED;
    /* deregister message handler on NFA SYS */
    nfa_sys_deregister(NFA_ID_HCI);
  }

  if ((event != NFC_DATA_CEVT) || (p_pkt == nullptr)) return;

  if ((nfa_hci_cb.hci_state == NFA_HCI_STATE_WAIT_NETWK_ENABLE) ||
      (nfa_hci_cb.hci_state == NFA_HCI_STATE_RESTORE_NETWK_ENABLE)) {
    /* Received HCP Packet before timeout, Other Host initialization is not
     * complete */
    nfa_sys_stop_timer(&nfa_hci_cb.timer);
    if (nfa_hci_cb.w4_hci_netwk_init)
      nfa_sys_start_timer(&nfa_hci_cb.timer, NFA_HCI_RSP_TIMEOUT_EVT,
                          p_nfa_hci_cfg->hci_netwk_enable_timeout);
  }

  p = (uint8_t*)(p_pkt + 1) + p_pkt->offset;
  pkt_len = p_pkt->len;

  if (pkt_len < 1) {
    LOG(ERROR) << StringPrintf("Insufficient packet length! Dropping :%u bytes",
                               pkt_len);
    /* release GKI buffer */
    GKI_freebuf(p_pkt);
    return;
  }

  chaining_bit = ((*p) >> 0x07) & 0x01;
  pipe = (*p++) & 0x7F;
  if (pkt_len != 0) pkt_len--;

  if (nfa_hci_cb.assembling == false) {
    if (pkt_len < 1) {
      LOG(ERROR) << StringPrintf(
          "Insufficient packet length! Dropping :%u bytes", pkt_len);
      /* release GKI buffer */
      GKI_freebuf(p_pkt);
      return;
    }
    /* First Segment of a packet */
    nfa_hci_cb.type = ((*p) >> 0x06) & 0x03;
    nfa_hci_cb.inst = (*p++ & 0x3F);
    if (pkt_len != 0) pkt_len--;
    nfa_hci_cb.assembly_failed = false;
    nfa_hci_cb.msg_len = 0;

    if (chaining_bit == NFA_HCI_MESSAGE_FRAGMENTATION) {
      nfa_hci_cb.assembling = true;
      nfa_hci_set_receive_buf(pipe);
      nfa_hci_assemble_msg(p, pkt_len);
    } else {
      if ((pipe >= NFA_HCI_FIRST_DYNAMIC_PIPE) &&
          (nfa_hci_cb.type == NFA_HCI_EVENT_TYPE)) {
        nfa_hci_set_receive_buf(pipe);
        nfa_hci_assemble_msg(p, pkt_len);
        p = nfa_hci_cb.p_msg_data;
      }
    }
  } else {
    if (nfa_hci_cb.assembly_failed) {
      /* If Reassembly failed because of insufficient buffer, just drop the new
       * segmented packets */
      LOG(ERROR) << StringPrintf(
          "Insufficient buffer to Reassemble HCP "
          "packet! Dropping :%u bytes",
          pkt_len);
    } else {
      /* Reassemble the packet */
      nfa_hci_assemble_msg(p, pkt_len);
    }

    if (chaining_bit == NFA_HCI_NO_MESSAGE_FRAGMENTATION) {
      /* Just added the last segment in the chain. Reset pointers */
      nfa_hci_cb.assembling = false;
      p = nfa_hci_cb.p_msg_data;
      pkt_len = nfa_hci_cb.msg_len;
    }
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nfa_hci_conn_cback Recvd data pipe:%d  %s  chain:%d  assmbl:%d  len:%d",
      (uint8_t)pipe,
      nfa_hciu_get_type_inst_names(pipe, nfa_hci_cb.type, nfa_hci_cb.inst, buff,
                                   MAX_BUFF_SIZE),
      (uint8_t)chaining_bit, (uint8_t)nfa_hci_cb.assembling, p_pkt->len);

  /* If still reassembling fragments, just return */
  if (nfa_hci_cb.assembling) {
    /* if not last packet, release GKI buffer */
    GKI_freebuf(p_pkt);
    return;
  }

  /* If we got a response, cancel the response timer. Also, if waiting for */
  /* a single response, we can go back to idle state                       */
  if ((nfa_hci_cb.hci_state == NFA_HCI_STATE_WAIT_RSP) &&
      ((nfa_hci_cb.type == NFA_HCI_RESPONSE_TYPE) ||
       (nfa_hci_cb.w4_rsp_evt && (nfa_hci_cb.type == NFA_HCI_EVENT_TYPE)))) {
    nfa_sys_stop_timer(&nfa_hci_cb.timer);
    nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;
  }

  switch (pipe) {
    case NFA_HCI_ADMIN_PIPE:
      /* Check if data packet is a command, response or event */
      if (nfa_hci_cb.type == NFA_HCI_COMMAND_TYPE) {
        nfa_hci_handle_admin_gate_cmd(p);
      } else if (nfa_hci_cb.type == NFA_HCI_RESPONSE_TYPE) {
        nfa_hci_handle_admin_gate_rsp(p, (uint8_t)pkt_len);
      } else if (nfa_hci_cb.type == NFA_HCI_EVENT_TYPE) {
        nfa_hci_handle_admin_gate_evt();
      }
      break;

    case NFA_HCI_LINK_MANAGEMENT_PIPE:
      /* We don't send Link Management commands, we only get them */
      if (nfa_hci_cb.type == NFA_HCI_COMMAND_TYPE)
        nfa_hci_handle_link_mgm_gate_cmd(p);
      break;

    default:
      if (pipe >= NFA_HCI_FIRST_DYNAMIC_PIPE)
        nfa_hci_handle_dyn_pipe_pkt(pipe, p, pkt_len);
      break;
  }

  if ((nfa_hci_cb.type == NFA_HCI_RESPONSE_TYPE) ||
      (nfa_hci_cb.w4_rsp_evt && (nfa_hci_cb.type == NFA_HCI_EVENT_TYPE))) {
    nfa_hci_cb.w4_rsp_evt = false;
  }

  /* Send a message to ouselves to check for anything to do */
  p_pkt->event = NFA_HCI_CHECK_QUEUE_EVT;
  p_pkt->len = 0;
  nfa_sys_sendmsg(p_pkt);
}

/*******************************************************************************
**
** Function         nfa_hci_handle_nv_read
**
** Description      handler function for nv read complete event
**
** Returns          None
**
*******************************************************************************/
void nfa_hci_handle_nv_read(uint8_t block, tNFA_STATUS status) {
  uint8_t session_id[NFA_HCI_SESSION_ID_LEN];
  uint8_t default_session[NFA_HCI_SESSION_ID_LEN] = {0xFF, 0xFF, 0xFF, 0xFF,
                                                     0xFF, 0xFF, 0xFF, 0xFF};
  uint8_t reset_session[NFA_HCI_SESSION_ID_LEN] = {0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00};
  uint32_t os_tick;

  if (block == DH_NV_BLOCK) {
    /* Stop timer as NVDATA Read Completed */
    nfa_sys_stop_timer(&nfa_hci_cb.timer);
    nfa_hci_cb.nv_read_cmplt = true;
    if ((status != NFA_STATUS_OK) || (!nfa_hci_is_valid_cfg()) ||
        (!(memcmp(nfa_hci_cb.cfg.admin_gate.session_id, default_session,
                  NFA_HCI_SESSION_ID_LEN))) ||
        (!(memcmp(nfa_hci_cb.cfg.admin_gate.session_id, reset_session,
                  NFA_HCI_SESSION_ID_LEN)))) {
      nfa_hci_cb.b_hci_netwk_reset = true;
      /* Set a new session id so that we clear all pipes later after seeing a
       * difference with the HC Session ID */
      memcpy(&session_id[(NFA_HCI_SESSION_ID_LEN / 2)],
             nfa_hci_cb.cfg.admin_gate.session_id,
             (NFA_HCI_SESSION_ID_LEN / 2));
      os_tick = GKI_get_os_tick_count();
      memcpy(session_id, (uint8_t*)&os_tick, (NFA_HCI_SESSION_ID_LEN / 2));
      nfa_hci_restore_default_config(session_id);
    }
    nfa_hci_startup();
  }
}
/*******************************************************************************
**
** Function         nfa_hci_rsp_timeout
**
** Description      action function to process timeout
**
** Returns          None
**
*******************************************************************************/
void nfa_hci_rsp_timeout (tNFA_HCI_EVENT_DATA *p_evt_data)
{
    tNFA_HCI_EVT        evt = 0;
    tNFA_HCI_EVT_DATA   evt_data;
    UINT8               delete_pipe;

    //NFA_TRACE_EVENT2 ("nfa_hci_rsp_timeout () State: %u  Cmd: %u", nfa_hci_cb.hci_state, nfa_hci_cb.cmd_sent);

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nfa_hci_rsp_timeout () State: %u  Cmd: %u", nfa_hci_cb.hci_state, nfa_hci_cb.cmd_sent);
    evt_data.status      = NFA_STATUS_FAILED;

    switch (nfa_hci_cb.hci_state)
    {
    case NFA_HCI_STATE_STARTUP:
    case NFA_HCI_STATE_RESTORE:
        NFA_TRACE_ERROR0 ("nfa_hci_rsp_timeout - Initialization failed!");
        nfa_hci_startup_complete (NFA_STATUS_TIMEOUT);
        break;

    case NFA_HCI_STATE_WAIT_NETWK_ENABLE:
    case NFA_HCI_STATE_RESTORE_NETWK_ENABLE:

        if (nfa_hci_cb.w4_hci_netwk_init)
        {
            /* HCI Network is enabled */
            nfa_hci_cb.w4_hci_netwk_init = FALSE;
            nfa_hciu_send_get_param_cmd (NFA_HCI_ADMIN_PIPE, NFA_HCI_HOST_LIST_INDEX);
        }
        else
        {
            nfa_hci_startup_complete (NFA_STATUS_FAILED);
        }
        break;

    case NFA_HCI_STATE_REMOVE_GATE:
        /* Something wrong, NVRAM data could be corrupt */
        if (nfa_hci_cb.cmd_sent == NFA_HCI_ADM_DELETE_PIPE)
        {
            nfa_hciu_send_clear_all_pipe_cmd ();
        }
        else
        {
            nfa_hciu_remove_all_pipes_from_host (0);
            nfa_hci_api_dealloc_gate (NULL);
        }
        break;

    case NFA_HCI_STATE_APP_DEREGISTER:
        /* Something wrong, NVRAM data could be corrupt */
        if (nfa_hci_cb.cmd_sent == NFA_HCI_ADM_DELETE_PIPE)
        {
            nfa_hciu_send_clear_all_pipe_cmd ();
        }
        else
        {
            nfa_hciu_remove_all_pipes_from_host (0);
            nfa_hci_api_deregister (NULL);
        }
        break;

    case NFA_HCI_STATE_WAIT_RSP:
        nfa_hci_cb.hci_state = NFA_HCI_STATE_IDLE;

        if (nfa_hci_cb.w4_rsp_evt)
        {
            nfa_hci_cb.w4_rsp_evt       = FALSE;
            evt                         = NFA_HCI_EVENT_RCVD_EVT;
            evt_data.rcvd_evt.pipe      = nfa_hci_cb.pipe_in_use;
            evt_data.rcvd_evt.evt_code  = 0;
            evt_data.rcvd_evt.evt_len   = 0;
            evt_data.rcvd_evt.p_evt_buf = NULL;
            nfa_hci_cb.rsp_buf_size     = 0;
            nfa_hci_cb.p_rsp_buf        = NULL;

            break;
        }

        delete_pipe          = 0;
        switch (nfa_hci_cb.cmd_sent)
        {
        case NFA_HCI_ANY_SET_PARAMETER:
            /*
             * As no response to the command sent on this pipe, we may assume the pipe is
             * deleted already and release the pipe. But still send delete pipe command to be safe.
             */
            delete_pipe                = nfa_hci_cb.pipe_in_use;
            evt_data.registry.pipe     = nfa_hci_cb.pipe_in_use;
            evt_data.registry.data_len = 0;
            evt_data.registry.index    = nfa_hci_cb.param_in_use;
            evt                        = NFA_HCI_SET_REG_RSP_EVT;
            break;

        case NFA_HCI_ANY_GET_PARAMETER:
            /*
             * As no response to the command sent on this pipe, we may assume the pipe is
             * deleted already and release the pipe. But still send delete pipe command to be safe.
             */
            delete_pipe                = nfa_hci_cb.pipe_in_use;
            evt_data.registry.pipe     = nfa_hci_cb.pipe_in_use;
            evt_data.registry.data_len = 0;
            evt_data.registry.index    = nfa_hci_cb.param_in_use;
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
            evt_data.registry.status   = NFA_HCI_ANY_E_TIMEOUT;
#endif
            evt                        = NFA_HCI_GET_REG_RSP_EVT;
            break;

        case NFA_HCI_ANY_OPEN_PIPE:
            /*
             * As no response to the command sent on this pipe, we may assume the pipe is
             * deleted already and release the pipe. But still send delete pipe command to be safe.
             */
            delete_pipe          = nfa_hci_cb.pipe_in_use;
            evt_data.opened.pipe = nfa_hci_cb.pipe_in_use;
            evt                  = NFA_HCI_OPEN_PIPE_EVT;
            break;

        case NFA_HCI_ANY_CLOSE_PIPE:
            /*
             * As no response to the command sent on this pipe, we may assume the pipe is
             * deleted already and release the pipe. But still send delete pipe command to be safe.
             */
            delete_pipe          = nfa_hci_cb.pipe_in_use;
            evt_data.closed.pipe = nfa_hci_cb.pipe_in_use;
            evt                  = NFA_HCI_CLOSE_PIPE_EVT;
            break;

        case NFA_HCI_ADM_CREATE_PIPE:
            evt_data.created.pipe        = nfa_hci_cb.pipe_in_use;
            evt_data.created.source_gate = nfa_hci_cb.local_gate_in_use;
            evt_data.created.dest_host   = nfa_hci_cb.remote_host_in_use;
            evt_data.created.dest_gate   = nfa_hci_cb.remote_gate_in_use;
            evt                          = NFA_HCI_CREATE_PIPE_EVT;
            break;

        case NFA_HCI_ADM_DELETE_PIPE:
            /*
             * As no response to the command sent on this pipe, we may assume the pipe is
             * deleted already. Just release the pipe.
             */
            if (nfa_hci_cb.pipe_in_use <= NFA_HCI_LAST_DYNAMIC_PIPE)
                nfa_hciu_release_pipe (nfa_hci_cb.pipe_in_use);
            evt_data.deleted.pipe = nfa_hci_cb.pipe_in_use;
            evt                   = NFA_HCI_DELETE_PIPE_EVT;
            break;

        default:
            /*
             * As no response to the command sent on this pipe, we may assume the pipe is
             * deleted already and release the pipe. But still send delete pipe command to be safe.
             */
            delete_pipe                = nfa_hci_cb.pipe_in_use;
            break;
        }
        if (delete_pipe && (delete_pipe <= NFA_HCI_LAST_DYNAMIC_PIPE)) {
          nfa_hciu_send_delete_pipe_cmd(delete_pipe);
          nfa_hciu_release_pipe(delete_pipe);
        }

        break;
    case NFA_HCI_STATE_DISABLED:
    default:
        NFA_TRACE_DEBUG0 ("nfa_hci_rsp_timeout () Timeout in DISABLED/ Invalid state");
        break;
    }
    if (evt != 0)
        nfa_hciu_send_to_app (evt, &evt_data, nfa_hci_cb.app_in_use);
}


/*******************************************************************************
**
** Function         nfa_hci_set_receive_buf
**
** Description      Set reassembly buffer for incoming message
**
** Returns          status
**
*******************************************************************************/
static void nfa_hci_set_receive_buf(uint8_t pipe) {
  if ((pipe >= NFA_HCI_FIRST_DYNAMIC_PIPE) &&
      (nfa_hci_cb.type == NFA_HCI_EVENT_TYPE)) {
    if ((nfa_hci_cb.rsp_buf_size) && (nfa_hci_cb.p_rsp_buf != nullptr)) {
      nfa_hci_cb.p_msg_data = nfa_hci_cb.p_rsp_buf;
      nfa_hci_cb.max_msg_len = nfa_hci_cb.rsp_buf_size;
      return;
    }
  }
  nfa_hci_cb.p_msg_data = nfa_hci_cb.msg_data;
  nfa_hci_cb.max_msg_len = NFA_MAX_HCI_EVENT_LEN;
}

/*******************************************************************************
**
** Function         nfa_hci_assemble_msg
**
** Description      Reassemble the incoming message
**
** Returns          None
**
*******************************************************************************/
static void nfa_hci_assemble_msg(uint8_t* p_data, uint16_t data_len) {
  if ((nfa_hci_cb.msg_len + data_len) > nfa_hci_cb.max_msg_len) {
    /* Fill the buffer as much it can hold */
    memcpy(&nfa_hci_cb.p_msg_data[nfa_hci_cb.msg_len], p_data,
           (nfa_hci_cb.max_msg_len - nfa_hci_cb.msg_len));
    nfa_hci_cb.msg_len = nfa_hci_cb.max_msg_len;
    /* Set Reassembly failed */
    nfa_hci_cb.assembly_failed = true;
    LOG(ERROR) << StringPrintf(
        "Insufficient buffer to Reassemble HCP "
        "packet! Dropping :%u bytes",
        ((nfa_hci_cb.msg_len + data_len) - nfa_hci_cb.max_msg_len));
  } else {
    memcpy(&nfa_hci_cb.p_msg_data[nfa_hci_cb.msg_len], p_data, data_len);
    nfa_hci_cb.msg_len += data_len;
  }
}

/*******************************************************************************
**
** Function         nfa_hci_evt_hdlr
**
** Description      Processing all event for NFA HCI
**
** Returns          TRUE if p_msg needs to be deallocated
**
*******************************************************************************/
static bool nfa_hci_evt_hdlr(NFC_HDR* p_msg) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "nfa_hci_evt_hdlr state: %s (%d) event: %s (0x%04x)",
      nfa_hciu_get_state_name(nfa_hci_cb.hci_state).c_str(),
      nfa_hci_cb.hci_state, nfa_hciu_get_event_name(p_msg->event).c_str(),
      p_msg->event);

  /* If this is an API request, queue it up */
  if ((p_msg->event >= NFA_HCI_FIRST_API_EVENT) &&
      (p_msg->event <= NFA_HCI_LAST_API_EVENT)) {
    GKI_enqueue(&nfa_hci_cb.hci_api_q, p_msg);
  } else {
    tNFA_HCI_EVENT_DATA* p_evt_data = (tNFA_HCI_EVENT_DATA*)p_msg;
    switch (p_msg->event) {
      case NFA_HCI_RSP_NV_READ_EVT:
        nfa_hci_handle_nv_read(p_evt_data->nv_read.block,
                               p_evt_data->nv_read.status);
        break;

      case NFA_HCI_RSP_NV_WRITE_EVT:
        /* NV Ram write completed - nothing to do... */
        break;

      case NFA_HCI_RSP_TIMEOUT_EVT:
        nfa_hci_rsp_timeout ((tNFA_HCI_EVENT_DATA *)p_msg);
        break;

      case NFA_HCI_CHECK_QUEUE_EVT:
        if (HCI_LOOPBACK_DEBUG == NFA_HCI_DEBUG_ON) {
          if (p_msg->len != 0) {
            tNFC_CONN nfc_conn;
            nfc_conn.data.p_data = p_msg;
            nfa_hci_conn_cback(0, NFC_DATA_CEVT, &nfc_conn);
            return false;
          }
        }
        break;
    }
  }

  if ((p_msg->event > NFA_HCI_LAST_API_EVENT)) GKI_freebuf(p_msg);

  nfa_hci_check_api_requests();

  if (nfa_hciu_is_no_host_resetting()) nfa_hci_check_pending_api_requests();

  if ((nfa_hci_cb.hci_state == NFA_HCI_STATE_IDLE) &&
      (nfa_hci_cb.nv_write_needed)) {
    nfa_hci_cb.nv_write_needed = false;
    nfa_nv_co_write((uint8_t*)&nfa_hci_cb.cfg, sizeof(nfa_hci_cb.cfg),
                    DH_NV_BLOCK);
  }

  return false;
}

#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
void nfa_hci_release_transcieve()
{
    NFA_TRACE_DEBUG0 ("nfa_hci_release_transcieve (); Release ongoing transcieve");
    if(nfa_hci_cb.hci_state == NFA_HCI_STATE_WAIT_RSP)
    {
        nfa_sys_stop_timer(&nfa_hci_cb.timer);
        nfa_hci_rsp_timeout(NULL);
    }
}
#endif
