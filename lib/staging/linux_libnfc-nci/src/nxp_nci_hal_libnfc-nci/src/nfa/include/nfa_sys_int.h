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
 *  This is the private interface file for the NFA system manager.
 *
 ******************************************************************************/
#ifndef NFA_SYS_INT_H
#define NFA_SYS_INT_H

#include "nfa_sys_ptim.h"
/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* nfa_sys flags */
#define NFA_SYS_FL_INITIALIZED 0x00000001 /* nfa_sys initialized */

/*****************************************************************************
**  state table
*****************************************************************************/

/* system manager control block */
typedef struct {
  uint32_t flags; /* nfa_sys flags (must be first element of structure) */
  tNFA_SYS_REG* reg[NFA_ID_MAX]; /* registration structures */
  bool is_reg[NFA_ID_MAX];       /* registration structures */
  tPTIM_CB ptim_cb;              /* protocol timer list */
  tNFA_SYS_ENABLE_CBACK* p_enable_cback;
  uint16_t enable_cplt_flags;
  uint16_t enable_cplt_mask;

  tNFA_SYS_PROC_NFCC_PWR_MODE_CMPL* p_proc_nfcc_pwr_mode_cmpl_cback;
  uint16_t proc_nfcc_pwr_mode_cplt_flags;
  uint16_t proc_nfcc_pwr_mode_cplt_mask;

  bool graceful_disable; /* TRUE if NFA_Disable () is called with TRUE */
  bool timers_disabled;  /* TRUE if sys timers disabled */

  uint8_t                   trace_level;            /* Trace level */
} tNFA_SYS_CB;

/*****************************************************************************
**  Global variables
*****************************************************************************/

/* system manager control block */
extern tNFA_SYS_CB nfa_sys_cb;

/* system manager configuration structure */
extern tNFA_SYS_CFG* p_nfa_sys_cfg;

#endif /* NFA_SYS_INT_H */
