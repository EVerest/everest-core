/******************************************************************************
 *
 *  Copyright (C) 2011-2014 Broadcom Corporation
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
 *  This is the private interface file for NFA_CE
 *
 ******************************************************************************/
#ifndef NFA_CE_INT_H
#define NFA_CE_INT_H

#include "nfa_api.h"
#include "nfa_ce_api.h"
#include "nfa_dm_int.h"
#include "nfa_sys.h"
#include "nfc_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* CE events */
enum {
  /* device manager local device API events */
  NFA_CE_API_CFG_LOCAL_TAG_EVT = NFA_SYS_EVT_START(NFA_ID_CE),
  NFA_CE_API_REG_LISTEN_EVT,
  NFA_CE_API_DEREG_LISTEN_EVT,
  NFA_CE_API_CFG_ISODEP_TECH_EVT,
  NFA_CE_ACTIVATE_NTF_EVT,
  NFA_CE_DEACTIVATE_NTF_EVT

};

/* Listen registration types */
enum { NFA_CE_REG_TYPE_ISO_DEP, NFA_CE_REG_TYPE_FELICA, NFA_CE_REG_TYPE_UICC };
typedef uint8_t tNFA_CE_REG_TYPE;

/* data type for NFA_CE_API_CFG_LOCAL_TAG_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_PROTOCOL_MASK protocol_mask;
  uint8_t* p_ndef_data;
  uint16_t ndef_cur_size;
  uint16_t ndef_max_size;
  bool read_only;
  uint8_t uid_len;
  uint8_t uid[NFA_MAX_UID_LEN];
} tNFA_CE_API_CFG_LOCAL_TAG;

/* data type for NFA_CE_ACTIVATE_NTF_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFC_ACTIVATE_DEVT* p_activation_params;
} tNFA_CE_ACTIVATE_NTF;

/* data type for NFA_CE_API_REG_LISTEN_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_CONN_CBACK* p_conn_cback;

  tNFA_CE_REG_TYPE listen_type;

  /* For registering Felica */
  uint16_t system_code;
  uint8_t nfcid2[NCI_RF_F_UID_LEN];
  uint8_t t3tPmm[NCI_T3T_PMM_LEN];

  /* For registering Type-4 */
  uint8_t aid[NFC_MAX_AID_LEN]; /* AID to listen for (For type-4 only)  */
  uint8_t aid_len;              /* AID length                           */

  /* For registering UICC */
  tNFA_HANDLE ee_handle;
  tNFA_TECHNOLOGY_MASK tech_mask;
} tNFA_CE_API_REG_LISTEN;

/* data type for NFA_CE_API_DEREG_LISTEN_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE handle;
  uint32_t listen_info;
} tNFA_CE_API_DEREG_LISTEN;

/* union of all data types */
typedef union {
  /* GKI event buffer header */
  NFC_HDR hdr;
  tNFA_CE_API_CFG_LOCAL_TAG local_tag;
  tNFA_CE_API_REG_LISTEN reg_listen;
  tNFA_CE_API_DEREG_LISTEN dereg_listen;
  tNFA_CE_ACTIVATE_NTF activate_ntf;
} tNFA_CE_MSG;

/****************************************************************************
** LISTEN_INFO definitions
*****************************************************************************/
/* Entry 0 is reserved for local NDEF tag */
#define NFA_CE_LISTEN_INFO_IDX_NDEF 0
#define NFA_CE_LISTEN_INFO_IDX_INVALID (NFA_CE_LISTEN_INFO_MAX)

/* Flags for listen request */
/* LISTEN_INFO entry is in use                                      */
#define NFA_CE_LISTEN_INFO_IN_USE 0x00000001
/* NDEF is read-only                                                */
#define NFC_CE_LISTEN_INFO_READONLY_NDEF 0x00000010
/* App has not been notified of ACTIVATE_EVT yet for this T4T AID   */
#define NFA_CE_LISTEN_INFO_T4T_ACTIVATE_PND 0x00000040
/* This is a listen_info for T4T AID                                */
#define NFA_CE_LISTEN_INFO_T4T_AID 0x00000080
/* App has not been notified of LISTEN_START yet                    */
#define NFA_CE_LISTEN_INFO_START_NTF_PND 0x00000100
/* This is a listen_info for non-NDEF Felica                        */
#define NFA_CE_LISTEN_INFO_FELICA 0x00000200
/* This is a listen_info for UICC                                   */
#define NFA_CE_LISTEN_INFO_UICC 0x00000400
/* App has not been notified of ACTIVATE_EVT yet for this HCEF NFCID2 */
#define NFA_CE_LISTEN_INFO_T3T_ACTIVATE_PND 0x00010000

/* Structure for listen look up table */
typedef struct {
  uint32_t flags;
  tNFA_CONN_CBACK* p_conn_cback; /* Callback for this listen request */
  tNFA_PROTOCOL_MASK
      protocol_mask;          /* Mask of protocols for this listen request    */
  tNFA_HANDLE rf_disc_handle; /* RF Discover handle */

  /* For host tag emulation (NFA_CeRegisterVirtualT4tSE and
   * NFA_CeRegisterT4tAidOnDH) */
  uint8_t t3t_pmm[NCI_T3T_PMM_LEN];
  uint8_t t3t_nfcid2[NCI_RF_F_UID_LEN];
  uint16_t t3t_system_code; /* Type-3 system code */
  uint8_t
      t4t_aid_handle; /* Type-4 aid callback handle (from CE_T4tRegisterAID) */

  /* For UICC */
  tNFA_HANDLE ee_handle;
  tNFA_TECHNOLOGY_MASK tech_mask; /* listening technologies               */
  tNFA_DM_DISC_TECH_PROTO_MASK
      tech_proto_mask; /* listening technologies and protocols */
} tNFA_CE_LISTEN_INFO;

/****************************************************************************/

/* Internal flags for nfa_ce */
/* Deactivation locally initiated by application */
#define NFA_CE_FLAGS_APP_INIT_DEACTIVATION 0x00000001
/* Tag is in listen active or sleep state        */
#define NFA_CE_FLAGS_LISTEN_ACTIVE_SLEEP 0x00000002
typedef uint32_t tNFA_CE_FLAGS;

/* NFA_CE control block */
typedef struct {
  uint8_t* p_scratch_buf; /* Scratch buffer for write requests    */
  uint32_t scratch_buf_size;

  tNFC_ACTIVATE_DEVT activation_params; /* Activation params        */
  tNFA_CE_FLAGS flags;                  /* internal flags           */
  tNFA_CONN_CBACK* p_active_conn_cback; /* Callback of activated CE */

  /* listen_info table (table of listen paramters and app callbacks) */
  tNFA_CE_LISTEN_INFO
      listen_info[NFA_CE_LISTEN_INFO_MAX]; /* listen info table */
  uint8_t idx_cur_active; /* listen_info index for currently activated CE */
  uint8_t idx_wild_card;  /* listen_info index for T4T wild card CE */

  tNFA_DM_DISC_TECH_PROTO_MASK
      isodep_disc_mask; /* the technology/protocol mask for ISO-DEP */

  /* Local ndef tag info */
  uint8_t* p_ndef_data;
  uint16_t ndef_cur_size;
  uint16_t ndef_max_size;

  tNFA_SYS_EVT_HDLR* p_vs_evt_hdlr; /* VS event handler */
} tNFA_CE_CB;
extern tNFA_CE_CB nfa_ce_cb;

/* type definition for action functions */
typedef bool (*tNFA_CE_ACTION)(tNFA_CE_MSG* p_data);

extern uint8_t NFC_GetNCIVersion();

/* Action function prototypes */
bool nfa_ce_api_cfg_local_tag(tNFA_CE_MSG* p_ce_msg);
bool nfa_ce_api_reg_listen(tNFA_CE_MSG* p_ce_msg);
bool nfa_ce_api_dereg_listen(tNFA_CE_MSG* p_ce_msg);
bool nfa_ce_api_cfg_isodep_tech(tNFA_CE_MSG* p_ce_msg);
bool nfa_ce_activate_ntf(tNFA_CE_MSG* p_ce_msg);
bool nfa_ce_deactivate_ntf(tNFA_CE_MSG* p_ce_msg);

/* Internal function prototypes */
void nfa_ce_t3t_generate_rand_nfcid(uint8_t nfcid2[NCI_RF_F_UID_LEN]);
bool nfa_ce_hdl_event(NFC_HDR* p_msg);
tNFC_STATUS nfa_ce_set_content(void);
tNFA_STATUS nfa_ce_start_listening(void);
void nfa_ce_remove_listen_info_entry(uint8_t listen_info_idx, bool notify_app);
void nfa_ce_sys_disable(void);
void nfa_ce_free_scratch_buf(void);
bool nfa_ce_restart_listen_check(void);
#endif /* NFA_DM_INT_H */
