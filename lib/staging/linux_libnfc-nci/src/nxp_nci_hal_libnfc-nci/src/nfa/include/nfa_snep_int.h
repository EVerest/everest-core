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
 *  This is the private interface file for the NFA SNEP.
 *
 ******************************************************************************/
#ifndef NFA_SNEP_INT_H
#define NFA_SNEP_INT_H

#if (NFA_SNEP_INCLUDED == TRUE)
#include "llcp_api.h"
#include "nfa_snep_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/
/* SNEP Acceptable Length size */

#define NFA_SNEP_DEFAULT_SERVER_SAP     0x04    /* SNEP default server SAP   */
#define NFA_SNEP_HEADER_SIZE            6       /* SNEP header size          */
#define NFA_SNEP_ACCEPT_LEN_SIZE        4       /* SNEP Acceptable Length size */
#define NFA_SNEP_CLIENT_TIMEOUT         1000    /* ms, waiting for response  */
/* NFA SNEP events */
enum {
  NFA_SNEP_API_START_DEFAULT_SERVER_EVT = NFA_SYS_EVT_START(NFA_ID_SNEP),
  NFA_SNEP_API_STOP_DEFAULT_SERVER_EVT,
  NFA_SNEP_API_REG_SERVER_EVT,
  NFA_SNEP_API_REG_CLIENT_EVT,
  NFA_SNEP_API_DEREG_EVT,
  NFA_SNEP_API_CONNECT_EVT,
  NFA_SNEP_API_GET_REQ_EVT,
  NFA_SNEP_API_PUT_REQ_EVT,
  NFA_SNEP_API_GET_RESP_EVT,
  NFA_SNEP_API_PUT_RESP_EVT,
  NFA_SNEP_API_DISCONNECT_EVT,


    NFA_SNEP_LAST_EVT
};

/* data type for NFA_SNEP_API_START_DEFAULT_SERVER_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_SNEP_CBACK* p_cback;
} tNFA_SNEP_API_START_DEFAULT_SERVER;
/* data type for NFA_SNEP_API_STOP_DEFAULT_SERVER_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_SNEP_CBACK* p_cback;
} tNFA_SNEP_API_STOP_DEFAULT_SERVER;

/* data type for NFA_SNEP_API_REG_SERVER_EVT */
typedef struct {
  NFC_HDR hdr;
  uint8_t server_sap;
  char service_name[LLCP_MAX_SN_LEN + 1];
  tNFA_SNEP_CBACK* p_cback;
} tNFA_SNEP_API_REG_SERVER;

/* data type for NFA_SNEP_API_REG_CLIENT_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_SNEP_CBACK* p_cback;
} tNFA_SNEP_API_REG_CLIENT;

/* data type for NFA_SNEP_API_DEREG_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE reg_handle; /* handle for registered server/client */
} tNFA_SNEP_API_DEREG;

/* data type for NFA_SNEP_API_CONNECT_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE client_handle; /* handle for client                   */
  char service_name[LLCP_MAX_SN_LEN + 1];
} tNFA_SNEP_API_CONNECT;

/* data type for NFA_SNEP_API_GET_REQ_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE conn_handle; /* handle for data link connection      */
  uint32_t buff_length;    /* length of buffer; acceptable length  */
  uint32_t ndef_length;    /* length of current NDEF message       */
  uint8_t* p_ndef_buff;    /* buffer for NDEF message              */
} tNFA_SNEP_API_GET_REQ;

/* data type for NFA_SNEP_API_PUT_REQ_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE conn_handle; /* handle for data link connection */
  uint32_t ndef_length;    /* length of NDEF message          */
  uint8_t* p_ndef_buff;    /* buffer for NDEF message         */
} tNFA_SNEP_API_PUT_REQ;

/* data type for NFA_SNEP_API_GET_RESP_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE conn_handle;       /* handle for data link connection */
  tNFA_SNEP_RESP_CODE resp_code; /* response code                   */
  uint32_t ndef_length;          /* length of NDEF message          */
  uint8_t* p_ndef_buff;          /* buffer for NDEF message         */
} tNFA_SNEP_API_GET_RESP;

/* data type for NFA_SNEP_API_PUT_RESP_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE conn_handle;       /* handle for data link connection */
  tNFA_SNEP_RESP_CODE resp_code; /* response code                   */
} tNFA_SNEP_API_PUT_RESP;

/* data type for NFA_SNEP_API_DISCONNECT_EVT */
typedef struct {
  NFC_HDR hdr;
  tNFA_HANDLE conn_handle; /* response code                   */
  bool flush;              /* TRUE if discard pending data    */
} tNFA_SNEP_API_DISCONNECT;

/* union of all event data types */
typedef union {
  NFC_HDR hdr;
  tNFA_SNEP_API_START_DEFAULT_SERVER
      api_start_default_server; /* NFA_SNEP_API_START_DEFAULT_SERVER_EVT */
  tNFA_SNEP_API_STOP_DEFAULT_SERVER
      api_stop_default_server; /* NFA_SNEP_API_STOP_DEFAULT_SERVER_EVT  */
  tNFA_SNEP_API_REG_SERVER api_reg_server; /* NFA_SNEP_API_REG_SERVER_EVT   */
  tNFA_SNEP_API_REG_CLIENT api_reg_client; /* NFA_SNEP_API_REG_CLIENT_EVT   */
  tNFA_SNEP_API_DEREG api_dereg;           /* NFA_SNEP_API_DEREG_EVT        */
  tNFA_SNEP_API_CONNECT api_connect;       /* NFA_SNEP_API_CONNECT_EVT      */
  tNFA_SNEP_API_GET_REQ api_get_req;       /* NFA_SNEP_API_GET_REQ_EVT      */
  tNFA_SNEP_API_PUT_REQ api_put_req;       /* NFA_SNEP_API_PUT_REQ_EVT      */
  tNFA_SNEP_API_GET_RESP api_get_resp;     /* NFA_SNEP_API_GET_RESP_EVT     */
  tNFA_SNEP_API_PUT_RESP api_put_resp;     /* NFA_SNEP_API_PUT_RESP_EVT     */
  tNFA_SNEP_API_DISCONNECT api_disc;       /* NFA_SNEP_API_DISCONNECT_EVT   */
} tNFA_SNEP_MSG;

/* NFA SNEP service control block */
#define NFA_SNEP_FLAG_ANY               0x00   /* ignore flags while searching   */
#define NFA_SNEP_FLAG_SERVER            0x01   /* server */
#define NFA_SNEP_FLAG_CLIENT            0x02   /* client */
#define NFA_SNEP_FLAG_CONNECTING        0x04   /* waiting for connection confirm */
#define NFA_SNEP_FLAG_CONNECTED         0x08   /* data link connected            */
#define NFA_SNEP_FLAG_W4_RESP_CONTINUE  0x10   /* Waiting for continue response  */
#define NFA_SNEP_FLAG_W4_REQ_CONTINUE   0x20   /* Waiting for continue request   */

/*****************************************************************************
**  control block
*****************************************************************************/

/* NFA SNEP service control block */
/* ignore flags while searching   */
/* waiting for connection confirm */
/* data link connected            */
/* Waiting for continue response  */
/* Waiting for continue request   */

typedef struct {
  uint8_t local_sap;        /* local SAP of service */
  uint8_t remote_sap;       /* local SAP of service */
  uint8_t flags;            /* internal flags       */
  tNFA_SNEP_CBACK* p_cback; /* callback for event   */
  TIMER_LIST_ENT timer;     /* timer for client     */

  uint16_t tx_miu;   /* adjusted MIU for throughput              */
  bool congest;      /* TRUE if data link connection is congested */
  bool rx_fragments; /* TRUE if waiting more fragments            */

  uint8_t tx_code; /* transmitted code in request/response */
  uint8_t rx_code; /* received code in request/response    */

  uint32_t acceptable_length;
  uint32_t buff_length; /* size of buffer for NDEF message   */
  uint32_t ndef_length; /* length of NDEF message            */
  uint32_t cur_length;  /* currently sent or received length */
  uint8_t* p_ndef_buff; /* NDEF message buffer               */
} tNFA_SNEP_CONN;

/*
** NFA SNEP control block
*/
typedef struct {
  tNFA_SNEP_CONN conn[NFA_SNEP_MAX_CONN];
  bool listen_enabled;
  bool is_dta_mode;
  uint8_t trace_level;
} tNFA_SNEP_CB;

/*
** NFA SNEP default server control block
*/

/* multiple data link connections for default server */
typedef struct {
  tNFA_HANDLE conn_handle; /* connection handle for default server   */
  uint8_t* p_rx_ndef;      /* buffer to receive NDEF                 */
} tNFA_SNEP_DEFAULT_CONN;

#define NFA_SNEP_DEFAULT_MAX_CONN 3

typedef struct {
  tNFA_HANDLE server_handle; /* registered handle for default server   */
  tNFA_SNEP_DEFAULT_CONN
      conn[NFA_SNEP_DEFAULT_MAX_CONN]; /* connections for default server */

} tNFA_SNEP_DEFAULT_CB;

/*****************************************************************************
**  External variables
*****************************************************************************/

/* NFA SNEP control block */
#if NFA_DYNAMIC_MEMORY == FALSE
extern tNFA_SNEP_CB nfa_snep_cb;
#else
extern tNFA_SNEP_CB *nfa_snep_cb_ptr;
#define nfa_snep_cb (*nfa_snep_cb_ptr)
#endif

/* NFA SNEP default server control block */
#if NFA_DYNAMIC_MEMORY == FALSE
extern tNFA_SNEP_DEFAULT_CB nfa_snep_default_cb;
#else
extern tNFA_SNEP_DEFAULT_CB *nfa_snep_default_cb_ptr;
#define nfa_snep_default_cb (*nfa_snep_default_cb_ptr)
#endif

/*****************************************************************************
**  External functions
*****************************************************************************/
/*
**  nfa_snep_main.c
*/
void nfa_snep_init(bool is_dta_mode);

#endif /* (NFA_SNEP_INCLUDED == TRUE) */

void nfa_snep_default_init (void);
BOOLEAN nfa_snep_start_default_server (tNFA_SNEP_MSG *p_msg);
BOOLEAN nfa_snep_stop_default_server (tNFA_SNEP_MSG *p_msg);
/*
**  nfa_snep_srv.c
*/
UINT8 nfa_snep_allocate_cb (void);
void nfa_snep_deallocate_cb (UINT8 xx);
void nfa_snep_send_msg (UINT8 opcode, UINT8 dlink);

void nfa_snep_llcp_cback (tLLCP_SAP_CBACK_DATA *p_data);

void nfa_snep_proc_llcp_data_ind (tLLCP_SAP_CBACK_DATA  *p_data);
void nfa_snep_proc_llcp_connect_ind (tLLCP_SAP_CBACK_DATA  *p_data);
void nfa_snep_proc_llcp_connect_resp (tLLCP_SAP_CBACK_DATA  *p_data);
void nfa_snep_proc_llcp_disconnect_ind (tLLCP_SAP_CBACK_DATA  *p_data);
void nfa_snep_proc_llcp_disconnect_resp (tLLCP_SAP_CBACK_DATA  *p_data);
void nfa_snep_proc_llcp_congest (tLLCP_SAP_CBACK_DATA  *p_data);
void nfa_snep_proc_llcp_link_status (tLLCP_SAP_CBACK_DATA  *p_data);
void nfa_snep_proc_llcp_tx_complete (tLLCP_SAP_CBACK_DATA  *p_data);

BOOLEAN nfa_snep_reg_server (tNFA_SNEP_MSG *p_msg);
BOOLEAN nfa_snep_reg_client (tNFA_SNEP_MSG *p_msg);
BOOLEAN nfa_snep_dereg (tNFA_SNEP_MSG *p_msg);
BOOLEAN nfa_snep_connect (tNFA_SNEP_MSG *p_msg);
BOOLEAN nfa_snep_put_resp (tNFA_SNEP_MSG *p_msg);
BOOLEAN nfa_snep_get_resp (tNFA_SNEP_MSG *p_msg);
BOOLEAN nfa_snep_put_req (tNFA_SNEP_MSG *p_msg);
BOOLEAN nfa_snep_get_req (tNFA_SNEP_MSG *p_msg);
BOOLEAN nfa_snep_disconnect (tNFA_SNEP_MSG *p_msg);

#endif /* NFA_SNEP_INT_H */
