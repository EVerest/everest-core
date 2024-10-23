/******************************************************************************
 *
 *  Copyright (C) 2010-2012 Broadcom Corporation
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
 *  NFA SNEP interface to LLCP
 *
 ******************************************************************************/
#include <string.h>
#include "nfa_api.h"
#include "nfa_sys.h"
#include "nfa_sys_int.h"
#include "nfa_snep_int.h"
#include "nfa_mem_co.h"
#include "nfa_snep_api.h"
/*****************************************************************************
**  Constants
*****************************************************************************/

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
tNFA_STATUS NFA_SnepStartDefaultServer (tNFA_SNEP_CBACK *p_cback)
{
    tNFA_SNEP_API_START_DEFAULT_SERVER *p_msg;

    SNEP_TRACE_API0 ("NFA_SnepStartDefaultServer ()");

    if (p_cback == NULL)
    {
        SNEP_TRACE_ERROR0 ("NFA_SnepStartDefaultServer (): p_cback is NULL");
        return (NFA_STATUS_INVALID_PARAM);
    }

    if ((p_msg = (tNFA_SNEP_API_START_DEFAULT_SERVER *) GKI_getbuf (sizeof (tNFA_SNEP_API_START_DEFAULT_SERVER))) != NULL)
    {
        p_msg->hdr.event = NFA_SNEP_API_START_DEFAULT_SERVER_EVT;
        p_msg->p_cback   = p_cback;

        nfa_sys_sendmsg (p_msg);

        return (NFA_STATUS_OK);
    }

    return (NFA_STATUS_FAILED);
}

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
tNFA_STATUS NFA_SnepStopDefaultServer (tNFA_SNEP_CBACK *p_cback)
{
    tNFA_SNEP_API_STOP_DEFAULT_SERVER *p_msg;

    SNEP_TRACE_API0 ("NFA_SnepStopDefaultServer ()");

    if (p_cback == NULL)
    {
        SNEP_TRACE_ERROR0 ("NFA_SnepStopDefaultServer (): p_cback is NULL");
        return (NFA_STATUS_INVALID_PARAM);
    }

    if ((p_msg = (tNFA_SNEP_API_STOP_DEFAULT_SERVER *) GKI_getbuf (sizeof (tNFA_SNEP_API_STOP_DEFAULT_SERVER))) != NULL)
    {
        p_msg->hdr.event = NFA_SNEP_API_STOP_DEFAULT_SERVER_EVT;
        p_msg->p_cback   = p_cback;

        nfa_sys_sendmsg (p_msg);

        return (NFA_STATUS_OK);
    }

    return (NFA_STATUS_FAILED);
}

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
**                  by NFA_SnepStartDefaultServer().
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
tNFA_STATUS NFA_SnepRegisterServer (UINT8           server_sap,
                                    char            *p_service_name,
                                    tNFA_SNEP_CBACK *p_cback)
{
    tNFA_SNEP_API_REG_SERVER *p_msg;

    SNEP_TRACE_API2 ("NFA_SnepRegisterServer (): SAP:0x%X, SN:<%s>", server_sap, p_service_name);

    if ((p_service_name == NULL) || (p_cback == NULL))
    {
        SNEP_TRACE_ERROR0 ("NFA_SnepRegisterServer (): p_service_name or p_cback is NULL");
        return (NFA_STATUS_INVALID_PARAM);
    }

    if ((p_msg = (tNFA_SNEP_API_REG_SERVER *) GKI_getbuf (sizeof (tNFA_SNEP_API_REG_SERVER))) != NULL)
    {
        p_msg->hdr.event = NFA_SNEP_API_REG_SERVER_EVT;

        p_msg->server_sap = server_sap;

        BCM_STRNCPY_S (p_msg->service_name, sizeof (p_msg->service_name),
                       p_service_name, LLCP_MAX_SN_LEN);
        p_msg->service_name[LLCP_MAX_SN_LEN] = 0;

        p_msg->p_cback = p_cback;

        nfa_sys_sendmsg (p_msg);

        return (NFA_STATUS_OK);
    }

    return (NFA_STATUS_FAILED);
}

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
tNFA_STATUS NFA_SnepRegisterClient (tNFA_SNEP_CBACK *p_cback)
{
    tNFA_SNEP_API_REG_CLIENT *p_msg;

    SNEP_TRACE_API0 ("NFA_SnepRegisterClient () Entry....");

    if (p_cback == NULL)
    {
        SNEP_TRACE_ERROR0 ("NFA_SnepRegisterClient (): p_cback is NULL");
        return (NFA_STATUS_INVALID_PARAM);
    }

    SNEP_TRACE_API0 ("NFA_SnepRegisterClient () p_cback is not NULL");
    if ((p_msg = (tNFA_SNEP_API_REG_CLIENT *) GKI_getbuf (sizeof (tNFA_SNEP_API_REG_CLIENT))) != NULL)
    {
        SNEP_TRACE_API0 ("NFA_SnepRegisterClient () GKI_getbuf is Success");
        p_msg->hdr.event = NFA_SNEP_API_REG_CLIENT_EVT;

        p_msg->p_cback = p_cback;

        nfa_sys_sendmsg (p_msg);

        SNEP_TRACE_API0 ("NFA_SnepRegisterClient ()creturn NFA_STATUS_OK");
        return (NFA_STATUS_OK);
    }

    SNEP_TRACE_API0 ("NFA_SnepRegisterClient ()creturn NFA_STATUS_FAILED");
    return (NFA_STATUS_FAILED);
}

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
tNFA_STATUS NFA_SnepDeregister (tNFA_HANDLE reg_handle)
{
    tNFA_SNEP_API_DEREG *p_msg;
    tNFA_HANDLE          xx;

    SNEP_TRACE_API1 ("NFA_SnepDeregister (): reg_handle:0x%X", reg_handle);

    xx = reg_handle & NFA_HANDLE_MASK;

    if (  (xx >= NFA_SNEP_MAX_CONN)
        ||(nfa_snep_cb.conn[xx].p_cback == NULL)  )
    {
        SNEP_TRACE_ERROR0 ("NFA_SnepDeregister (): Handle is invalid or not registered");
        return (NFA_STATUS_BAD_HANDLE);
    }

    if ((p_msg = (tNFA_SNEP_API_DEREG *) GKI_getbuf (sizeof (tNFA_SNEP_API_DEREG))) != NULL)
    {
        p_msg->hdr.event = NFA_SNEP_API_DEREG_EVT;

        p_msg->reg_handle = reg_handle;

        nfa_sys_sendmsg (p_msg);

        return (NFA_STATUS_OK);
    }

    return (NFA_STATUS_FAILED);
}

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
tNFA_STATUS NFA_SnepConnect (tNFA_HANDLE     client_handle,
                             char            *p_service_name)
{
    tNFA_SNEP_API_CONNECT *p_msg;
    tNFA_HANDLE            xx;

    SNEP_TRACE_API1 ("NFA_SnepConnect(): client_handle:0x%X", client_handle);

    xx = client_handle & NFA_HANDLE_MASK;

    if (  (xx >= NFA_SNEP_MAX_CONN)
        ||(nfa_snep_cb.conn[xx].p_cback == NULL)
        ||(!(nfa_snep_cb.conn[xx].flags & NFA_SNEP_FLAG_CLIENT))  )
    {
        SNEP_TRACE_ERROR0 ("NFA_SnepConnect (): Client handle is invalid");
        return (NFA_STATUS_BAD_HANDLE);
    }

    if (p_service_name == NULL)
    {
        SNEP_TRACE_ERROR0 ("NFA_SnepConnect (): p_service_name is NULL");
        return (NFA_STATUS_INVALID_PARAM);
    }

    if ((p_msg = (tNFA_SNEP_API_CONNECT*) GKI_getbuf (sizeof (tNFA_SNEP_API_CONNECT))) != NULL)
    {
        p_msg->hdr.event = NFA_SNEP_API_CONNECT_EVT;

        p_msg->client_handle = client_handle;
        BCM_STRNCPY_S (p_msg->service_name, sizeof (p_msg->service_name),
                       p_service_name, LLCP_MAX_SN_LEN);
        p_msg->service_name[LLCP_MAX_SN_LEN] = 0;

        nfa_sys_sendmsg (p_msg);

        return (NFA_STATUS_OK);
    }

    return (NFA_STATUS_FAILED);
}

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
tNFA_STATUS NFA_SnepGet (tNFA_HANDLE     conn_handle,
                         UINT32          buff_length,
                         UINT32          ndef_length,
                         UINT8           *p_ndef_buff)
{
    tNFA_SNEP_API_GET_REQ *p_msg;
    tNFA_HANDLE            xx;

    SNEP_TRACE_API1 ("NFA_SnepGet (): conn_handle:0x%X", conn_handle);

    xx = conn_handle & NFA_HANDLE_MASK;

    if (  (xx >= NFA_SNEP_MAX_CONN)
        ||(nfa_snep_cb.conn[xx].p_cback == NULL)
        ||(!(nfa_snep_cb.conn[xx].flags & NFA_SNEP_FLAG_CLIENT))  )
    {
        SNEP_TRACE_ERROR0 ("NFA_SnepGet (): Connection handle is invalid");
        return (NFA_STATUS_BAD_HANDLE);
    }

    if ((p_msg = (tNFA_SNEP_API_GET_REQ *) GKI_getbuf (sizeof (tNFA_SNEP_API_GET_REQ))) != NULL)
    {
        p_msg->hdr.event = NFA_SNEP_API_GET_REQ_EVT;

        p_msg->conn_handle = conn_handle;
        p_msg->buff_length = buff_length;
        p_msg->ndef_length = ndef_length;
        p_msg->p_ndef_buff = p_ndef_buff;

        nfa_sys_sendmsg (p_msg);

        return (NFA_STATUS_OK);
    }

    return (NFA_STATUS_FAILED);
}

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
**                  through registered p_cback. Application may free the buffer after
**                  receiving these events.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_BAD_HANDLE if handle is not valid
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
tNFA_STATUS NFA_SnepPut (tNFA_HANDLE     conn_handle,
                         UINT32          ndef_length,
                         UINT8           *p_ndef_buff)
{
    tNFA_SNEP_API_PUT_REQ *p_msg;
    tNFA_HANDLE            xx;

    SNEP_TRACE_API1 ("NFA_SnepPut (): conn_handle:0x%X", conn_handle);

    xx = conn_handle & NFA_HANDLE_MASK;

    if (  (xx >= NFA_SNEP_MAX_CONN)
        ||(nfa_snep_cb.conn[xx].p_cback == NULL)
        ||(!(nfa_snep_cb.conn[xx].flags & NFA_SNEP_FLAG_CLIENT))  )
    {
        SNEP_TRACE_ERROR0 ("NFA_SnepPut (): Connection handle is invalid");
        return (NFA_STATUS_BAD_HANDLE);
    }

    if ((p_msg = (tNFA_SNEP_API_PUT_REQ *) GKI_getbuf (sizeof (tNFA_SNEP_API_PUT_REQ))) != NULL)
    {
        p_msg->hdr.event = NFA_SNEP_API_PUT_REQ_EVT;

        p_msg->conn_handle = conn_handle;
        p_msg->ndef_length = ndef_length;
        p_msg->p_ndef_buff = p_ndef_buff;

        nfa_sys_sendmsg (p_msg);

        return (NFA_STATUS_OK);
    }

    return (NFA_STATUS_FAILED);
}

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
tNFA_STATUS NFA_SnepGetResponse (tNFA_HANDLE         conn_handle,
                                 tNFA_SNEP_RESP_CODE resp_code,
                                 UINT32              ndef_length,
                                 UINT8               *p_ndef_buff)
{
    tNFA_SNEP_API_GET_RESP *p_msg;
    tNFA_HANDLE            xx;

    SNEP_TRACE_API1 ("NFA_SnepGetResponse (): conn_handle:0x%X", conn_handle);

    xx = conn_handle & NFA_HANDLE_MASK;

    if (  (xx >= NFA_SNEP_MAX_CONN)
        ||(nfa_snep_cb.conn[xx].p_cback == NULL)
        ||(!(nfa_snep_cb.conn[xx].flags & NFA_SNEP_FLAG_SERVER))  )
    {
        SNEP_TRACE_ERROR0 ("NFA_SnepGetResponse (): Handle is invalid");
        return (NFA_STATUS_BAD_HANDLE);
    }

    if ((p_msg = (tNFA_SNEP_API_GET_RESP *) GKI_getbuf (sizeof (tNFA_SNEP_API_GET_RESP))) != NULL)
    {
        p_msg->hdr.event = NFA_SNEP_API_GET_RESP_EVT;

        p_msg->conn_handle = conn_handle;
        p_msg->resp_code   = resp_code;
        p_msg->ndef_length = ndef_length;
        p_msg->p_ndef_buff = p_ndef_buff;

        nfa_sys_sendmsg (p_msg);

        return (NFA_STATUS_OK);
    }

    return (NFA_STATUS_FAILED);
}


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
tNFA_STATUS NFA_SnepPutResponse (tNFA_HANDLE         conn_handle,
                                 tNFA_SNEP_RESP_CODE resp_code)
{
    tNFA_SNEP_API_PUT_RESP *p_msg;
    tNFA_HANDLE            xx;

    SNEP_TRACE_API1 ("NFA_SnepPutResponse (): conn_handle:0x%X", conn_handle);

    xx = conn_handle & NFA_HANDLE_MASK;

    if (  (xx >= NFA_SNEP_MAX_CONN)
        ||(nfa_snep_cb.conn[xx].p_cback == NULL)
        ||(!(nfa_snep_cb.conn[xx].flags & NFA_SNEP_FLAG_SERVER))  )
    {
        SNEP_TRACE_ERROR0 ("NFA_SnepPutResponse (): Handle is invalid");
        return (NFA_STATUS_BAD_HANDLE);
    }

    if ((p_msg = (tNFA_SNEP_API_PUT_RESP *) GKI_getbuf (sizeof (tNFA_SNEP_API_PUT_RESP))) != NULL)
    {
        p_msg->hdr.event = NFA_SNEP_API_PUT_RESP_EVT;

        p_msg->conn_handle = conn_handle;
        p_msg->resp_code   = resp_code;

        nfa_sys_sendmsg (p_msg);

        return (NFA_STATUS_OK);
    }

    return (NFA_STATUS_FAILED);
}

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
tNFA_STATUS NFA_SnepDisconnect (tNFA_HANDLE conn_handle, BOOLEAN flush)
{
    tNFA_SNEP_API_DISCONNECT *p_msg;
    tNFA_HANDLE              xx;

    SNEP_TRACE_API2 ("NFA_SnepDisconnect (): conn_handle:0x%X, flush=%d", conn_handle, flush);

    xx = conn_handle & NFA_HANDLE_MASK;

    if (  (xx >= NFA_SNEP_MAX_CONN)
        ||(nfa_snep_cb.conn[xx].p_cback == NULL))
    {
        SNEP_TRACE_ERROR0 ("NFA_SnepDisconnect (): Handle is invalid");
        return (NFA_STATUS_BAD_HANDLE);
    }

    if ((p_msg = (tNFA_SNEP_API_DISCONNECT *) GKI_getbuf (sizeof (tNFA_SNEP_API_DISCONNECT))) != NULL)
    {
        p_msg->hdr.event   = NFA_SNEP_API_DISCONNECT_EVT;
        p_msg->conn_handle = conn_handle;
        p_msg->flush       = flush;

        nfa_sys_sendmsg (p_msg);

        return (NFA_STATUS_OK);
    }

    return (NFA_STATUS_FAILED);
}

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
UINT8 NFA_SnepSetTraceLevel (UINT8 new_level)
{
    if (new_level != 0xFF)
        nfa_snep_cb.trace_level = new_level;

    return (nfa_snep_cb.trace_level);
}
