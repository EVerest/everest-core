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
 *  This is the private interface file for the NFA P2P.
 *
 ******************************************************************************/
#ifndef NFA_P2P_INT_H
#define NFA_P2P_INT_H

#if (NFA_P2P_INCLUDED == TRUE)
#include "nfa_dm_int.h"
#include "nfa_p2p_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* NFA P2P LLCP link state */
enum {
  NFA_P2P_LLCP_STATE_IDLE,
  NFA_P2P_LLCP_STATE_LISTENING,
  NFA_P2P_LLCP_STATE_ACTIVATED

};

typedef uint8_t tNFA_P2P_LLCP_STATE;

/* NFA P2P events */
enum {
  NFA_P2P_API_REG_SERVER_EVT = NFA_SYS_EVT_START(NFA_ID_P2P),
  NFA_P2P_API_REG_CLIENT_EVT,
  NFA_P2P_API_DEREG_EVT,
  NFA_P2P_API_ACCEPT_CONN_EVT,
  NFA_P2P_API_REJECT_CONN_EVT,
  NFA_P2P_API_DISCONNECT_EVT,
  NFA_P2P_API_CONNECT_EVT,
  NFA_P2P_API_SEND_UI_EVT,
  NFA_P2P_API_SEND_DATA_EVT,
  NFA_P2P_API_SET_LOCAL_BUSY_EVT,
  NFA_P2P_API_GET_LINK_INFO_EVT,
  NFA_P2P_API_GET_REMOTE_SAP_EVT,
  NFA_P2P_API_SET_LLCP_CFG_EVT,
  NFA_P2P_INT_RESTART_RF_DISC_EVT,

  NFA_P2P_LAST_EVT
};

/* data type for NFA_P2P_API_REG_SERVER_EVT */
typedef struct {
  NFC_HDR hdr;
  uint8_t server_sap;
  tNFA_P2P_LINK_TYPE link_type;
  char service_name[LLCP_MAX_SN_LEN + 1];
  tNFA_P2P_CBACK* p_cback;
} tNFA_P2P_API_REG_SERVER;

/* data type for NFA_P2P_API_REG_CLIENT_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_P2P_LINK_TYPE link_type;
  tNFA_P2P_CBACK* p_cback;
} tNFA_P2P_API_REG_CLIENT;

/* data type for NFA_P2P_API_DEREG_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE handle;
} tNFA_P2P_API_DEREG;

/* data type for NFA_P2P_API_ACCEPT_CONN_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE conn_handle;
  uint16_t miu;
  uint8_t rw;
} tNFA_P2P_API_ACCEPT_CONN;

/* data type for NFA_P2P_API_REJECT_CONN_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE conn_handle;
} tNFA_P2P_API_REJECT_CONN;

/* data type for NFA_P2P_API_DISCONNECT_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE conn_handle;
  bool flush;
} tNFA_P2P_API_DISCONNECT;

/* data type for NFA_P2P_API_CONNECT_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE client_handle;
  char service_name[LLCP_MAX_SN_LEN + 1];
  uint8_t dsap;
  uint16_t miu;
  uint8_t rw;
} tNFA_P2P_API_CONNECT;

/* data type for NFA_P2P_API_SEND_UI_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE handle;
  uint8_t dsap;
  NFC_HDR* p_msg;
} tNFA_P2P_API_SEND_UI;

/* data type for NFA_P2P_API_SEND_DATA_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE conn_handle;
  NFC_HDR* p_msg;
} tNFA_P2P_API_SEND_DATA;

/* data type for NFA_P2P_API_SET_LOCAL_BUSY_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE conn_handle;
  bool is_busy;
} tNFA_P2P_API_SET_LOCAL_BUSY;

/* data type for NFA_P2P_API_GET_LINK_INFO_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE handle;
} tNFA_P2P_API_GET_LINK_INFO;

/* data type for NFA_P2P_API_GET_REMOTE_SAP_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE handle;
  char service_name[LLCP_MAX_SN_LEN + 1];
} tNFA_P2P_API_GET_REMOTE_SAP;

/* data type for NFA_P2P_API_SET_LLCP_CFG_EVT */
typedef struct {
  NFC_HDR hdr;
  uint16_t link_miu;
  uint8_t opt;
  uint8_t wt;
  uint16_t link_timeout;
  uint16_t inact_timeout_init;
  uint16_t inact_timeout_target;
  uint16_t symm_delay;
  uint16_t data_link_timeout;
  uint16_t delay_first_pdu_timeout;
} tNFA_P2P_API_SET_LLCP_CFG;

/* union of all event data types */
typedef union {
  NFC_HDR hdr;
  tNFA_P2P_API_REG_SERVER api_reg_server;
  tNFA_P2P_API_REG_CLIENT api_reg_client;
  tNFA_P2P_API_DEREG api_dereg;
  tNFA_P2P_API_ACCEPT_CONN api_accept;
  tNFA_P2P_API_REJECT_CONN api_reject;
  tNFA_P2P_API_DISCONNECT api_disconnect;
  tNFA_P2P_API_CONNECT api_connect;
  tNFA_P2P_API_SEND_UI api_send_ui;
  tNFA_P2P_API_SEND_DATA api_send_data;
  tNFA_P2P_API_SET_LOCAL_BUSY api_local_busy;
  tNFA_P2P_API_GET_LINK_INFO api_link_info;
  tNFA_P2P_API_GET_REMOTE_SAP api_remote_sap;
  tNFA_P2P_API_SET_LLCP_CFG api_set_llcp_cfg;
} tNFA_P2P_MSG;

/*****************************************************************************
**  control block
*****************************************************************************/
/* Bit flag for connection handle           */
#define NFA_P2P_HANDLE_FLAG_CONN 0x80

/* NFA P2P Connection block */
/* Connection control block is used         */
#define NFA_P2P_CONN_FLAG_IN_USE 0x01
/* Remote set RW to 0 (flow off)            */
#define NFA_P2P_CONN_FLAG_REMOTE_RW_ZERO 0x02
/* data link connection is congested        */
#define NFA_P2P_CONN_FLAG_CONGESTED 0x04

typedef struct {
  uint8_t flags;             /* internal flags for data link connection  */
  uint8_t local_sap;         /* local SAP of data link connection        */
  uint8_t remote_sap;        /* remote SAP of data link connection       */
  uint16_t remote_miu;       /* MIU of remote end point                  */
  uint8_t num_pending_i_pdu; /* number of tx I PDU not processed by NFA  */
} tNFA_P2P_CONN_CB;

/* NFA P2P SAP control block */
/* registered server                        */
#define NFA_P2P_SAP_FLAG_SERVER 0x01
/* registered client                        */
#define NFA_P2P_SAP_FLAG_CLIENT 0x02
/* logical link connection is congested     */
#define NFA_P2P_SAP_FLAG_LLINK_CONGESTED 0x04

typedef struct {
  uint8_t flags;              /* internal flags for local SAP             */
  tNFA_P2P_CBACK* p_cback;    /* callback function for local SAP          */
  uint8_t num_pending_ui_pdu; /* number of tx UI PDU not processed by NFA */
} tNFA_P2P_SAP_CB;

/* NFA P2P SDP control block */
typedef struct {
  uint8_t tid; /* transaction ID */
  uint8_t local_sap;
} tNFA_P2P_SDP_CB;

#define NFA_P2P_NUM_SAP 64

/* NFA P2P control block */
typedef struct {
  tNFA_HANDLE dm_disc_handle;

  tNFA_DM_RF_DISC_STATE rf_disc_state;
  tNFA_P2P_LLCP_STATE llcp_state;
  bool is_initiator;
  bool is_active_mode;
  uint16_t local_link_miu;
  uint16_t remote_link_miu;

  tNFA_TECHNOLOGY_MASK listen_tech_mask; /* for P2P listening */
  tNFA_TECHNOLOGY_MASK
      listen_tech_mask_to_restore; /* to retry without active listen mode */
  TIMER_LIST_ENT
  active_listen_restore_timer; /* timer to restore active listen mode */
  bool is_p2p_listening;
  bool is_snep_listening;

  tNFA_P2P_SAP_CB sap_cb[NFA_P2P_NUM_SAP];
  tNFA_P2P_CONN_CB conn_cb[LLCP_MAX_DATA_LINK];
  tNFA_P2P_SDP_CB sdp_cb[LLCP_MAX_SDP_TRANSAC];

  uint8_t
      total_pending_ui_pdu; /* total number of tx UI PDU not processed by NFA */
  uint8_t
      total_pending_i_pdu; /* total number of tx I PDU not processed by NFA */
  uint8_t               trace_level;
} tNFA_P2P_CB;

/*****************************************************************************
**  External variables
*****************************************************************************/

/* NFA P2P control block */
extern tNFA_P2P_CB nfa_p2p_cb;

/*****************************************************************************
**  External functions
*****************************************************************************/
/*
**  nfa_p2p_main.c
*/
void nfa_p2p_init(void);
void nfa_p2p_update_listen_tech(tNFA_TECHNOLOGY_MASK tech_mask);
void nfa_p2p_enable_listening(tNFA_SYS_ID sys_id, bool update_wks);
void nfa_p2p_disable_listening(tNFA_SYS_ID sys_id, bool update_wks);
void nfa_p2p_activate_llcp(tNFC_DISCOVER* p_data);
void nfa_p2p_deactivate_llcp(void);
void nfa_p2p_set_config(tNFA_DM_DISC_TECH_PROTO_MASK disc_mask);

/*
**  nfa_p2p_act.c
*/
void nfa_p2p_proc_llcp_data_ind(tLLCP_SAP_CBACK_DATA* p_data);
void nfa_p2p_proc_llcp_connect_ind(tLLCP_SAP_CBACK_DATA* p_data);
void nfa_p2p_proc_llcp_connect_resp(tLLCP_SAP_CBACK_DATA* p_data);
void nfa_p2p_proc_llcp_disconnect_ind(tLLCP_SAP_CBACK_DATA* p_data);
void nfa_p2p_proc_llcp_disconnect_resp(tLLCP_SAP_CBACK_DATA* p_data);
void nfa_p2p_proc_llcp_congestion(tLLCP_SAP_CBACK_DATA* p_data);
void nfa_p2p_proc_llcp_link_status(tLLCP_SAP_CBACK_DATA* p_data);

bool nfa_p2p_start_sdp(char* p_service_name, uint8_t local_sap);

bool nfa_p2p_reg_server(tNFA_P2P_MSG* p_msg);
bool nfa_p2p_reg_client(tNFA_P2P_MSG* p_msg);
bool nfa_p2p_dereg(tNFA_P2P_MSG* p_msg);
bool nfa_p2p_accept_connection(tNFA_P2P_MSG* p_msg);
bool nfa_p2p_reject_connection(tNFA_P2P_MSG* p_msg);
bool nfa_p2p_disconnect(tNFA_P2P_MSG* p_msg);
bool nfa_p2p_create_data_link_connection(tNFA_P2P_MSG* p_msg);
bool nfa_p2p_send_ui(tNFA_P2P_MSG* p_msg);
bool nfa_p2p_send_data(tNFA_P2P_MSG* p_msg);
bool nfa_p2p_set_local_busy(tNFA_P2P_MSG* p_msg);
bool nfa_p2p_get_link_info(tNFA_P2P_MSG* p_msg);
bool nfa_p2p_get_remote_sap(tNFA_P2P_MSG* p_msg);
bool nfa_p2p_set_llcp_cfg(tNFA_P2P_MSG* p_msg);
bool nfa_p2p_restart_rf_discovery(tNFA_P2P_MSG* p_msg);

#else

#define nfa_p2p_init ()
#define nfa_p2p_activate_llcp (a){};
#define nfa_p2p_deactivate_llcp ()
#define nfa_p2p_set_config ()

#endif /* (NFA_P2P_INCLUDED == TRUE) */
#endif /* NFA_P2P_INT_H */
