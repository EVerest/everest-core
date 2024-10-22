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
 *  Registration/deregistration functions for inter-module callbacks
 *
 ******************************************************************************/
#include <android-base/stringprintf.h>
#include <base/logging.h>

#include "nfa_sys.h"
#include "nfa_sys_int.h"

//using android::base::StringPrintf;

extern bool nfc_debug_enabled;

/*******************************************************************************
**
** Function         nfa_sys_cback_reg_enable_complete
**
** Description      Called to register an initialization complete callback
**                  function
**
** Returns          void
**
*******************************************************************************/
void nfa_sys_cback_reg_enable_complete(tNFA_SYS_ENABLE_CBACK* p_cback) {
  nfa_sys_cb.p_enable_cback = p_cback;
  nfa_sys_cb.enable_cplt_flags = 0;
}

/*******************************************************************************
**
** Function         nfa_sys_cback_notify_enable_complete
**
** Description      Called by other NFA subsystems to notify initialization is
**                  complete
**
** Returns          void
**
*******************************************************************************/
void nfa_sys_cback_notify_enable_complete(uint8_t id) {
  nfa_sys_cb.enable_cplt_flags |= (0x0001 << id);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "enable_cplt_flags=0x%x, enable_cplt_mask=0x%x",
      nfa_sys_cb.enable_cplt_flags, nfa_sys_cb.enable_cplt_mask);

  if ((nfa_sys_cb.enable_cplt_flags == nfa_sys_cb.enable_cplt_mask) &&
      (nfa_sys_cb.p_enable_cback)) {
    nfa_sys_cb.p_enable_cback();
    nfa_sys_cb.p_enable_cback = nullptr;
  }
}

/*******************************************************************************
**
** Function         nfa_sys_cback_reg_nfcc_power_mode_proc_complete
**
** Description      Called to register a callback function for complete of
**                  processing NFCC power mode change from NFA sub-systems
**
** Returns          void
**
*******************************************************************************/
void nfa_sys_cback_reg_nfcc_power_mode_proc_complete(
    tNFA_SYS_PROC_NFCC_PWR_MODE_CMPL* p_cback) {
  nfa_sys_cb.p_proc_nfcc_pwr_mode_cmpl_cback = p_cback;
  nfa_sys_cb.proc_nfcc_pwr_mode_cplt_flags = 0;
}

/*******************************************************************************
**
** Function         nfa_sys_cback_notify_nfcc_power_mode_proc_complete
**
** Description      Called by other NFA subsystems to notify processing NFCC
**                  power mode is complete
**
** Returns          void
**
*******************************************************************************/
void nfa_sys_cback_notify_nfcc_power_mode_proc_complete(uint8_t id) {
  nfa_sys_cb.proc_nfcc_pwr_mode_cplt_flags |= (0x0001 << id);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "flags=0x%x, mask=0x%x", nfa_sys_cb.proc_nfcc_pwr_mode_cplt_flags,
      nfa_sys_cb.proc_nfcc_pwr_mode_cplt_mask);

  /* except SYS */
  if ((nfa_sys_cb.proc_nfcc_pwr_mode_cplt_flags ==
       nfa_sys_cb.proc_nfcc_pwr_mode_cplt_mask) &&
      (nfa_sys_cb.p_proc_nfcc_pwr_mode_cmpl_cback)) {
    nfa_sys_cb.p_proc_nfcc_pwr_mode_cmpl_cback();
    nfa_sys_cb.p_proc_nfcc_pwr_mode_cmpl_cback = nullptr;
  }
}
