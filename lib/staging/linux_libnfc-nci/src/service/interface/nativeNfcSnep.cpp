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

#include <malloc.h>
#include <pthread.h>

#include "nativeNfcSnep.h"
#include "nativeNfcManager.h"
#include "SyncEvent.h"
#include "nfa_api.h"
#include "phNxpLog.h"
#include "nfa_snep_api.h"
#include "ndef_utils.h"

static tNFA_HANDLE sSnepClientHandle = 0;
static tNFA_HANDLE sSnepClientConnectionHandle = 0;
static tNFA_STATUS sSnepClientPutState;
static SyncEvent sNfaSnepClientRegEvent;
static SyncEvent sNfaSnepClientConnEvent;
static SyncEvent sNfaSnepClientDisconnEvent;
static SyncEvent sNfaSnepClientPutMsgEvent;
static BOOLEAN sRfEnabled;

typedef enum {
	SNEP_SERVER_IDLE = 0,
	SNEP_SERVER_STARTING,
	SNEP_SERVER_STARTED,
}SNEP_SERVER_STATE;

static char SNEP_SERVER_NAME[] = {'u','r','n',':','n','f','c',':','s','n',':','s','n','e','p','\0'};

/* SNEP Server Handles */
static tNFA_HANDLE sSnepServerHandle = 0;
static tNFA_HANDLE sSnepServerConnectionHandle = 0;
static SNEP_SERVER_STATE sSnepServerState = SNEP_SERVER_IDLE;

static SyncEvent sNfaSnepServerRegEvent;
static SyncEvent sNfaSnepServerPutRspEvent;
static tNFA_SNEP_RESP_CODE sNfaSnepRespCode;

static nfcSnepServerCallback_t *sServerCallback = NULL;
static nfcSnepClientCallback_t *sClientCallback =NULL;

static void nfaSnepClientCallback (tNFA_SNEP_EVT snepEvent, tNFA_SNEP_EVT_DATA *eventData);
static void nfaSnepServerCallback (tNFA_SNEP_EVT snepEvent, tNFA_SNEP_EVT_DATA *eventData);
static void nativeNfcSnep_notifyClientActivated();
static void nativeNfcSnep_notifyClientDeactivated();
static void nativeNfcSnep_notifyServerActivated();
static void nativeNfcSnep_notifyServerDeactivated();
static void nativeNfcSnep_abortClientWaits();
static void nativeNfcSnep_doPutCompleted (tNFA_STATUS status);
static void nativeNfcSnep_doPutReceived (tNFA_HANDLE handle, UINT8 *data, UINT32 length);
static void *snepServerThread(void *arg);
static void nativeNfcSnep_abortClientWaits();
static void nativeNfcSnep_abortServerWaits();

extern Mutex gSyncMutex;
extern void nativeNfcTag_registerNdefTypeHandler ();
extern void nativeNfcTag_deregisterNdefTypeHandler ();
extern void startRfDiscovery (BOOLEAN isStart);
extern BOOLEAN isDiscoveryStarted();

static void nfaSnepClientCallback (tNFA_SNEP_EVT snepEvent, tNFA_SNEP_EVT_DATA *eventData)
{
    NXPLOG_API_D("%s: snepEvent= %u", __FUNCTION__, snepEvent);

    switch (snepEvent)
    {
        case NFA_SNEP_REG_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_REG_EVT; Status: 0x%04x\n", __FUNCTION__, eventData->reg.status);
            NXPLOG_API_D ("%s: NFA_SNEP_REG_EVT; Client Register Handle: 0x%04x\n", __FUNCTION__, eventData->reg.reg_handle);
            sSnepClientHandle = eventData->reg.reg_handle;
            {
                SyncEventGuard guard (sNfaSnepClientRegEvent);
                sNfaSnepClientRegEvent.notifyOne ();
            }
            break;

        case NFA_SNEP_ACTIVATED_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_ACTIVATED_EVT; Client Activated Handle: 0x%04x\n", __FUNCTION__, eventData->activated.client_handle);
            nativeNfcTag_deregisterNdefTypeHandler ();
            if((eventData->activated.client_handle) &&
                   (sSnepClientHandle == eventData->activated.client_handle))
            {
                nativeNfcSnep_notifyClientActivated();
            }
            break;

        case NFA_SNEP_DEACTIVATED_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_DEACTIVATED_EVT: Client Deactivated Handle: 0x%04x\n", __FUNCTION__, eventData->deactivated.client_handle);
            if((eventData->deactivated.client_handle) &&
                   (sSnepClientHandle == eventData->deactivated.client_handle))
            {
                nativeNfcSnep_notifyClientDeactivated();
            }
            nativeNfcSnep_abortClientWaits();
            nativeNfcTag_registerNdefTypeHandler();
            break;

        case NFA_SNEP_CONNECTED_EVT:
            if((eventData->connect.reg_handle) &&
               (eventData->connect.conn_handle) &&
               (sSnepClientHandle == eventData->connect.reg_handle))
            {
                SyncEventGuard guard (sNfaSnepClientConnEvent);
                NXPLOG_API_D ("%s: NFA_SNEP_CONNECTED_EVT: Client Register Handle: 0x%04x\n", __FUNCTION__, eventData->connect.reg_handle);
                sSnepClientConnectionHandle = eventData->connect.conn_handle;
                sNfaSnepClientConnEvent.notifyOne ();
            }
            break;
        case NFA_SNEP_DISC_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_DISC_EVT: Client Connection/Register Handle: 0x%04x\n", __FUNCTION__, eventData->disc.conn_handle);
            {
                nativeNfcSnep_abortClientWaits();
            }
            break;
        case NFA_SNEP_PUT_RESP_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_PUT_RESP_EVT: Server Response Code: 0x%04x\n", __FUNCTION__, eventData->put_resp.resp_code);
            if((sSnepClientConnectionHandle == eventData->put_resp.conn_handle)
                    && (NFA_SNEP_RESP_CODE_SUCCESS == eventData->put_resp.resp_code))
            {
                nativeNfcSnep_doPutCompleted (NFA_STATUS_OK);
            }
            else if((sSnepClientConnectionHandle == eventData->put_resp.conn_handle)
                    && (NFA_SNEP_RESP_CODE_UNSUPP_VER == eventData->put_resp.resp_code))
            {
                nativeNfcSnep_doPutCompleted (NFA_STATUS_OK);  //workaround need to fix later
            }
            else
            {
                nativeNfcSnep_doPutCompleted (NFA_STATUS_FAILED);
            }
            break;
        case NFA_SNEP_ALLOC_BUFF_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_ALLOC_BUFF_EVT: Handle: 0x%04x\n", __FUNCTION__, eventData->alloc.conn_handle);
            break;
        case NFA_SNEP_FREE_BUFF_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_FREE_BUFF_EVT: \n", __FUNCTION__);
            break;
        default:
            NXPLOG_API_D ("%s: unknown event 0x%X\n", snepEvent);
            break;
    }
}

static void nfaSnepServerCallback (tNFA_SNEP_EVT snepEvent, tNFA_SNEP_EVT_DATA *eventData)
{
    NXPLOG_API_D("%s: Function Entry  snepEvent: 0x%X\n",__FUNCTION__, snepEvent);
    switch (snepEvent)
    {
        case NFA_SNEP_REG_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_REG_EVT; Status: 0x%04x\n", __FUNCTION__, eventData->reg.status);
            NXPLOG_API_D ("%s: NFA_SNEP_REG_EVT; Server Register Handle: 0x%04x\n", __FUNCTION__, eventData->reg.reg_handle);
            sSnepServerHandle = eventData->reg.reg_handle;
            {
                SyncEventGuard guard (sNfaSnepServerRegEvent);
                sNfaSnepServerRegEvent.notifyOne ();
            }
            break;

        case NFA_SNEP_DEFAULT_SERVER_STARTED_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_DEFAULT_SERVER_STARTED_EVT\n", __FUNCTION__);
            break;

        case NFA_SNEP_DEFAULT_SERVER_STOPPED_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_DEFAULT_SERVER_STOPPED_EVT\n", __FUNCTION__);
            break;

        case NFA_SNEP_ACTIVATED_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_ACTIVATED_EVT: Server Activated\n", __FUNCTION__);
            break;

        case NFA_SNEP_DEACTIVATED_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_DEACTIVATED_EVT: Server Deactivated\n", __FUNCTION__);
            break;

        case NFA_SNEP_CONNECTED_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_CONNECTED_EVT: Server Register Handle: 0x%04x\n", __FUNCTION__, eventData->connect.reg_handle);
            NXPLOG_API_D ("%s: NFA_SNEP_CONNECTED_EVT: Server Connection handle: 0x%04x\n", __FUNCTION__, eventData->connect.conn_handle);
            if((eventData->connect.reg_handle) &&
               (eventData->connect.conn_handle))
            {
                sSnepServerConnectionHandle = eventData->connect.conn_handle;
                nativeNfcSnep_notifyServerActivated();
            }
            break;

        case NFA_SNEP_DISC_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_DISC_EVT: Server Connection/Register Handle: 0x%04x\n", __FUNCTION__, eventData->disc.conn_handle);
            if(sSnepServerConnectionHandle == eventData->disc.conn_handle)
            {
                nativeNfcSnep_notifyServerDeactivated();
            }
            break;

        case NFA_SNEP_PUT_REQ_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_PUT_REQ_EVT: Server Connection Handle: 0x%04x\n", __FUNCTION__, eventData->put_req.conn_handle);
            NXPLOG_API_D ("%s: NFA_SNEP_PUT_REQ_EVT: NDEF Message Length: 0x%04x\n", __FUNCTION__, eventData->put_req.ndef_length);
            nativeNfcSnep_doPutReceived(eventData->put_req.conn_handle, eventData->put_req.p_ndef, eventData->put_req.ndef_length);
            break;

        case NFA_SNEP_ALLOC_BUFF_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_ALLOC_BUFF_EVT: Server Connection Handle: 0x%04x\n", __FUNCTION__, eventData->alloc.conn_handle);
            NXPLOG_API_D ("%s: NFA_SNEP_ALLOC_BUFF_EVT: Request Code: 0x%04x\n", __FUNCTION__, eventData->alloc.req_code);
            NXPLOG_API_D ("%s: NFA_SNEP_ALLOC_BUFF_EVT: NDEF Message Length: 0x%04x\n", __FUNCTION__, eventData->alloc.ndef_length);
            NXPLOG_API_D ("%s: NFA_SNEP_ALLOC_BUFF_EVT: Response Code: 0x%04x\n", __FUNCTION__, eventData->alloc.resp_code);

            if(0x00 != eventData->alloc.ndef_length)
            {
                eventData->alloc.p_buff = (UINT8*)malloc(eventData->alloc.ndef_length);
                if(NULL == eventData->alloc.p_buff)
                {
                    NXPLOG_API_D("Memory Allocation Failed !!!\n");
                }
            }
            break;

        case NFA_SNEP_FREE_BUFF_EVT:
            NXPLOG_API_D ("%s: NFA_SNEP_FREE_BUFF_EVT: Server Connection Handle: 0x%04x\n",__FUNCTION__, eventData->free.conn_handle);
            NXPLOG_API_D ("%s: NFA_SNEP_FREE_BUFF_EVT: Buffer to Free: 0x%04x\n",__FUNCTION__, eventData->free.p_buff);

            if(eventData->free.p_buff)
            {
                free(eventData->free.p_buff);
                eventData->free.p_buff = NULL;
            }
            break;

        default:
            NXPLOG_API_D ("%s: unknown event 0x%X ?\n", __FUNCTION__, snepEvent);
            break;
    }
}

static void *snepServerThread(void *arg)
{
    (void)arg;
    SyncEventGuard guard (sNfaSnepServerPutRspEvent);

    NXPLOG_API_D ("%s: enter\n", __FUNCTION__);
    while(sSnepServerState == SNEP_SERVER_STARTED)
    {
        sNfaSnepServerPutRspEvent.wait();
        if (sSnepServerConnectionHandle == 0)
            break;
        if(NFA_STATUS_OK != NFA_SnepPutResponse(sSnepServerConnectionHandle, sNfaSnepRespCode))
        {
            NXPLOG_API_D ("%s: send response failed.", __FUNCTION__);
        }
    }
    NXPLOG_API_D ("%s: exit\n", __FUNCTION__);
    pthread_exit(NULL);
    return NULL;
}

void nativeNfcSnep_notifyClientActivated()
{
    if (nfcManager_isNfcActive())
    {
        if(sClientCallback&& (NULL != sClientCallback->onDeviceArrival))
        {
            sClientCallback->onDeviceArrival();
        }
    }
}

void nativeNfcSnep_notifyClientDeactivated()
{
    if (nfcManager_isNfcActive())
    {
        if(sClientCallback&& (NULL != sClientCallback->onDeviceDeparture))
        {
            sClientCallback->onDeviceDeparture();
        }
    }
}

void nativeNfcSnep_notifyServerActivated()
{
    if (nfcManager_isNfcActive())
    {
        if(sServerCallback&& (NULL != sServerCallback->onDeviceArrival))
        {
            sServerCallback->onDeviceArrival();
        }
    }
}

void nativeNfcSnep_notifyServerDeactivated()
{
    if (nfcManager_isNfcActive())
    {
        sSnepServerConnectionHandle = 0;
        if(sServerCallback&& (NULL != sServerCallback->onDeviceDeparture))
        {
            sServerCallback->onDeviceDeparture();
        }
    }
}


/*******************************************************************************
**
** Function:        nativeNfcTag_abortWaits
**
** Description:     Unblock all thread synchronization objects.
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcSnep_abortServerWaits ()
{
    NXPLOG_API_D ("%s", __FUNCTION__);
    if (sSnepServerState == SNEP_SERVER_STARTED)
    {
        SyncEventGuard g (sNfaSnepServerPutRspEvent);
        sSnepServerState = SNEP_SERVER_IDLE;
        sSnepServerConnectionHandle = 0;
        sNfaSnepServerPutRspEvent.notifyOne();
    }
}

void nativeNfcSnep_abortClientWaits()
{
    NXPLOG_API_D ("%s", __FUNCTION__);
    sSnepClientConnectionHandle = 0;
    {
        SyncEventGuard g (sNfaSnepClientPutMsgEvent);
        sNfaSnepClientPutMsgEvent.notifyOne ();
    }
    {
        SyncEventGuard g (sNfaSnepClientConnEvent);
        sNfaSnepClientConnEvent.notifyOne ();
    }
    {
        SyncEventGuard g (sNfaSnepClientDisconnEvent);
        sNfaSnepClientDisconnEvent.notifyOne ();
    }
    NXPLOG_API_D ("%s exit", __FUNCTION__);
}

void nativeNfcSnep_handleNfcOnOff(BOOLEAN isOn)
{
    if(isOn)
    {
        sSnepServerConnectionHandle = 0;
        sSnepServerState = SNEP_SERVER_IDLE;
        sSnepClientConnectionHandle = 0;
        //TODO: if we want to restart server?
    }
    else
    {
        nativeNfcSnep_abortServerWaits();
        nativeNfcSnep_abortClientWaits();
    }
}

static void nativeNfcSnep_doPutCompleted (tNFA_STATUS status)
{
    NXPLOG_API_D ("%s: status=0x%X", __FUNCTION__, status);

    sSnepClientPutState = status;
    SyncEventGuard g (sNfaSnepClientPutMsgEvent);
    sNfaSnepClientPutMsgEvent.notifyOne ();
}

static void nativeNfcSnep_doPutReceived (tNFA_HANDLE handle, UINT8 *data, UINT32 length)
{
    NXPLOG_API_D ("%s: handle=0x%X, msg length =%d", __FUNCTION__, handle, length);
    if (!nfcManager_isNfcActive())
    {
        return;
    }
    if((sSnepServerConnectionHandle == handle) &&
           NULL != data && 0x00 != length)
    {
        if (sServerCallback&& (NULL != sServerCallback->onMessageReceived))
        {
            sServerCallback->onMessageReceived(data, length);
        }
        sNfaSnepRespCode = NFA_SNEP_RESP_CODE_SUCCESS;
    }
    else
    {
        sNfaSnepRespCode = NFA_SNEP_RESP_CODE_REJECT;
    }
    {
        SyncEventGuard guard (sNfaSnepServerPutRspEvent);
        sNfaSnepServerPutRspEvent.notifyOne ();
    }
}

INT32 nativeNfcSnep_registerClientCallback(nfcSnepClientCallback_t *clientCallback)
{
    tNFA_STATUS status = NFA_STATUS_FAILED;
    NXPLOG_API_D ("%s:", __FUNCTION__);
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
        SyncEventGuard g (sNfaSnepClientRegEvent);
        if(NFA_STATUS_OK != (status = NFA_SnepRegisterClient(nfaSnepClientCallback)))
        {
            NXPLOG_API_E ("%s: fail to register client callback for SNEP", __FUNCTION__);
            goto clean_and_return;
        }
        sNfaSnepClientRegEvent.wait();
    }
    sClientCallback = clientCallback;
    status = NFA_STATUS_OK;
clean_and_return:
    if (sRfEnabled)
    {
        // Stop RF Discovery if we were polling
        startRfDiscovery (TRUE);
    }
    gSyncMutex.unlock();
    return status;
}

void nativeNfcSnep_deregisterClientCallback()
{
    NXPLOG_API_D ("%s:", __FUNCTION__);

    gSyncMutex.lock();
    if (!nfcManager_isNfcActive())
    {
        NXPLOG_API_E ("%s: Nfc not initialized.", __FUNCTION__);
        gSyncMutex.unlock();
        return;
    }
    nativeNfcSnep_abortClientWaits();
    sRfEnabled = isDiscoveryStarted();
    if (sRfEnabled)
    {
        // Stop RF Discovery if we were polling
        startRfDiscovery (FALSE);
    }
    NFA_SnepDeregister(sSnepClientHandle);
    sClientCallback = NULL;
    if (sRfEnabled)
    {
        startRfDiscovery (TRUE);
    }
    gSyncMutex.unlock();
}

INT32 nativeNfcSnep_startServer(nfcSnepServerCallback_t *serverCallback)
{
    tNFA_STATUS status = NFA_STATUS_OK;
    int ret;
    pthread_t snepRespThread;

    NXPLOG_API_D ("%s:", __FUNCTION__);
    if (serverCallback == NULL)
    {
        NXPLOG_API_E ("%s: callback is NULL!", __FUNCTION__);
        return NFA_STATUS_FAILED;
    }

    gSyncMutex.lock();
    if (!nfcManager_isNfcActive())
    {
        NXPLOG_API_E ("%s: Nfc not initialized.", __FUNCTION__);
        gSyncMutex.unlock();
        return NFA_STATUS_FAILED;
    }

    if (sSnepServerState == SNEP_SERVER_STARTED && serverCallback == sServerCallback)
    {
        NXPLOG_API_D ("%s: alread started!", __FUNCTION__);
        gSyncMutex.unlock();
        return NFA_STATUS_OK;
    }
    if (sSnepServerState != SNEP_SERVER_IDLE)
    {
        NXPLOG_API_E ("%s: Server is started or busy. State = 0x%X", __FUNCTION__, sSnepServerState);
        gSyncMutex.unlock();
        return NFA_STATUS_FAILED;
    }

    sServerCallback = serverCallback;
    sSnepServerState = SNEP_SERVER_STARTING;

    sRfEnabled = isDiscoveryStarted();
    if (sRfEnabled)
    {
        // Stop RF Discovery if we were polling
        startRfDiscovery (FALSE);
    }
    {
        SyncEventGuard guard (sNfaSnepServerRegEvent);
        if(NFA_STATUS_OK != NFA_SnepRegisterServer(0x04, SNEP_SERVER_NAME, nfaSnepServerCallback))
        {
            status = NFA_STATUS_FAILED;
            sSnepServerState = SNEP_SERVER_IDLE;
            sServerCallback = NULL;
            goto clean_and_return;
        }
        sNfaSnepServerRegEvent.wait();
    }
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&snepRespThread, &attr, snepServerThread, NULL);
    if(ret != 0)
    {
        NXPLOG_API_E("Unable to create snep server thread");
        sSnepServerState = SNEP_SERVER_IDLE;
        NFA_SnepDeregister(sSnepServerHandle);
        sServerCallback = NULL;
        status = NFA_STATUS_FAILED;
        goto clean_and_return;
    }
    else
    {
        if(pthread_setname_np(snepRespThread,"NFC_SNEP_RESP"))
        {
        	NXPLOG_API_E("pthread_setname_np in %s failed", __FUNCTION__);
        }
    }

    sSnepServerState = SNEP_SERVER_STARTED;
clean_and_return:
    if (sRfEnabled)
    {
        startRfDiscovery (TRUE);
    }
    gSyncMutex.unlock();
    return status;
}
void nativeNfcSnep_stopServer()
{
    NXPLOG_API_D ("%s:", __FUNCTION__);
    gSyncMutex.lock();
    if (!nfcManager_isNfcActive())
    {
        NXPLOG_API_E ("%s: Nfc not initialized.", __FUNCTION__);
        gSyncMutex.unlock();
        return;
    }

    nativeNfcSnep_abortServerWaits();
    sServerCallback = NULL;

    sRfEnabled = isDiscoveryStarted();
    if (sRfEnabled)
    {
        // Stop RF Discovery if we were polling
        startRfDiscovery (FALSE);
    }
    NFA_SnepDeregister(sSnepServerHandle);
    if (sRfEnabled)
    {
        startRfDiscovery (TRUE);
    }
    sSnepServerState = SNEP_SERVER_IDLE;
    gSyncMutex.unlock();
}

INT32 nativeNfcSnep_putMessage(UINT8* msg, UINT32 length)
{
    tNFA_STATUS status = NFA_STATUS_FAILED;
    NXPLOG_API_D ("%s: data length = %d", __FUNCTION__, length);

    if (!sSnepClientHandle)
    {
        NXPLOG_API_E ("%s: no connection", __FUNCTION__);
        return NFA_STATUS_FAILED;
    }
    if (!msg || length == 0)
    {
        NXPLOG_API_E ("%s: wrong param", __FUNCTION__);
        return NFA_STATUS_FAILED;
    }
    if(NFA_STATUS_OK != NDEF_MsgValidate(msg, length, FALSE))
    {
        NXPLOG_API_E ("%s: not NDEF message", __FUNCTION__);
        return NFA_STATUS_FAILED;
    }
    gSyncMutex.lock();
    if (!nfcManager_isNfcActive())
    {
        NXPLOG_API_E ("%s: Nfc not initialized.", __FUNCTION__);
        status = NFA_STATUS_FAILED;
        goto clean_and_return;
    }
#if (NFC_SNEP_PUT_DISCONNECT == 1)
    if (sSnepClientHandle)
#else
    if ((sSnepClientHandle) && (sSnepClientConnectionHandle == 0))
#endif
	{
        SyncEventGuard guard (sNfaSnepClientConnEvent);
        if(NFA_STATUS_OK != NFA_SnepConnect(sSnepClientHandle, SNEP_SERVER_NAME))
        {
            status = NFA_STATUS_FAILED;
            goto clean_and_return;
        }
        sNfaSnepClientConnEvent.wait();
    }

    /* Send Put Request */
    if (sSnepClientConnectionHandle != 0)
    {
        SyncEventGuard guard (sNfaSnepClientPutMsgEvent);
        if(NFA_STATUS_OK != NFA_SnepPut (sSnepClientConnectionHandle, length, msg))
        {
            status = NFA_STATUS_FAILED;
            goto clean_and_return;
        }
        sNfaSnepClientPutMsgEvent.wait();
        if (sSnepClientPutState != NFA_STATUS_OK)
        {
            status = NFA_STATUS_FAILED;
        }
        else
        {
            status = NFA_STATUS_OK;
            sSnepClientPutState = NFA_STATUS_FAILED;
        }
    }
#if (NFC_SNEP_PUT_DISCONNECT == 1)
    /* Disconnect from Snep Server */
    if (sSnepClientConnectionHandle != 0)
    {
        SyncEventGuard guard (sNfaSnepClientDisconnEvent);
        if(NFA_STATUS_OK != NFA_SnepDisconnect (sSnepClientConnectionHandle, 0x01))
        {
            status = NFA_STATUS_FAILED;
            goto clean_and_return;
        }
        sNfaSnepClientDisconnEvent.wait();
    }
#endif
clean_and_return:
    NXPLOG_API_D ("%s: return = %d", __FUNCTION__, status);
    gSyncMutex.unlock();
    return status;
}
