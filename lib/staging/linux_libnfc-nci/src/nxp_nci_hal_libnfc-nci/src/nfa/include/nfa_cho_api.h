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
 *  The original Work has been changed by NXP Semiconductors.
 *
 *  Copyright (C) 2015 NXP Semiconductors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
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
 *  This is the public interface file for NFA Connection Handover,
 *  Broadcom's NFC application layer for mobile phones.
 *
 ******************************************************************************/
#ifndef NFA_CHO_API_H
#define NFA_CHO_API_H

#include "nfa_api.h"
#include "ndef_utils.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* Handover version */
#define NFA_CHO_VERSION             0x12    /* version 1.2 */
#define NFA_CHO_GET_MAJOR_VERSION(x) ((UINT8)(x) >> 4)
#define NFA_CHO_GET_MINOR_VERSION(x) ((UINT8)(x) & 0x0F)

/*
** NFA Connection Handover callback events
*/
#define NFA_CHO_REG_EVT             0x00    /* Registered                       */
#define NFA_CHO_ACTIVATED_EVT       0x01    /* LLCP link activated              */
#define NFA_CHO_DEACTIVATED_EVT     0x02    /* LLCP link deactivated            */
#define NFA_CHO_CONNECTED_EVT       0x03    /* data link connected              */
#define NFA_CHO_DISCONNECTED_EVT    0x04    /* data link disconnected           */
#define NFA_CHO_REQUEST_EVT         0x05    /* AC information in "Hr" record    */
#define NFA_CHO_SELECT_EVT          0x06    /* AC information in "Hs" record    */
#define NFA_CHO_SEL_ERR_EVT         0x07    /* Received select with error       */
#define NFA_CHO_TX_FAIL_EVT         0x08    /* Cannot send message to peer      */

typedef UINT8 tNFA_CHO_EVT;

/*
** Data for NFA_CHO_ACTIVATED_EVT
*/
typedef struct
{
    BOOLEAN         is_initiator;   /* TRUE if local LLCP is initiator */
} tNFA_CHO_ACTIVATED;

/* NFA Connection Handover Carrier Power State */
#define NFA_CHO_CPS_INACTIVE        0x00    /* Carrier is currently off         */
#define NFA_CHO_CPS_ACTIVE          0x01    /* Carrier is currently on          */
#define NFA_CHO_CPS_ACTIVATING      0x02    /* Activating carrier               */
#define NFA_CHO_CPS_UNKNOWN         0x03    /* Unknown                          */

typedef UINT8 tNFA_CHO_CPS;

/* Data for Alternative Carrier Information */
typedef struct
{
    tNFA_CHO_CPS        cps;            /* carrier power state                      */
    UINT8               num_aux_data;   /* number of Auxiliary NDEF records         */
} tNFA_CHO_AC_INFO;

/* Device Role of Handover */
#define NFA_CHO_ROLE_REQUESTER  0
#define NFA_CHO_ROLE_SELECTOR   1
#define NFA_CHO_ROLE_UNDECIDED  2

typedef UINT8 tNFA_CHO_ROLE_TYPE;

/*
** Data for NFA_CHO_CONNECTED_EVT
*/
typedef struct
{
    tNFA_CHO_ROLE_TYPE  initial_role;   /* NFA_CHO_ROLE_REQUESTER if local initiated */
                                        /* NFA_CHO_ROLE_SELECTOR if remote initiated */
} tNFA_CHO_CONNECTED;

/* Disconnected reason */
#define NFA_CHO_DISC_REASON_API_REQUEST         0
#define NFA_CHO_DISC_REASON_ALEADY_CONNECTED    1
#define NFA_CHO_DISC_REASON_CONNECTION_FAIL     2
#define NFA_CHO_DISC_REASON_PEER_REQUEST        3
#define NFA_CHO_DISC_REASON_LINK_DEACTIVATED    4
#define NFA_CHO_DISC_REASON_TIMEOUT             5
#define NFA_CHO_DISC_REASON_UNKNOWN_MSG         6
#define NFA_CHO_DISC_REASON_INVALID_MSG         7
#define NFA_CHO_DISC_REASON_SEMANTIC_ERROR      8
#define NFA_CHO_DISC_REASON_INTERNAL_ERROR      9

typedef UINT8 tNFA_CHO_DISC_REASON;

/*
** Data for NFA_CHO_DISCONNECTED_EVT
*/
typedef struct
{
    tNFA_CHO_DISC_REASON    reason;     /* disconnected reason */
} tNFA_CHO_DISCONNECTED;

/* Reference ID */
typedef struct
{
    UINT8               ref_len;
    UINT8               ref_name[NFA_CHO_MAX_REF_NAME_LEN];
} tNFA_CHO_REF_ID;

/* Alternative Carrier records including carrier power state, carrier data reference and aux data reference */
typedef struct
{
    tNFA_CHO_CPS        cps;                                      /* carrier power state    */
    tNFA_CHO_REF_ID     carrier_data_ref;                         /* carrier data reference */
    UINT8               aux_data_ref_count;                       /* number of aux data     */
    tNFA_CHO_REF_ID     aux_data_ref[NFA_CHO_MAX_AUX_DATA_COUNT]; /* aux data reference     */
} tNFA_CHO_AC_REC;

/*
** Data for NFA_CHO_REQUEST_EVT
** Application may receive it while waiting for NFA_CHO_SELECT_EVT because of handover collision.
*/
typedef struct
{
    tNFA_STATUS         status;
    UINT8               num_ac_rec;                     /* number of Alternative Carrier records*/
    tNFA_CHO_AC_REC     ac_rec[NFA_CHO_MAX_AC_INFO];    /* Alternative Carrier records          */
    UINT8               *p_ref_ndef;                    /* pointer of NDEF including AC records */
    UINT32              ref_ndef_len;                   /* length of NDEF                       */
} tNFA_CHO_REQUEST;

/*
** Data for NFA_CHO_SELECT_EVT
*/
typedef struct
{
    tNFA_STATUS         status;
    UINT8               num_ac_rec;                     /* number of Alternative Carrier records*/
    tNFA_CHO_AC_REC     ac_rec[NFA_CHO_MAX_AC_INFO];    /* Alternative Carrier records          */
    UINT8               *p_ref_ndef;                    /* pointer of NDEF including AC records */
    UINT32              ref_ndef_len;                   /* length of NDEF                       */
} tNFA_CHO_SELECT;

/* Error reason */
#define NFA_CHO_ERROR_TEMP_MEM  0x01
#define NFA_CHO_ERROR_PERM_MEM  0x02
#define NFA_CHO_ERROR_CARRIER   0x03

/*
** Data for NFA_CHO_SEL_ERR_EVT
*/
typedef struct
{
    UINT8               error_reason;   /* Error reason          */
    UINT32              error_data;     /* Error Data per reason */
} tNFA_CHO_SEL_ERR;

/* Union of all Connection Handover callback structures */
typedef union
{
    tNFA_STATUS             status;         /* NFA_CHO_REG_EVT          */
                                            /* NFA_CHO_DEACTIVATED_EVT  */
                                            /* NFA_CHO_TX_FAIL_EVT      */
    tNFA_CHO_ACTIVATED      activated;      /* NFA_CHO_ACTIVATED_EVT    */
    tNFA_CHO_CONNECTED      connected;      /* NFA_CHO_CONNECTED_EVT    */
    tNFA_CHO_DISCONNECTED   disconnected;   /* NFA_CHO_DISCONNECTED_EVT */
    tNFA_CHO_SELECT         select;         /* NFA_CHO_SELECT_EVT       */
    tNFA_CHO_REQUEST        request;        /* NFA_CHO_REQUEST_EVT      */
    tNFA_CHO_SEL_ERR        sel_err;        /* NFA_CHO_SEL_ERR_EVT      */
} tNFA_CHO_EVT_DATA;

/* NFA Connection Handover callback */
typedef void (tNFA_CHO_CBACK) (tNFA_CHO_EVT event, tNFA_CHO_EVT_DATA *p_data);

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
**
** Function         NFA_ChoRegister
**
** Description      This function is called to register callback function to receive
**                  connection handover events.
**
**                  On this registration, "urn:nfc:sn:handover" server will be
**                  registered on LLCP if enable_server is TRUE.
**
**                  The result of the registration is reported with NFA_CHO_REG_EVT.
**
** Note:            If RF discovery is started, NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT
**                  should happen before calling this function
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_ChoRegister (BOOLEAN        enable_server,
                                            tNFA_CHO_CBACK *p_cback);

/*******************************************************************************
**
** Function         NFA_ChoDeregister
**
** Description      This function is called to deregister callback function from NFA
**                  Connection Handover Application.
**
**                  If this is the valid deregistration, NFA Connection Handover
**                  Application will close the service with "urn:nfc:sn:handover"
**                  on LLCP and deregister NDEF type handler if any.
**
** Note:            If RF discovery is started, NFA_StopRfDiscovery()/NFA_RF_DISCOVERY_STOPPED_EVT
**                  should happen before calling this function
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_ChoDeregister (void);

/*******************************************************************************
**
** Function         NFA_ChoConnect
**
** Description      This function is called to create data link connection to
**                  Connection Handover server on peer device.
**
**                  It must be called after receiving NFA_CHO_ACTIVATED_EVT.
**                  NFA_CHO_CONNECTED_EVT will be returned if successful.
**                  Otherwise, NFA_CHO_DISCONNECTED_EVT will be returned.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_ChoConnect (void);

/*******************************************************************************
**
** Function         NFA_ChoDisconnect
**
** Description      This function is called to disconnect data link connection with
**                  Connection Handover server on peer device.
**
**                  NFA_CHO_DISCONNECTED_EVT will be returned.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_ChoDisconnect (void);

/*******************************************************************************
**
** Function         NFA_ChoSendHr
**
** Description      This function is called to send Handover Request Message with
**                  Handover Carrier records or Alternative Carrier records.
**
**                  It must be called after receiving NFA_CHO_CONNECTED_EVT.
**
**                  NDEF may include one or more Handover Carrier records or Alternative
**                  Carrier records with auxiliary data.
**                  The records in NDEF must be matched with tNFA_CHO_AC_INFO in order.
**                  Payload ID must be unique and Payload ID length must be less than
**                  or equal to NFA_CHO_MAX_REF_NAME_LEN.
**
**                  The alternative carrier information of Handover Select record
**                  will be sent to application by NFA_CHO_SELECT_EVT. Application
**                  may receive NFA_CHO_REQUEST_EVT because of handover collision.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_ChoSendHr (UINT8             num_ac_info,
                                          tNFA_CHO_AC_INFO *p_ac_info,
                                          UINT8            *p_ndef,
                                          UINT32            ndef_len);

/*******************************************************************************
**
** Function         NFA_ChoSendHs
**
** Description      This function is called to send Handover Select message with
**                  Alternative Carrier records as response to Handover Request
**                  message.
**
**                  NDEF may include one or more Alternative Carrier records with
**                  auxiliary data.
**                  The records in NDEF must be matched with tNFA_CHO_AC_INFO in order.
**                  Payload ID must be unique and Payload ID length must be less than
**                  or equal to NFA_CHO_MAX_REF_NAME_LEN.
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_ChoSendHs (UINT8            *p_ndef,
                                          UINT32            ndef_len);

/*******************************************************************************
**
** Function         NFA_ChoSendSelectError
**
** Description      This function is called to send Error record to indicate failure
**                  to process the most recently received Handover Request message.
**
**                  error_reason : NFA_CHO_ERROR_TEMP_MEM
**                                 NFA_CHO_ERROR_PERM_MEM
**                                 NFA_CHO_ERROR_CARRIER
**
** Returns          NFA_STATUS_OK if successfully initiated
**                  NFA_STATUS_FAILED otherwise
**
*******************************************************************************/
NFC_API extern tNFA_STATUS NFA_ChoSendSelectError (UINT8  error_reason,
                                                   UINT32 error_data);

/*******************************************************************************
**
** Function         NFA_ChoSetTraceLevel
**
** Description      This function sets the trace level for CHO.  If called with
**                  a value of 0xFF, it simply returns the current trace level.
**
** Returns          The new or current trace level
**
*******************************************************************************/
NFC_API extern UINT8 NFA_ChoSetTraceLevel (UINT8 new_level);

#if (defined (NFA_CHO_TEST_INCLUDED) && (NFA_CHO_TEST_INCLUDED == TRUE))

#define NFA_CHO_TEST_VERSION    0x01
#define NFA_CHO_TEST_RANDOM     0x02
/*******************************************************************************
**
** Function         NFA_ChoSetTestParam
**
** Description      This function is called to set test parameters.
**
*******************************************************************************/
NFC_API extern void NFA_ChoSetTestParam (UINT8  test_enable,
                                         UINT8  test_version,
                                         UINT16 test_random_number);
#endif

#ifdef __cplusplus
}
#endif

#endif /* NFA_CHO_API_H */
