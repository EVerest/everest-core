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
 *  This is the public interface file for NFA SNEP, Broadcom's NFC
 *  application layer for mobile phones.
 *
 ******************************************************************************/
#ifndef NFA_SNEP_API_H
#define NFA_SNEP_API_H

#include "nfa_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/
#define NFA_SNEP_VERSION                0x10    /* SNEP Version 1.0          */

#define NFA_SNEP_REQ_CODE_CONTINUE      0x00    /* send remaining fragments         */

#define NFA_SNEP_REQ_CODE_REJECT        0x7F    /* do not send remaining fragments  */
/* return an NDEF message           */
#define NFA_SNEP_REQ_CODE_GET 0x01
/* accept an NDEF message           */
#define NFA_SNEP_REQ_CODE_PUT 0x02
/* do not send remaining fragments  */
#define NFA_SNEP_RESP_CODE_CONTINUE     0x80    /* continue send remaining fragments    */
#define NFA_SNEP_RESP_CODE_SUCCESS      0x81    /* the operation succeeded              */
#define NFA_SNEP_RESP_CODE_NOT_FOUND    0xC0    /* resource not found                   */
#define NFA_SNEP_RESP_CODE_EXCESS_DATA  0xC1    /* resource exceeds data size limit     */
#define NFA_SNEP_RESP_CODE_BAD_REQ      0xC2    /* malformed request not understood     */
#define NFA_SNEP_RESP_CODE_NOT_IMPLM    0xE0    /* unsupported functionality requested  */
#define NFA_SNEP_RESP_CODE_UNSUPP_VER   0xE1    /* unsupported protocol version         */
#define NFA_SNEP_RESP_CODE_REJECT       0xFF    /* do not send remaining fragments      */


#define tNFA_SNEP_REQ_CODE uint8_t

#define tNFA_SNEP_RESP_CODE uint8_t

/* NFA SNEP callback events */
/* Server/client Registeration Status   */
#define NFA_SNEP_REG_EVT 0x00
/* LLCP link has been activated, client only   */
#define NFA_SNEP_ACTIVATED_EVT 0x01
/* LLCP link has been deactivated, client only */
#define NFA_SNEP_DEACTIVATED_EVT 0x02
/* Data link has been created           */
#define NFA_SNEP_CONNECTED_EVT 0x03
/* GET request from client              */
#define NFA_SNEP_GET_REQ_EVT 0x04
/* PUT request from client              */
#define NFA_SNEP_PUT_REQ_EVT 0x05
/* GET response from server             */
#define NFA_SNEP_GET_RESP_EVT 0x06
/* PUT response from server             */
#define NFA_SNEP_PUT_RESP_EVT 0x07
/* Failed to connect or disconnected    */
#define NFA_SNEP_DISC_EVT 0x08

#define NFA_SNEP_ALLOC_BUFF_EVT                                         \
  0x09                              /* Request to allocate a buffer for \
                                       NDEF*/
#define NFA_SNEP_FREE_BUFF_EVT 0x0A /* Request to deallocate buffer for NDEF*/
/* GET response sent to client          */
#define NFA_SNEP_GET_RESP_CMPL_EVT 0x0B

#define NFA_SNEP_DEFAULT_SERVER_STARTED_EVT 0x0C    /* SNEP default server is started       */
#define NFA_SNEP_DEFAULT_SERVER_STOPPED_EVT 0x0D    /* SNEP default server is stopped       */
typedef uint8_t tNFA_SNEP_EVT;
#define NFA_SNEP_ANY_SAP         LLCP_INVALID_SAP

/* Data for NFA_SNEP_REG_EVT */
typedef struct {
  tNFA_STATUS status;
  tNFA_HANDLE reg_handle; /* handle for registered server/client */
  char service_name[LLCP_MAX_SN_LEN + 1]; /* only for server */
} tNFA_SNEP_REG;

/* Data for NFA_SNEP_ACTIVATED_EVT */
typedef struct {
  tNFA_HANDLE client_handle; /* handle for registered client    */
} tNFA_SNEP_ACTIVATED;

/* Data for NFA_SNEP_DEACTIVATED_EVT */
typedef tNFA_SNEP_ACTIVATED tNFA_SNEP_DEACTIVATED;

/* Data for NFA_SNEP_CONNECTED_EVT */
/*
** for server, new handle will be assigned for conn_handle
** for client, handle used in NFA_SnepConnect () is returned in conn_handle
*/
typedef struct {
  tNFA_HANDLE reg_handle;  /* server/client handle            */
  tNFA_HANDLE conn_handle; /* handle for data link connection */
} tNFA_SNEP_CONNECT;

/* Data for NFA_SNEP_GET_REQ_EVT */
typedef struct {
  tNFA_HANDLE conn_handle;    /* handle for data link connection */
  uint32_t acceptable_length; /* acceptable length from client   */
  uint32_t ndef_length;       /* NDEF message length             */
  uint8_t* p_ndef;            /* NDEF message                    */
} tNFA_SNEP_GET_REQ;

/* Data for NFA_SNEP_PUT_REQ_EVT */
typedef struct {
  tNFA_HANDLE conn_handle; /* handle for data link connection */
  uint32_t ndef_length;    /* NDEF message length             */
  uint8_t* p_ndef;         /* NDEF message                    */
} tNFA_SNEP_PUT_REQ;

/* Data for NFA_SNEP_GET_RESP_EVT */
typedef struct {
  tNFA_HANDLE conn_handle;       /* handle for data link connection */
  tNFA_SNEP_RESP_CODE resp_code; /* response code from server       */
  uint32_t ndef_length;          /* NDEF message length             */
  uint8_t* p_ndef;               /* NDEF message                    */
} tNFA_SNEP_GET_RESP;

/* Data for NFA_SNEP_PUT_RESP_EVT */
typedef struct {
  tNFA_HANDLE conn_handle;       /* handle for data link connection */
  tNFA_SNEP_RESP_CODE resp_code; /* response code from server       */
} tNFA_SNEP_PUT_RESP;

/* Data for NFA_SNEP_DISC_EVT */
typedef struct {
  tNFA_HANDLE conn_handle; /* handle for data link connection */
                           /* client_handle if connection failed */
} tNFA_SNEP_DISC;

/* Data for NFA_SNEP_ALLOC_BUFF_EVT */
typedef struct {
  tNFA_HANDLE conn_handle; /* handle for data link connection                */
  tNFA_SNEP_REQ_CODE
      req_code; /* NFA_SNEP_REQ_CODE_GET or NFA_SNEP_REQ_CODE_PUT */
  tNFA_SNEP_RESP_CODE resp_code; /* Response code if cannot allocate buffer */
  uint32_t ndef_length; /* NDEF message length                            */
  uint8_t* p_buff;      /* buffer for NDEF message                        */
} tNFA_SNEP_ALLOC;

/* Data for NFA_SNEP_FREE_BUFF_EVT */
typedef struct {
  tNFA_HANDLE conn_handle; /* handle for data link connection */
  uint8_t* p_buff;         /* buffer to free                  */
} tNFA_SNEP_FREE;

/* Data for NFA_SNEP_GET_RESP_CMPL_EVT */
typedef struct {
  tNFA_HANDLE conn_handle; /* handle for data link connection */
  uint8_t* p_buff;         /* buffer for NDEF message         */
} tNFA_SNEP_GET_RESP_CMPL;

/* Union of all SNEP callback structures */
typedef union {
  tNFA_SNEP_REG reg;                     /* NFA_SNEP_REG_EVT             */
  tNFA_SNEP_ACTIVATED activated;         /* NFA_SNEP_ACTIVATED_EVT       */
  tNFA_SNEP_DEACTIVATED deactivated;     /* NFA_SNEP_DEACTIVATED_EVT     */
  tNFA_SNEP_CONNECT connect;             /* NFA_SNEP_CONNECTED_EVT       */
  tNFA_SNEP_GET_REQ get_req;             /* NFA_SNEP_GET_REQ_EVT         */
  tNFA_SNEP_PUT_REQ put_req;             /* NFA_SNEP_PUT_REQ_EVT         */
  tNFA_SNEP_GET_RESP get_resp;           /* NFA_SNEP_GET_RESP_EVT        */
  tNFA_SNEP_PUT_RESP put_resp;           /* NFA_SNEP_PUT_RESP_EVT        */
  tNFA_SNEP_DISC disc;                   /* NFA_SNEP_DISC_EVT            */
  tNFA_SNEP_ALLOC alloc;                 /* NFA_SNEP_ALLOC_BUFF_EVT      */
  tNFA_SNEP_FREE free;                   /* NFA_SNEP_FREE_BUFF_EVT       */
  tNFA_SNEP_GET_RESP_CMPL get_resp_cmpl; /* NFA_SNEP_GET_RESP_CMPL_EVT   */
} tNFA_SNEP_EVT_DATA;

/* NFA SNEP callback */
typedef void(tNFA_SNEP_CBACK)(tNFA_SNEP_EVT event, tNFA_SNEP_EVT_DATA* p_data);

/*******************************************************************************
**
** Function         NFA_SnepStartDefaultServer
**
** Description      This function is called to listen to SAP, 0x04 as SNEP default
**                  server ("urn:nfc:sn:snep") on LLCP.
**
**                  NFA_SNEP_DEFAULT_SERVER_STARTED_EVT without data will be returned.
**
** Note:            If RF discovery is started, NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT
**                  should happen before calling this function
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_SnepStartDefaultServer (tNFA_SNEP_CBACK *p_cback);

/*******************************************************************************
**
** Function         NFA_SnepStopDefaultServer
**
** Description      This function is called to stop SNEP default server on LLCP.
**
**                  NFA_SNEP_DEFAULT_SERVER_STOPPED_EVT without data will be returned.
**
** Note:            If RF discovery is started, NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT
**                  should happen before calling this function
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_SnepStopDefaultServer (tNFA_SNEP_CBACK *p_cback);

/*******************************************************************************
**
** Function         NFA_SnepRegisterServer
**
** Description      This function is called to listen to a SAP as SNEP server.
**
**                  If server_sap is set to NFA_SNEP_ANY_SAP, then NFA will allocate
**                  a SAP between LLCP_LOWER_BOUND_SDP_SAP and LLCP_UPPER_BOUND_SDP_SAP
**
**                  NFC Forum default SNEP server ("urn:nfc:sn:snep") may be launched
**                  by NFA_SnepStartDefaultServer ().
**
**                  NFA_SNEP_REG_EVT will be returned with status, handle and service name.
**
** Note:            If RF discovery is started, NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT
**                  should happen before calling this function
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_INVALID_PARAM if p_service_name or p_cback is NULL
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_SnepRegisterServer (UINT8           server_sap,
                                                   char            *p_service_name,
                                                   tNFA_SNEP_CBACK *p_cback);

/*******************************************************************************
**
** Function         NFA_SnepRegisterClient
**
** Description      This function is called to register SNEP client.
**                  NFA_SNEP_REG_EVT will be returned with status, handle
**                  and zero-length service name.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_INVALID_PARAM if p_cback is NULL
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_SnepRegisterClient (tNFA_SNEP_CBACK *p_cback);

/*******************************************************************************
**
** Function         NFA_SnepDeregister
**
** Description      This function is called to stop listening as SNEP server
**                  or SNEP client. Application shall use reg_handle returned in
**                  NFA_SNEP_REG_EVT.
**
** Note:            If this function is called to de-register a SNEP server and RF
**                  discovery is started, NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT
**                  should happen before calling this function
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_BAD_HANDLE if handle is not valid
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_SnepDeregister (tNFA_HANDLE reg_handle);

/*******************************************************************************
**
** Function         NFA_SnepConnect
**
** Description      This function is called by client to create data link connection
**                  to SNEP server on peer device.
**
**                  Client handle and service name of server to connect shall be provided.
**                  A conn_handle will be returned in NFA_SNEP_CONNECTED_EVT, if
**                  successfully connected. Otherwise NFA_SNEP_DISC_EVT will be returned.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_BAD_HANDLE if handle is not valid
**                  NFA_STATUS_INVALID_PARAM if p_service_name or p_cback is NULL
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_SnepConnect (tNFA_HANDLE     client_handle,
                                            char            *p_service_name);

/*******************************************************************************
**
** Function         NFA_SnepGet
**
** Description      This function is called by client to send GET request.
**
**                  Application shall allocate a buffer and put NDEF message with
**                  desired record type to get from server. NDEF message from server
**                  will be returned in the same buffer with NFA_SNEP_GET_RESP_EVT.
**                  The size of buffer will be used as "Acceptable Length".
**
**                  NFA_SNEP_GET_RESP_EVT or NFA_SNEP_DISC_EVT will be returned
**                  through registered p_cback. Application may free the buffer
**                  after receiving these events.
**
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_BAD_HANDLE if handle is not valid
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_SnepGet (tNFA_HANDLE     conn_handle,
                                        UINT32          buff_length,
                                        UINT32          ndef_length,
                                        UINT8           *p_ndef_buff);

/*******************************************************************************
**
** Function         NFA_SnepPut
**
** Description      This function is called by client to send PUT request.
**
**                  Application shall allocate a buffer and put desired NDEF message
**                  to send to server.
**
**                  NFA_SNEP_PUT_RESP_EVT or NFA_SNEP_DISC_EVT will be returned
**                  through p_cback. Application may free the buffer after receiving
**                  these events.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_BAD_HANDLE if handle is not valid
**                  NFA_STATUS_INVALID_PARAM if p_service_name or p_cback is NULL
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_SnepPut (tNFA_HANDLE     conn_handle,
                                        UINT32          ndef_length,
                                        UINT8           *p_ndef_buff);

/*******************************************************************************
**
** Function         NFA_SnepGetResponse
**
** Description      This function is called by server to send response of GET request.
**
**                  When server application receives NFA_SNEP_ALLOC_BUFF_EVT,
**                  it shall allocate a buffer for incoming NDEF message and
**                  pass the pointer within callback context. This buffer will be
**                  returned with NFA_SNEP_GET_REQ_EVT after receiving complete
**                  NDEF message. If buffer is not allocated, NFA_SNEP_RESP_CODE_NOT_FOUND
**                  (Note:There is no proper response code for this case)
**                  or NFA_SNEP_RESP_CODE_REJECT will be sent to client.
**
**                  Server application shall provide conn_handle which is received in
**                  NFA_SNEP_GET_REQ_EVT.
**
**                  Server application shall allocate a buffer and put NDEF message if
**                  response code is NFA_SNEP_RESP_CODE_SUCCESS. Otherwise, ndef_length
**                  shall be set to zero.
**
**                  NFA_SNEP_GET_RESP_CMPL_EVT or NFA_SNEP_DISC_EVT will be returned
**                  through registered callback function. Application may free
**                  the buffer after receiving these events.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_BAD_HANDLE if handle is not valid
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_SnepGetResponse (tNFA_HANDLE         conn_handle,
                                                tNFA_SNEP_RESP_CODE resp_code,
                                                UINT32              ndef_length,
                                                UINT8               *p_ndef_buff);

/*******************************************************************************
**
** Function         NFA_SnepPutResponse
**
** Description      This function is called by server to send response of PUT request.
**
**                  When server application receives NFA_SNEP_ALLOC_BUFF_EVT,
**                  it shall allocate a buffer for incoming NDEF message and
**                  pass the pointer within callback context. This buffer will be
**                  returned with NFA_SNEP_PUT_REQ_EVT after receiving complete
**                  NDEF message.  If buffer is not allocated, NFA_SNEP_RESP_CODE_REJECT
**                  will be sent to client or NFA will discard request and send
**                  NFA_SNEP_RESP_CODE_SUCCESS (Note:There is no proper response code for
**                  this case).
**
**                  Server application shall provide conn_handle which is received in
**                  NFA_SNEP_PUT_REQ_EVT.
**
**                  NFA_SNEP_DISC_EVT will be returned through registered callback
**                  function when client disconnects data link connection.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_BAD_HANDLE if handle is not valid
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_SnepPutResponse (tNFA_HANDLE         conn_handle,
                                                tNFA_SNEP_RESP_CODE resp_code);

/*******************************************************************************
**
** Function         NFA_SnepDisconnect
**
** Description      This function is called to disconnect data link connection.
**                  discard any pending data if flush is set to TRUE
**
**                  Client application shall provide conn_handle in NFA_SNEP_GET_RESP_EVT
**                  or NFA_SNEP_PUT_RESP_EVT.
**
**                  Server application shall provide conn_handle in NFA_SNEP_GET_REQ_EVT
**                  or NFA_SNEP_PUT_REQ_EVT.
**
**                  NFA_SNEP_DISC_EVT will be returned
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_BAD_HANDLE if handle is not valid
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_SnepDisconnect (tNFA_HANDLE conn_handle,
                                               BOOLEAN     flush);


/*******************************************************************************
**
** Function         NFA_SnepSetTraceLevel
**
** Description      This function sets the trace level for SNEP.  If called with
**                  a value of 0xFF, it simply returns the current trace level.
**
** Returns          The new or current trace level
**
*******************************************************************************/
NFC_API extern UINT8 NFA_SnepSetTraceLevel (UINT8 new_level);


#endif /* NFA_P2P_API_H */
