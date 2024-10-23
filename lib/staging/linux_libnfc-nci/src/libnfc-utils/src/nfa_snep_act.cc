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
 *  This is the generic SNEP implementation file for the NFA SNEP.
 *
 ******************************************************************************/
#include <string.h>
#include "nfa_api.h"
#include "nfa_sys.h"
#include "nfa_sys_int.h"
#include "llcp_defs.h"
#include "nfa_p2p_int.h"
#include "nfa_snep_int.h"
#include "nfa_mem_co.h"
#include "trace_api.h"

/*****************************************************************************
**  Global Variables
*****************************************************************************/

/*****************************************************************************
**  Static Functions
*****************************************************************************/

/* debug functions type */
#if (BT_TRACE_VERBOSE == TRUE)
static char *nfa_snep_opcode (UINT8 opcode);
#endif

/*****************************************************************************
**  Constants
*****************************************************************************/

/*******************************************************************************
**
** Function         nfa_snep_sap_to_index
**
** Description      find a connection control block with SAP
**
**
** Returns          index of connection control block if success
**                  NFA_SNEP_MAX_CONN, otherwise
**
*******************************************************************************/
UINT8 nfa_snep_sap_to_index (UINT8 local_sap, UINT8 remote_sap, UINT8 flags)
{
    UINT8 xx;

    for (xx = 0; xx < NFA_SNEP_MAX_CONN; xx++)
    {
        if (  (nfa_snep_cb.conn[xx].p_cback)
            &&(nfa_snep_cb.conn[xx].local_sap == local_sap)
            &&((remote_sap == NFA_SNEP_ANY_SAP) || (nfa_snep_cb.conn[xx].remote_sap == remote_sap))
            &&((nfa_snep_cb.conn[xx].flags & flags) == flags)  )
        {
            return xx;
        }
    }
    return NFA_SNEP_MAX_CONN;
}

/*******************************************************************************
**
** Function         nfa_snep_allocate_cb
**
** Description      Allocate a connection control block
**
**
** Returns          index of connection control block if success
**                  NFA_SNEP_MAX_CONN, otherwise
**
*******************************************************************************/
UINT8 nfa_snep_allocate_cb (void)
{
    UINT8 xx;

    for (xx = 0; xx < NFA_SNEP_MAX_CONN; xx++)
    {
        if (nfa_snep_cb.conn[xx].p_cback == NULL)
        {
            memset (&nfa_snep_cb.conn[xx], 0x00, sizeof (tNFA_SNEP_CONN));
            return xx;
        }
    }
    return NFA_SNEP_MAX_CONN;
}

/*******************************************************************************
**
** Function         nfa_snep_deallocate_cb
**
** Description      Deallocate a connection control block
**
**
** Returns          void
**
*******************************************************************************/
void nfa_snep_deallocate_cb (UINT8 xx)
{
    nfa_snep_cb.conn[xx].p_cback = NULL;
}

/*******************************************************************************
**
** Function         nfa_snep_timer_cback
**
** Description      Process timeout event when timer expires
**
**
** Returns          None
**
*******************************************************************************/
static void nfa_snep_timer_cback (void *p_tle)
{
    UINT8 dlink = (UINT8) ((TIMER_LIST_ENT*)p_tle)->event;

    SNEP_TRACE_DEBUG1 ("nfa_snep_timer_cback () dlink = %d", dlink);

    /* application will free buffer when receiving NFA_SNEP_DISC_EVT */
    nfa_snep_cb.conn[dlink].p_ndef_buff = NULL;

    LLCP_DisconnectReq (nfa_snep_cb.conn[dlink].local_sap,
                        nfa_snep_cb.conn[dlink].remote_sap, TRUE);
}

/*******************************************************************************
**
** Function         nfa_snep_get_efficent_miu
**
** Description      Calculate best MIU to send data for throughput
**
**
** Returns          most efficent MIU for throughput
**
*******************************************************************************/
static UINT16 nfa_snep_get_efficent_miu (UINT16 remote_miu, UINT8 remote_rw)
{
    UINT16 local_link_miu, remote_link_miu;
    UINT16 max_num_pdu_in_agf;
    UINT16 efficent_miu;

    SNEP_TRACE_DEBUG2 ("nfa_snep_get_efficent_miu () remote_miu = %d, remote_rw = %d",
                        remote_miu, remote_rw);

    LLCP_GetLinkMIU (&local_link_miu, &remote_link_miu);

    /* local buffer size is small than max receiving size of peer */
    if (local_link_miu < remote_link_miu)
    {
        remote_link_miu = local_link_miu;
    }

    /*
    ** 9 bytes overhead if AGF is used
    **  - 2 byts AGF header
    **  - at least two of 2 bytes length field for I-PDU
    **  - 3 bytes header for I-PDU
    */
    if (remote_link_miu - remote_miu > 9)
    {
        /*
        ** 5 bytes overhead for each I-PDU in AGF
        **  - 2 bytes length field
        **  - 3 bytes header for I-PDU
        */
        max_num_pdu_in_agf = remote_link_miu / (remote_miu + 5);

        if (remote_link_miu % (remote_miu + 5))
        {
            max_num_pdu_in_agf += 1;
        }

        /* if local devie can put all I-PDU in one AGF */
        if (max_num_pdu_in_agf <= remote_rw)
        {
            efficent_miu = (remote_link_miu - max_num_pdu_in_agf*5)/max_num_pdu_in_agf;
        }
        else
        {
            efficent_miu = remote_miu;
        }
    }
    else
    {
        efficent_miu = remote_miu;
    }

    SNEP_TRACE_DEBUG2 ("nfa_snep_get_efficent_miu () remote_link_miu = %d, efficent_miu = %d",
                        remote_link_miu, efficent_miu);

    return efficent_miu;
}

/*******************************************************************************
**
** Function         nfa_snep_check_version
**
** Description      Check version of SNEP
**
**
** Returns          TRUE if supported
**
*******************************************************************************/
BOOLEAN nfa_snep_check_version (UINT8 version)
{
    /* if major version is matched */
    if ((version & 0xF0) == (NFA_SNEP_VERSION & 0xF0))
        return TRUE;
    else
        return FALSE;
}

/*******************************************************************************
**
** Function         nfa_snep_send_msg
**
** Description      Send complete or the first fragment of SNEP message with or
**                  without information.
**
** Returns          void
**
*******************************************************************************/
void nfa_snep_send_msg (UINT8 opcode, UINT8 dlink)
{
    BT_HDR *p_msg;
    UINT32 length;
    UINT8  *p;
    tLLCP_STATUS status = LLCP_STATUS_FAIL;

#if (BT_TRACE_VERBOSE == TRUE)
    SNEP_TRACE_DEBUG4 ("nfa_snep_send_msg () [0x%x, 0x%x]: %s (0x%02x)",
                       nfa_snep_cb.conn[dlink].local_sap,
                       nfa_snep_cb.conn[dlink].remote_sap,
                       nfa_snep_opcode (opcode), opcode);
#else
    SNEP_TRACE_DEBUG3 ("nfa_snep_send_msg () [0x%x, 0x%x]: opcode 0x%02x",
                       nfa_snep_cb.conn[dlink].local_sap,
                       nfa_snep_cb.conn[dlink].remote_sap,
                       opcode);
#endif

    /* if there is pending SNEP message and opcode can have information */
    if (  (nfa_snep_cb.conn[dlink].p_ndef_buff)
        &&((opcode == NFA_SNEP_REQ_CODE_GET) || (opcode == NFA_SNEP_REQ_CODE_PUT) || (opcode == NFA_SNEP_RESP_CODE_SUCCESS))  )
    {
        length = NFA_SNEP_HEADER_SIZE + nfa_snep_cb.conn[dlink].ndef_length;

        if (opcode == NFA_SNEP_REQ_CODE_GET)
        {
            length += NFA_SNEP_ACCEPT_LEN_SIZE; /* add acceptable length field */
        }

        /* if message is bigger than peer's MIU, send the first fragment */
        if (length > nfa_snep_cb.conn[dlink].tx_miu)
        {
            length = nfa_snep_cb.conn[dlink].tx_miu;
        }

        if ((p_msg = (BT_HDR *) GKI_getpoolbuf (LLCP_POOL_ID)) != NULL)
        {
            p_msg->len    = (UINT16) length;
            p_msg->offset = LLCP_MIN_OFFSET;

            p = (UINT8*) (p_msg + 1) + p_msg->offset;

            /* add SNEP header */
            UINT8_TO_BE_STREAM (p, NFA_SNEP_VERSION);
            UINT8_TO_BE_STREAM (p, opcode);

            if (opcode == NFA_SNEP_REQ_CODE_GET)
            {
                /* add acceptable length field in information field*/
                UINT32_TO_BE_STREAM (p, nfa_snep_cb.conn[dlink].ndef_length + NFA_SNEP_ACCEPT_LEN_SIZE);
                UINT32_TO_BE_STREAM (p, nfa_snep_cb.conn[dlink].acceptable_length);
                length -= NFA_SNEP_ACCEPT_LEN_SIZE;
            }
            else
            {
                UINT32_TO_BE_STREAM (p, nfa_snep_cb.conn[dlink].ndef_length);
            }

            length -= NFA_SNEP_HEADER_SIZE;


            /* add the first fragment or complete of NDEF message */
            memcpy (p, nfa_snep_cb.conn[dlink].p_ndef_buff, length);

#if (BT_TRACE_PROTOCOL == TRUE)
            /*DispSNEP (nfa_snep_cb.conn[dlink].local_sap,
                      nfa_snep_cb.conn[dlink].remote_sap,
                     (UINT8*)(p_msg + 1) + p_msg->offset,
                      NFA_SNEP_HEADER_SIZE,
                      FALSE);*/
#endif
            status = LLCP_SendData (nfa_snep_cb.conn[dlink].local_sap,
                                    nfa_snep_cb.conn[dlink].remote_sap, (NFC_HDR *) p_msg);

            if (status != LLCP_STATUS_FAIL)
            {
                SNEP_TRACE_DEBUG2 ("nfa_snep_send_msg (): sending %d out of %d",
                                   length, nfa_snep_cb.conn[dlink].ndef_length);

                /* if sent complete SNEP message */
                if (length == nfa_snep_cb.conn[dlink].ndef_length)
                {
                    nfa_snep_cb.conn[dlink].cur_length = 0;

                    if (  (opcode == NFA_SNEP_RESP_CODE_SUCCESS)
                        &&(nfa_snep_cb.conn[dlink].rx_code == NFA_SNEP_REQ_CODE_GET)  )
                    {
                        /* Set LLCP to send LLCP_SAP_EVT_TX_COMPLETE */
                        LLCP_SetTxCompleteNtf (nfa_snep_cb.conn[dlink].local_sap,
                                               nfa_snep_cb.conn[dlink].remote_sap);
                    }
                }
                else
                {
                    /* update sent length */
                    nfa_snep_cb.conn[dlink].cur_length = length;

                    if ((opcode == NFA_SNEP_REQ_CODE_GET) || (opcode == NFA_SNEP_REQ_CODE_PUT))
                    {
                        nfa_snep_cb.conn[dlink].flags |= NFA_SNEP_FLAG_W4_RESP_CONTINUE;
                    }
                    else /* (opcode == NFA_SNEP_RESP_CODE_SUCCESS) */
                    {
                        nfa_snep_cb.conn[dlink].flags |= NFA_SNEP_FLAG_W4_REQ_CONTINUE;
                    }
                }
            }
        }
    }
    else /* opcode without information */
    {
        if ((p_msg = (BT_HDR *) GKI_getpoolbuf (LLCP_POOL_ID)) != NULL)
        {
            p_msg->len    = NFA_SNEP_HEADER_SIZE;
            p_msg->offset = LLCP_MIN_OFFSET;

            p = (UINT8*) (p_msg + 1) + p_msg->offset;

            /* add SNEP header without information */
            UINT8_TO_BE_STREAM (p, NFA_SNEP_VERSION);
            UINT8_TO_BE_STREAM (p, opcode);
            UINT32_TO_BE_STREAM (p, 0);

#if (BT_TRACE_PROTOCOL == TRUE)
            /*DispSNEP(nfa_snep_cb.conn[dlink].local_sap,
                     nfa_snep_cb.conn[dlink].remote_sap,
                     (UINT8*)(p_msg + 1) + p_msg->offset,
                     NFA_SNEP_HEADER_SIZE,
                     FALSE);*/
#endif
            status = LLCP_SendData (nfa_snep_cb.conn[dlink].local_sap,
                                    nfa_snep_cb.conn[dlink].remote_sap,(NFC_HDR *) p_msg);
        }
    }

    if (status == LLCP_STATUS_FAIL)
    {
        SNEP_TRACE_ERROR0 ("Cannot allocate buffer or failed to send data");

        /* upper layer will free buffer when NFA_SNEP_DISC_EVT is received */
        nfa_snep_cb.conn[dlink].p_ndef_buff = 0;

        LLCP_DisconnectReq (nfa_snep_cb.conn[dlink].local_sap,
                            nfa_snep_cb.conn[dlink].remote_sap, TRUE);
    }
    else if (status == LLCP_STATUS_CONGESTED)
    {
        nfa_snep_cb.conn[dlink].congest = TRUE;
    }
}

/*******************************************************************************
**
** Function         nfa_snep_send_remaining
**
** Description      Send remaining fragments of SNEP message
**
**
** Returns          void
**
*******************************************************************************/
void nfa_snep_send_remaining (UINT8 dlink)
{
    BT_HDR *p_msg;
    UINT8  *p_src, *p_dst;
    UINT32 length;
    tLLCP_STATUS status;

    SNEP_TRACE_DEBUG1 ("nfa_snep_send_remaining (): dlink:0x%02X", dlink);

    /* while data link connection is not congested */
    while (  (nfa_snep_cb.conn[dlink].congest == FALSE)
           &&(nfa_snep_cb.conn[dlink].cur_length > 0)   /* if any fragment was sent */
           &&(nfa_snep_cb.conn[dlink].cur_length < nfa_snep_cb.conn[dlink].ndef_length)  )
    {
        /* start of remaining fragments */
        p_src = nfa_snep_cb.conn[dlink].p_ndef_buff + nfa_snep_cb.conn[dlink].cur_length;

        length = nfa_snep_cb.conn[dlink].ndef_length - nfa_snep_cb.conn[dlink].cur_length;

        /* sending up to peer's MIU */
        if (length > nfa_snep_cb.conn[dlink].tx_miu)
        {
            length = nfa_snep_cb.conn[dlink].tx_miu;
        }

        status = LLCP_STATUS_FAIL;

        if ((p_msg = (BT_HDR *) GKI_getpoolbuf (LLCP_POOL_ID)) != NULL)
        {
            p_msg->len    = (UINT16) length;
            p_msg->offset = LLCP_MIN_OFFSET;

            p_dst = (UINT8*) (p_msg + 1) + p_msg->offset;

            memcpy (p_dst, p_src, length);

            status = LLCP_SendData (nfa_snep_cb.conn[dlink].local_sap,
                                    nfa_snep_cb.conn[dlink].remote_sap, (NFC_HDR *)p_msg);

            if (status != LLCP_STATUS_FAIL)
            {
                /* update sent length */
                nfa_snep_cb.conn[dlink].cur_length += length;

                SNEP_TRACE_DEBUG2 ("nfa_snep_send_remaining (): sending %d out of %d",
                                   nfa_snep_cb.conn[dlink].cur_length,
                                   nfa_snep_cb.conn[dlink].ndef_length);

                /* if sent the last fragment */
                if (nfa_snep_cb.conn[dlink].cur_length == nfa_snep_cb.conn[dlink].ndef_length)
                {
                    nfa_snep_cb.conn[dlink].cur_length = 0;

                    if (  (nfa_snep_cb.conn[dlink].tx_code == NFA_SNEP_RESP_CODE_SUCCESS)
                        &&(nfa_snep_cb.conn[dlink].rx_code == NFA_SNEP_REQ_CODE_CONTINUE)  )
                    {
                        /* Set LLCP to send LLCP_SAP_EVT_TX_COMPLETE */
                        LLCP_SetTxCompleteNtf (nfa_snep_cb.conn[dlink].local_sap,
                                               nfa_snep_cb.conn[dlink].remote_sap);
                    }
                }
            }
        }

        if (status == LLCP_STATUS_CONGESTED)
        {
            nfa_snep_cb.conn[dlink].congest = TRUE;

            /* wait for uncongested event from LLCP */
            break;
        }
        else if (status == LLCP_STATUS_FAIL)
        {
            SNEP_TRACE_ERROR0 ("Cannot allocate buffer or failed to send data");

            /* upper layer will free buffer when NFA_SNEP_DISC_EVT is received */
            nfa_snep_cb.conn[dlink].p_ndef_buff = 0;

            LLCP_DisconnectReq (nfa_snep_cb.conn[dlink].local_sap,
                                nfa_snep_cb.conn[dlink].remote_sap, TRUE);
            return;
        }
    }
}

/*******************************************************************************
**
** Function         nfa_snep_llcp_cback
**
** Description      Processing event from LLCP
**
**
** Returns          None
**
*******************************************************************************/
void nfa_snep_llcp_cback (tLLCP_SAP_CBACK_DATA *p_data)
{
    SNEP_TRACE_DEBUG2 ("nfa_snep_llcp_cback (): event:0x%02X, local_sap:0x%02X", p_data->hdr.event, p_data->hdr.local_sap);

    switch (p_data->hdr.event)
    {
    case LLCP_SAP_EVT_DATA_IND:
        nfa_snep_proc_llcp_data_ind (p_data);
        break;

    case LLCP_SAP_EVT_CONNECT_IND:
        nfa_snep_proc_llcp_connect_ind (p_data);
        break;

    case LLCP_SAP_EVT_CONNECT_RESP:
        nfa_snep_proc_llcp_connect_resp (p_data);
        break;

    case LLCP_SAP_EVT_DISCONNECT_IND:
        nfa_snep_proc_llcp_disconnect_ind (p_data);
        break;

    case LLCP_SAP_EVT_DISCONNECT_RESP:
        nfa_snep_proc_llcp_disconnect_resp (p_data);
        break;

    case LLCP_SAP_EVT_CONGEST:
        /* congestion start/end */
        nfa_snep_proc_llcp_congest (p_data);
        break;

    case LLCP_SAP_EVT_LINK_STATUS:
        nfa_snep_proc_llcp_link_status (p_data);
        break;

    case LLCP_SAP_EVT_TX_COMPLETE:
        nfa_snep_proc_llcp_tx_complete (p_data);
        break;

    default:
        SNEP_TRACE_ERROR1 ("Unknown event:0x%02X", p_data->hdr.event);
        return;
    }
}

/*******************************************************************************
**
** Function         nfa_snep_validate_rx_msg
**
** Description      Validate version, opcode, length in received message
**
**
** Returns          TRUE if message is valid
**
*******************************************************************************/
BOOLEAN nfa_snep_validate_rx_msg (UINT8 dlink)
{
    UINT32  length;
    UINT8   buffer[NFA_SNEP_HEADER_SIZE], *p;
    BOOLEAN more;
    UINT8   version, opcode;
    UINT32  info_len;

    SNEP_TRACE_DEBUG0 ("nfa_snep_validate_rx_msg ()");

    more = LLCP_ReadDataLinkData (nfa_snep_cb.conn[dlink].local_sap,
                                  nfa_snep_cb.conn[dlink].remote_sap,
                                  NFA_SNEP_HEADER_SIZE,
                                  (uint32_t *)&length, buffer);

#if (BT_TRACE_PROTOCOL == TRUE)
    /*DispSNEP(nfa_snep_cb.conn[dlink].local_sap,
             nfa_snep_cb.conn[dlink].remote_sap,
             buffer,
             (UINT16)length,
             TRUE);*/
#endif

    /* check if it has minimum header,
    ** the first fragment shall include at least the entier SNEP header
    */
    if (length < NFA_SNEP_HEADER_SIZE)
    {
        SNEP_TRACE_ERROR0 ("The first fragment shall include at least the entire SNEP header");

        /* application will free buffer when receiving NFA_SNEP_DISC_EVT */
        nfa_snep_cb.conn[dlink].p_ndef_buff = NULL;

        LLCP_DisconnectReq (nfa_snep_cb.conn[dlink].local_sap,
                            nfa_snep_cb.conn[dlink].remote_sap, TRUE);
        return FALSE;
    }

    p = buffer;

    /* parse SNEP header */
    BE_STREAM_TO_UINT8 (version, p);
    BE_STREAM_TO_UINT8 (opcode,  p);
    BE_STREAM_TO_UINT32 (info_len, p);

    /* check version of SNEP */
    if (!nfa_snep_check_version (version))
    {
        nfa_snep_send_msg (NFA_SNEP_RESP_CODE_UNSUPP_VER, dlink);
        return FALSE;
    }

    /* check valid opcode for server */
    if (nfa_snep_cb.conn[dlink].flags & NFA_SNEP_FLAG_SERVER)
    {
        /* if this is response message */
        if (opcode & NFA_SNEP_RESP_CODE_CONTINUE)
        {
            SNEP_TRACE_ERROR0 ("Invalid opcode for server");

            /* application will free buffer when receiving NFA_SNEP_DISC_EVT */
            nfa_snep_cb.conn[dlink].p_ndef_buff = NULL;

            LLCP_DisconnectReq (nfa_snep_cb.conn[dlink].local_sap,
                                nfa_snep_cb.conn[dlink].remote_sap, TRUE);
            return FALSE;
        }
        else if (  (opcode != NFA_SNEP_REQ_CODE_CONTINUE)
                 &&(opcode != NFA_SNEP_REQ_CODE_GET)
                 &&(opcode != NFA_SNEP_REQ_CODE_PUT)
                 &&(opcode != NFA_SNEP_REQ_CODE_REJECT)  )
        {
            SNEP_TRACE_ERROR0 ("Not supported opcode for server");
            nfa_snep_send_msg (NFA_SNEP_RESP_CODE_NOT_IMPLM, dlink);
            return FALSE;
        }
    }
    /* check valid opcode for client */
    else
    {
        if (  (opcode != NFA_SNEP_RESP_CODE_CONTINUE)
            &&(opcode != NFA_SNEP_RESP_CODE_SUCCESS)
            &&(opcode != NFA_SNEP_RESP_CODE_NOT_FOUND)
            &&(opcode != NFA_SNEP_RESP_CODE_EXCESS_DATA)
            &&(opcode != NFA_SNEP_RESP_CODE_BAD_REQ)
            &&(opcode != NFA_SNEP_RESP_CODE_NOT_IMPLM)
            &&(opcode != NFA_SNEP_RESP_CODE_UNSUPP_VER)
            &&(opcode != NFA_SNEP_RESP_CODE_REJECT)  )
        {
            SNEP_TRACE_ERROR0 ("Invalid opcode for client");
            /* client cannot send error code so disconnect */
            /* application will free buffer when receiving NFA_SNEP_DISC_EVT */
            nfa_snep_cb.conn[dlink].p_ndef_buff = NULL;

            LLCP_DisconnectReq (nfa_snep_cb.conn[dlink].local_sap,
                                nfa_snep_cb.conn[dlink].remote_sap, TRUE);
            return FALSE;
        }
    }

    if (opcode == NFA_SNEP_REQ_CODE_GET)
    {
        more = LLCP_ReadDataLinkData (nfa_snep_cb.conn[dlink].local_sap,
                                      nfa_snep_cb.conn[dlink].remote_sap,
                                      NFA_SNEP_ACCEPT_LEN_SIZE,
                                      (uint32_t *)&length, buffer);

        if (length < NFA_SNEP_ACCEPT_LEN_SIZE)
        {
            /*
            ** Including acceptable length in the first segment is not mandated in spec
            ** but MIU is always big enough to include acceptable length field.
            */
            nfa_snep_send_msg (NFA_SNEP_RESP_CODE_BAD_REQ, dlink);
            return FALSE;
        }

        p = buffer;
        BE_STREAM_TO_UINT32 (nfa_snep_cb.conn[dlink].acceptable_length, p);

        /* store expected NDEF message length */
        nfa_snep_cb.conn[dlink].ndef_length = info_len - NFA_SNEP_ACCEPT_LEN_SIZE;
    }
    else if (  (opcode == NFA_SNEP_REQ_CODE_PUT)
             ||((opcode == NFA_SNEP_RESP_CODE_SUCCESS) && (nfa_snep_cb.conn[dlink].tx_code == NFA_SNEP_REQ_CODE_GET)))
    {
        /* store expected NDEF message length */
        nfa_snep_cb.conn[dlink].ndef_length = info_len;
    }
    else
    {
        if (more)
        {
            SNEP_TRACE_ERROR0 ("The information field shall not be transmitted with this request or response");

            if (nfa_snep_cb.conn[dlink].flags & NFA_SNEP_FLAG_SERVER)
            {
                nfa_snep_send_msg (NFA_SNEP_RESP_CODE_BAD_REQ, dlink);
            }
            /* client cannot send error code so disconnect */
            /* application will free buffer when receiving NFA_SNEP_DISC_EVT */
            nfa_snep_cb.conn[dlink].p_ndef_buff = NULL;

            LLCP_DisconnectReq (nfa_snep_cb.conn[dlink].local_sap,
                                nfa_snep_cb.conn[dlink].remote_sap, TRUE);
            return FALSE;
        }
    }

    /* store received opcode */
    nfa_snep_cb.conn[dlink].rx_code = opcode;

    return TRUE;
}

/*******************************************************************************
**
** Function         nfa_snep_store_first_rx_msg
**
** Description      Allocate buffer and store the first fragment
**
**
** Returns          TRUE if the received fragment is successfully stored
**
*******************************************************************************/
BOOLEAN nfa_snep_store_first_rx_msg (UINT8 dlink)
{
    tNFA_SNEP_EVT_DATA evt_data;
    BOOLEAN            more;
    UINT32             length;

    /* send event to upper layer of this data link connection to allocate buffer */
    evt_data.alloc.conn_handle = (NFA_HANDLE_GROUP_SNEP | dlink);
    evt_data.alloc.req_code    = nfa_snep_cb.conn[dlink].rx_code;
    evt_data.alloc.ndef_length = nfa_snep_cb.conn[dlink].ndef_length;
    evt_data.alloc.p_buff      = NULL;

    nfa_snep_cb.conn[dlink].p_cback (NFA_SNEP_ALLOC_BUFF_EVT, &evt_data);
    nfa_snep_cb.conn[dlink].p_ndef_buff = evt_data.alloc.p_buff;

    /* store information into application buffer */
    if (nfa_snep_cb.conn[dlink].p_ndef_buff)
    {
        /* store buffer size */
        nfa_snep_cb.conn[dlink].buff_length = evt_data.alloc.ndef_length;

        more = LLCP_ReadDataLinkData (nfa_snep_cb.conn[dlink].local_sap,
                                      nfa_snep_cb.conn[dlink].remote_sap,
                                      nfa_snep_cb.conn[dlink].buff_length,
                                      (uint32_t *)&length,
                                      nfa_snep_cb.conn[dlink].p_ndef_buff);

        /* store received message length */
        nfa_snep_cb.conn[dlink].cur_length  = (UINT32) length;

        SNEP_TRACE_DEBUG2 ("Received NDEF on SNEP, %d ouf of %d",
                           nfa_snep_cb.conn[dlink].cur_length,
                           nfa_snep_cb.conn[dlink].ndef_length);

        /* if fragmented */
        if (nfa_snep_cb.conn[dlink].ndef_length > nfa_snep_cb.conn[dlink].cur_length)
        {
            nfa_snep_cb.conn[dlink].rx_fragments = TRUE;
        }
        else if (more)
        {
            /* ignore extra bytes in the message */
            length = LLCP_FlushDataLinkRxData (nfa_snep_cb.conn[dlink].local_sap,
                                               nfa_snep_cb.conn[dlink].remote_sap);

            SNEP_TRACE_WARNING1 ("Received extra %d bytes on SNEP", length);
        }

        return TRUE;
    }
    else
    {
        SNEP_TRACE_ERROR1 ("Upper layer cannot allocate buffer for %d bytes",
                           nfa_snep_cb.conn[dlink].ndef_length);

        /* clear data in data link connection */
        length = LLCP_FlushDataLinkRxData (nfa_snep_cb.conn[dlink].local_sap,
                                           nfa_snep_cb.conn[dlink].remote_sap);

        /* if fragmented */
        if (nfa_snep_cb.conn[dlink].ndef_length > nfa_snep_cb.conn[dlink].cur_length)
        {
            /* notify peer not to send any more fragment */
            if (evt_data.alloc.resp_code != NFA_SNEP_RESP_CODE_NOT_IMPLM)
            {
                /* Set proper code */
                evt_data.alloc.resp_code = NFA_SNEP_REQ_CODE_REJECT;
            }
        }
        else
        {
            if (evt_data.alloc.resp_code != NFA_SNEP_RESP_CODE_NOT_IMPLM)
            {
                /* Set proper code */
                evt_data.alloc.resp_code = NFA_SNEP_RESP_CODE_NOT_FOUND;
            }
        }

        nfa_snep_send_msg (evt_data.alloc.resp_code, dlink);

        return FALSE;
    }
}

/*******************************************************************************
**
** Function         nfa_snep_proc_first_rx_msg
**
** Description      Process the first part of received message
**
**
** Returns          TRUE if it is not fragmented message
**                  FALSE if it is fragmented or found error
**
*******************************************************************************/
BOOLEAN nfa_snep_proc_first_rx_msg (UINT8 dlink)
{
    UINT32             length;
    tNFA_SNEP_EVT_DATA evt_data;
    BOOLEAN            more;

    SNEP_TRACE_DEBUG0 ("nfa_snep_proc_first_rx_msg ()");

    /* if version, opcode or length is not valid in received message */
    if (!nfa_snep_validate_rx_msg (dlink))
    {
        /* clear data in data link connection */
        LLCP_FlushDataLinkRxData (nfa_snep_cb.conn[dlink].local_sap,
                                  nfa_snep_cb.conn[dlink].remote_sap);
        return FALSE;
    }

    if (nfa_snep_cb.conn[dlink].rx_code == NFA_SNEP_REQ_CODE_GET)
    {
        /* if failed to allocate buffer */
        if (!nfa_snep_store_first_rx_msg (dlink))
        {
            return FALSE;
        }
        else
        {
            if (nfa_snep_cb.conn[dlink].rx_fragments == TRUE)
            {
                /* let peer send remaining fragments */
                nfa_snep_send_msg (NFA_SNEP_RESP_CODE_CONTINUE, dlink);

                return FALSE;
            }
        }
    }
    else if (nfa_snep_cb.conn[dlink].rx_code == NFA_SNEP_REQ_CODE_PUT)
    {
        /* if failed to allocate buffer */
        if (!nfa_snep_store_first_rx_msg (dlink))
        {
            return FALSE;
        }
        else
        {
            if (nfa_snep_cb.conn[dlink].rx_fragments == TRUE)
            {
                /* let peer send remaining fragments */
                nfa_snep_send_msg (NFA_SNEP_RESP_CODE_CONTINUE, dlink);
                return FALSE;
            }
        }
    }
    /* if we got response of GET request from server */
    else if (  (nfa_snep_cb.conn[dlink].rx_code == NFA_SNEP_RESP_CODE_SUCCESS)
             &&(nfa_snep_cb.conn[dlink].tx_code == NFA_SNEP_REQ_CODE_GET)  )
    {
        /* if server is sending more than acceptable length */
        if (nfa_snep_cb.conn[dlink].ndef_length > nfa_snep_cb.conn[dlink].acceptable_length)
        {
            SNEP_TRACE_ERROR0 ("Server is sending more than acceptable length");

            length = LLCP_FlushDataLinkRxData (nfa_snep_cb.conn[dlink].local_sap,
                                               nfa_snep_cb.conn[dlink].remote_sap);

            /* if fragmented */
            if (nfa_snep_cb.conn[dlink].ndef_length > length)
            {
                nfa_snep_send_msg (NFA_SNEP_REQ_CODE_REJECT, dlink);
                nfa_snep_cb.conn[dlink].rx_fragments = FALSE;
            }

            /* return error to client so buffer can be freed */
            evt_data.get_resp.conn_handle = (NFA_HANDLE_GROUP_SNEP | dlink);
            evt_data.get_resp.resp_code   = NFA_SNEP_RESP_CODE_EXCESS_DATA;
            evt_data.get_resp.ndef_length = 0;
            evt_data.get_resp.p_ndef      = nfa_snep_cb.conn[dlink].p_ndef_buff;

            nfa_snep_cb.conn[dlink].p_cback (NFA_SNEP_GET_RESP_EVT, &evt_data);
            nfa_snep_cb.conn[dlink].p_ndef_buff = NULL;

            return FALSE;
        }

        more = LLCP_ReadDataLinkData (nfa_snep_cb.conn[dlink].local_sap,
                                      nfa_snep_cb.conn[dlink].remote_sap,
                                      nfa_snep_cb.conn[dlink].buff_length,
                                      (uint32_t *)&length,
                                      nfa_snep_cb.conn[dlink].p_ndef_buff);

        /* store received message length */
        nfa_snep_cb.conn[dlink].cur_length = length;

        SNEP_TRACE_DEBUG2 ("Received NDEF on SNEP, %d ouf of %d",
                           nfa_snep_cb.conn[dlink].cur_length,
                           nfa_snep_cb.conn[dlink].ndef_length);

        if (nfa_snep_cb.conn[dlink].ndef_length > nfa_snep_cb.conn[dlink].cur_length)
        {
            nfa_snep_cb.conn[dlink].rx_fragments = TRUE;
        }
        else if (more)
        {
            /* ignore extra bytes in the message */
            length = LLCP_FlushDataLinkRxData (nfa_snep_cb.conn[dlink].local_sap,
                                               nfa_snep_cb.conn[dlink].remote_sap);

            SNEP_TRACE_WARNING1 ("Received extra %d bytes on SNEP", length);
        }

        if (nfa_snep_cb.conn[dlink].rx_fragments == TRUE)
        {
            /* let peer send remaining fragments */
            nfa_snep_send_msg (NFA_SNEP_REQ_CODE_CONTINUE, dlink);

            /* start timer for next fragment */
            nfa_sys_start_timer (&nfa_snep_cb.conn[dlink].timer, dlink, NFA_SNEP_CLIENT_TIMEOUT);

            return FALSE;
        }
    }

    /* other than above cases, there is no inforamtion field */

    return TRUE;
}

/*******************************************************************************
**
** Function         nfa_snep_assemble_fragments
**
** Description      Assemble fragments of SNEP message
**
**
** Returns          TRUE if it is not fragmented message
**                  FALSE if it is fragmented or found error
**
*******************************************************************************/
BOOLEAN nfa_snep_assemble_fragments (UINT8 dlink)
{
    BOOLEAN more;
    UINT32  length;

    more = LLCP_ReadDataLinkData (nfa_snep_cb.conn[dlink].local_sap,
                                  nfa_snep_cb.conn[dlink].remote_sap,
                                  nfa_snep_cb.conn[dlink].buff_length - nfa_snep_cb.conn[dlink].cur_length,
                                  (uint32_t *)&length,
                                  nfa_snep_cb.conn[dlink].p_ndef_buff + nfa_snep_cb.conn[dlink].cur_length);

    nfa_snep_cb.conn[dlink].cur_length += length;

    SNEP_TRACE_DEBUG2 ("Received NDEF on SNEP, %d ouf of %d",
                       nfa_snep_cb.conn[dlink].cur_length,
                       nfa_snep_cb.conn[dlink].ndef_length);

    /* if received the last fragment */
    if (nfa_snep_cb.conn[dlink].ndef_length == nfa_snep_cb.conn[dlink].cur_length)
    {
        nfa_snep_cb.conn[dlink].rx_fragments = FALSE;

        if (more)
        {
            length = LLCP_FlushDataLinkRxData (nfa_snep_cb.conn[dlink].local_sap,
                                               nfa_snep_cb.conn[dlink].remote_sap);

            SNEP_TRACE_ERROR2 ("Received extra %d bytes more than NDEF length (%d)",
                               length,
                               nfa_snep_cb.conn[dlink].ndef_length);

            /* application will free buffer when receiving NFA_SNEP_DISC_EVT */
            nfa_snep_cb.conn[dlink].p_ndef_buff = NULL;

            LLCP_DisconnectReq (nfa_snep_cb.conn[dlink].local_sap,
                                nfa_snep_cb.conn[dlink].remote_sap, TRUE);

            return FALSE;
        }
    }
    else
    {
        if (nfa_snep_cb.conn[dlink].flags & NFA_SNEP_FLAG_CLIENT)
        {
            /* wait for more fragments */
            nfa_sys_start_timer (&nfa_snep_cb.conn[dlink].timer, dlink, NFA_SNEP_CLIENT_TIMEOUT);
        }

        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
**
** Function         nfa_snep_proc_llcp_data_ind
**
** Description      Processing incoming data from LLCP
**
**
** Returns          None
**
*******************************************************************************/
void nfa_snep_proc_llcp_data_ind (tLLCP_SAP_CBACK_DATA  *p_data)
{
    UINT8              dlink;
    tNFA_SNEP_EVT_DATA evt_data;

    SNEP_TRACE_DEBUG0 ("nfa_snep_proc_llcp_data_ind ()");

    /* find connection control block with SAP */
    dlink = nfa_snep_sap_to_index (p_data->data_ind.local_sap,
                                   p_data->data_ind.remote_sap,
                                   NFA_SNEP_FLAG_ANY);

    /* if found */
    if (  (dlink < NFA_SNEP_MAX_CONN)
        &&(nfa_snep_cb.conn[dlink].flags & NFA_SNEP_FLAG_CONNECTED)  )
    {
        if (nfa_snep_cb.conn[dlink].flags & NFA_SNEP_FLAG_CLIENT)
        {
            /* stop timer for response from server */
            nfa_sys_stop_timer (&nfa_snep_cb.conn[dlink].timer);
        }

        /* if received the first fragment or complete SNEP message */
        if (nfa_snep_cb.conn[dlink].rx_fragments == FALSE)
        {
            if (!nfa_snep_proc_first_rx_msg (dlink))
            {
                /* need more data or found error */
                return;
            }
        }
        /* if received other than the first fragment */
        else
        {
            if (!nfa_snep_assemble_fragments (dlink))
            {
                /* need more data or found error */
                return;
            }
        }

        /* processing complete SNEP message */
        switch (nfa_snep_cb.conn[dlink].rx_code)
        {
        case NFA_SNEP_REQ_CODE_CONTINUE:
            if (nfa_snep_cb.conn[dlink].flags & NFA_SNEP_FLAG_W4_REQ_CONTINUE)
            {
                nfa_snep_cb.conn[dlink].flags &= ~NFA_SNEP_FLAG_W4_REQ_CONTINUE;

                /* send remaining fragments of GET response */
                nfa_snep_send_remaining (dlink);
            }
            else
            {
                SNEP_TRACE_ERROR0 ("Received invalid NFA_SNEP_REQ_CODE_CONTINUE");

                /* application will free buffer when receiving NFA_SNEP_DISC_EVT */
                nfa_snep_cb.conn[dlink].p_ndef_buff = NULL;

                LLCP_DisconnectReq (nfa_snep_cb.conn[dlink].local_sap,
                                    nfa_snep_cb.conn[dlink].remote_sap, TRUE);
            }
            break;

        case NFA_SNEP_REQ_CODE_GET:
            evt_data.get_req.conn_handle       = (NFA_HANDLE_GROUP_SNEP | dlink);
            evt_data.get_req.acceptable_length = nfa_snep_cb.conn[dlink].acceptable_length;

            /* NDEF message */
            evt_data.get_req.ndef_length = nfa_snep_cb.conn[dlink].ndef_length;
            evt_data.get_req.p_ndef      = nfa_snep_cb.conn[dlink].p_ndef_buff;

            nfa_snep_cb.conn[dlink].p_ndef_buff = NULL;

            /* send event to server of this data link connection */
            nfa_snep_cb.conn[dlink].p_cback (NFA_SNEP_GET_REQ_EVT, &evt_data);
            break;

        case NFA_SNEP_REQ_CODE_PUT:
            evt_data.put_req.conn_handle = (NFA_HANDLE_GROUP_SNEP | dlink);

            /* NDEF message */
            evt_data.put_req.ndef_length = nfa_snep_cb.conn[dlink].ndef_length;
            evt_data.put_req.p_ndef      = nfa_snep_cb.conn[dlink].p_ndef_buff;

            nfa_snep_cb.conn[dlink].p_ndef_buff = NULL;

            /* send event to server of this data link connection */
            nfa_snep_cb.conn[dlink].p_cback (NFA_SNEP_PUT_REQ_EVT, &evt_data);
            break;

        case NFA_SNEP_RESP_CODE_CONTINUE:
            if (nfa_snep_cb.conn[dlink].flags & NFA_SNEP_FLAG_W4_RESP_CONTINUE)
            {
                nfa_snep_cb.conn[dlink].flags &= ~NFA_SNEP_FLAG_W4_RESP_CONTINUE;
                /* send remaining fragments GET/PUT request */
                nfa_snep_send_remaining (dlink);
            }
            else
            {
                SNEP_TRACE_ERROR0 ("Received invalid NFA_SNEP_RESP_CODE_CONTINUE");

                /* application will free buffer when receiving NFA_SNEP_DISC_EVT */
                nfa_snep_cb.conn[dlink].p_ndef_buff = NULL;

                LLCP_DisconnectReq (nfa_snep_cb.conn[dlink].local_sap,
                                    nfa_snep_cb.conn[dlink].remote_sap, TRUE);
            }
            break;

        case NFA_SNEP_RESP_CODE_SUCCESS:
            if (nfa_snep_cb.conn[dlink].tx_code == NFA_SNEP_REQ_CODE_GET)
            {
                evt_data.get_resp.conn_handle = (NFA_HANDLE_GROUP_SNEP | dlink);
                evt_data.get_resp.resp_code   = NFA_SNEP_RESP_CODE_SUCCESS;
                evt_data.get_resp.ndef_length = nfa_snep_cb.conn[dlink].ndef_length;
                evt_data.get_resp.p_ndef      = nfa_snep_cb.conn[dlink].p_ndef_buff;

                nfa_snep_cb.conn[dlink].p_ndef_buff = NULL;

                /* send event to client of this data link connection */
                nfa_snep_cb.conn[dlink].p_cback (NFA_SNEP_GET_RESP_EVT, &evt_data);
            }
            else
            {
                evt_data.put_resp.conn_handle = (NFA_HANDLE_GROUP_SNEP | dlink);
                evt_data.put_resp.resp_code   = NFA_SNEP_RESP_CODE_SUCCESS;

                /* send event to client of this data link connection */
                nfa_snep_cb.conn[dlink].p_cback (NFA_SNEP_PUT_RESP_EVT, &evt_data);
            }
            break;

        case NFA_SNEP_RESP_CODE_NOT_FOUND:
        case NFA_SNEP_RESP_CODE_EXCESS_DATA:
        case NFA_SNEP_RESP_CODE_BAD_REQ:
        case NFA_SNEP_RESP_CODE_NOT_IMPLM:
        case NFA_SNEP_RESP_CODE_UNSUPP_VER:
        case NFA_SNEP_RESP_CODE_REJECT:
            /* if client sent GET request */
            if (nfa_snep_cb.conn[dlink].tx_code == NFA_SNEP_REQ_CODE_GET)
            {
                evt_data.get_resp.conn_handle = (NFA_HANDLE_GROUP_SNEP | dlink);
                evt_data.get_resp.resp_code   = nfa_snep_cb.conn[dlink].rx_code;
                evt_data.get_resp.ndef_length = 0;
                evt_data.get_resp.p_ndef      = nfa_snep_cb.conn[dlink].p_ndef_buff;

                /* send event to client of this data link connection */
                nfa_snep_cb.conn[dlink].p_cback (NFA_SNEP_GET_RESP_EVT, &evt_data);
            }
            /* if client sent PUT request */
            else if (nfa_snep_cb.conn[dlink].tx_code == NFA_SNEP_REQ_CODE_PUT)
            {
                evt_data.put_resp.conn_handle = (NFA_HANDLE_GROUP_SNEP | dlink);
                evt_data.put_resp.resp_code   = nfa_snep_cb.conn[dlink].rx_code;

                /* send event to client of this data link connection */
                nfa_snep_cb.conn[dlink].p_cback (NFA_SNEP_PUT_RESP_EVT, &evt_data);
            }

            /* if there is remaining SNEP message */
            if (nfa_snep_cb.conn[dlink].p_ndef_buff)
            {
                nfa_snep_cb.conn[dlink].p_ndef_buff = NULL;
            }
            break;
        }
    }
}

/*******************************************************************************
**
** Function         nfa_snep_proc_llcp_connect_ind
**
** Description      Processing connection request from peer
**
**
** Returns          None
**
*******************************************************************************/
void nfa_snep_proc_llcp_connect_ind (tLLCP_SAP_CBACK_DATA  *p_data)
{
    UINT8 server, dlink;
    tLLCP_CONNECTION_PARAMS params;
    tNFA_SNEP_EVT_DATA      evt_data;

    SNEP_TRACE_DEBUG0 ("nfa_snep_proc_llcp_connect_ind ()");

    server = nfa_snep_sap_to_index (p_data->connect_ind.server_sap,
                                    NFA_SNEP_ANY_SAP,
                                    NFA_SNEP_FLAG_SERVER);

    /* if found valid server */
    if (server < NFA_SNEP_MAX_CONN)
    {
        /* allocate connection control block for data link connection */
        dlink = nfa_snep_allocate_cb ();

        if (dlink < NFA_SNEP_MAX_CONN)
        {
            /* set data link connection's callback to server's callback */
            /* request will be sent to this server */
            nfa_snep_cb.conn[dlink].local_sap  = p_data->connect_ind.local_sap;
            nfa_snep_cb.conn[dlink].remote_sap = p_data->connect_ind.remote_sap;
            nfa_snep_cb.conn[dlink].p_cback    = nfa_snep_cb.conn[server].p_cback;
            nfa_snep_cb.conn[dlink].flags      = NFA_SNEP_FLAG_SERVER|NFA_SNEP_FLAG_CONNECTED;

            nfa_snep_cb.conn[dlink].tx_miu = nfa_snep_get_efficent_miu (p_data->connect_ind.miu,
                                                                        p_data->connect_ind.rw);

            /* accept connection request */
            params.miu = NFA_SNEP_MIU;
            params.rw  = NFA_SNEP_RW;
            params.sn[0] = 0;

            LLCP_ConnectCfm (p_data->connect_ind.local_sap,
                             p_data->connect_ind.remote_sap, &params);

            evt_data.connect.reg_handle  = (NFA_HANDLE_GROUP_SNEP | server);
            evt_data.connect.conn_handle = (NFA_HANDLE_GROUP_SNEP | dlink);
            nfa_snep_cb.conn[dlink].p_cback (NFA_SNEP_CONNECTED_EVT, &evt_data);
        }
        else
        {
            SNEP_TRACE_ERROR0 ("Cannot allocate connection control block");
            LLCP_ConnectReject (p_data->connect_ind.local_sap,
                                p_data->connect_ind.remote_sap,
                                LLCP_SAP_DM_REASON_TEMP_REJECT_THIS);
        }
    }
    else
    {
        SNEP_TRACE_ERROR0 ("Cannot find SNEP server");
        LLCP_ConnectReject (p_data->connect_ind.local_sap,
                            p_data->connect_ind.remote_sap,
                            LLCP_SAP_DM_REASON_NO_SERVICE);
    }
}

/*******************************************************************************
**
** Function         nfa_snep_proc_llcp_connect_resp
**
** Description      Processing connection response from peer
**
**
** Returns          None
**
*******************************************************************************/
void nfa_snep_proc_llcp_connect_resp (tLLCP_SAP_CBACK_DATA  *p_data)
{
    UINT8 dlink;
    tNFA_SNEP_EVT_DATA evt_data;

    SNEP_TRACE_DEBUG0 ("nfa_snep_proc_llcp_connect_resp ()");

    /* find client by SAP */
    dlink = nfa_snep_sap_to_index (p_data->connect_resp.local_sap,
                                   NFA_SNEP_ANY_SAP,
                                   NFA_SNEP_FLAG_CLIENT|NFA_SNEP_FLAG_CONNECTING);

    /* if found client */
    if (dlink < NFA_SNEP_MAX_CONN)
    {
        nfa_snep_cb.conn[dlink].remote_sap = p_data->connect_resp.remote_sap;
        nfa_snep_cb.conn[dlink].flags      = NFA_SNEP_FLAG_CLIENT|NFA_SNEP_FLAG_CONNECTED;

        nfa_snep_cb.conn[dlink].tx_miu = nfa_snep_get_efficent_miu (p_data->connect_resp.miu,
                                                                    p_data->connect_resp.rw);

        evt_data.connect.reg_handle  = (NFA_HANDLE_GROUP_SNEP | dlink);
        evt_data.connect.conn_handle = (NFA_HANDLE_GROUP_SNEP | dlink);
        nfa_snep_cb.conn[dlink].p_cback (NFA_SNEP_CONNECTED_EVT, &evt_data);
    }
    else
    {
        SNEP_TRACE_ERROR0 ("Cannot find SNEP client");
        LLCP_DisconnectReq (p_data->connect_resp.local_sap,
                            p_data->connect_resp.remote_sap, TRUE);
    }
}

/*******************************************************************************
**
** Function         nfa_snep_proc_llcp_disconnect_ind
**
** Description      Processing disconnection request from peer
**
**
** Returns          None
**
*******************************************************************************/
void nfa_snep_proc_llcp_disconnect_ind (tLLCP_SAP_CBACK_DATA  *p_data)
{
    UINT8              dlink;
    tNFA_SNEP_EVT_DATA evt_data;

    SNEP_TRACE_DEBUG0 ("nfa_snep_proc_llcp_disconnect_ind ()");

    /* find connection control block by SAP */
    dlink = nfa_snep_sap_to_index (p_data->disconnect_ind.local_sap,
                                   p_data->disconnect_ind.remote_sap,
                                   NFA_SNEP_FLAG_ANY);

    /* if found */
    if (dlink < NFA_SNEP_MAX_CONN)
    {
        evt_data.disc.conn_handle = (NFA_HANDLE_GROUP_SNEP | dlink);

        nfa_snep_cb.conn[dlink].p_cback (NFA_SNEP_DISC_EVT, &evt_data);

        if (nfa_snep_cb.conn[dlink].flags & NFA_SNEP_FLAG_CLIENT)
        {
            /* clear other flags */
            nfa_snep_cb.conn[dlink].flags      = NFA_SNEP_FLAG_CLIENT;
            nfa_snep_cb.conn[dlink].remote_sap = LLCP_INVALID_SAP;
        }
        else
        {
            nfa_snep_deallocate_cb (dlink);
        }
    }
    else
    {
        SNEP_TRACE_ERROR0 ("Cannot find SNEP connection");
    }
}

/*******************************************************************************
**
** Function         nfa_snep_proc_llcp_disconnect_resp
**
** Description      Processing rejected connection from peer
**
**
** Returns          None
**
*******************************************************************************/
void nfa_snep_proc_llcp_disconnect_resp (tLLCP_SAP_CBACK_DATA  *p_data)
{
    UINT8              dlink, flags;
    UINT8              remote_sap;
    tNFA_SNEP_EVT_DATA evt_data;

    SNEP_TRACE_DEBUG0 ("nfa_snep_proc_llcp_disconnect_resp ()");

    /* if remote sent response to disconnection requested by local */
    if (p_data->disconnect_resp.reason == LLCP_SAP_DM_REASON_RESP_DISC)
    {
        remote_sap = p_data->disconnect_resp.remote_sap;
        flags      = NFA_SNEP_FLAG_CLIENT|NFA_SNEP_FLAG_CONNECTED;
    }
    else /* connection failed so we don't have remote SAP */
    {
        remote_sap = NFA_SNEP_ANY_SAP;
        flags      = NFA_SNEP_FLAG_CLIENT|NFA_SNEP_FLAG_CONNECTING;
    }

    /* find connection control block by SAP */
    dlink = nfa_snep_sap_to_index (p_data->disconnect_resp.local_sap,
                                   remote_sap,
                                   flags);

    /* if found client */
    if (dlink < NFA_SNEP_MAX_CONN)
    {
        evt_data.disc.conn_handle = (NFA_HANDLE_GROUP_SNEP | dlink);

        nfa_snep_cb.conn[dlink].p_cback (NFA_SNEP_DISC_EVT, &evt_data);

        /* clear other flags */
        nfa_snep_cb.conn[dlink].flags      = NFA_SNEP_FLAG_CLIENT;
        nfa_snep_cb.conn[dlink].remote_sap = LLCP_INVALID_SAP;
    }
    else
    {
        /* find server connection control block by SAP */
        dlink = nfa_snep_sap_to_index (p_data->disconnect_resp.local_sap,
                                       remote_sap,
                                       NFA_SNEP_FLAG_SERVER|NFA_SNEP_FLAG_CONNECTED);

        /* if found server connection */
        if (dlink < NFA_SNEP_MAX_CONN)
        {
            evt_data.disc.conn_handle = (NFA_HANDLE_GROUP_SNEP | dlink);

            nfa_snep_cb.conn[dlink].p_cback (NFA_SNEP_DISC_EVT, &evt_data);

            nfa_snep_deallocate_cb (dlink);
        }
        else
        {
            SNEP_TRACE_ERROR0 ("Cannot find SNEP connection");
        }
    }
}

/*******************************************************************************
**
** Function         nfa_snep_proc_llcp_congest
**
** Description      Processing LLCP congestion event
**
**
** Returns          None
**
*******************************************************************************/
void nfa_snep_proc_llcp_congest (tLLCP_SAP_CBACK_DATA  *p_data)
{
    UINT8 dlink;

    SNEP_TRACE_DEBUG3 ("nfa_snep_proc_llcp_congest () local_sap=0x%x, remote_sap=0x%x, is_congested=%d",
                       p_data->congest.local_sap,
                       p_data->congest.remote_sap,
                       p_data->congest.is_congested);

    /* if data link connection is congested */
    if (p_data->congest.link_type == LLCP_LINK_TYPE_DATA_LINK_CONNECTION)
    {
        dlink = nfa_snep_sap_to_index (p_data->congest.local_sap,
                                       p_data->congest.remote_sap,
                                       NFA_SNEP_FLAG_ANY);

        if (  (dlink < NFA_SNEP_MAX_CONN)
            &&(nfa_snep_cb.conn[dlink].flags & NFA_SNEP_FLAG_CONNECTED)  )
        {
            nfa_snep_cb.conn[dlink].congest = p_data->congest.is_congested;

            if (!nfa_snep_cb.conn[dlink].congest)
            {
                /* if received CONTINUE then continue to send remaining fragments */
                if (  (nfa_snep_cb.conn[dlink].rx_code == NFA_SNEP_REQ_CODE_CONTINUE)
                    ||(nfa_snep_cb.conn[dlink].rx_code == NFA_SNEP_RESP_CODE_CONTINUE)  )
                {
                    nfa_snep_send_remaining (dlink);
                }
            }
        }
    }
}

/*******************************************************************************
**
** Function         nfa_snep_proc_llcp_link_status
**
** Description      Processing LLCP link status
**
**
** Returns          none
**
*******************************************************************************/
void nfa_snep_proc_llcp_link_status (tLLCP_SAP_CBACK_DATA  *p_data)
{
    UINT8              xx;
    tNFA_SNEP_EVT_DATA evt_data;

    SNEP_TRACE_DEBUG1 ("nfa_snep_proc_llcp_link_status () is_activated:%d",
                       p_data->link_status.is_activated);

    xx = nfa_snep_sap_to_index (p_data->link_status.local_sap,
                                NFA_SNEP_ANY_SAP,
                                NFA_SNEP_FLAG_CLIENT);

    if (xx < NFA_SNEP_MAX_CONN)
    {
        evt_data.activated.client_handle = (NFA_HANDLE_GROUP_SNEP | xx);

        /* if LLCP link is activated */
        if (p_data->link_status.is_activated == TRUE)
        {
            /* notify only client which may want to connect */
            nfa_snep_cb.conn[xx].p_cback (NFA_SNEP_ACTIVATED_EVT, &evt_data);
        }
        else
        {
            /* LLCP link is deactivated */
            nfa_snep_cb.conn[xx].p_cback (NFA_SNEP_DEACTIVATED_EVT, &evt_data);
        }
    }
}

/*******************************************************************************
**
** Function         nfa_snep_proc_llcp_tx_complete
**
** Description      Processing LLCP tx complete event
**
**
** Returns          none
**
*******************************************************************************/
void nfa_snep_proc_llcp_tx_complete (tLLCP_SAP_CBACK_DATA  *p_data)
{
    UINT8              dlink;
    tNFA_SNEP_EVT_DATA evt_data;

    SNEP_TRACE_DEBUG0 ("nfa_snep_proc_llcp_tx_complete ()");

    dlink = nfa_snep_sap_to_index (p_data->tx_complete.local_sap,
                                   p_data->tx_complete.remote_sap,
                                   NFA_SNEP_FLAG_SERVER|NFA_SNEP_FLAG_CONNECTED);

    if (dlink < NFA_SNEP_MAX_CONN)
    {
        /* notify upper layer that transmission is complete */
        evt_data.get_resp_cmpl.conn_handle = (NFA_HANDLE_GROUP_SNEP | dlink);
        evt_data.get_resp_cmpl.p_buff      = nfa_snep_cb.conn[dlink].p_ndef_buff;

        nfa_snep_cb.conn[dlink].p_cback (NFA_SNEP_GET_RESP_CMPL_EVT, &evt_data);
        nfa_snep_cb.conn[dlink].p_ndef_buff = NULL;
    }
}

/*******************************************************************************
**
** Function         nfa_snep_reg_server
**
** Description      Allocate a connection control block as server and register to LLCP
**
**
** Returns          TRUE to deallocate message
**
*******************************************************************************/
BOOLEAN nfa_snep_reg_server (tNFA_SNEP_MSG *p_msg)
{
    tNFA_SNEP_EVT_DATA  evt_data;
    UINT8               xx, local_sap = LLCP_INVALID_SAP;

    SNEP_TRACE_DEBUG0 ("nfa_snep_reg_server ()");

    xx = nfa_snep_allocate_cb ();

    if (xx < NFA_SNEP_MAX_CONN)
    {
        local_sap = LLCP_RegisterServer (p_msg->api_reg_server.server_sap,
                                         LLCP_LINK_TYPE_DATA_LINK_CONNECTION,
                                         p_msg->api_reg_server.service_name,
                                         nfa_snep_llcp_cback);
    }

    BCM_STRNCPY_S (evt_data.reg.service_name, sizeof (evt_data.reg.service_name),
                   p_msg->api_reg_server.service_name, LLCP_MAX_SN_LEN);
    evt_data.reg.service_name[LLCP_MAX_SN_LEN] = 0x00;

    if ((xx == NFA_SNEP_MAX_CONN) || (local_sap == LLCP_INVALID_SAP))
    {
        SNEP_TRACE_ERROR0 ("Cannot allocate or register SNEP server");

        evt_data.reg.status = NFA_STATUS_FAILED;
        p_msg->api_reg_server.p_cback (NFA_SNEP_REG_EVT, &evt_data);
        return TRUE;
    }

    if (!nfa_snep_cb.is_dta_mode)
    {
        /* if need to update WKS in LLCP Gen bytes */
        if (local_sap <= LLCP_UPPER_BOUND_WK_SAP)
        {
            nfa_p2p_enable_listening (NFA_ID_SNEP, TRUE);
            nfa_snep_cb.listen_enabled = TRUE;
        }
        else if (!nfa_snep_cb.listen_enabled)
        {
            nfa_p2p_enable_listening (NFA_ID_SNEP, FALSE);
            nfa_snep_cb.listen_enabled = TRUE;
        }
    }

    nfa_snep_cb.conn[xx].local_sap  = local_sap;
    nfa_snep_cb.conn[xx].remote_sap = LLCP_INVALID_SAP;
    nfa_snep_cb.conn[xx].p_cback    = p_msg->api_reg_server.p_cback;
    nfa_snep_cb.conn[xx].flags      = NFA_SNEP_FLAG_SERVER;

    evt_data.reg.status     = NFA_STATUS_OK;
    evt_data.reg.reg_handle = (NFA_HANDLE_GROUP_SNEP | xx);

    /* notify NFA_SNEP_REG_EVT to application */
    nfa_snep_cb.conn[xx].p_cback (NFA_SNEP_REG_EVT, &evt_data);

    return TRUE;
}

/*******************************************************************************
**
** Function         nfa_snep_reg_client
**
** Description      Allocate a connection control block as client and register to LLCP
**
**
** Returns          TRUE to deallocate message
**
*******************************************************************************/
BOOLEAN nfa_snep_reg_client (tNFA_SNEP_MSG *p_msg)
{
    tNFA_SNEP_EVT_DATA  evt_data;
    UINT8               xx, local_sap = LLCP_INVALID_SAP;

    SNEP_TRACE_DEBUG0 ("nfa_snep_reg_client ()");

    xx = nfa_snep_allocate_cb ();

    if (xx < NFA_SNEP_MAX_CONN)
    {
        local_sap = LLCP_RegisterClient (LLCP_LINK_TYPE_DATA_LINK_CONNECTION,
                                         nfa_snep_llcp_cback);
    }

    evt_data.reg.service_name[0] = 0x00;

    if ((xx == NFA_SNEP_MAX_CONN) || (local_sap == LLCP_INVALID_SAP))
    {
        SNEP_TRACE_ERROR0 ("Cannot allocate or register SNEP client");

        evt_data.reg.status = NFA_STATUS_FAILED;
        p_msg->api_reg_client.p_cback (NFA_SNEP_REG_EVT, &evt_data);
        return TRUE;
    }

    nfa_snep_cb.conn[xx].local_sap  = local_sap;
    nfa_snep_cb.conn[xx].remote_sap = LLCP_INVALID_SAP;
    nfa_snep_cb.conn[xx].p_cback    = p_msg->api_reg_client.p_cback;
    nfa_snep_cb.conn[xx].flags      = NFA_SNEP_FLAG_CLIENT;

    /* initialize timer callback */
    //nfa_snep_cb.conn[xx].timer.p_cback = nfa_snep_timer_cback;

    evt_data.reg.status     = NFA_STATUS_OK;
    evt_data.reg.reg_handle = (NFA_HANDLE_GROUP_SNEP | xx);

    /* notify NFA_SNEP_REG_EVT to application */
    nfa_snep_cb.conn[xx].p_cback (NFA_SNEP_REG_EVT, &evt_data);

    return TRUE;
}

/*******************************************************************************
**
** Function         nfa_snep_dereg
**
** Description      Deallocate a connection control block and deregister to LLCP
**                  LLCP will deallocate any data link connection created for this
**
** Returns          TRUE to deallocate message
**
*******************************************************************************/
BOOLEAN nfa_snep_dereg (tNFA_SNEP_MSG *p_msg)
{
    UINT8 xx;
    UINT8 local_sap;

    SNEP_TRACE_DEBUG0 ("nfa_snep_dereg ()");

    xx = (UINT8) (p_msg->api_dereg.reg_handle & NFA_HANDLE_MASK);

    if (  (xx < NFA_SNEP_MAX_CONN)
        &&(nfa_snep_cb.conn[xx].p_cback)
        &&(nfa_snep_cb.conn[xx].flags & (NFA_SNEP_FLAG_SERVER|NFA_SNEP_FLAG_CLIENT))  )
    {
        local_sap = nfa_snep_cb.conn[xx].local_sap;
        LLCP_Deregister (local_sap);
        nfa_snep_deallocate_cb (xx);
    }
    else
    {
        SNEP_TRACE_ERROR0 ("Cannot find SNEP server/client");
        return TRUE;
    }

    if (!nfa_snep_cb.is_dta_mode)
    {
        if (nfa_snep_cb.listen_enabled)
        {
            for (xx = 0; xx < NFA_SNEP_MAX_CONN; xx++)
            {
                if (  (nfa_snep_cb.conn[xx].p_cback)
                    &&(nfa_snep_cb.conn[xx].flags & NFA_SNEP_FLAG_SERVER)  )
                {
                    break;
                }
            }

            if (xx >= NFA_SNEP_MAX_CONN)
            {
                /* if need to update WKS in LLCP Gen bytes */
                if (local_sap <= LLCP_UPPER_BOUND_WK_SAP)
                    nfa_p2p_disable_listening (NFA_ID_SNEP, TRUE);
                else
                    nfa_p2p_disable_listening (NFA_ID_SNEP, FALSE);

                nfa_snep_cb.listen_enabled = FALSE;
            }
            /* if need to update WKS in LLCP Gen bytes */
            else if (local_sap <= LLCP_UPPER_BOUND_WK_SAP)
            {
                nfa_p2p_enable_listening (NFA_ID_SNEP, TRUE);
            }
        }
    }
    return TRUE;
}

/*******************************************************************************
**
** Function         nfa_snep_connect
**
** Description      Create data link connection for client
**
** Returns          TRUE to deallocate message
**
*******************************************************************************/
BOOLEAN nfa_snep_connect (tNFA_SNEP_MSG *p_msg)
{
    tLLCP_CONNECTION_PARAMS conn_params;
    UINT8 xx;

    SNEP_TRACE_DEBUG0 ("nfa_snep_connect ()");

    xx = (UINT8) (p_msg->api_connect.client_handle & NFA_HANDLE_MASK);

    if (xx < NFA_SNEP_MAX_CONN)
    {
        nfa_snep_cb.conn[xx].congest = FALSE;

        /* Set remote_sap to SDP to find callback in case that link is deactivted before connected */
        nfa_snep_cb.conn[xx].remote_sap = LLCP_SAP_SDP;

        /* in order to send NFA_SNEP_DISC_EVT in case of connection failure */
        nfa_snep_cb.conn[xx].flags = NFA_SNEP_FLAG_CLIENT|NFA_SNEP_FLAG_CONNECTING;

        /* create data link connection with server name */
        conn_params.miu = NFA_SNEP_MIU;
        conn_params.rw  = NFA_SNEP_RW;
        BCM_STRNCPY_S (conn_params.sn, sizeof (conn_params.sn),
                       p_msg->api_connect.service_name, LLCP_MAX_SN_LEN);
        conn_params.sn[LLCP_MAX_SN_LEN] = 0;

        LLCP_ConnectReq (nfa_snep_cb.conn[xx].local_sap, LLCP_SAP_SDP, &conn_params);
    }
    return TRUE;
}

/*******************************************************************************
**
** Function         nfa_snep_get_req
**
** Description      Send SNEP GET request on data link connection
**
** Returns          TRUE to deallocate message
**
*******************************************************************************/
BOOLEAN nfa_snep_get_req (tNFA_SNEP_MSG *p_msg)
{
    UINT8 dlink;

    SNEP_TRACE_DEBUG0 ("nfa_snep_get_req ()");

    dlink = (UINT8) (p_msg->api_get_req.conn_handle & NFA_HANDLE_MASK);

    if (  (dlink < NFA_SNEP_MAX_CONN)
        &&(nfa_snep_cb.conn[dlink].flags & NFA_SNEP_FLAG_CONNECTED)  )
    {
        nfa_snep_cb.conn[dlink].tx_code           = NFA_SNEP_REQ_CODE_GET;
        nfa_snep_cb.conn[dlink].buff_length       = p_msg->api_get_req.buff_length;
        nfa_snep_cb.conn[dlink].ndef_length       = p_msg->api_get_req.ndef_length;
        nfa_snep_cb.conn[dlink].p_ndef_buff       = p_msg->api_get_req.p_ndef_buff;
        nfa_snep_cb.conn[dlink].acceptable_length = p_msg->api_get_req.buff_length;

        nfa_snep_send_msg (NFA_SNEP_REQ_CODE_GET, dlink);

        /* start timer for response from server */
        nfa_sys_start_timer (&nfa_snep_cb.conn[dlink].timer, dlink, NFA_SNEP_CLIENT_TIMEOUT);
    }
    else
    {
        SNEP_TRACE_ERROR0 ("Data link connection is not established");
    }
    return TRUE;
}

/*******************************************************************************
**
** Function         nfa_snep_put_req
**
** Description      Send SNEP PUT request on data link connection
**
** Returns          TRUE to deallocate message
**
*******************************************************************************/
BOOLEAN nfa_snep_put_req (tNFA_SNEP_MSG *p_msg)
{
    UINT8 dlink;

    SNEP_TRACE_DEBUG0 ("nfa_snep_put_req ()");

    dlink = (UINT8) (p_msg->api_put_req.conn_handle & NFA_HANDLE_MASK);

    if (  (dlink < NFA_SNEP_MAX_CONN)
        &&(nfa_snep_cb.conn[dlink].flags & NFA_SNEP_FLAG_CONNECTED)  )
    {
        nfa_snep_cb.conn[dlink].tx_code     = NFA_SNEP_REQ_CODE_PUT;
        nfa_snep_cb.conn[dlink].buff_length = p_msg->api_put_req.ndef_length;
        nfa_snep_cb.conn[dlink].ndef_length = p_msg->api_put_req.ndef_length;
        nfa_snep_cb.conn[dlink].p_ndef_buff = p_msg->api_put_req.p_ndef_buff;

        nfa_snep_send_msg (NFA_SNEP_REQ_CODE_PUT, dlink);

        /* start timer for response from server */
        nfa_sys_start_timer (&nfa_snep_cb.conn[dlink].timer, dlink, NFA_SNEP_CLIENT_TIMEOUT);
    }
    else
    {
        SNEP_TRACE_ERROR0 ("Data link connection is not established");
    }
    return TRUE;
}

/*******************************************************************************
**
** Function         nfa_snep_get_resp
**
** Description      Server responds to GET request
**
**
** Returns          TRUE to deallocate message
**
*******************************************************************************/
BOOLEAN nfa_snep_get_resp (tNFA_SNEP_MSG *p_msg)
{
    UINT8 dlink;

    SNEP_TRACE_DEBUG0 ("nfa_snep_get_resp ()");

    dlink = (UINT8) (p_msg->api_get_resp.conn_handle & NFA_HANDLE_MASK);

    if (  (dlink < NFA_SNEP_MAX_CONN)
        &&(nfa_snep_cb.conn[dlink].flags & NFA_SNEP_FLAG_CONNECTED)  )
    {
        nfa_snep_cb.conn[dlink].buff_length = p_msg->api_get_resp.ndef_length;
        nfa_snep_cb.conn[dlink].ndef_length = p_msg->api_get_resp.ndef_length;
        nfa_snep_cb.conn[dlink].p_ndef_buff = p_msg->api_get_resp.p_ndef_buff;

        nfa_snep_cb.conn[dlink].tx_code     = p_msg->api_get_resp.resp_code;

        nfa_snep_send_msg (p_msg->api_get_resp.resp_code, dlink);
    }
    else
    {
        SNEP_TRACE_ERROR0 ("Data link connection is not established");
    }
    return TRUE;
}

/*******************************************************************************
**
** Function         nfa_snep_put_resp
**
** Description      Server responds to PUT request
**
**
** Returns          TRUE to deallocate message
**
*******************************************************************************/
BOOLEAN nfa_snep_put_resp (tNFA_SNEP_MSG *p_msg)
{
    UINT8 dlink;

    SNEP_TRACE_DEBUG0 ("nfa_snep_put_resp ()");

    dlink = (UINT8) (p_msg->api_put_resp.conn_handle & NFA_HANDLE_MASK);

    if (  (dlink < NFA_SNEP_MAX_CONN)
        &&(nfa_snep_cb.conn[dlink].flags & NFA_SNEP_FLAG_CONNECTED)  )
    {
        nfa_snep_cb.conn[dlink].tx_code = p_msg->api_put_resp.resp_code;

        nfa_snep_send_msg (p_msg->api_put_resp.resp_code, dlink);
    }
    else
    {
        SNEP_TRACE_ERROR0 ("Data link connection is not established");
    }
    return TRUE;
}

/*******************************************************************************
**
** Function         nfa_snep_disconnect
**
** Description      Disconnect data link connection
**
**
** Returns          TRUE to deallocate message
**
*******************************************************************************/
BOOLEAN nfa_snep_disconnect (tNFA_SNEP_MSG *p_msg)
{
    UINT8 dlink;

    SNEP_TRACE_DEBUG0 ("nfa_snep_disconnect ()");

    dlink = (UINT8) (p_msg->api_disc.conn_handle & NFA_HANDLE_MASK);

    if (  (dlink < NFA_SNEP_MAX_CONN)
        &&(nfa_snep_cb.conn[dlink].flags & NFA_SNEP_FLAG_CONNECTED)  )
    {
        LLCP_DisconnectReq (nfa_snep_cb.conn[dlink].local_sap,
                            nfa_snep_cb.conn[dlink].remote_sap,
                            p_msg->api_disc.flush);
    }
    else
    {
        SNEP_TRACE_ERROR0 ("Data link connection is not established");
    }
    return TRUE;
}

#if (BT_TRACE_VERBOSE == TRUE)
/*******************************************************************************
**
** Function         nfa_snep_opcode
**
** Description
**
** Returns          string of event
**
*******************************************************************************/
static char *nfa_snep_opcode (UINT8 opcode)
{
    switch (opcode)
    {
    case NFA_SNEP_REQ_CODE_CONTINUE:
        return "REQ_CONTINUE";
    case NFA_SNEP_REQ_CODE_GET:
        return "REQ_GET";
    case NFA_SNEP_REQ_CODE_PUT:
        return "REQ_PUT";
    case NFA_SNEP_REQ_CODE_REJECT:
        return "REQ_REJECT";

    case NFA_SNEP_RESP_CODE_CONTINUE:
        return "RESP_CONTINUE";
    case NFA_SNEP_RESP_CODE_SUCCESS:
        return "RESP_SUCCESS";
    case NFA_SNEP_RESP_CODE_NOT_FOUND:
        return "RESP_NOT_FOUND";
    case NFA_SNEP_RESP_CODE_EXCESS_DATA:
        return "RESP_EXCESS_DATA";
    case NFA_SNEP_RESP_CODE_BAD_REQ:
        return "RESP_BAD_REQ";
    case NFA_SNEP_RESP_CODE_NOT_IMPLM:
        return "RESP_NOT_IMPLM";
    case NFA_SNEP_RESP_CODE_UNSUPP_VER:
        return "RESP_UNSUPP_VER";
    case NFA_SNEP_RESP_CODE_REJECT:
        return "RESP_REJECT";

    default:
        return "Reserved opcode";
    }
}

#endif  /* Debug Functions */

