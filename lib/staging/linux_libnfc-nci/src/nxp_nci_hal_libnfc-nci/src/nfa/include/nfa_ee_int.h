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
 *  This is the private interface file for the NFA EE.
 *
 ******************************************************************************/
#ifndef NFA_EE_INT_H
#define NFA_EE_INT_H
#include "nfa_ee_api.h"
#include "nfa_sys.h"
#include "nfc_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/
/* the number of tNFA_EE_ECBs (for NFCEEs and DH) */
#define NFA_EE_NUM_ECBS (NFA_EE_MAX_EE_SUPPORTED + 1)
/* The index for DH in nfa_ee_cb.ee_cb[] */
#define NFA_EE_CB_4_DH NFA_EE_MAX_EE_SUPPORTED
#define NFA_EE_INVALID 0xFF
/* only A, B, F, Bprime are supported by UICC now */
#define NFA_EE_MAX_TECH_ROUTE 4

#ifndef NFA_EE_AID_CFG_TAG_NAME
/* AID                             */
#define NFA_EE_AID_CFG_TAG_NAME 0x4F
#endif

/* NFA EE events */
enum {
  NFA_EE_API_DISCOVER_EVT = NFA_SYS_EVT_START(NFA_ID_EE),
  NFA_EE_API_REGISTER_EVT,
  NFA_EE_API_DEREGISTER_EVT,
  NFA_EE_API_MODE_SET_EVT,
  NFA_EE_API_SET_TECH_CFG_EVT,
  NFA_EE_API_CLEAR_TECH_CFG_EVT,
  NFA_EE_API_SET_PROTO_CFG_EVT,
  NFA_EE_API_CLEAR_PROTO_CFG_EVT,
  NFA_EE_API_ADD_AID_EVT,
  NFA_EE_API_REMOVE_AID_EVT,
  NFA_EE_API_ADD_SYSCODE_EVT,
  NFA_EE_API_REMOVE_SYSCODE_EVT,
  NFA_EE_API_LMRT_SIZE_EVT,
  NFA_EE_API_UPDATE_NOW_EVT,
  NFA_EE_API_CONNECT_EVT,
  NFA_EE_API_SEND_DATA_EVT,
  NFA_EE_API_DISCONNECT_EVT,

  NFA_EE_NCI_DISC_RSP_EVT,
  NFA_EE_NCI_DISC_NTF_EVT,
  NFA_EE_NCI_MODE_SET_RSP_EVT,
  NFA_EE_NCI_CONN_EVT,
  NFA_EE_NCI_DATA_EVT,
  NFA_EE_NCI_ACTION_NTF_EVT,
  NFA_EE_NCI_DISC_REQ_NTF_EVT,
  NFA_EE_NCI_WAIT_RSP_EVT,

  NFA_EE_ROUT_TIMEOUT_EVT,
  NFA_EE_DISCV_TIMEOUT_EVT,
  NFA_EE_CFG_TO_NFCC_EVT,
  NFA_EE_NCI_NFCEE_STATUS_NTF_EVT,
  NFA_EE_MAX_EVT

};

typedef uint16_t tNFA_EE_INT_EVT;
/* for listen mode routing table*/
#define NFA_EE_AE_ROUTE 0x80
#define NFA_EE_AE_VS 0x40

/* NFA EE Management state */
enum {
  NFA_EE_EM_STATE_INIT = 0,
  NFA_EE_EM_STATE_INIT_DONE,
  NFA_EE_EM_STATE_RESTORING,
  NFA_EE_EM_STATE_DISABLING,
  NFA_EE_EM_STATE_DISABLED,

  NFA_EE_EM_STATE_MAX
};
typedef uint8_t tNFA_EE_EM_STATE;

/* NFA EE connection status */
enum {
  NFA_EE_CONN_ST_NONE, /* not connected */
  NFA_EE_CONN_ST_WAIT, /* connection is initiated; waiting for ack */
  NFA_EE_CONN_ST_CONN, /* connected; can send/receive data */
  NFA_EE_CONN_ST_DISC  /* disconnecting; waiting for ack */
};
typedef uint8_t tNFA_EE_CONN_ST;

#define NFA_EE_MAX_AID_CFG_LEN (510)
// Technology A/B/F reserved: 5*3 = 15
// Protocol ISODEP/NFCDEP/T3T reserved: 5*3 = 15
// Extends (APDU pattern/SC)reserved: 30
#define NFA_EE_MAX_PROTO_TECH_EXT_ROUTE_LEN 60

#define NFA_EE_SYSTEM_CODE_LEN 02
#define NFA_EE_SYSTEM_CODE_TLV_SIZE 06
#define NFA_EE_MAX_SYSTEM_CODE_ENTRIES 10
#define NFA_EE_MAX_SYSTEM_CODE_CFG_LEN \
  (NFA_EE_MAX_SYSTEM_CODE_ENTRIES * NFA_EE_SYSTEM_CODE_TLV_SIZE)

/* NFA EE control block flags:
 * use to indicate an API function has changed the configuration of the
 * associated NFCEE
 * The flags are cleared when the routing table/VS is updated */
/* technology routing changed         */
#define NFA_EE_ECB_FLAGS_TECH 0x02
/* protocol routing changed           */
#define NFA_EE_ECB_FLAGS_PROTO 0x04
/* AID routing changed                */
#define NFA_EE_ECB_FLAGS_AID 0x08
/* System Code routing changed        */
#define NFA_EE_ECB_FLAGS_SYSCODE 0xE0
/* VS changed                         */
#define NFA_EE_ECB_FLAGS_VS 0x10
/* Restore related                    */
#define NFA_EE_ECB_FLAGS_RESTORE 0x20
/* routing flags changed              */
#define NFA_EE_ECB_FLAGS_ROUTING 0x0E
/* NFCEE Discover Request NTF is set  */
#define NFA_EE_ECB_FLAGS_DISC_REQ 0x40
/* DISC_REQ N reported before DISC N  */
#define NFA_EE_ECB_FLAGS_ORDER 0x80
typedef uint8_t tNFA_EE_ECB_FLAGS;

/* part of tNFA_EE_STATUS; for internal use only  */
/* waiting for restore to full power mode to complete */
#define NFA_EE_STATUS_RESTORING 0x20
/* this bit is in ee_status for internal use only */
#define NFA_EE_STATUS_INT_MASK 0x20

/* NFA-EE information for a particular NFCEE Entity (including DH) */
typedef struct {
  tNFA_TECHNOLOGY_MASK
      tech_switch_on; /* default routing - technologies switch_on  */
  tNFA_TECHNOLOGY_MASK
      tech_switch_off; /* default routing - technologies switch_off */
  tNFA_TECHNOLOGY_MASK
      tech_battery_off; /* default routing - technologies battery_off*/
  tNFA_TECHNOLOGY_MASK
      tech_screen_lock; /* default routing - technologies screen_lock*/
  tNFA_TECHNOLOGY_MASK
      tech_screen_off; /* default routing - technologies screen_off*/
  tNFA_TECHNOLOGY_MASK
      tech_screen_off_lock; /* default routing - technologies screen_off_lock*/
  tNFA_PROTOCOL_MASK
      proto_switch_on; /* default routing - protocols switch_on     */
  tNFA_PROTOCOL_MASK
      proto_switch_off; /* default routing - protocols switch_off    */
  tNFA_PROTOCOL_MASK
      proto_battery_off;     /* default routing - protocols battery_off   */
  tNFA_PROTOCOL_MASK
      proto_screen_lock; /* default routing - protocols screen_lock    */
  tNFA_PROTOCOL_MASK
      proto_screen_off; /* default routing - protocols screen_off  */
  tNFA_PROTOCOL_MASK
      proto_screen_off_lock; /* default routing - protocols screen_off_lock  */
  tNFA_EE_CONN_ST conn_st;   /* connection status */
  uint8_t conn_id;           /* connection id */
  tNFA_EE_CBACK* p_ee_cback; /* the callback function */

  /* Each AID entry has an ssociated aid_len, aid_pwr_cfg, aid_rt_info.
   * aid_cfg[] contains AID and maybe some other VS information in TLV format
   * The first T is always NFA_EE_AID_CFG_TAG_NAME, the L is the actual AID
   * length
   * the aid_len is the total length of all the TLVs associated with this AID
   * entry
   */
  uint8_t* aid_len;     /* the actual lengths in aid_cfg */
  uint8_t* aid_pwr_cfg; /* power configuration of this
                                                  AID entry */
  uint8_t* aid_rt_info; /* route/vs info for this AID
                                                  entry */
  uint8_t* aid_cfg;     /* routing entries based on AID */
  uint8_t* aid_info;    /* Aid Info Prefix/Suffix/Exact */

  uint8_t aid_entries;   /* The number of AID entries in aid_cfg */
  uint8_t nfcee_id;      /* ID for this NFCEE */
  uint8_t ee_status;     /* The NFCEE status */
  uint8_t ee_old_status; /* The NFCEE status before going to low power mode */
  tNFA_EE_INTERFACE
      ee_interface[NFC_MAX_EE_INTERFACE]; /* NFCEE supported interface */
  tNFA_EE_TLV ee_tlv[NFC_MAX_EE_TLVS];    /* the TLV */
  uint8_t num_interface;                  /* number of Target interface */
  uint8_t num_tlvs;                       /* number of TLVs */
  uint8_t ee_power_supply_status;         /* power supply of NFCEE*/
  tNFA_EE_ECB_FLAGS ecb_flags;            /* the flags of this control block */
  tNFA_EE_INTERFACE use_interface; /* NFCEE interface used for the connection */
  tNFA_NFC_PROTOCOL la_protocol;   /* Listen A protocol    */
  tNFA_NFC_PROTOCOL lb_protocol;   /* Listen B protocol    */
  tNFA_NFC_PROTOCOL lf_protocol;   /* Listen F protocol    */
  tNFA_NFC_PROTOCOL lbp_protocol;  /* Listen B' protocol   */
  uint8_t size_mask_proto;         /* the size for protocol routing */
  uint8_t size_mask_tech;          /* the size for technology routing */
  uint16_t size_aid; /* the size for aid routing */
  /*System Code Based Routing Variables*/
  uint8_t sys_code_cfg[NFA_EE_MAX_SYSTEM_CODE_ENTRIES * NFA_EE_SYSTEM_CODE_LEN];
  uint8_t sys_code_pwr_cfg[NFA_EE_MAX_SYSTEM_CODE_ENTRIES];
  uint8_t sys_code_rt_loc[NFA_EE_MAX_SYSTEM_CODE_ENTRIES];
  uint8_t sys_code_rt_loc_vs_info[NFA_EE_MAX_SYSTEM_CODE_ENTRIES];
  /* The number of SC entries in sys_code_cfg buffer*/
  uint8_t sys_code_cfg_entries;
  uint16_t size_sys_code; /* The size for system code routing */
} tNFA_EE_ECB;

/* data type for NFA_EE_API_DISCOVER_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_EE_CBACK* p_cback;
} tNFA_EE_API_DISCOVER;

/* data type for NFA_EE_API_REGISTER_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_EE_CBACK* p_cback;
} tNFA_EE_API_REGISTER;

/* data type for NFA_EE_API_DEREGISTER_EVT */
typedef struct {
  NFC_HDR hdr;
  int index;
} tNFA_EE_API_DEREGISTER;

/* data type for NFA_EE_API_MODE_SET_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_EE_ECB* p_cb;
  uint8_t nfcee_id;
  uint8_t mode;
} tNFA_EE_API_MODE_SET;

/* data type for NFA_EE_API_SET_TECH_CFG_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_EE_ECB* p_cb;
  uint8_t nfcee_id;
  tNFA_TECHNOLOGY_MASK technologies_switch_on;
  tNFA_TECHNOLOGY_MASK technologies_switch_off;
  tNFA_TECHNOLOGY_MASK technologies_battery_off;
  tNFA_TECHNOLOGY_MASK technologies_screen_lock;
  tNFA_TECHNOLOGY_MASK technologies_screen_off;
  tNFA_TECHNOLOGY_MASK technologies_screen_off_lock;
} tNFA_EE_API_SET_TECH_CFG, tNFA_EE_API_CLEAR_TECH_CFG;

/* data type for NFA_EE_API_SET_PROTO_CFG_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_EE_ECB* p_cb;
  uint8_t nfcee_id;
  tNFA_PROTOCOL_MASK protocols_switch_on;
  tNFA_PROTOCOL_MASK protocols_switch_off;
  tNFA_PROTOCOL_MASK protocols_battery_off;
  tNFA_PROTOCOL_MASK protocols_screen_lock;
  tNFA_PROTOCOL_MASK protocols_screen_off;
  tNFA_PROTOCOL_MASK protocols_screen_off_lock;
} tNFA_EE_API_SET_PROTO_CFG, tNFA_EE_API_CLEAR_PROTO_CFG;

/* data type for NFA_EE_API_ADD_AID_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_EE_ECB* p_cb;
  uint8_t nfcee_id;
  uint8_t aid_len;
  uint8_t* p_aid;
  tNFA_EE_PWR_STATE power_state;
  uint8_t aidInfo;
} tNFA_EE_API_ADD_AID;

/* data type for NFA_EE_API_REMOVE_AID_EVT */
typedef struct {
  NFC_HDR hdr;
  uint8_t aid_len;
  uint8_t* p_aid;
} tNFA_EE_API_REMOVE_AID;

/* data type for NFA_EE_API_ADD_SYSCODE_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_EE_ECB* p_cb;
  uint8_t nfcee_id;
  uint16_t syscode;
  tNFA_EE_PWR_STATE power_state;
} tNFA_EE_API_ADD_SYSCODE;

/* data type for NFA_EE_API_REMOVE_SYSCODE_EVT */
typedef struct {
  NFC_HDR hdr;
  uint16_t syscode;
} tNFA_EE_API_REMOVE_SYSCODE;

/* data type for NFA_EE_API_LMRT_SIZE_EVT */
typedef NFC_HDR tNFA_EE_API_LMRT_SIZE;

/* data type for NFA_EE_API_CONNECT_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_EE_ECB* p_cb;
  uint8_t nfcee_id;
  uint8_t ee_interface;
  tNFA_EE_CBACK* p_cback;
} tNFA_EE_API_CONNECT;

/* data type for NFA_EE_API_SEND_DATA_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_EE_ECB* p_cb;
  uint8_t nfcee_id;
  uint16_t data_len;
  uint8_t* p_data;
} tNFA_EE_API_SEND_DATA;

/* data type for NFA_EE_API_DISCONNECT_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_EE_ECB* p_cb;
  uint8_t nfcee_id;
} tNFA_EE_API_DISCONNECT;

/* common data type for internal events with nfa_ee_use_cfg_cb[] as TRUE */
typedef struct {
  NFC_HDR hdr;
  tNFA_EE_ECB* p_cb;
  uint8_t nfcee_id;
} tNFA_EE_CFG_HDR;

/* data type for NFA_EE_NCI_DISC_RSP_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFC_NFCEE_DISCOVER_REVT* p_data;
} tNFA_EE_NCI_DISC_RSP;

/* data type for NFA_EE_NCI_DISC_NTF_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFC_NFCEE_INFO_REVT* p_data;
} tNFA_EE_NCI_DISC_NTF;

/* data type for NFA_EE_NCI_MODE_SET_RSP_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFC_NFCEE_MODE_SET_REVT* p_data;
} tNFA_EE_NCI_MODE_SET;

/* data type for NFA_EE_NCI_WAIT_RSP_EVT */
typedef struct {
  NFC_HDR hdr;
  void* p_data;
  uint8_t opcode;
} tNFA_EE_NCI_WAIT_RSP;

/* data type for NFA_EE_NCI_CONN_EVT and NFA_EE_NCI_DATA_EVT */
typedef struct {
  NFC_HDR hdr;
  uint8_t conn_id;
  tNFC_CONN_EVT event;
  tNFC_CONN* p_data;
} tNFA_EE_NCI_CONN;

/* data type for NFA_EE_NCI_ACTION_NTF_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFC_EE_ACTION_REVT* p_data;
} tNFA_EE_NCI_ACTION;

/* data type for NFA_EE_NCI_DISC_REQ_NTF_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFC_EE_DISCOVER_REQ_REVT* p_data;
} tNFA_EE_NCI_DISC_REQ;

/* data type for NFA_EE_NCI_NFCEE_STATUS_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFC_NFCEE_STATUS_REVT* p_data;
} tNFA_EE_NCI_NFCEE_STATUS_NTF;

/* union of all event data types */
typedef union {
  NFC_HDR hdr;
  tNFA_EE_CFG_HDR cfg_hdr;
  tNFA_EE_API_DISCOVER ee_discover;
  tNFA_EE_API_REGISTER ee_register;
  tNFA_EE_API_DEREGISTER deregister;
  tNFA_EE_API_MODE_SET mode_set;
  tNFA_EE_API_SET_TECH_CFG set_tech;
  tNFA_EE_API_CLEAR_TECH_CFG clear_tech;
  tNFA_EE_API_SET_PROTO_CFG set_proto;
  tNFA_EE_API_CLEAR_PROTO_CFG clear_proto;
  tNFA_EE_API_ADD_AID add_aid;
  tNFA_EE_API_REMOVE_AID rm_aid;
  tNFA_EE_API_ADD_SYSCODE add_syscode;
  tNFA_EE_API_REMOVE_SYSCODE rm_syscode;
  tNFA_EE_API_LMRT_SIZE lmrt_size;
  tNFA_EE_API_CONNECT connect;
  tNFA_EE_API_SEND_DATA send_data;
  tNFA_EE_API_DISCONNECT disconnect;
  tNFA_EE_NCI_DISC_RSP disc_rsp;
  tNFA_EE_NCI_DISC_NTF disc_ntf;
  tNFA_EE_NCI_MODE_SET mode_set_rsp;
  tNFA_EE_NCI_WAIT_RSP wait_rsp;
  tNFA_EE_NCI_CONN conn;
  tNFA_EE_NCI_ACTION act;
  tNFA_EE_NCI_DISC_REQ disc_req;
  tNFA_EE_NCI_NFCEE_STATUS_NTF nfcee_status_ntf;
} tNFA_EE_MSG;

/* type for State Machine (SM) action functions */
typedef void (*tNFA_EE_SM_ACT)(tNFA_EE_MSG* p_data);

/*****************************************************************************
**  control block
*****************************************************************************/
#define NFA_EE_CFGED_UPDATE_NOW 0x80
/* either switch off or battery off is configured */
#define NFA_EE_CFGED_OFF_ROUTING 0x40

/* the following status are the definition used in ee_cfg_sts */
#define NFA_EE_STS_CHANGED_ROUTING 0x01
#define NFA_EE_STS_CHANGED 0x0f
#define NFA_EE_STS_PREV_ROUTING 0x10
#define NFA_EE_STS_PREV 0xf0

/* need to report NFA_EE_UPDATED_EVT */
#define NFA_EE_WAIT_UPDATE 0x10
/* waiting for the rsp of set routing commands */
#define NFA_EE_WAIT_UPDATE_RSP 0x20
#define NFA_EE_WAIT_UPDATE_ALL 0xF0

typedef uint8_t tNFA_EE_WAIT;

/* set this bit when waiting for HCI to finish the initialization process in
 * NFA_EE_EM_STATE_RESTORING */
#define NFA_EE_FLAG_WAIT_HCI 0x01
/* set this bit when EE needs to notify the p_enable_cback at the end of NFCEE
 * discover process in NFA_EE_EM_STATE_RESTORING */
#define NFA_EE_FLAG_NOTIFY_HCI 0x02
/* set this bit when gracefully disable with outstanding NCI connections */
#define NFA_EE_FLAG_WAIT_DISCONN 0x04
typedef uint8_t tNFA_EE_FLAGS;

/* NFCEE DISCOVER in progress       */
#define NFA_EE_DISC_STS_ON 0x00
/* disable NFCEE DISCOVER           */
#define NFA_EE_DISC_STS_OFF 0x01
/* received NFCEE DISCOVER REQ NTF  */
#define NFA_EE_DISC_STS_REQ 0x02
/* received NFA_EE_MODE_SET_COMPLETE  */
#define NFA_EE_MODE_SET_COMPLETE 0x03
/* initialize EE_RECOVERY             */
#define NFA_EE_RECOVERY_INIT 0x04
/* update ee config during EE_RECOVERY */
#define NFA_EE_RECOVERY_REDISCOVERED 0x05
typedef uint8_t tNFA_EE_DISC_STS;

typedef void(tNFA_EE_ENABLE_DONE_CBACK)(tNFA_EE_DISC_STS status);

/* NFA EE Management control block */
typedef struct {
  tNFA_EE_ECB ecb[NFA_EE_NUM_ECBS]; /* control block for DH and NFCEEs  */
  TIMER_LIST_ENT timer;             /* timer to send info to NFCC       */
  TIMER_LIST_ENT discv_timer;       /* timer to end NFCEE discovery     */
  tNFA_EE_CBACK* p_ee_cback[NFA_EE_MAX_CBACKS]; /* to report EE events       */
  tNFA_EE_CBACK* p_ee_disc_cback; /* to report EE discovery result    */
  tNFA_EE_ENABLE_DONE_CBACK*
      p_enable_cback;          /* callback to notify on enable done*/
  tNFA_EE_EM_STATE em_state;   /* NFA-EE state initialized or not  */
  uint8_t wait_rsp;            /* num of NCI rsp expected (update) */
  uint8_t num_ee_expecting;    /* number of ee_info still expecting*/
  uint8_t cur_ee;              /* the number of ee_info in cb      */
  uint8_t ee_cfged;            /* the bit mask of configured ECBs  */
  uint8_t ee_cfg_sts;          /* configuration status             */
  tNFA_EE_WAIT ee_wait_evt;    /* Pending event(s) to be reported  */
  tNFA_EE_FLAGS ee_flags;      /* flags                            */
  uint8_t route_block_control; /* controls route block feature   */
  bool isDiscoveryStopped;     /* discovery status                  */
} tNFA_EE_CB;

/* Order of Routing entries in Routing Table */
#define NCI_ROUTE_ORDER_AID 0x01        /* AID routing order */
#define NCI_ROUTE_ORDER_SYS_CODE 0x03   /* System Code routing order*/
#define NCI_ROUTE_ORDER_PROTOCOL 0x04   /* Protocol routing order*/
#define NCI_ROUTE_ORDER_TECHNOLOGY 0x05 /* Technology routing order*/

/*****************************************************************************
**  External variables
*****************************************************************************/

/* NFA EE control block */
extern tNFA_EE_CB nfa_ee_cb;

/*****************************************************************************
**  External functions
*****************************************************************************/
/* function prototypes - exported from nfa_ee_main.c */
void nfa_ee_sys_enable(void);
void nfa_ee_sys_disable(void);

/* event handler function type */
bool nfa_ee_evt_hdlr(NFC_HDR* p_msg);
void nfa_ee_proc_nfcc_power_mode(uint8_t nfcc_power_mode);
#if (NFC_NFCEE_INCLUDED == TRUE)
void nfa_ee_get_tech_route(uint8_t power_state, uint8_t* p_handles);
#endif
void nfa_ee_proc_evt(tNFC_RESPONSE_EVT event, void* p_data);
tNFA_EE_ECB* nfa_ee_find_ecb(uint8_t nfcee_id);
tNFA_EE_ECB* nfa_ee_find_ecb_by_conn_id(uint8_t conn_id);
uint8_t nfa_ee_ecb_to_mask(tNFA_EE_ECB* p_cb);
void nfa_ee_restore_one_ecb(tNFA_EE_ECB* p_cb);
bool nfa_ee_is_active(tNFA_HANDLE nfcee_id);

/* Action function prototypes - nfa_ee_act.c */
void nfa_ee_api_discover(tNFA_EE_MSG* p_data);
void nfa_ee_api_register(tNFA_EE_MSG* p_data);
void nfa_ee_api_deregister(tNFA_EE_MSG* p_data);
void nfa_ee_api_mode_set(tNFA_EE_MSG* p_data);
void nfa_ee_api_set_tech_cfg(tNFA_EE_MSG* p_data);
void nfa_ee_api_clear_tech_cfg(tNFA_EE_MSG* p_data);
void nfa_ee_api_set_proto_cfg(tNFA_EE_MSG* p_data);
void nfa_ee_api_clear_proto_cfg(tNFA_EE_MSG* p_data);
void nfa_ee_api_add_aid(tNFA_EE_MSG* p_data);
void nfa_ee_api_remove_aid(tNFA_EE_MSG* p_data);
void nfa_ee_api_add_sys_code(tNFA_EE_MSG* p_data);
void nfa_ee_api_remove_sys_code(tNFA_EE_MSG* p_data);
void nfa_ee_api_lmrt_size(tNFA_EE_MSG* p_data);
void nfa_ee_api_update_now(tNFA_EE_MSG* p_data);
void nfa_ee_api_connect(tNFA_EE_MSG* p_data);
void nfa_ee_api_send_data(tNFA_EE_MSG* p_data);
void nfa_ee_api_disconnect(tNFA_EE_MSG* p_data);
void nfa_ee_report_disc_done(bool notify_sys);
void nfa_ee_nci_disc_rsp(tNFA_EE_MSG* p_data);
void nfa_ee_nci_disc_ntf(tNFA_EE_MSG* p_data);
void nfa_ee_nci_mode_set_rsp(tNFA_EE_MSG* p_data);
void nfa_ee_nci_nfcee_status_ntf(tNFA_EE_MSG* p_data);
void nfa_ee_nci_wait_rsp(tNFA_EE_MSG* p_data);
void nfa_ee_nci_conn(tNFA_EE_MSG* p_data);
void nfa_ee_nci_action_ntf(tNFA_EE_MSG* p_data);
void nfa_ee_nci_disc_req_ntf(tNFA_EE_MSG* p_data);
void nfa_ee_rout_timeout(tNFA_EE_MSG* p_data);
void nfa_ee_discv_timeout(tNFA_EE_MSG* p_data);
void nfa_ee_lmrt_to_nfcc(tNFA_EE_MSG* p_data);
void nfa_ee_update_rout(void);
void nfa_ee_report_event(tNFA_EE_CBACK* p_cback, tNFA_EE_EVT event,
                         tNFA_EE_CBACK_DATA* p_data);
tNFA_EE_ECB* nfa_ee_find_aid_offset(uint8_t aid_len, uint8_t* p_aid,
                                    int* p_offset, int* p_entry);
tNFA_EE_ECB* nfa_ee_find_sys_code_offset(uint16_t sys_code, int* p_offset,
                                         int* p_entry);
int nfa_ee_find_total_aid_len(tNFA_EE_ECB* p_cb, int start_entry);
void nfa_ee_start_timer(void);
void nfa_ee_reg_cback_enable_done(tNFA_EE_ENABLE_DONE_CBACK* p_cback);
void nfa_ee_report_update_evt(void);

extern void nfa_ee_proc_hci_info_cback(void);
void nfa_ee_check_disable(void);
bool nfa_ee_restore_ntf_done(void);
void nfa_ee_check_restore_complete(void);
int nfa_ee_find_max_aid_cfg_len(void);
#endif /* NFA_P2P_INT_H */
