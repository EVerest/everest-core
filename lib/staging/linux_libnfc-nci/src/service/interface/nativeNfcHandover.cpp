/******************************************************************************
 *
 *  Copyright (C) 2015 NXP Semiconductors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License")
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

#include <string.h>
#include <pthread.h>

#include "nativeNfcHandover.h"
#include "nativeNfcManager.h"
#include "SyncEvent.h"

#include "nfa_api.h"
#include "phNxpLog.h"
#include "nfa_cho_api.h"

typedef enum {
	HO_SERVER_IDLE = 0,
	HO_SERVER_REGISTERED,
	HO_SERVER_HR_RECEIVED,
}HO_SERVER_STATE;

const static UINT8 RTD_Hs[2] = {'H', 's'};
const static UINT8 RTD_Hr[2] = {'H', 'r'};

static BOOLEAN sRfEnabled;
static SyncEvent sNfaHORegEvent;
static SyncEvent sNfaHOSendMsgEvent;
static HO_SERVER_STATE sNfaHOStatus = HO_SERVER_IDLE;
static tNFA_STATUS sNfaHOSendMessageResult;

static nfcHandoverCallback_t *sCallback = NULL;

static void nfaHoCallback (tNFA_CHO_EVT event, tNFA_CHO_EVT_DATA *p_data);

extern Mutex gSyncMutex;
extern void nativeNfcTag_registerNdefTypeHandler ();
extern void nativeNfcTag_deregisterNdefTypeHandler ();
extern void startRfDiscovery (BOOLEAN isStart);
extern BOOLEAN isDiscoveryStarted();

static void nativeNfcHandover_notifyHrRecieved(UINT8 *data, UINT32 length)
{
    if (nfcManager_isNfcActive())
    {
        if(sCallback && (NULL != sCallback->onHandoverRequestReceived))
        {
            sCallback->onHandoverRequestReceived(data, length);
        }
    }
}

static void nativeNfcHandover_notifyHsRecieved(UINT8 *data, UINT32 length)
{
    if (nfcManager_isNfcActive())
    {
        if(sCallback && (NULL != sCallback->onHandoverSelectReceived))
        {
            sCallback->onHandoverSelectReceived(data, length);
        }
    }
}

void nativeNfcHandover_abortWaits()
{
    NXPLOG_API_D ("%s", __FUNCTION__);
    {
        SyncEventGuard g (sNfaHORegEvent);
        sNfaHORegEvent.notifyOne ();
    }
    {
        SyncEventGuard g (sNfaHOSendMsgEvent);
        sNfaHOSendMsgEvent.notifyOne ();
    }
    NXPLOG_API_D ("%s exit", __FUNCTION__);
}

static void nativeNfcHandover_doSendMsgCompleted (tNFA_STATUS status)
{
    NXPLOG_API_D ("%s: status=0x%X;", __FUNCTION__, status);
    sNfaHOSendMessageResult = status;
    sNfaHOStatus = HO_SERVER_IDLE;
    SyncEventGuard g (sNfaHOSendMsgEvent);
    sNfaHOSendMsgEvent.notifyOne ();
}

static void nfaHoCallback (tNFA_CHO_EVT event, tNFA_CHO_EVT_DATA *eventData)
{
    NXPLOG_API_D("%s: HoEvent= %u", __FUNCTION__, event);

    switch (event)
    {
        case NFA_CHO_REG_EVT:
            NXPLOG_API_D ("%s: NFA_CHO_REG_EVT; Status: 0x%04x\n", __FUNCTION__, eventData->status);
            {
                SyncEventGuard guard (sNfaHORegEvent);
                if (NFA_STATUS_OK == eventData->status)
                {
                    sNfaHOStatus = HO_SERVER_REGISTERED;
                }
                sNfaHORegEvent.notifyOne ();
            }
            break;

        case NFA_CHO_ACTIVATED_EVT:
            NXPLOG_API_D ("%s: NFA_CHO_ACTIVATED_EVT; is_initiator: %d\n", __FUNCTION__, eventData->activated.is_initiator);
            nativeNfcTag_deregisterNdefTypeHandler ();
            break;

        case NFA_CHO_DEACTIVATED_EVT:
            NXPLOG_API_D ("%s: NFA_CHO_DEACTIVATED_EVT\n", __FUNCTION__);
            nativeNfcHandover_abortWaits();
            sNfaHOStatus = HO_SERVER_IDLE;
            nativeNfcTag_registerNdefTypeHandler();
            break;

        case NFA_CHO_CONNECTED_EVT:
            NXPLOG_API_D ("%s: NFA_CHO_CONNECTED_EVT: initial_role: 0x%04x\n", __FUNCTION__, eventData->connected.initial_role);
            break;

        case NFA_CHO_DISCONNECTED_EVT:
            NXPLOG_API_D ("%s: NFA_CHO_DISCONNECTED_EVT: reason: 0x%04x\n", __FUNCTION__, eventData->disconnected.reason);
            sNfaHOStatus = HO_SERVER_IDLE;
            break;
        case NFA_CHO_REQUEST_EVT:
            NXPLOG_API_D ("%s: NFA_CHO_REQUEST_EVT:\n", __FUNCTION__);
            sNfaHOStatus = HO_SERVER_HR_RECEIVED;
            nativeNfcHandover_notifyHrRecieved(eventData->request.p_ref_ndef, eventData->request.ref_ndef_len);
            break;
        case NFA_CHO_SELECT_EVT:
            NXPLOG_API_D ("%s: NFA_CHO_SELECT_EVT:\n", __FUNCTION__);
            nativeNfcHandover_notifyHsRecieved(eventData->select.p_ref_ndef, eventData->select.ref_ndef_len);
            break;
        case NFA_CHO_SEL_ERR_EVT:
            NXPLOG_API_D ("%s: NFA_CHO_SEL_ERR_EVT: \n", __FUNCTION__);
            nativeNfcHandover_doSendMsgCompleted(NFA_STATUS_FAILED);
            break;
        case NFA_CHO_TX_FAIL_EVT:
            NXPLOG_API_D ("%s: NFA_CHO_TX_FAIL_EVT: \n", __FUNCTION__);
            nativeNfcHandover_doSendMsgCompleted(NFA_STATUS_FAILED);
            break;
        default:
            NXPLOG_API_D ("%s: unknown event 0x%X\n", event);
            break;
    }
}

INT32 nativeNfcHO_registerCallback(nfcHandoverCallback_t *callback)
{
    tNFA_STATUS status = NFA_STATUS_OK;

    NXPLOG_API_D ("%s:", __FUNCTION__);
    if (callback == NULL)
    {
        NXPLOG_API_E ("%s: callback is NULL!", __FUNCTION__);
        return NFA_STATUS_FAILED;
    }
    if (sCallback != NULL)
    {
        NXPLOG_API_E ("%s: already registered. Deregister first.", __FUNCTION__);
        return NFA_STATUS_FAILED;
    }

    gSyncMutex.lock();
    if (!nfcManager_isNfcActive())
    {
        NXPLOG_API_E ("%s: Nfc not initialized.", __FUNCTION__);
        gSyncMutex.unlock();
        return NFA_STATUS_FAILED;
    }
    sRfEnabled = isDiscoveryStarted();
    if (sRfEnabled)
    {
        // Stop RF Discovery if we were polling
        startRfDiscovery (FALSE);
    }
    {
        SyncEventGuard guard (sNfaHORegEvent);
        /*if(NFA_STATUS_OK != NFA_ChoRegister(TRUE, nfaHoCallback))
        {
            status = NFA_STATUS_FAILED;
            goto clean_and_return;
        }*/
        sNfaHOStatus = HO_SERVER_IDLE;
        sNfaHORegEvent.wait();
        if (sNfaHOStatus != HO_SERVER_REGISTERED)
        {
            status = NFA_STATUS_FAILED;
            goto clean_and_return;
        }
        sCallback = callback;
    }
clean_and_return:
    if (sRfEnabled)
    {
        startRfDiscovery (TRUE);
    }
    gSyncMutex.unlock();
    return status;
}

void nativeNfcHO_deregisterCallback()
{
    NXPLOG_API_D ("%s:", __FUNCTION__);
    gSyncMutex.lock();
    if (!nfcManager_isNfcActive())
    {
        NXPLOG_API_E ("%s: Nfc not initialized.", __FUNCTION__);
        gSyncMutex.unlock();
        return;
    }
    nativeNfcHandover_abortWaits();
    sRfEnabled = isDiscoveryStarted();
    if (sRfEnabled)
    {
        // Stop RF Discovery if we were polling
        startRfDiscovery (FALSE);
    }

    //NFA_ChoDeregister();
    if (sRfEnabled)
    {
        startRfDiscovery (TRUE);
    }
    sNfaHOStatus = HO_SERVER_IDLE;
    sCallback = NULL;
    gSyncMutex.unlock();
}

INT32 nativeNfcHO_sendHs(UINT8 *msg, UINT32 length)
{
    tNFA_STATUS status = NFA_STATUS_OK;
    UINT32 num_of_ndef, xx;
    UINT8 *p_ndef, tnf, len;
    UINT32 ndef_len;
    NXPLOG_API_D ("%s:", __FUNCTION__);

    if (msg == NULL || length == 0)
    {
        NXPLOG_API_E ("%s: wrong message\n", __FUNCTION__);
        return NFA_STATUS_FAILED;
    }
    //check ndef message
    num_of_ndef = NDEF_MsgGetNumRecs(msg);
    if (num_of_ndef == 0)
    {
        NXPLOG_API_E ("%s: wrong message\n", __FUNCTION__);
        return NFA_STATUS_FAILED;
    }

    //Check Hs record
    p_ndef = NDEF_RecGetType(msg, &tnf, &len);
    if (p_ndef == NULL || tnf != NDEF_TNF_WELLKNOWN || len != 2 || (memcmp(p_ndef, RTD_Hs, len) != 0))
    {
        NXPLOG_API_E ("%s: Not a valid Hs message\n", __FUNCTION__);
        return NFA_STATUS_FAILED;
     }
    p_ndef = NDEF_RecGetPayload(msg, (uint32_t *)&ndef_len);
    if (p_ndef == NULL  || p_ndef[0] > NFA_CHO_VERSION)
    {
        NXPLOG_API_E ("%s: Unsupported Handover version\n", __FUNCTION__);
        return NFA_STATUS_FAILED;
    }

    gSyncMutex.lock();
    if (!nfcManager_isNfcActive())
    {
        NXPLOG_API_E ("%s: Nfc not initialized.", __FUNCTION__);
        gSyncMutex.unlock();
        return NFA_STATUS_FAILED;
    }
    if (sNfaHOStatus != HO_SERVER_HR_RECEIVED)
    {
        NXPLOG_API_E ("%s: Handover connection isn't connected\n", __FUNCTION__);
        status = NFA_STATUS_FAILED;
        goto end_and_clean;
    }
    {
        SyncEventGuard guard (sNfaHOSendMsgEvent);
        /*if (NFA_STATUS_OK !=NFA_ChoSendHs(msg, length))
        {
            NXPLOG_API_E ("%s: Handover select message send failed\n", __FUNCTION__);
            status = NFA_STATUS_FAILED;
            goto end_and_clean;
        }*/
        sNfaHOSendMsgEvent.wait();
    }
end_and_clean:
    gSyncMutex.unlock();
    return status;
}

INT32 nativeNfcHO_sendSelectError(UINT8  error_reason, UINT32 error_data)
{
    tNFA_STATUS status = NFA_STATUS_OK;
    NXPLOG_API_D ("%s:", __FUNCTION__);
    gSyncMutex.lock();
    if (!nfcManager_isNfcActive())
    {
        NXPLOG_API_E ("%s: Nfc not initialized.", __FUNCTION__);
        status = NFA_STATUS_FAILED;
        goto end_and_clean;
    }
    if (sNfaHOStatus != HO_SERVER_HR_RECEIVED)
    {
        NXPLOG_API_E ("%s: Handover connection isn't connected\n", __FUNCTION__);
        status = NFA_STATUS_FAILED;
        goto end_and_clean;
    }
    {
        SyncEventGuard guard (sNfaHOSendMsgEvent);
        /*if (NFA_STATUS_OK !=NFA_ChoSendSelectError(error_reason, error_data))
        {
            status = NFA_STATUS_FAILED;
            goto end_and_clean;
        }*/
        sNfaHOSendMsgEvent.wait();
    }
end_and_clean:
    gSyncMutex.unlock();
    return status;

}
