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
 *  This is the implementation file for the NFA SNEP default server.
 *
 ******************************************************************************/
#include <string.h>
#include "nfa_sys.h"
#include "nfa_sys_int.h"
#include "nfa_snep_int.h"
#include "nfa_mem_co.h"
#include "nfa_dm_int.h"
#include "trace_api.h"
#include "nfa_snep_int.h"
/*****************************************************************************
**  Global Variables
*****************************************************************************/

/* system manager control block definition */
#if NFA_DYNAMIC_MEMORY == FALSE
tNFA_SNEP_DEFAULT_CB nfa_snep_default_cb;
#endif

/*****************************************************************************
**  Static Functions
*****************************************************************************/

/*****************************************************************************
**  Constants
*****************************************************************************/

/*******************************************************************************
**
** Function         nfa_snep_default_init
**
** Description      Initialize NFA SNEP default server
**
**
** Returns          None
**
*******************************************************************************/
void nfa_snep_default_init (void)
{
    UINT8 xx;

    SNEP_TRACE_DEBUG0 ("nfa_snep_default_init ()");

    /* initialize control block */
    memset (&nfa_snep_default_cb, 0, sizeof (tNFA_SNEP_DEFAULT_CB));

    /* initialize non-zero value */
    nfa_snep_default_cb.server_handle = NFA_HANDLE_INVALID;

    for (xx = 0; xx < NFA_SNEP_DEFAULT_MAX_CONN; xx++)
    {
        nfa_snep_default_cb.conn[xx].conn_handle = NFA_HANDLE_INVALID;
    }
}

/*******************************************************************************
**
** Function         nfa_snep_default_service_cback
**
** Description      Processing event to default SNEP server/client
**
**
** Returns          None
**
*******************************************************************************/
void nfa_snep_default_service_cback (tNFA_SNEP_EVT event, tNFA_SNEP_EVT_DATA *p_data)
{
    UINT8 xx;
    tNFA_SNEP_API_DISCONNECT api_disconnect;
    tNFA_SNEP_API_PUT_RESP   api_put_resp;

    SNEP_TRACE_DEBUG1 ("nfa_snep_default_service_cback () event:0x%X", event);

    switch (event)
    {
    case NFA_SNEP_REG_EVT:
        if (p_data->reg.status == NFA_STATUS_OK)
        {
            nfa_snep_default_cb.server_handle = p_data->reg.reg_handle;
        }
        else
        {
            SNEP_TRACE_ERROR0 ("Default SNEP server failed to register");
        }
        break;

    case NFA_SNEP_CONNECTED_EVT:
        if (p_data->connect.reg_handle == nfa_snep_default_cb.server_handle)
        {
            for (xx = 0; xx < NFA_SNEP_DEFAULT_MAX_CONN; xx++)
            {
                if (nfa_snep_default_cb.conn[xx].conn_handle == NFA_HANDLE_INVALID)
                {
                    nfa_snep_default_cb.conn[xx].conn_handle = p_data->connect.conn_handle;
                    break;
                }
            }

            if (xx >= NFA_SNEP_DEFAULT_MAX_CONN)
            {
                SNEP_TRACE_ERROR1 ("Default SNEP server cannot handle more than %d connections",
                                  NFA_SNEP_DEFAULT_MAX_CONN);

                api_disconnect.conn_handle = p_data->connect.conn_handle;
                api_disconnect.flush       = TRUE;
                nfa_snep_disconnect ((tNFA_SNEP_MSG *) &api_disconnect);
            }
        }
        break;

    case NFA_SNEP_ALLOC_BUFF_EVT:
        if (p_data->alloc.req_code == NFA_SNEP_REQ_CODE_GET)
        {
            /*
            ** Default server doesn't support GET
            ** Send NFA_SNEP_RESP_CODE_NOT_IMPLM to peer
            */
            SNEP_TRACE_WARNING0 ("Default SNEP server doesn't support GET");
            p_data->alloc.p_buff    = NULL;
            p_data->alloc.resp_code = NFA_SNEP_RESP_CODE_NOT_IMPLM;
        }
        else /* NFA_SNEP_REQ_CODE_PUT */
        {
            p_data->alloc.p_buff = NULL;

            for (xx = 0; xx < NFA_SNEP_DEFAULT_MAX_CONN; xx++)
            {
                if (nfa_snep_default_cb.conn[xx].conn_handle == p_data->alloc.conn_handle)
                {
                    if (p_data->alloc.ndef_length <= NFA_SNEP_DEFAULT_SERVER_MAX_NDEF_SIZE)
                    {
                        /* allocate memory, allocated buffer will be returned in NFA_SNEP_PUT_REQ_EVT */
                        p_data->alloc.p_buff = (UINT8*) nfa_mem_co_alloc (p_data->alloc.ndef_length);
                    }

                    /* store buffer pointer in case of failure in the middle */
                    nfa_snep_default_cb.conn[xx].p_rx_ndef = p_data->alloc.p_buff;
                    break;
                }
            }
        }
        break;

    case NFA_SNEP_PUT_REQ_EVT:
        for (xx = 0; xx < NFA_SNEP_DEFAULT_MAX_CONN; xx++)
        {
            if (nfa_snep_default_cb.conn[xx].conn_handle == p_data->put_req.conn_handle)
            {
                if (!nfa_snep_cb.is_dta_mode)
                {
                    nfa_dm_ndef_handle_message (NFA_STATUS_OK,
                                                p_data->put_req.p_ndef,
                                                p_data->put_req.ndef_length);
                }
#if (BT_TRACE_PROTOCOL == TRUE)
                else
                {
                    //DispNDEFMsg (p_data->put_req.p_ndef,
                    //              p_data->put_req.ndef_length, TRUE);
                }
#endif

                nfa_mem_co_free (p_data->put_req.p_ndef);
                nfa_snep_default_cb.conn[xx].p_rx_ndef = NULL;

                api_put_resp.conn_handle = p_data->put_req.conn_handle;
                api_put_resp.resp_code   = NFA_SNEP_RESP_CODE_SUCCESS;

                nfa_snep_put_resp ((tNFA_SNEP_MSG *) &api_put_resp);
                break;
            }
        }
        break;

    case NFA_SNEP_DISC_EVT:
        for (xx = 0; xx < NFA_SNEP_DEFAULT_MAX_CONN; xx++)
        {
            if (nfa_snep_default_cb.conn[xx].conn_handle == p_data->disc.conn_handle)
            {
                nfa_snep_default_cb.conn[xx].conn_handle = NFA_HANDLE_INVALID;

                /* if buffer is not freed */
                if (nfa_snep_default_cb.conn[xx].p_rx_ndef)
                {
                    nfa_mem_co_free (nfa_snep_default_cb.conn[xx].p_rx_ndef);
                    nfa_snep_default_cb.conn[xx].p_rx_ndef  = NULL;
                }
                break;
            }
        }
        break;

    default:
        SNEP_TRACE_ERROR0 ("Unexpected event for default SNEP server");
        break;
    }
}

/*******************************************************************************
**
** Function         nfa_snep_start_default_server
**
** Description      Launching default SNEP server
**
**
** Returns          TRUE to deallocate message
**
*******************************************************************************/
BOOLEAN nfa_snep_start_default_server (tNFA_SNEP_MSG *p_msg)
{
    tNFA_SNEP_API_REG_SERVER msg;

    SNEP_TRACE_DEBUG0 ("nfa_snep_start_default_server ()");

    if (nfa_snep_default_cb.server_handle == NFA_HANDLE_INVALID)
    {
        msg.server_sap = NFA_SNEP_DEFAULT_SERVER_SAP;

        BCM_STRNCPY_S (msg.service_name, sizeof (msg.service_name),
                      "urn:nfc:sn:snep", LLCP_MAX_SN_LEN);
        msg.service_name[LLCP_MAX_SN_LEN] = 0;

        msg.p_cback = nfa_snep_default_service_cback;
        nfa_snep_reg_server ((tNFA_SNEP_MSG *) &msg);
    }

    (*p_msg->api_start_default_server.p_cback) (NFA_SNEP_DEFAULT_SERVER_STARTED_EVT, NULL);

    return TRUE;
}

/*******************************************************************************
**
** Function         nfa_snep_stop_default_server
**
** Description      Stoppping default SNEP server
**
**
** Returns          TRUE to deallocate message
**
*******************************************************************************/
BOOLEAN nfa_snep_stop_default_server (tNFA_SNEP_MSG *p_msg)
{
    tNFA_SNEP_API_DEREG msg;

    SNEP_TRACE_DEBUG0 ("nfa_snep_stop_default_server ()");

    if (nfa_snep_default_cb.server_handle != NFA_HANDLE_INVALID)
    {
        msg.reg_handle = nfa_snep_default_cb.server_handle;

        nfa_snep_dereg ((tNFA_SNEP_MSG *) &msg);

        nfa_snep_default_cb.server_handle = NFA_HANDLE_INVALID;
    }

    (*p_msg->api_stop_default_server.p_cback) (NFA_SNEP_DEFAULT_SERVER_STOPPED_EVT, NULL);

    return TRUE;
}

