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
 *  This file contains LLCP internal definitions
 *
 ******************************************************************************/
#ifndef LLCP_INT_H
#define LLCP_INT_H

#include "llcp_api.h"
#include "nfc_api.h"

/*
** LLCP link states
*/
enum {
  LLCP_LINK_STATE_DEACTIVATED,      /* llcp link is deactivated     */
  LLCP_LINK_STATE_ACTIVATED,        /* llcp link has been activated */
  LLCP_LINK_STATE_DEACTIVATING,     /* llcp link is deactivating    */
  LLCP_LINK_STATE_ACTIVATION_FAILED /* llcp link activation was failed */
};
typedef uint8_t tLLCP_LINK_STATE;

/*
** LLCP Symmetric state
*/

#define LLCP_LINK_SYMM_LOCAL_XMIT_NEXT 0
#define LLCP_LINK_SYMM_REMOTE_XMIT_NEXT 1

/*
** LLCP internal flags
*/
/* Received any LLC PDU in activated state */
#define LLCP_LINK_FLAGS_RX_ANY_LLC_PDU 0x01

/*
** LLCP link control block
*/
typedef struct {
  tLLCP_LINK_STATE link_state; /* llcp link state */
  tLLCP_LINK_CBACK*
      p_link_cback; /* callback function to report llcp link status */
  uint16_t wks;     /* well-known service bit-map                   */

  bool is_initiator;    /* TRUE if initiator role                       */
  bool is_sending_data; /* TRUE if llcp_link_check_send_data() is excuting    */
  uint8_t flags;        /* LLCP internal flags                          */
  bool received_first_packet; /* TRUE if a packet has been received from remote
                                 */
  uint8_t agreed_major_version; /* llcp major version used in activated state */
  uint8_t agreed_minor_version; /* llcp minor version used in activated state */

  uint8_t peer_version;   /* llcp version of peer device                  */
  uint16_t peer_miu;      /* link MIU of peer device                      */
  uint16_t peer_wks;      /* WKS of peer device                           */
  uint16_t peer_lto;      /* link timeout of peer device in ms            */
  uint8_t peer_opt;       /* Option field of peer device                  */
  uint16_t effective_miu; /* MIU to send PDU in activated state           */

  TIMER_LIST_ENT timer; /* link timer for LTO and SYMM response         */
  uint8_t symm_state;   /* state of symmectric procedure                */
  bool ll_served;       /* TRUE if last transmisstion was for UI        */
  uint8_t ll_idx;       /* for scheduler of logical link connection     */
  uint8_t dl_idx;       /* for scheduler of data link connection        */

  TIMER_LIST_ENT inact_timer; /* inactivity timer                             */
  uint16_t inact_timeout;     /* inactivity timeout in ms                     */

  uint8_t link_deact_reason; /* reason of LLCP link deactivated              */

  BUFFER_Q sig_xmit_q; /* tx signaling PDU queue                       */

  /* runtime configuration parameters */
  uint16_t local_link_miu; /* Maximum Information Unit                     */
  uint8_t local_opt;       /* Option parameter                             */
  uint8_t local_wt;        /* Response Waiting Time Index                  */
  uint16_t local_lto;      /* Local Link Timeout                           */
  uint16_t inact_timeout_init;   /* Inactivity Timeout as initiator role */
  uint16_t inact_timeout_target; /* Inactivity Timeout as target role */
  uint16_t symm_delay;        /* Delay SYMM response                          */
  uint16_t data_link_timeout; /* data link conneciton timeout                 */
  uint16_t delay_first_pdu_timeout; /* delay timeout to send first PDU as
                                       initiator */

} tLLCP_LCB;

/*
** LLCP Application's registration control block on service access point (SAP)
*/

typedef struct {
  uint8_t link_type;    /* logical link and/or data link                */
  char* p_service_name; /* GKI buffer containing service name           */
  tLLCP_APP_CBACK* p_app_cback; /* application's callback pointer */

  BUFFER_Q ui_xmit_q;      /* UI PDU queue for transmitting                */
  BUFFER_Q ui_rx_q;        /* UI PDU queue for receiving                   */
  bool is_ui_tx_congested; /* TRUE if transmitting UI PDU is congested     */

} tLLCP_APP_CB;

/*
** LLCP data link connection states
*/
enum {
  LLCP_DLC_STATE_IDLE, /* initial state                                    */
  LLCP_DLC_STATE_W4_REMOTE_RESP, /* waiting for connection confirm from peer */
  LLCP_DLC_STATE_W4_LOCAL_RESP,  /* waiting for connection confirm from upper
                                    layer  */
  LLCP_DLC_STATE_CONNECTED,      /* data link connection has been established */
  LLCP_DLC_STATE_W4_REMOTE_DM /* waiting for disconnection confirm from peer */
};
typedef uint8_t tLLCP_DLC_STATE;

/*
** LLCP data link connection events
*/
enum {
  LLCP_DLC_EVENT_API_CONNECT_REQ,    /* connection request from upper layer  */
  LLCP_DLC_EVENT_API_CONNECT_CFM,    /* connection confirm from upper layer  */
  LLCP_DLC_EVENT_API_CONNECT_REJECT, /* connection reject from upper layer   */
  LLCP_DLC_EVENT_PEER_CONNECT_IND,   /* connection request from peer         */
  LLCP_DLC_EVENT_PEER_CONNECT_CFM,   /* connection confirm from peer         */

  LLCP_DLC_EVENT_API_DATA_REQ,  /* data packet from upper layer         */
  LLCP_DLC_EVENT_PEER_DATA_IND, /* data packet from peer                */

  LLCP_DLC_EVENT_API_DISCONNECT_REQ,  /* disconnect request from upper layer  */
  LLCP_DLC_EVENT_PEER_DISCONNECT_IND, /* disconnect request from peer         */
  LLCP_DLC_EVENT_PEER_DISCONNECT_RESP, /* disconnect response from peer */

  LLCP_DLC_EVENT_FRAME_ERROR, /* received erroneous frame from peer   */
  LLCP_DLC_EVENT_LINK_ERROR,  /* llcp link has been deactivated       */

  LLCP_DLC_EVENT_TIMEOUT /* timeout event                        */
};
typedef uint8_t tLLCP_DLC_EVENT;

/*
** LLCP data link connection control block
*/

/* send DISC when tx queue is empty       */
#define LLCP_DATA_LINK_FLAG_PENDING_DISC 0x01
/* send RR/RNR with valid sequence        */
#define LLCP_DATA_LINK_FLAG_PENDING_RR_RNR 0x02
/* notify upper later when tx complete    */
#define LLCP_DATA_LINK_FLAG_NOTIFY_TX_DONE 0x04

typedef struct {
  tLLCP_DLC_STATE state;  /* data link connection state               */
  uint8_t flags;          /* specific action flags                    */
  tLLCP_APP_CB* p_app_cb; /* pointer of application registration      */
  TIMER_LIST_ENT timer;   /* timer for connection complete            */

  uint8_t local_sap;  /* SAP of local end point                   */
  uint16_t local_miu; /* MIU of local SAP                         */
  uint8_t local_rw;   /* RW of local SAP                          */
  bool local_busy;    /* TRUE if local SAP is busy                */

  uint8_t remote_sap;  /* SAP of remote end point                  */
  uint16_t remote_miu; /* MIU of remote SAP                        */
  uint8_t remote_rw;   /* RW of remote SAP                         */
  bool remote_busy;    /* TRUE if remote SAP is busy               */

  uint8_t next_tx_seq;  /* V(S), send state variable                */
  uint8_t rcvd_ack_seq; /* V(SA), send ack state variable           */
  uint8_t next_rx_seq;  /* V(R), receive state variable             */
  uint8_t sent_ack_seq; /* V(RA), receive ack state variable        */

  BUFFER_Q i_xmit_q;    /* tx queue of I PDU                        */
  bool is_tx_congested; /* TRUE if tx I PDU is congested            */

  BUFFER_Q i_rx_q;              /* rx queue of I PDU                        */
  bool is_rx_congested;         /* TRUE if rx I PDU is congested            */
  uint8_t num_rx_i_pdu;         /* number of I PDU in rx queue              */
  uint8_t rx_congest_threshold; /* dynamic congest threshold for rx I PDU */

} tLLCP_DLCB;

/*
** LLCP service discovery control block
*/

typedef struct {
  uint8_t tid;              /* transaction ID                           */
  tLLCP_SDP_CBACK* p_cback; /* callback function for service discovery  */
} tLLCP_SDP_TRANSAC;

typedef struct {
  uint8_t next_tid;                                /* next TID to use         */
  tLLCP_SDP_TRANSAC transac[LLCP_MAX_SDP_TRANSAC]; /* active SDP transactions */
  NFC_HDR* p_snl;                                  /* buffer for SNL PDU      */
} tLLCP_SDP_CB;

/*
** LLCP control block
*/

typedef struct {
  tLLCP_SDP_CB sdp_cb; /* SDP control block                            */
  tLLCP_LCB lcb;       /* LLCP link control block                      */
  tLLCP_APP_CB wks_cb[LLCP_MAX_WKS]; /* Application's registration for
                                        well-known services */
  tLLCP_APP_CB server_cb
      [LLCP_MAX_SERVER]; /* Application's registration for SDP services  */
  tLLCP_APP_CB
      client_cb[LLCP_MAX_CLIENT]; /* Application's registration for client */
  tLLCP_DLCB dlcb[LLCP_MAX_DATA_LINK]; /* Data link connection control block */

  uint8_t max_num_ll_tx_buff; /* max number of tx UI PDU in queue             */
  uint8_t max_num_tx_buff;    /* max number of tx UI/I PDU in queue           */

  uint8_t num_logical_data_link; /* number of logical data link */
  uint8_t
      num_data_link_connection; /* number of established data link connection */

  /* these two thresholds (number of tx UI PDU) are dynamically adjusted based
   * on number of logical links */
  uint8_t
      ll_tx_congest_start;   /* congest start threshold for each logical link*/
  uint8_t ll_tx_congest_end; /* congest end threshold for each logical link  */

  uint8_t total_tx_ui_pdu;   /* total number of tx UI PDU in all of ui_xmit_q*/
  uint8_t total_tx_i_pdu;    /* total number of tx I PDU in all of i_xmit_q  */
  bool overall_tx_congested; /* TRUE if tx link is congested                 */

  /* start point of uncongested status notification is in round robin */
  uint8_t ll_tx_uncongest_ntf_start_sap; /* next start of logical data link */
  uint8_t
      dl_tx_uncongest_ntf_start_idx; /* next start of data link connection */

  /*
  ** when overall rx link congestion starts, RNR is sent to remote end point
  ** of data link connection while rx link is congested, UI PDU is discarded.
  */
  uint8_t num_rx_buff; /* reserved number of rx UI/I PDU in queue      */
  uint8_t
      overall_rx_congest_start;   /* threshold of overall rx congestion start */
  uint8_t overall_rx_congest_end; /* threshold of overall rx congestion end */
  uint8_t max_num_ll_rx_buff; /* max number of rx UI PDU in queue             */

  /*
  ** threshold (number of rx UI PDU) is dynamically adjusted based on number
  ** of logical links when number of rx UI PDU is more than
  ** ll_rx_congest_start, the oldest UI PDU is discarded
  */
  uint8_t ll_rx_congest_start; /* rx congest start threshold for each logical
                                  link */

  uint8_t total_rx_ui_pdu;   /* total number of rx UI PDU in all of ui_rx_q  */
  uint8_t total_rx_i_pdu;    /* total number of rx I PDU in all of i_rx_q    */
  bool overall_rx_congested; /* TRUE if overall rx link is congested         */
  tLLCP_DTA_CBACK* p_dta_cback; /* callback to notify DTA when respoding SNL */
  bool dta_snl_resp; /* TRUE if need to notify DTA when respoding SNL*/
} tLLCP_CB;

#if (LLCP_TEST_INCLUDED == TRUE) /* this is for LLCP testing */

typedef struct {
  uint8_t version;
  uint16_t wks;
} tLLCP_TEST_PARAMS;

#endif

/*
** LLCP global data
*/

extern tLLCP_CB llcp_cb;

/*
** Functions provided by llcp_main.c
*/
void llcp_init(void);
void llcp_cleanup(void);
void llcp_process_timeout(TIMER_LIST_ENT* p_tle);

/*
** Functions provided by llcp_link.c
*/
tLLCP_STATUS llcp_link_activate(tLLCP_ACTIVATE_CONFIG* p_config);
void llcp_link_process_link_timeout(void);
void llcp_link_deactivate(uint8_t reason);

void llcp_link_check_send_data(void);
void llcp_link_connection_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                                tNFC_CONN* p_data);

/*
**  Functions provided by llcp_util.c
*/
void llcp_util_adjust_ll_congestion(void);
void llcp_util_adjust_dl_rx_congestion(void);
void llcp_util_check_rx_congested_status(void);
bool llcp_util_parse_link_params(uint16_t length, uint8_t* p_bytes);
tLLCP_STATUS llcp_util_send_ui(uint8_t ssap, uint8_t dsap,
                               tLLCP_APP_CB* p_app_cb, NFC_HDR* p_msg);
void llcp_util_send_disc(uint8_t dsap, uint8_t ssap);
tLLCP_DLCB* llcp_util_allocate_data_link(uint8_t reg_sap, uint8_t remote_sap);
void llcp_util_deallocate_data_link(tLLCP_DLCB* p_dlcb);
tLLCP_STATUS llcp_util_send_connect(tLLCP_DLCB* p_dlcb,
                                    tLLCP_CONNECTION_PARAMS* p_params);
tLLCP_STATUS llcp_util_parse_connect(uint8_t* p_bytes, uint16_t length,
                                     tLLCP_CONNECTION_PARAMS* p_params);
tLLCP_STATUS llcp_util_send_cc(tLLCP_DLCB* p_dlcb,
                               tLLCP_CONNECTION_PARAMS* p_params);
tLLCP_STATUS llcp_util_parse_cc(uint8_t* p_bytes, uint16_t length,
                                uint16_t* p_miu, uint8_t* p_rw);
void llcp_util_send_dm(uint8_t dsap, uint8_t ssap, uint8_t reason);
void llcp_util_build_info_pdu(tLLCP_DLCB* p_dlcb, NFC_HDR* p_msg);
tLLCP_STATUS llcp_util_send_frmr(tLLCP_DLCB* p_dlcb, uint8_t flags,
                                 uint8_t ptype, uint8_t sequence);
void llcp_util_send_rr_rnr(tLLCP_DLCB* p_dlcb);
tLLCP_APP_CB* llcp_util_get_app_cb(uint8_t sap);
/*
** Functions provided by llcp_dlc.c
*/
tLLCP_STATUS llcp_dlsm_execute(tLLCP_DLCB* p_dlcb, tLLCP_DLC_EVENT event,
                               void* p_data);
tLLCP_DLCB* llcp_dlc_find_dlcb_by_sap(uint8_t local_sap, uint8_t remote_sap);
void llcp_dlc_flush_q(tLLCP_DLCB* p_dlcb);
void llcp_dlc_proc_i_pdu(uint8_t dsap, uint8_t ssap, uint16_t i_pdu_length,
                         uint8_t* p_i_pdu, NFC_HDR* p_msg);
void llcp_dlc_proc_rx_pdu(uint8_t dsap, uint8_t ptype, uint8_t ssap,
                          uint16_t length, uint8_t* p_data);
void llcp_dlc_check_to_send_rr_rnr(void);
bool llcp_dlc_is_rw_open(tLLCP_DLCB* p_dlcb);
NFC_HDR* llcp_dlc_get_next_pdu(tLLCP_DLCB* p_dlcb);
uint16_t llcp_dlc_get_next_pdu_length(tLLCP_DLCB* p_dlcb);

/*
** Functions provided by llcp_sdp.c
*/
void llcp_sdp_proc_data(tLLCP_SAP_CBACK_DATA* p_data);
tLLCP_STATUS llcp_sdp_send_sdreq(uint8_t tid, char* p_name);
uint8_t llcp_sdp_get_sap_by_name(char* p_name, uint8_t length);
tLLCP_STATUS llcp_sdp_proc_snl(uint16_t sdu_length, uint8_t* p);
void llcp_sdp_check_send_snl(void);
void llcp_sdp_proc_deactivation(void);
#endif
