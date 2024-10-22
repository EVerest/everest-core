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
 *  This is the private interface file for the NFA HCI.
 *
 ******************************************************************************/
#ifndef NFA_HCI_INT_H
#define NFA_HCI_INT_H

#include <string>
#include "nfa_ee_api.h"
#include "nfa_hci_api.h"
#include "nfa_sys.h"
extern uint8_t HCI_LOOPBACK_DEBUG;

/* NFA HCI DEBUG states */
#define NFA_HCI_DEBUG_ON 0x01
#define NFA_HCI_DEBUG_OFF 0x00
/*****************************************************************************
**  Constants and data types
*****************************************************************************/

#define NFA_HCI_HOST_ID_UICC0 0x02 /* Host ID for UICC 0 */
/* First dynamically allocated host ID */
#define NFA_HCI_HOST_ID_FIRST_DYNAMICALLY_ALLOCATED 0x80
/* Lost host specific gate */
#define NFA_HCI_LAST_HOST_SPECIFIC_GATE 0xEF

#define NFA_HCI_SESSION_ID_LEN 8 /* HCI Session ID length */

/* HCI SW Version number                       */
#define NFA_HCI_VERSION_SW 0x090000
/* HCI HW Version number                       */
#define NFA_HCI_VERSION_HW 0x000000
#define NFA_HCI_VENDOR_NAME \
  "HCI" /* Vendor Name                                 */
/* Model ID                                    */
#define NFA_HCI_MODEL_ID 00
/* HCI Version                                 */
#define NFA_HCI_VERSION 90

/* NFA HCI states */
#define NFA_HCI_STATE_DISABLED 0x00 /* HCI is disabled  */
/* HCI performing Initialization sequence */
#define NFA_HCI_STATE_STARTUP 0x01
/* HCI is waiting for initialization of other host in the network */
#define NFA_HCI_STATE_WAIT_NETWK_ENABLE 0x02
/* HCI is waiting to handle api commands  */
#define NFA_HCI_STATE_IDLE 0x03
/* HCI is waiting for response to command sent */
#define NFA_HCI_STATE_WAIT_RSP 0x04
/* Removing all pipes prior to removing the gate */
#define NFA_HCI_STATE_REMOVE_GATE 0x05
/* Removing all pipes and gates prior to deregistering the app */
#define NFA_HCI_STATE_APP_DEREGISTER 0x06
#define NFA_HCI_STATE_RESTORE 0x07 /* HCI restore */
/* HCI is waiting for initialization of other host in the network after restore
 */
#define NFA_HCI_STATE_RESTORE_NETWK_ENABLE 0x08

#define NFA_HCI_STATE_EE_RECOVERY 0x09

#if(NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
#define NFA_HCI_MAX_RSP_WAIT_TIME           0x0C
#define NFA_HCI_CHAIN_PKT_RSP_TIMEOUT       30000    /* After the reception of WTX, maximum response timeout value is 30 sec */
#endif

typedef uint8_t tNFA_HCI_STATE;

/* NFA HCI PIPE states */
#define NFA_HCI_PIPE_CLOSED 0x00 /* Pipe is closed */
#define NFA_HCI_PIPE_OPENED 0x01 /* Pipe is opened */

typedef uint8_t tNFA_HCI_COMMAND;
typedef uint8_t tNFA_HCI_RESPONSE;

/* NFA HCI Internal events */
enum {
  NFA_HCI_API_REGISTER_APP_EVT =
      NFA_SYS_EVT_START(NFA_ID_HCI), /* Register APP with HCI */
  NFA_HCI_API_DEREGISTER_APP_EVT,    /* Deregister an app from HCI */
  NFA_HCI_API_GET_APP_GATE_PIPE_EVT, /* Get the list of gate and pipe associated
                                        to the application */
  NFA_HCI_API_ALLOC_GATE_EVT, /* Allocate a dyanmic gate for the application */
  NFA_HCI_API_DEALLOC_GATE_EVT, /* Deallocate a previously allocated gate to the
                                   application */
  NFA_HCI_API_GET_HOST_LIST_EVT,   /* Get the list of Host in the network */
  NFA_HCI_API_GET_REGISTRY_EVT,    /* Get a registry entry from a host */
  NFA_HCI_API_SET_REGISTRY_EVT,    /* Set a registry entry on a host */
  NFA_HCI_API_CREATE_PIPE_EVT,     /* Create a pipe between two gates */
  NFA_HCI_API_OPEN_PIPE_EVT,       /* Open a pipe */
  NFA_HCI_API_CLOSE_PIPE_EVT,      /* Close a pipe */
  NFA_HCI_API_DELETE_PIPE_EVT,     /* Delete a pipe */
  NFA_HCI_API_ADD_STATIC_PIPE_EVT, /* Add a static pipe */
  NFA_HCI_API_SEND_CMD_EVT,        /* Send command via pipe */
  NFA_HCI_API_SEND_RSP_EVT,        /* Application Response to a command */
  NFA_HCI_API_SEND_EVENT_EVT,      /* Send event via pipe */

  NFA_HCI_RSP_NV_READ_EVT,  /* Non volatile read complete event */
  NFA_HCI_RSP_NV_WRITE_EVT, /* Non volatile write complete event */
  NFA_HCI_RSP_TIMEOUT_EVT,  /* Timeout to response for the HCP Command packet */
  NFA_HCI_CHECK_QUEUE_EVT
};

#define NFA_HCI_FIRST_API_EVENT NFA_HCI_API_REGISTER_APP_EVT
#define NFA_HCI_LAST_API_EVENT NFA_HCI_API_SEND_EVENT_EVT

/* Internal event structures.
**
** Note, every internal structure starts with a NFC_HDR and an app handle
*/

/* data type for NFA_HCI_API_REGISTER_APP_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  char app_name[NFA_MAX_HCI_APP_NAME_LEN + 1];
  tNFA_HCI_CBACK* p_cback;
  bool b_send_conn_evts;
} tNFA_HCI_API_REGISTER_APP;

/* data type for NFA_HCI_API_DEREGISTER_APP_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  char app_name[NFA_MAX_HCI_APP_NAME_LEN + 1];
} tNFA_HCI_API_DEREGISTER_APP;

/* data type for NFA_HCI_API_GET_APP_GATE_PIPE_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
} tNFA_HCI_API_GET_APP_GATE_PIPE;

/* data type for NFA_HCI_API_ALLOC_GATE_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  uint8_t gate;
} tNFA_HCI_API_ALLOC_GATE;

/* data type for NFA_HCI_API_DEALLOC_GATE_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  uint8_t gate;
} tNFA_HCI_API_DEALLOC_GATE;

/* data type for NFA_HCI_API_GET_HOST_LIST_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  tNFA_STATUS status;
} tNFA_HCI_API_GET_HOST_LIST;

/* data type for NFA_HCI_API_GET_REGISTRY_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  uint8_t pipe;
  uint8_t reg_inx;
} tNFA_HCI_API_GET_REGISTRY;

/* data type for NFA_HCI_API_SET_REGISTRY_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  uint8_t pipe;
  uint8_t reg_inx;
  uint8_t size;
  uint8_t data[NFA_MAX_HCI_CMD_LEN];
} tNFA_HCI_API_SET_REGISTRY;

/* data type for NFA_HCI_API_CREATE_PIPE_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  tNFA_STATUS status;
  uint8_t source_gate;
  uint8_t dest_host;
  uint8_t dest_gate;
} tNFA_HCI_API_CREATE_PIPE_EVT;

/* data type for NFA_HCI_API_OPEN_PIPE_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  tNFA_STATUS status;
  uint8_t pipe;
} tNFA_HCI_API_OPEN_PIPE_EVT;

/* data type for NFA_HCI_API_CLOSE_PIPE_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  tNFA_STATUS status;
  uint8_t pipe;
} tNFA_HCI_API_CLOSE_PIPE_EVT;

/* data type for NFA_HCI_API_DELETE_PIPE_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  tNFA_STATUS status;
  uint8_t pipe;
} tNFA_HCI_API_DELETE_PIPE_EVT;

/* data type for NFA_HCI_API_ADD_STATIC_PIPE_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  tNFA_STATUS status;
  uint8_t host;
  uint8_t gate;
  uint8_t pipe;
} tNFA_HCI_API_ADD_STATIC_PIPE_EVT;

/* data type for NFA_HCI_API_SEND_EVENT_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  uint8_t pipe;
  uint8_t evt_code;
  uint16_t evt_len;
  uint8_t* p_evt_buf;
  uint16_t rsp_len;
  uint8_t* p_rsp_buf;
  uint16_t rsp_timeout;
} tNFA_HCI_API_SEND_EVENT_EVT;

/* data type for NFA_HCI_API_SEND_CMD_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  uint8_t pipe;
  uint8_t cmd_code;
  uint16_t cmd_len;
  uint8_t data[NFA_MAX_HCI_CMD_LEN];
} tNFA_HCI_API_SEND_CMD_EVT;

/* data type for NFA_HCI_RSP_NV_READ_EVT */
typedef struct {
  NFC_HDR hdr;
  uint8_t block;
  uint16_t size;
  tNFA_STATUS status;
} tNFA_HCI_RSP_NV_READ_EVT;

/* data type for NFA_HCI_RSP_NV_WRITE_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_STATUS status;
} tNFA_HCI_RSP_NV_WRITE_EVT;

/* data type for NFA_HCI_API_SEND_RSP_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
  uint8_t pipe;
  uint8_t response;
  uint8_t size;
  uint8_t data[NFA_MAX_HCI_RSP_LEN];
} tNFA_HCI_API_SEND_RSP_EVT;

/* common data type for internal events */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE hci_handle;
} tNFA_HCI_COMM_DATA;

/* union of all event data types */
typedef union {
  NFC_HDR hdr;
  tNFA_HCI_COMM_DATA comm;

  /* API events */
  tNFA_HCI_API_REGISTER_APP app_info; /* Register/Deregister an application */
  tNFA_HCI_API_GET_APP_GATE_PIPE get_gate_pipe_list; /* Get the list of gates
                                                        and pipes created for
                                                        the application */
  tNFA_HCI_API_ALLOC_GATE
      gate_info; /* Allocate a dynamic gate to the application */
  tNFA_HCI_API_DEALLOC_GATE
      gate_dealloc; /* Deallocate the gate allocated to the application */
  tNFA_HCI_API_CREATE_PIPE_EVT create_pipe;         /* Create a pipe */
  tNFA_HCI_API_OPEN_PIPE_EVT open_pipe;             /* Open a pipe */
  tNFA_HCI_API_CLOSE_PIPE_EVT close_pipe;           /* Close a pipe */
  tNFA_HCI_API_DELETE_PIPE_EVT delete_pipe;         /* Delete a pipe */
  tNFA_HCI_API_ADD_STATIC_PIPE_EVT add_static_pipe; /* Add a static pipe */
  tNFA_HCI_API_GET_HOST_LIST
      get_host_list; /* Get the list of Host in the network */
  tNFA_HCI_API_GET_REGISTRY get_registry; /* Get a registry entry on a host */
  tNFA_HCI_API_SET_REGISTRY set_registry; /* Set a registry entry on a host */
  tNFA_HCI_API_SEND_CMD_EVT send_cmd;     /* Send a event on a pipe to a host */
  tNFA_HCI_API_SEND_RSP_EVT
      send_rsp; /* Response to a command sent on a pipe to a host */
  tNFA_HCI_API_SEND_EVENT_EVT send_evt; /* Send a command on a pipe to a host */

  /* Internal events */
  tNFA_HCI_RSP_NV_READ_EVT nv_read;   /* Read Non volatile data */
  tNFA_HCI_RSP_NV_WRITE_EVT nv_write; /* Write Non volatile data */
} tNFA_HCI_EVENT_DATA;

/*****************************************************************************
**  control block
*****************************************************************************/

/* Dynamic pipe control block */
typedef struct {
  uint8_t pipe_id;                /* Pipe ID */
  tNFA_HCI_PIPE_STATE pipe_state; /* State of the Pipe */
  uint8_t local_gate;             /* local gate id */
  uint8_t dest_host; /* Peer host to which this pipe is connected */
  uint8_t dest_gate; /* Peer gate to which this pipe is connected */
} tNFA_HCI_DYN_PIPE;

/* Dynamic gate control block */
typedef struct {
  uint8_t gate_id;        /* local gate id */
  tNFA_HANDLE gate_owner; /* NFA-HCI handle assigned to the application which
                             owns the gate */
  uint32_t pipe_inx_mask; /* Bit 0 == pipe inx 0, etc */
} tNFA_HCI_DYN_GATE;

/* Admin gate control block */
typedef struct {
  tNFA_HCI_PIPE_STATE pipe01_state; /* State of Pipe '01' */
  uint8_t
      session_id[NFA_HCI_SESSION_ID_LEN]; /* Session ID of the host network */
} tNFA_ADMIN_GATE_INFO;

/* Link management gate control block */
typedef struct {
  tNFA_HCI_PIPE_STATE pipe00_state; /* State of Pipe '00' */
  uint16_t rec_errors;              /* Receive errors */
} tNFA_LINK_MGMT_GATE_INFO;

/* Identity management gate control block */
typedef struct {
  uint32_t pipe_inx_mask;  /* Bit 0 == pipe inx 0, etc */
  uint16_t version_sw;     /* Software version number */
  uint16_t version_hw;     /* Hardware version number */
  uint8_t vendor_name[20]; /* Vendor name */
  uint8_t model_id;        /* Model ID */
  uint8_t hci_version;     /* HCI Version */
} tNFA_ID_MGMT_GATE_INFO;

/* NFA HCI control block */
typedef struct {
  tNFA_HCI_STATE hci_state; /* state of the HCI */
  uint8_t num_nfcee;        /* Number of NFCEE ID Discovered */
  tNFA_EE_INFO ee_info[NFA_HCI_MAX_HOST_IN_NETWORK]; /*NFCEE ID Info*/
  uint8_t num_ee_dis_req_ntf; /* Number of ee discovery request ntf received */
  uint8_t num_hot_plug_evts;  /* Number of Hot plug events received after ee
                                 discovery disable ntf */
  /* Inactive host in the host network */
  uint8_t inactive_host[NFA_HCI_MAX_HOST_IN_NETWORK];
  /* active host in the host network */
  uint8_t active_host[NFA_HCI_MAX_HOST_IN_NETWORK];
  uint8_t reset_host[NFA_HCI_MAX_HOST_IN_NETWORK]; /* List of host reseting */
  bool b_low_power_mode;  /* Host controller in low power mode */
  bool b_hci_new_sessionId; /* Command sent to set a new session Id */
  bool b_hci_netwk_reset; /* Command sent to reset HCI Network */
  bool w4_hci_netwk_init; /* Wait for other host in network to initialize */
  TIMER_LIST_ENT timer;   /* Timer to avoid indefinitely waiting for response */
  uint8_t conn_id;        /* Connection ID */
  uint8_t buff_size;      /* Connection buffer size */
  bool nv_read_cmplt;     /* NV Read completed */
  bool nv_write_needed;   /* Something changed - NV write is needed */
  bool assembling;        /* Set true if in process of assembling a message  */
  bool assembly_failed;   /* Set true if Insufficient buffer to Reassemble
                             incoming message */
  bool w4_rsp_evt;        /* Application command sent on HCP Event */
  tNFA_HANDLE
      app_in_use; /* Index of the application that is waiting for response */
  uint8_t local_gate_in_use;  /* Local gate currently working with */
  uint8_t remote_gate_in_use; /* Remote gate currently working with */
  uint8_t remote_host_in_use; /* The remote host to which a command is sent */
  uint8_t pipe_in_use;        /* The pipe currently working with */
  uint8_t param_in_use;      /* The registry parameter currently working with */
  tNFA_HCI_COMMAND cmd_sent; /* The last command sent */
  bool ee_disc_cmplt;        /* EE Discovery operation completed */
  bool ee_disable_disc;      /* EE Discovery operation is disabled */
  uint16_t msg_len;     /* For segmentation - length of the combined message */
  uint16_t max_msg_len; /* Maximum reassembled message size */
  uint8_t msg_data[NFA_MAX_HCI_EVENT_LEN]; /* For segmentation - the combined
                                              message data */
  uint8_t* p_msg_data; /* For segmentation - reassembled message */
  uint8_t type;        /* Instruction type of incoming message */
  uint8_t inst;        /* Instruction of incoming message */

  BUFFER_Q hci_api_q;            /* Buffer Q to hold incoming API commands */
  BUFFER_Q hci_host_reset_api_q; /* Buffer Q to hold incoming API commands to a
                                    host that is reactivating */
  tNFA_HCI_CBACK* p_app_cback[NFA_HCI_MAX_APP_CB]; /* Callback functions
                                                      registered by the
                                                      applications */
  uint16_t rsp_buf_size; /* Maximum size of APDU buffer */
  uint8_t* p_rsp_buf;    /* Buffer to hold response to sent event */
  struct                 /* Persistent information for Device Host */
  {
    char reg_app_names[NFA_HCI_MAX_APP_CB][NFA_MAX_HCI_APP_NAME_LEN + 1];

    tNFA_HCI_DYN_GATE dyn_gates[NFA_HCI_MAX_GATE_CB];
    tNFA_HCI_DYN_PIPE dyn_pipes[NFA_HCI_MAX_PIPE_CB];

    bool b_send_conn_evts[NFA_HCI_MAX_APP_CB];
    tNFA_ADMIN_GATE_INFO admin_gate;
    tNFA_LINK_MGMT_GATE_INFO link_mgmt_gate;
    tNFA_ID_MGMT_GATE_INFO id_mgmt_gate;
  } cfg;

} tNFA_HCI_CB;

/*****************************************************************************
**  External variables
*****************************************************************************/

/* NFA HCI control block */
extern tNFA_HCI_CB nfa_hci_cb;

/*****************************************************************************
**  External functions
*****************************************************************************/

/* Functions in nfa_hci_main.c
*/
extern void nfa_hci_init(void);
extern void nfa_hci_proc_nfcc_power_mode(uint8_t nfcc_power_mode);
extern void nfa_hci_dh_startup_complete(void);
extern void nfa_hci_startup_complete(tNFA_STATUS status);
extern void nfa_hci_startup(void);
extern void nfa_hci_restore_default_config(uint8_t* p_session_id);
extern void nfa_hci_enable_one_nfcee(void);

#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
extern void nfa_hci_release_transcieve();
extern void nfa_hci_network_enable();
#endif
/* Action functions in nfa_hci_act.c
*/
extern void nfa_hci_check_pending_api_requests(void);
extern void nfa_hci_check_api_requests(void);
extern void nfa_hci_handle_admin_gate_cmd(uint8_t* p_data);
extern void nfa_hci_handle_admin_gate_rsp(uint8_t* p_data, uint8_t data_len);
extern void nfa_hci_handle_admin_gate_evt();
extern void nfa_hci_handle_link_mgm_gate_cmd(uint8_t* p_data);
extern void nfa_hci_handle_dyn_pipe_pkt(uint8_t pipe, uint8_t* p_data,
                                        uint16_t data_len);
extern void nfa_hci_handle_pipe_open_close_cmd(tNFA_HCI_DYN_PIPE* p_pipe);
extern void nfa_hci_api_dealloc_gate(tNFA_HCI_EVENT_DATA* p_evt_data);
extern void nfa_hci_api_deregister(tNFA_HCI_EVENT_DATA* p_evt_data);

/* Utility functions in nfa_hci_utils.c
*/
extern tNFA_HCI_DYN_GATE* nfa_hciu_alloc_gate(uint8_t gate_id,
                                              tNFA_HANDLE app_handle);
extern tNFA_HCI_DYN_GATE* nfa_hciu_find_gate_by_gid(uint8_t gate_id);
extern tNFA_HCI_DYN_GATE* nfa_hciu_find_gate_by_owner(tNFA_HANDLE app_handle);
extern tNFA_HCI_DYN_GATE* nfa_hciu_find_gate_with_nopipes_by_owner(
    tNFA_HANDLE app_handle);
extern tNFA_HCI_DYN_PIPE* nfa_hciu_find_pipe_by_pid(uint8_t pipe_id);
extern tNFA_HCI_DYN_PIPE* nfa_hciu_find_pipe_by_owner(tNFA_HANDLE app_handle);
extern tNFA_HCI_DYN_PIPE* nfa_hciu_find_active_pipe_by_owner(
    tNFA_HANDLE app_handle);
extern tNFA_HCI_DYN_PIPE* nfa_hciu_find_pipe_on_gate(uint8_t gate_id);
extern tNFA_HANDLE nfa_hciu_get_gate_owner(uint8_t gate_id);
extern bool nfa_hciu_check_pipe_between_gates(uint8_t local_gate,
                                              uint8_t dest_host,
                                              uint8_t dest_gate);
extern bool nfa_hciu_is_active_host(uint8_t host_id);
extern bool nfa_hciu_is_host_reseting(uint8_t host_id);
extern bool nfa_hciu_is_no_host_resetting(void);
extern tNFA_HCI_DYN_PIPE* nfa_hciu_find_active_pipe_on_gate(uint8_t gate_id);
extern tNFA_HANDLE nfa_hciu_get_pipe_owner(uint8_t pipe_id);
extern uint8_t nfa_hciu_count_open_pipes_on_gate(tNFA_HCI_DYN_GATE* p_gate);
extern uint8_t nfa_hciu_count_pipes_on_gate(tNFA_HCI_DYN_GATE* p_gate);

extern tNFA_HCI_RESPONSE nfa_hciu_add_pipe_to_gate(uint8_t pipe,
                                                   uint8_t local_gate,
                                                   uint8_t dest_host,
                                                   uint8_t dest_gate);
extern tNFA_HCI_RESPONSE nfa_hciu_add_pipe_to_static_gate(uint8_t local_gate,
                                                          uint8_t pipe_id,
                                                          uint8_t dest_host,
                                                          uint8_t dest_gate);

extern tNFA_HCI_RESPONSE nfa_hciu_release_pipe(uint8_t pipe_id);
extern void nfa_hciu_release_gate(uint8_t gate);
extern void nfa_hciu_remove_all_pipes_from_host(uint8_t host);
extern uint8_t nfa_hciu_get_allocated_gate_list(uint8_t* p_gate_list);

extern void nfa_hciu_send_to_app(tNFA_HCI_EVT event, tNFA_HCI_EVT_DATA* p_evt,
                                 tNFA_HANDLE app_handle);
extern void nfa_hciu_send_to_all_apps(tNFA_HCI_EVT event,
                                      tNFA_HCI_EVT_DATA* p_evt);
extern void nfa_hciu_send_to_apps_handling_connectivity_evts(
    tNFA_HCI_EVT event, tNFA_HCI_EVT_DATA* p_evt);

extern tNFA_STATUS nfa_hciu_send_close_pipe_cmd(uint8_t pipe);
extern tNFA_STATUS nfa_hciu_send_delete_pipe_cmd(uint8_t pipe);
extern tNFA_STATUS nfa_hciu_send_clear_all_pipe_cmd(void);
extern tNFA_STATUS nfa_hciu_send_open_pipe_cmd(uint8_t pipe);
extern tNFA_STATUS nfa_hciu_send_get_param_cmd(uint8_t pipe, uint8_t index);
extern tNFA_STATUS nfa_hciu_send_create_pipe_cmd(uint8_t source_gate,
                                                 uint8_t dest_host,
                                                 uint8_t dest_gate);
extern tNFA_STATUS nfa_hciu_send_set_param_cmd(uint8_t pipe, uint8_t index,
                                               uint8_t length, uint8_t* p_data);
extern tNFA_STATUS nfa_hciu_send_msg(uint8_t pipe_id, uint8_t type,
                                     uint8_t instruction, uint16_t pkt_len,
                                     uint8_t* p_pkt);

extern std::string nfa_hciu_instr_2_str(uint8_t type);
extern std::string nfa_hciu_get_event_name(uint16_t event);
extern std::string nfa_hciu_get_state_name(uint8_t state);
extern char* nfa_hciu_get_type_inst_names(uint8_t pipe, uint8_t type,
                                          uint8_t inst, char* p_buff,
                                          const uint8_t max_buff_size);
extern std::string nfa_hciu_evt_2_str(uint8_t pipe_id, uint8_t evt);

#endif /* NFA_HCI_INT_H */
