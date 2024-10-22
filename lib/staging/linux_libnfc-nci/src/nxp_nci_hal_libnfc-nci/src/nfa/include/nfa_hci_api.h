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
 *  This is the public interface file for NFA HCI, Broadcom's NFC
 *  application layer for mobile phones.
 *
 ******************************************************************************/
#ifndef NFA_HCI_API_H
#define NFA_HCI_API_H

#include "nfa_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* NFA HCI Debug constants */
#define NFA_HCI_DEBUG_DISPLAY_CB 0
#define NFA_HCI_DEBUG_SIM_HCI_EVENT 1
#define NFA_HCI_DEBUG_ENABLE_LOOPBACK 101
#define NFA_HCI_DEBUG_DISABLE_LOOPBACK 102

/* NFA HCI callback events */
#define NFA_HCI_REGISTER_EVT \
  0x00 /* Application registered                       */
/* Application deregistered                     */
#define NFA_HCI_DEREGISTER_EVT 0x01
/* Retrieved gates,pipes assoc. to application  */
#define NFA_HCI_GET_GATE_PIPE_LIST_EVT 0x02
#define NFA_HCI_ALLOCATE_GATE_EVT \
  0x03 /* A generic gate allocated to the application  */
#define NFA_HCI_DEALLOCATE_GATE_EVT \
  0x04 /* A generic gate is released                   */
#define NFA_HCI_CREATE_PIPE_EVT \
  0x05 /* Pipe is created                              */
#define NFA_HCI_OPEN_PIPE_EVT \
  0x06 /* Pipe is opened / could not open              */
#define NFA_HCI_CLOSE_PIPE_EVT \
  0x07 /* Pipe is closed / could not close             */
#define NFA_HCI_DELETE_PIPE_EVT \
  0x08 /* Pipe is deleted                              */
#define NFA_HCI_HOST_LIST_EVT \
  0x09 /* Received list of Host from Host controller   */
/* HCI subsytem initialized                     */
#define NFA_HCI_INIT_EVT 0x0A
/* HCI subsytem exited                          */
#define NFA_HCI_EXIT_EVT 0x0B
/* Response recvd to cmd sent on app owned pipe */
#define NFA_HCI_RSP_RCVD_EVT 0x0C
/* Response sent on app owned pipe              */
#define NFA_HCI_RSP_SENT_EVT 0x0D
/* Command sent on app owned pipe               */
#define NFA_HCI_CMD_SENT_EVT 0x0E
/* Event sent on app owned pipe                 */
#define NFA_HCI_EVENT_SENT_EVT 0x0F
/* Command received on app owned pipe           */
#define NFA_HCI_CMD_RCVD_EVT 0x10
/* Event received on app owned pipe             */
#define NFA_HCI_EVENT_RCVD_EVT 0x11
/* Registry read command sent                   */
#define NFA_HCI_GET_REG_CMD_EVT 0x12
/* Registry write command sent                  */
#define NFA_HCI_SET_REG_CMD_EVT 0x13
/* Received response to read registry command   */
#define NFA_HCI_GET_REG_RSP_EVT 0x14
/* Received response to write registry command  */
#define NFA_HCI_SET_REG_RSP_EVT 0x15
/* A static pipe is added                       */
#define NFA_HCI_ADD_STATIC_PIPE_EVT 0x16

typedef uint8_t tNFA_HCI_EVT;

/* Max application name length */
#define NFA_MAX_HCI_APP_NAME_LEN 0x10
/* Max HCI command length */
#define NFA_MAX_HCI_CMD_LEN 255
/* Max HCI event length */
#define NFA_MAX_HCI_RSP_LEN 255
/* Max HCI event length */
#define NFA_MAX_HCI_EVENT_LEN 300
/* Max HCI data length */
#define NFA_MAX_HCI_DATA_LEN 260

/* NFA HCI PIPE states */
#define NFA_HCI_PIPE_CLOSED 0x00 /* Pipe is closed */
#define NFA_HCI_PIPE_OPENED 0x01 /* Pipe is opened */

typedef uint8_t tNFA_HCI_PIPE_STATE;
/* Dynamic pipe control block */
typedef struct {
  uint8_t pipe_id;                /* Pipe ID */
  tNFA_HCI_PIPE_STATE pipe_state; /* State of the Pipe */
  uint8_t local_gate;             /* local gate id */
  uint8_t dest_host; /* Peer host to which this pipe is connected */
  uint8_t dest_gate; /* Peer gate to which this pipe is connected */
} tNFA_HCI_PIPE_INFO;

/* Data for NFA_HCI_REGISTER_EVT */
typedef struct {
  tNFA_STATUS status;     /* Status of registration */
  tNFA_HANDLE hci_handle; /* Handle assigned to the application */
  uint8_t num_pipes; /* Number of dynamic pipes exist for the application */
  uint8_t num_gates; /* Number of generic gates exist for the application */
} tNFA_HCI_REGISTER;

/* Data for NFA_HCI_DEREGISTER_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of deregistration */
} tNFA_HCI_DEREGISTER;

/* Data for NFA_HCI_GET_GATE_PIPE_LIST_EVT */
typedef struct {
  tNFA_STATUS status;
  uint8_t num_pipes; /* Number of dynamic pipes exist for the application */
  tNFA_HCI_PIPE_INFO
      pipe[NFA_HCI_MAX_PIPE_CB]; /* List of pipe created for the application */
  uint8_t num_gates; /* Number of generic gates exist for the application */
  uint8_t gate[NFA_HCI_MAX_GATE_CB]; /* List of generic gates allocated to the
                                        application */
  uint8_t num_uicc_created_pipes;    /* Number of pipes created by UICC host */
  tNFA_HCI_PIPE_INFO uicc_created_pipe
      [NFA_HCI_MAX_PIPE_CB]; /* Pipe information of the UICC created pipe */
} tNFA_HCI_GET_GATE_PIPE_LIST;

/* Data for NFA_HCI_ALLOCATE_GATE_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of response to allocate gate request */
  uint8_t gate;       /* The gate allocated to the application */
} tNFA_HCI_ALLOCATE_GATE;

/* Data for NFA_HCI_DEALLOCATE_GATE_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of response to deallocate gate request */
  uint8_t gate;       /* The gate deallocated from the application */
} tNFA_HCI_DEALLOCATE_GATE;

/* Data for NFA_HCI_CREATE_PIPE_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of creating dynamic pipe for the application */
  uint8_t pipe;       /* The pipe created for the application */
  uint8_t
      source_gate; /* DH host gate to which the one end of pipe is attached */
  uint8_t
      dest_host; /* Destination host whose gate is the other end of the pipe is
                    attached to */
  uint8_t dest_gate; /* Destination host gate to which the other end of pipe is
                        attached */
} tNFA_HCI_CREATE_PIPE;

/* Data for NFA_HCI_OPEN_PIPE_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of open pipe operation */
  uint8_t pipe;       /* The dynamic pipe for open operation */
} tNFA_HCI_OPEN_PIPE;

/* Data for NFA_HCI_CLOSE_PIPE_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of close pipe operation */
  uint8_t pipe;       /* The dynamic pipe for close operation */
} tNFA_HCI_CLOSE_PIPE;

/* Data for NFA_HCI_DELETE_PIPE_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of delete pipe operation */
  uint8_t pipe;       /* The dynamic pipe for delete operation */
} tNFA_HCI_DELETE_PIPE;

/* Data for NFA_HCI_HOST_LIST_EVT */
typedef struct {
  tNFA_STATUS status; /* Status og get host list operation */
  uint8_t num_hosts;  /* Number of hosts in the host network */
  uint8_t
      host[NFA_HCI_MAX_HOST_IN_NETWORK]; /* List of host in the host network */
} tNFA_HCI_HOST_LIST;

/* Data for NFA_HCI_RSP_RCVD_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of RSP to HCP CMD sent */
  uint8_t pipe;       /* The pipe on which HCP packet is exchanged */
  uint8_t rsp_code;   /* Response id */
  uint16_t rsp_len;   /* Response parameter length */
  uint8_t rsp_data[NFA_MAX_HCI_RSP_LEN]; /* Response received */
} tNFA_HCI_RSP_RCVD;

/* Data for NFA_HCI_EVENT_RCVD_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of Event received */
  uint8_t pipe;       /* The pipe on which HCP EVT packet is received */
  uint8_t evt_code;   /* HCP EVT id */
  uint16_t evt_len;   /* HCP EVT parameter length */
  uint8_t* p_evt_buf; /* HCP EVT Parameter */
} tNFA_HCI_EVENT_RCVD;

/* Data for NFA_HCI_CMD_RCVD_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of Command received */
  uint8_t pipe;       /* The pipe on which HCP CMD packet is received */
  uint8_t cmd_code;   /* HCP CMD id */
  uint16_t cmd_len;   /* HCP CMD parameter length */
  uint8_t cmd_data[NFA_MAX_HCI_CMD_LEN]; /* HCP CMD Parameter */
} tNFA_HCI_CMD_RCVD;

/* Data for NFA_HCI_INIT_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of Enabling HCI Network */
} tNFA_HCI_INIT;

/* Data for NFA_HCI_EXIT_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of Disabling HCI Network */
} tNFA_HCI_EXIT;

/* Data for NFA_HCI_RSP_SENT_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of HCP response send operation */
} tNFA_HCI_RSP_SENT;

/* Data for NFA_HCI_CMD_SENT_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of Command send operation */
} tNFA_HCI_CMD_SENT;

/* Data for NFA_HCI_EVENT_SENT_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of Event send operation */
} tNFA_HCI_EVENT_SENT;

/* Data for NFA_HCI_ADD_STATIC_PIPE_EVT */
typedef struct {
  tNFA_STATUS status; /* Status of adding proprietary pipe */
} tNFA_HCI_ADD_STATIC_PIPE_EVT;

/* data type for all registry-related events */
typedef struct {
  tNFA_STATUS status; /* Status of Registry operation */
  uint8_t pipe;       /* Pipe on whose registry is of interest */
  uint8_t index;      /* Index of the registry operated */
  uint8_t data_len;   /* length of the registry parameter */
  uint8_t reg_data[NFA_MAX_HCI_DATA_LEN]; /* Registry parameter */
} tNFA_HCI_REGISTRY;

/* Union of all hci callback structures */
typedef union {
  tNFA_HCI_REGISTER hci_register;          /* NFA_HCI_REGISTER_EVT           */
  tNFA_HCI_DEREGISTER hci_deregister;      /* NFA_HCI_DEREGISTER_EVT         */
  tNFA_HCI_GET_GATE_PIPE_LIST gates_pipes; /* NFA_HCI_GET_GATE_PIPE_LIST_EVT */
  tNFA_HCI_ALLOCATE_GATE allocated;        /* NFA_HCI_ALLOCATE_GATE_EVT      */
  tNFA_HCI_DEALLOCATE_GATE deallocated;    /* NFA_HCI_DEALLOCATE_GATE_EVT    */
  tNFA_HCI_CREATE_PIPE created;            /* NFA_HCI_CREATE_PIPE_EVT        */
  tNFA_HCI_OPEN_PIPE opened;               /* NFA_HCI_OPEN_PIPE_EVT          */
  tNFA_HCI_CLOSE_PIPE closed;              /* NFA_HCI_CLOSE_PIPE_EVT         */
  tNFA_HCI_DELETE_PIPE deleted;            /* NFA_HCI_DELETE_PIPE_EVT        */
  tNFA_HCI_HOST_LIST hosts;                /* NFA_HCI_HOST_LIST_EVT          */
  tNFA_HCI_RSP_RCVD rsp_rcvd;              /* NFA_HCI_RSP_RCVD_EVT           */
  tNFA_HCI_RSP_SENT rsp_sent;              /* NFA_HCI_RSP_SENT_EVT           */
  tNFA_HCI_CMD_SENT cmd_sent;              /* NFA_HCI_CMD_SENT_EVT           */
  tNFA_HCI_EVENT_SENT evt_sent;            /* NFA_HCI_EVENT_SENT_EVT         */
  tNFA_HCI_CMD_RCVD cmd_rcvd;              /* NFA_HCI_CMD_RCVD_EVT           */
  tNFA_HCI_EVENT_RCVD rcvd_evt;            /* NFA_HCI_EVENT_RCVD_EVT         */
  tNFA_STATUS status;                      /* status of api command request  */
  tNFA_HCI_REGISTRY registry;              /* all registry-related events -
                                              NFA_HCI_GET_REG_CMD_EVT,
                                              NFA_HCI_SET_REG_CMD_EVT,
                                              NFA_HCI_GET_REG_RSP_EVT,
                                              NFA_HCI_SET_REG_RSP_EVT */
  tNFA_HCI_INIT hci_init;                  /* NFA_HCI_INIT_EVT               */
  tNFA_HCI_EXIT hci_exit;                  /* NFA_HCI_EXIT_EVT               */
  tNFA_HCI_ADD_STATIC_PIPE_EVT pipe_added; /* NFA_HCI_ADD_STATIC_PIPE_EVT    */
} tNFA_HCI_EVT_DATA;

#if(NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
typedef enum
{
    Wait = 0,
    Release
}tNFA_HCI_TRANSCV_STATE;
#endif

/* NFA HCI callback */
typedef void(tNFA_HCI_CBACK)(tNFA_HCI_EVT event, tNFA_HCI_EVT_DATA* p_data);

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/

/*******************************************************************************
**
** Function         NFA_HciRegister
**
** Description      This function will register an application with hci and
**                  returns an application handle and provides a mechanism to
**                  register a callback with HCI to receive NFA HCI event
**                  notification. When the application is registered (or if an
**                  error occurs), the app will be notified with
**                  NFA_HCI_REGISTER_EVT. Previous session information
**                  including allocated gates, created pipes and pipes states
**                  will be returned as part of tNFA_HCI_REGISTER data.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_HciRegister(char* p_app_name, tNFA_HCI_CBACK* p_cback,
                                   bool b_send_conn_evts);

/*******************************************************************************
**
** Function         NFA_HciGetGateAndPipeList
**
** Description      This function will retrieve the list of gates allocated to
**                  the application and list of dynamic pipes created for the
**                  application. The app will be notified with
**                  NFA_HCI_GET_GATE_PIPE_LIST_EVT. List of allocated dynamic
**                  gates to the application and list of pipes created by the
**                  application will be returned as part of
**                  tNFA_HCI_GET_GATE_PIPE_LIST data.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_HciGetGateAndPipeList(tNFA_HANDLE hci_handle);

/*******************************************************************************
**
** Function         NFA_HciDeregister
**
** Description      This function is called to deregister an application
**                  from HCI. The app will be notified by NFA_HCI_DEREGISTER_EVT
**                  after deleting all the pipes owned by the app and
**                  deallocating all the gates allocated to the app or if an
**                  error occurs. The app can release the buffer provided for
**                  collecting long APDUs after receiving
**                  NFA_HCI_DEREGISTER_EVT. Even if deregistration fails, the
**                  app has to register again to provide a new cback function
**                  and event buffer for receiving long APDUs.
**
** Returns          NFA_STATUS_OK if the application is deregistered
**                  successfully
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_HciDeregister(char* p_app_name);

/*******************************************************************************
**
** Function         NFA_HciAllocGate
**
** Description      This function will allocate the gate if any specified or an
**                  available generic gate for the app to provide an entry point
**                  for a particular service to other host or to establish
**                  communication with other host. When the gate is
**                  allocated (or if an error occurs), the app will be notified
**                  with NFA_HCI_ALLOCATE_GATE_EVT with the gate id. The
**                  allocated Gate information will be stored in non volatile
**                  memory.
**
** Returns          NFA_STATUS_OK if this API started
**                  NFA_STATUS_FAILED if no generic gate is available
**
*******************************************************************************/
extern tNFA_STATUS NFA_HciAllocGate(tNFA_HANDLE hci_handle, uint8_t gate);

/*******************************************************************************
**
** Function         NFA_HciDeallocGate
**
** Description      This function will release the specified gate that was
**                  previously allocated to the application. When the generic
**                  gate is released (or if an error occurs), the app will be
**                  notified with NFA_HCI_DEALLOCATE_GATE_EVT with the gate id.
**                  The allocated Gate information will be deleted from non
**                  volatile memory and all the associated pipes are deleted
**                  by informing host controller.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_BAD_HANDLE if handle is not valid
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_HciDeallocGate(tNFA_HANDLE conn_handle, uint8_t gate);

/*******************************************************************************
**
** Function         NFA_HciGetHostList
**
** Description      This function will request the host controller to return the
**                  list of hosts that are present in the host network. When
**                  host controller responds with the host list (or if an error
**                  occurs), the app will be notified with NFA_HCI_HOST_LIST_EVT
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_BAD_HANDLE if handle is not valid
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_HciGetHostList(tNFA_HANDLE hci_handle);

/*******************************************************************************
**
** Function         NFA_HciCreatePipe
**
** Description      This function is called to create a dynamic pipe with the
**                  specified host. When the dynamic pipe is created (or
**                  if an error occurs), the app will be notified with
**                  NFA_HCI_CREATE_PIPE_EVT with the pipe id. If a pipe exists
**                  between the two gates passed as argument and if it was
**                  created earlier by the calling application then the pipe
**                  id of the existing pipe will be returned and a new pipe
**                  will not be created. After successful creation of pipe,
**                  registry entry will be created for the dynamic pipe and
**                  all information related to the pipe will be stored in non
**                  volatile memory.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_HciCreatePipe(tNFA_HANDLE hci_handle,
                                     uint8_t source_gate_id, uint8_t dest_host,
                                     uint8_t dest_gate);

/*******************************************************************************
**
** Function         NFA_HciOpenPipe
**
** Description      This function is called to open a dynamic pipe.
**                  When the dynamic pipe is opened (or
**                  if an error occurs), the app will be notified with
**                  NFA_HCI_OPEN_PIPE_EVT with the pipe id.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_HciOpenPipe(tNFA_HANDLE hci_handle, uint8_t pipe);

/*******************************************************************************
**
** Function         NFA_HciGetRegistry
**
** Description      This function requests a peer host to return the desired
**                  registry field value for the gate that the pipe is on.
**
**                  When the peer host responds,the app is notified with
**                  NFA_HCI_GET_REG_RSP_EVT or
**                  if an error occurs in sending the command the app will be
**                  notified by NFA_HCI_CMD_SENT_EVT
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_HciGetRegistry(tNFA_HANDLE hci_handle, uint8_t pipe,
                                      uint8_t reg_inx);

/*******************************************************************************
**
** Function         NFA_HciSendCommand
**
** Description      This function is called to send a command on a pipe created
**                  by the application.
**                  The app will be notified by NFA_HCI_CMD_SENT_EVT if an error
**                  occurs.
**                  When the peer host responds,the app is notified with
**                  NFA_HCI_RSP_RCVD_EVT
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_HciSendCommand(tNFA_HANDLE hci_handle, uint8_t pipe,
                                      uint8_t cmd_code, uint16_t cmd_size,
                                      uint8_t* p_data);

/*******************************************************************************
**
** Function         NFA_HciSendEvent
**
** Description      This function is called to send any event on a pipe created
**                  by the application.
**                  The app will be notified by NFA_HCI_EVENT_SENT_EVT
**                  after successfully sending the event on the specified pipe
**                  or if an error occurs. The application should wait for this
**                  event before releasing event buffer passed as argument.
**                  If the app is expecting a response to the event then it can
**                  provide response buffer for collecting the response. If it
**                  provides a response buffer it should also provide response
**                  timeout indicating duration validity of the response buffer.
**                  Maximum of NFA_MAX_HCI_EVENT_LEN bytes APDU can be received
**                  using internal buffer if no response buffer is provided by
**                  the application. The app will be notified by
**                  NFA_HCI_EVENT_RCVD_EVT after receiving the response event
**                  or on timeout if app provided response buffer.
**                  If response buffer is provided by the application, it should
**                  wait for this event before releasing the response buffer.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_HciSendEvent(tNFA_HANDLE hci_handle, uint8_t pipe,
                             uint8_t evt_code, uint16_t evt_size,
                             uint8_t* p_data, uint16_t rsp_size,
                             uint8_t* p_rsp_buf, uint16_t rsp_timeout);

/*******************************************************************************
**
** Function         NFA_HciClosePipe
**
** Description      This function is called to close a dynamic pipe.
**                  When the dynamic pipe is closed (or
**                  if an error occurs), the app will be notified with
**                  NFA_HCI_CLOSE_PIPE_EVT with the pipe id.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_HciClosePipe(tNFA_HANDLE hci_handle, uint8_t pipe);

/*******************************************************************************
**
** Function         NFA_HciDeletePipe
**
** Description      This function is called to delete a particular dynamic pipe.
**                  When the dynamic pipe is deleted (or if an error occurs),
**                  the app will be notified with NFA_HCI_DELETE_PIPE_EVT with
**                  the pipe id. After successful deletion of pipe, registry
**                  entry will be deleted for the dynamic pipe and all
**                  information related to the pipe will be deleted from non
**                  volatile memory.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_BAD_HANDLE if handle is not valid
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_HciDeletePipe(tNFA_HANDLE hci_handle, uint8_t pipe);

/*******************************************************************************
**
** Function         NFA_HciAddStaticPipe
**
** Description      This function is called to add a static pipe for sending
**                  7816 APDUs. When the static pipe is added (or if an error
**                  occurs), the app will be notified with
**                  NFA_HCI_ADD_STATIC_PIPE_EVT with status.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
extern tNFA_STATUS NFA_HciAddStaticPipe(tNFA_HANDLE hci_handle, uint8_t host,
                                        uint8_t gate, uint8_t pipe);

/*******************************************************************************
**
** Function         NFA_HciDebug
**
** Description      Debug function.
**
*******************************************************************************/
extern void NFA_HciDebug(uint8_t action, uint8_t size, uint8_t* p_data);
#if(NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         NFA_HciW4eSETransaction_Complete
**
** Description      This function is called to wait for eSE transaction
**                  to complete before NFCC shutdown or NFC service turn OFF
**
** Returns          None
**
*******************************************************************************/
NFC_API extern void NFA_HciW4eSETransaction_Complete(tNFA_HCI_TRANSCV_STATE type);
#endif
#endif /* NFA_P2P_API_H */
