/******************************************************************************
 *
 *  Copyright (C) 2003-2014 Broadcom Corporation
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
 *  This is the public interface file for the BTA system manager.
 *
 ******************************************************************************/
#ifndef NFA_SYS_H
#define NFA_SYS_H

#include "gki.h"
#include "nfa_api.h"
#include "nfc_target.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* SW sub-systems */
enum {
  NFA_ID_SYS,  /* system manager                      */
  NFA_ID_DM,   /* device manager                      */
  NFA_ID_EE,   /* NFCEE sub-system                    */
  NFA_ID_P2P,  /* Peer-to-Peer sub-system             */
  NFA_ID_SNEP, /* SNEP sub-system                     */
  NFA_ID_RW,   /* Reader/writer sub-system            */
  NFA_ID_CE,   /* Card-emulation sub-system           */
  NFA_ID_HCI,  /* Host controller interface sub-system*/
#if (NFA_DTA_INCLUDED == TRUE)
  NFA_ID_DTA, /* Device Test Application sub-system  */
#endif
#if (NXP_EXTNS == TRUE)
  NFA_ID_T4TNFCEE, /* t4T Nfcee sub-system  */
#endif
  NFA_ID_MAX
};
typedef uint8_t tNFA_SYS_ID;

/* enable function type */
typedef void(tNFA_SYS_ENABLE)(void);

/* event handler function type */
typedef bool(tNFA_SYS_EVT_HDLR)(NFC_HDR* p_msg);

/* disable function type */
typedef void(tNFA_SYS_DISABLE)(void);

/* function type for processing the change of NFCC power mode */
typedef void(tNFA_SYS_PROC_NFCC_PWR_MODE)(uint8_t nfcc_power_mode);

typedef void(tNFA_SYS_ENABLE_CBACK)(void);
typedef void(tNFA_SYS_PROC_NFCC_PWR_MODE_CMPL)(void);

/* registration structure */
typedef struct tNFA_SYS_REG{
  tNFA_SYS_ENABLE* enable;
  tNFA_SYS_EVT_HDLR* evt_hdlr;
  tNFA_SYS_DISABLE* disable;
  tNFA_SYS_PROC_NFCC_PWR_MODE* proc_nfcc_pwr_mode;
} tNFA_SYS_REG;

/* system manager configuration structure */
typedef struct {
  uint16_t mbox_evt;   /* GKI mailbox event */
  uint8_t mbox;        /* GKI mailbox id */
  uint8_t timer;       /* GKI timer id */
} tNFA_SYS_CFG;

/*****************************************************************************
**  Global data
*****************************************************************************/

/*****************************************************************************
**  Macros
*****************************************************************************/

/* Calculate start of event enumeration; id is top 8 bits of event */
#define NFA_SYS_EVT_START(id) ((id) << 8)

/*****************************************************************************
**  Function declarations
*****************************************************************************/

extern void nfa_sys_init(void);
extern void nfa_sys_event(NFC_HDR* p_msg);
extern void nfa_sys_timer_update(void);
extern void nfa_sys_disable_timers(void);

extern void nfa_sys_register(uint8_t id, const tNFA_SYS_REG* p_reg);
extern void nfa_sys_deregister(uint8_t id);
extern void nfa_sys_check_disabled(void);
extern bool nfa_sys_is_register(uint8_t id);
extern void nfa_sys_disable_subsystems(bool graceful);
extern void nfa_sys_enable_subsystems(void);

extern bool nfa_sys_is_graceful_disable(void);
extern void nfa_sys_sendmsg(void* p_msg);
extern void nfa_sys_start_timer(TIMER_LIST_ENT* p_tle, uint16_t type,
                                int32_t timeout);
extern void nfa_sys_stop_timer(TIMER_LIST_ENT* p_tle);

extern void nfa_sys_cback_reg_enable_complete(tNFA_SYS_ENABLE_CBACK* p_cback);
extern void nfa_sys_cback_notify_enable_complete(uint8_t id);

extern void nfa_sys_notify_nfcc_power_mode(uint8_t nfcc_power_mode);
extern void nfa_sys_cback_reg_nfcc_power_mode_proc_complete(
    tNFA_SYS_PROC_NFCC_PWR_MODE_CMPL* p_cback);
extern void nfa_sys_cback_notify_nfcc_power_mode_proc_complete(uint8_t id);

#endif /* NFA_SYS_H */
