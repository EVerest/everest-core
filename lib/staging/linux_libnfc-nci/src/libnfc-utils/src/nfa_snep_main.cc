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
 *  This is the main implementation file for the NFA SNEP.
 *
 ******************************************************************************/
#include <string.h>
#include "nfa_sys.h"
#include "nfa_sys_int.h"
#include "nfa_snep_int.h"

/*****************************************************************************
**  Global Variables
*****************************************************************************/

/* system manager control block definition */
#if NFA_DYNAMIC_MEMORY == FALSE
tNFA_SNEP_CB nfa_snep_cb;
#endif

/*****************************************************************************
**  Static Functions
*****************************************************************************/

/* event handler function type */
static bool  nfa_snep_evt_hdlr (NFC_HDR *p_msg);

/* disable function type */
static void nfa_snep_sys_disable (void);

/* debug functions type */
#if (BT_TRACE_VERBOSE == TRUE)
static char *nfa_snep_evt_code (UINT16 evt_code);
#endif

/*****************************************************************************
**  Constants
*****************************************************************************/
static const tNFA_SYS_REG nfa_snep_sys_reg =
{
    NULL,
    nfa_snep_evt_hdlr,
    nfa_snep_sys_disable,
    NULL
};

#define NFA_SNEP_NUM_ACTIONS  (NFA_SNEP_LAST_EVT & 0x00ff)

/* type for action functions */
typedef BOOLEAN (*tNFA_SNEP_ACTION) (tNFA_SNEP_MSG *p_data);

/* action function list */
const tNFA_SNEP_ACTION nfa_snep_action[] =
{
    nfa_snep_start_default_server,          /* NFA_SNEP_API_START_DEFAULT_SERVER_EVT */
    nfa_snep_stop_default_server,           /* NFA_SNEP_API_STOP_DEFAULT_SERVER_EVT  */
    nfa_snep_reg_server,                    /* NFA_SNEP_API_REG_SERVER_EVT           */
    nfa_snep_reg_client,                    /* NFA_SNEP_API_REG_CLIENT_EVT           */
    nfa_snep_dereg,                         /* NFA_SNEP_API_DEREG_EVT                */
    nfa_snep_connect,                       /* NFA_SNEP_API_CONNECT_EVT              */
    nfa_snep_get_req,                       /* NFA_SNEP_API_GET_REQ_EVT              */
    nfa_snep_put_req,                       /* NFA_SNEP_API_PUT_REQ_EVT              */
    nfa_snep_get_resp,                      /* NFA_SNEP_API_GET_RESP_EVT             */
    nfa_snep_put_resp,                      /* NFA_SNEP_API_PUT_RESP_EVT             */
    nfa_snep_disconnect                     /* NFA_SNEP_API_DISCONNECT_EVT           */
};

/*******************************************************************************
**
** Function         nfa_snep_init
**
** Description      Initialize NFA SNEP
**
**
** Returns          None
**
*******************************************************************************/
void nfa_snep_init (bool is_dta_mode)
{
    /* initialize control block */
    memset (&nfa_snep_cb, 0, sizeof (tNFA_SNEP_CB));
    nfa_snep_cb.trace_level = APPL_INITIAL_TRACE_LEVEL;
    nfa_snep_cb.is_dta_mode = is_dta_mode;

    SNEP_TRACE_DEBUG1 ("nfa_snep_init (): is_dta_mode=%d", is_dta_mode);

    nfa_snep_default_init ();

    /* register message handler on NFA SYS */
    nfa_sys_register (NFA_ID_SNEP,  &nfa_snep_sys_reg);
}

/*******************************************************************************
**
** Function         nfa_snep_sys_disable
**
** Description      Clean up and deregister NFA SNEP from NFA SYS/DM
**
**
** Returns          None
**
*******************************************************************************/
static void nfa_snep_sys_disable (void)
{
    UINT8 xx;

    SNEP_TRACE_DEBUG0 ("nfa_snep_sys_disable ()");

    /* deallocate any buffer and deregister from LLCP */
    for (xx = 0; xx < NFA_SNEP_MAX_CONN; xx++)
    {
        if (nfa_snep_cb.conn[xx].p_cback != NULL)
        {
            LLCP_Deregister (nfa_snep_cb.conn[xx].local_sap);
            nfa_snep_deallocate_cb (xx);
        }
    }

    /* deregister message handler on NFA SYS */
    nfa_sys_deregister (NFA_ID_SNEP);
}

/*******************************************************************************
**
** Function         nfa_snep_evt_hdlr
**
** Description      Processing event for NFA SNEP
**
**
** Returns          TRUE if p_msg needs to be deallocated
**
*******************************************************************************/
static bool nfa_snep_evt_hdlr (NFC_HDR *p_hdr)
{
    BOOLEAN delete_msg = TRUE;
    UINT16  event;

    tNFA_SNEP_MSG *p_msg = (tNFA_SNEP_MSG *) p_hdr;

#if (BT_TRACE_VERBOSE == TRUE)
    SNEP_TRACE_DEBUG1 ("nfa_snep_evt_hdlr (): Event [%s]", nfa_snep_evt_code (p_msg->hdr.event));
#else
    SNEP_TRACE_DEBUG1 ("nfa_snep_evt_hdlr(): Event 0x%02x", p_msg->hdr.event);
#endif

    event = p_msg->hdr.event & 0x00ff;

    /* execute action functions */
    if (event < NFA_SNEP_NUM_ACTIONS)
    {
        delete_msg = (*nfa_snep_action[event]) (p_msg);
    }
    else
    {
        SNEP_TRACE_ERROR0 ("Unhandled event");
    }

    return delete_msg;
}


#if (BT_TRACE_VERBOSE == TRUE)
/*******************************************************************************
**
** Function         nfa_snep_evt_code
**
** Description
**
** Returns          string of event
**
*******************************************************************************/
static char *nfa_snep_evt_code (UINT16 evt_code)
{
    switch (evt_code)
    {
    case NFA_SNEP_API_START_DEFAULT_SERVER_EVT:
        return "API_START_DEFAULT_SERVER";
    case NFA_SNEP_API_STOP_DEFAULT_SERVER_EVT:
        return "API_STOP_DEFAULT_SERVER";
    case NFA_SNEP_API_REG_SERVER_EVT:
        return "API_REG_SERVER";
    case NFA_SNEP_API_REG_CLIENT_EVT:
        return "API_REG_CLIENT";
    case NFA_SNEP_API_DEREG_EVT:
        return "API_DEREG";
    case NFA_SNEP_API_CONNECT_EVT:
        return "API_CONNECT";
    case NFA_SNEP_API_GET_REQ_EVT:
        return "API_GET_REQ";
    case NFA_SNEP_API_PUT_REQ_EVT:
        return "API_PUT_REQ";
    case NFA_SNEP_API_GET_RESP_EVT:
        return "API_GET_RESP";
    case NFA_SNEP_API_PUT_RESP_EVT:
        return "API_PUT_RESP";
    case NFA_SNEP_API_DISCONNECT_EVT:
        return "API_DISCONNECT";
    default:
        return "Unknown event";
    }
}
#endif  /* Debug Functions */
