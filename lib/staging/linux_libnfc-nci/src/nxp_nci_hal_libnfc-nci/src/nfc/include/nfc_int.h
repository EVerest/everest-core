/******************************************************************************
 *
 *  Copyright (C) 2009-2014 Broadcom Corporation
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
 *  this file contains the main NFC Upper Layer internal definitions and
 *  functions.
 *
 ******************************************************************************/

#ifndef NFC_INT_H_
#define NFC_INT_H_

#include "gki.h"
#include "nci_defs.h"
#include "nfc_api.h"
#include "nfc_target.h"

/****************************************************************************
** Internal NFC constants and definitions
****************************************************************************/

/****************************************************************************
** NFC_TASK definitions
****************************************************************************/

/* NFC_TASK event masks */
#define NFC_TASK_EVT_TRANSPORT_READY EVENT_MASK(APPL_EVT_0)

/* NFC Timer events */
#define NFC_TTYPE_NCI_WAIT_RSP 0
#define NFC_TTYPE_WAIT_2_DEACTIVATE 1
#define NFC_WAIT_RSP_RAW_VS 0x02
#define NFC_TTYPE_WAIT_MODE_SET_NTF 2

#define NFC_TTYPE_LLCP_LINK_MANAGER 100
#define NFC_TTYPE_LLCP_LINK_INACT 101
#define NFC_TTYPE_LLCP_DATA_LINK 102
#define NFC_TTYPE_LLCP_DELAY_FIRST_PDU 103
#define NFC_TTYPE_RW_T1T_RESPONSE 104
#define NFC_TTYPE_RW_T2T_RESPONSE 105
#define NFC_TTYPE_RW_T3T_RESPONSE 106
#define NFC_TTYPE_RW_T4T_RESPONSE 107
#define NFC_TTYPE_RW_I93_RESPONSE 108
#define NFC_TTYPE_CE_T4T_UPDATE 109
/* added for p2p prio logic timer */
#define NFC_TTYPE_P2P_PRIO_RESPONSE 110
/* added for p2p prio logic clenaup */
#define NFC_TTYPE_P2P_PRIO_LOGIC_CLEANUP 111
#define NFC_TTYPE_RW_MFC_RESPONSE 112
/* time out for mode set notification */
#define NFC_MODE_SET_NTF_TIMEOUT 2
/* NFC Task event messages */

enum {
  NFC_STATE_NONE,                /* not start up yet                         */
  NFC_STATE_W4_HAL_OPEN,         /* waiting for HAL_NFC_OPEN_CPLT_EVT        */
  NFC_STATE_CORE_INIT,           /* sending CORE_RESET and CORE_INIT         */
  NFC_STATE_W4_POST_INIT_CPLT,   /* waiting for HAL_NFC_POST_INIT_CPLT_EVT   */
  NFC_STATE_IDLE,                /* normal operation (discovery state)       */
  NFC_STATE_OPEN,                /* NFC link is activated                    */
  NFC_STATE_CLOSING,             /* de-activating                            */
  NFC_STATE_W4_HAL_CLOSE,        /* waiting for HAL_NFC_POST_INIT_CPLT_EVT   */
  NFC_STATE_NFCC_POWER_OFF_SLEEP /* NFCC is power-off sleep mode             */
};
typedef uint8_t tNFC_STATE;

/* DM P2P Priority event type */
enum {
  NFA_DM_P2P_PRIO_RSP = 0x01, /* P2P priority event from RSP   */
  NFA_DM_P2P_PRIO_NTF         /* P2P priority event from NTF   */
};

/* NFC control block flags */
/* NFC_Deactivate () is called and the NCI cmd is not sent   */
#define NFC_FL_DEACTIVATING 0x0001
/* restarting NFCC after PowerOffSleep          */
#define NFC_FL_RESTARTING 0x0002
/* enterning power off sleep mode               */
#define NFC_FL_POWER_OFF_SLEEP 0x0004
/* Power cycle NFCC                             */
#define NFC_FL_POWER_CYCLE_NFCC 0x0008
/* HAL requested control on NCI command window  */
#define NFC_FL_CONTROL_REQUESTED 0x0010
/* NCI command window is on the HAL side        */
#define NFC_FL_CONTROL_GRANTED 0x0020
/* NCI command window is on the HAL side        */
#define NFC_FL_DISCOVER_PENDING 0x0040
/* NFC_FL_CONTROL_REQUESTED on HAL request      */
#define NFC_FL_HAL_REQUESTED 0x0080
/* Waiting for NFCEE Mode Set NTF                 */
#define NFC_FL_WAIT_MODE_SET_NTF 0x0100

#define NFC_PEND_CONN_ID 0xFE
#define NFC_CONN_ID_ID_MASK NCI_CID_MASK
/* set num_buff to this for no flow control */
#define NFC_CONN_NO_FC 0xFF

#if (NFC_RW_ONLY == FALSE)
/* only allow the entries that the NFCC can support */
#define NFC_CHECK_MAX_CONN()                          \
  {                                                   \
    if (max > nfc_cb.max_conn) max = nfc_cb.max_conn; \
  }
#else
#define NFC_CHECK_MAX_CONN()
#endif

typedef struct {
  tNFC_CONN_CBACK* p_cback; /* the callback function to receive the data */
  BUFFER_Q tx_q;         /* transmit queue                                   */
  BUFFER_Q rx_q;         /* receive queue                                    */
  uint8_t id;            /* NFCEE ID or RF Discovery ID or NFC_TEST_ID       */
  uint8_t act_protocol;  /* the active protocol on this logical connection   */
  uint8_t act_interface; /* the active interface on this logical connection   */
  uint8_t conn_id;       /* the connection id assigned by NFCC for this conn */
  uint8_t buff_size;     /* the max buffer size for this connection.     .   */
  uint8_t num_buff;      /* num of buffers left to send on this connection   */
  uint8_t init_credits;  /* initial num of buffer credits                    */
} tNFC_CONN_CB;

/* This data type is for NFC task to send a NCI VS command to NCIT task */
typedef struct {
  NFC_HDR bt_hdr;         /* the NCI command          */
  tNFC_VS_CBACK* p_cback; /* the callback function to receive RSP   */
} tNFC_NCI_VS_MSG;

/* This data type is for HAL event */
typedef struct {
  NFC_HDR hdr;
  uint8_t hal_evt; /* HAL event code  */
  uint8_t status;  /* tHAL_NFC_STATUS */
} tNFC_HAL_EVT_MSG;

/* callback function pointer(8; use 8 to be safe + NFC_SAVED_CMD_SIZE(2) */
#define NFC_RECEIVE_MSGS_OFFSET (10)

#define NFC_SAVED_HDR_SIZE (2)
/* data Reassembly error (in NFC_HDR.layer_specific) */
#define NFC_RAS_TOO_BIG 0x08
#define NFC_RAS_FRAGMENTED 0x01

/* NCI command buffer contains a VSC (in NFC_HDR.layer_specific) */
#define NFC_WAIT_RSP_VSC 0x01

/* NFC control blocks */
typedef struct {
  uint16_t flags; /* NFC control block flags - NFC_FL_* */
  tNFC_CONN_CB conn_cb[NCI_MAX_CONN_CBS];
  uint8_t conn_id[NFC_MAX_CONN_ID + 1]; /* index: conn_id; conn_id[]: index(1
                                           based) to conn_cb[] */
  tNFC_DISCOVER_CBACK* p_discv_cback;
  tNFC_RESPONSE_CBACK* p_resp_cback;
  tNFC_TEST_CBACK* p_test_cback;
  tNFC_VS_CBACK*
      p_vs_cb[NFC_NUM_VS_CBACKS]; /* Register for vendor specific events  */

#if (NFC_RW_ONLY == FALSE)
  /* NFCC information at init rsp */
  uint32_t nci_features; /* the NCI features supported by NFCC */
  uint16_t max_ce_table; /* the max routing table size       */
  uint8_t max_conn;      /* the num of connections supported by NFCC */
#endif
  uint8_t nci_ctrl_size; /* Max Control Packet Payload Size */

  const tNCI_DISCOVER_MAPS*
      p_disc_maps; /* NCI RF Discovery interface mapping */
  uint8_t vs_interface
      [NFC_NFCC_MAX_NUM_VS_INTERFACE]; /* the NCI VS interfaces of NFCC    */
  uint16_t nci_interfaces;             /* the NCI interfaces of NFCC       */
  uint8_t nci_intf_extensions;
  uint8_t nci_intf_extension_map[NCI_INTERFACE_EXTENSION_MAX];
  uint8_t num_disc_maps; /* number of RF Discovery interface mappings */
  void* p_disc_pending;  /* the parameters associated with pending
                            NFC_DiscoveryStart */

  /* NFC_TASK timer management */
  TIMER_LIST_Q timer_queue; /* 1-sec timer event queue */
  TIMER_LIST_Q quick_timer_queue;
  TIMER_LIST_ENT mode_set_ntf_timer; /* Timer to wait for deactivation */
  TIMER_LIST_ENT deactivate_timer;   /* Timer to wait for deactivation */

  tNFC_STATE nfc_state;
  bool reassembly; /* Reassemble fragmented data pkt */
  UINT8               trace_level;
  uint8_t last_hdr[NFC_SAVED_HDR_SIZE]; /* part of last NCI command header */
  uint8_t last_cmd[NFC_SAVED_CMD_SIZE]; /* part of last NCI command payload */
  void* p_vsc_cback;       /* the callback function for last VSC command */
  BUFFER_Q nci_cmd_xmit_q; /* NCI command queue */
  TIMER_LIST_ENT
  nci_wait_rsp_timer;         /* Timer for waiting for nci command response */
  uint16_t nci_wait_rsp_tout; /* NCI command timeout (in ms) */
  uint8_t nci_wait_rsp;       /* layer_specific for last NCI message */

  uint8_t nci_cmd_window; /* Number of commands the controller can accecpt
                             without waiting for response */

  NFC_HDR* p_nci_init_rsp; /* holding INIT_RSP until receiving
                              HAL_NFC_POST_INIT_CPLT_EVT */
  tHAL_NFC_ENTRY* p_hal;

  uint8_t nci_version; /* NCI version used for NCI communication*/

  bool isScbrSupported; /* indicating if system code based route is supported */

  uint8_t hci_packet_size; /* maximum hci payload size*/

  uint8_t hci_conn_credits; /* maximum conn credits for static HCI*/

  uint16_t nci_max_v_size; /*maximum NFC V rf frame size*/

  uint8_t rawVsCbflag;
  uint8_t deact_reason;

  TIMER_LIST_ENT nci_mode_set_ntf_timer; /*Mode set notification timer*/

} tNFC_CB;

/*****************************************************************************
**  EXTERNAL FUNCTION DECLARATIONS
*****************************************************************************/

/* Global NFC data */
extern tNFC_CB nfc_cb;

/****************************************************************************
** Internal nfc functions
****************************************************************************/

#define NCI_CALCULATE_ACK(a, v) \
  { (a) &= ((1 << (v)) - 1); }
#define MAX_NUM_VALID_BITS_FOR_ACK 0x07

/* from nfc_utils.c */
extern tNFC_CONN_CB* nfc_alloc_conn_cb(tNFC_CONN_CBACK* p_cback);
extern tNFC_CONN_CB* nfc_find_conn_cb_by_conn_id(uint8_t conn_id);
extern tNFC_CONN_CB* nfc_find_conn_cb_by_handle(uint8_t target_handle);
extern void nfc_set_conn_id(tNFC_CONN_CB* p_cb, uint8_t conn_id);
extern void nfc_free_conn_cb(tNFC_CONN_CB* p_cb);
extern void nfc_reset_all_conn_cbs(void);
extern void nfc_data_event(tNFC_CONN_CB* p_cb);

extern uint8_t nfc_ncif_send_data(tNFC_CONN_CB* p_cb, NFC_HDR* p_data);
extern void nfc_ncif_cmd_timeout(void);
extern void nfc_wait_2_deactivate_timeout(void);
extern void nfc_mode_set_ntf_timeout(void);

extern bool nfc_ncif_process_event(NFC_HDR* p_msg);
extern void nfc_ncif_check_cmd_queue(NFC_HDR* p_buf);
extern void nfc_ncif_send_cmd(NFC_HDR* p_buf);
extern void nfc_ncif_proc_discover_ntf(uint8_t* p, uint16_t plen);
extern void nfc_ncif_rf_management_status(tNFC_DISCOVER_EVT event,
                                          uint8_t status);
extern void nfc_ncif_set_config_status(uint8_t* p, uint8_t len);
extern void nfc_ncif_event_status(tNFC_RESPONSE_EVT event, uint8_t status);
extern void nfc_ncif_error_status(uint8_t conn_id, uint8_t status);
extern void nfc_ncif_proc_credits(uint8_t* p, uint16_t plen);
extern void nfc_ncif_proc_activate(uint8_t* p, uint8_t len);
extern void nfc_ncif_proc_deactivate(uint8_t status, uint8_t deact_type,
                                     bool is_ntf);
#if (NFC_NFCEE_INCLUDED == TRUE && NFC_RW_ONLY == FALSE)
extern void nfc_ncif_proc_ee_action(uint8_t* p, uint16_t plen);
extern void nfc_ncif_proc_ee_discover_req(uint8_t* p, uint16_t plen);
extern void nfc_ncif_proc_get_routing(uint8_t* p, uint8_t len);
#endif
extern void nfc_ncif_proc_conn_create_rsp(uint8_t* p, uint16_t plen,
                                          uint8_t dest_type);
extern void nfc_ncif_report_conn_close_evt(uint8_t conn_id, tNFC_STATUS status);
extern void nfc_ncif_proc_t3t_polling_ntf(uint8_t* p, uint16_t plen);
extern void nfc_ncif_proc_reset_rsp(uint8_t* p, bool is_ntf);
extern void nfc_ncif_proc_init_rsp(NFC_HDR* p_msg);
extern void nfc_ncif_proc_get_config_rsp(NFC_HDR* p_msg);
extern void nfc_ncif_proc_data(NFC_HDR* p_msg);
extern bool nfa_dm_p2p_prio_logic(uint8_t event, uint8_t* p, uint8_t ntf_rsp);
extern void nfa_dm_p2p_timer_event();
extern bool nfc_ncif_proc_proprietary_rsp(uint8_t mt, uint8_t gid, uint8_t oid);
extern void nfa_dm_p2p_prio_logic_cleanup();
extern void nfc_ncif_proc_isodep_nak_presence_check_status(uint8_t status,
                                                           bool is_ntf);
extern void nfc_ncif_update_window(void);
#if (NFC_RW_ONLY == FALSE)
extern void nfc_ncif_proc_rf_field_ntf(uint8_t rf_status);
#else
#define nfc_ncif_proc_rf_field_ntf(rf_status)
#endif

/* From nfc_task.c */
extern uint32_t nfc_task(uint32_t);
void nfc_task_shutdown_nfcc(void);

/* From nfc_main.c */
void nfc_enabled(tNFC_STATUS nfc_status, NFC_HDR* p_init_rsp_msg);
void nfc_set_state(tNFC_STATE nfc_state);
void nfc_main_flush_cmd_queue(void);
void nfc_gen_cleanup(void);
void nfc_main_handle_hal_evt(tNFC_HAL_EVT_MSG* p_msg);

/* Timer functions */
void nfc_start_timer(TIMER_LIST_ENT* p_tle, uint16_t type, uint32_t timeout);
uint32_t nfc_remaining_time(TIMER_LIST_ENT* p_tle);
void nfc_stop_timer(TIMER_LIST_ENT* p_tle);

void nfc_start_quick_timer(TIMER_LIST_ENT* p_tle, uint16_t type,
                           uint32_t timeout);
void nfc_stop_quick_timer(TIMER_LIST_ENT* p_tle);
void nfc_process_quick_timer_evt(void);
#endif /* NFC_INT_H_ */
