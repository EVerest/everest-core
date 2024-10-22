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
 #include <string.h>
#include <pthread.h>

#include "nativeNfcLlcp.h"
#include "nativeNfcManager.h"
#include "SyncEvent.h"
#include "nfa_api.h"
#include "nfa_p2p_api.h"
#include "phNxpLog.h"
#include "ndef_utils.h"

typedef enum {
    LLCP_SERVER_IDLE = 0,
    LLCP_SERVER_STARTING,
    LLCP_SERVER_STARTED,
}LLCP_SERVER_STATE;

typedef enum {
    LLCP_CLIENT_IDLE = 0,
    LLCP_CLIENT_STARTING,
    LLCP_CLIENT_STARTED,
}LLCP_CLIENT_STATE;

#ifdef WIN_PLATFORM
static char LLCP_SERVER_NAME[] = {'u','r','n',':','n','f','c',':','s','n',':','l','l','c','p','\0'};
#else
static char LLCP_SERVER_NAME[] = {'u','r','n',':','n','f','c',':','s','n',':','l','l','c','p'};
#endif

extern Mutex gSyncMutex;

/* LLCP Client Handles */
static tNFA_HANDLE sLlcpConnLessClientHandle = 0;
static tNFA_HANDLE sLlcpConnLessClientConnectionHandle = 0;
static tNFA_STATUS sLlcpConnLessClientReadState;
static SyncEvent sNfaLlcpConnLessClientRegEvent;
static SyncEvent sNfaLlcpConnLessClientConnEvent;
static SyncEvent sNfaLlcpConnLessClientDisconnEvent;
static SyncEvent sNfaLlcpClientRegEvent;
static SyncEvent sNfaLlcpClientConnEvent;
static SyncEvent sNfaLlcpClientDisconnEvent;
static LLCP_CLIENT_STATE sLlcpClientState = LLCP_CLIENT_IDLE;

/* LLCP Server Handles */
static tNFA_HANDLE sLlcpConnLessServerHandle = 0;
static tNFA_HANDLE sLlcpConnLessServerConnectionHandle = 0;
static SyncEvent sNfaLlcpServerRegEvent;
static tNFA_STATUS sLlcpConnLessServerReadState;
static SyncEvent sNfaLlcpConnLessServerRegEvent;
static nfcllcpConnlessServerCallback_t *sServerCallback = NULL;
static nfcllcpConnlessClientCallback_t *sClientCallback =NULL;
static LLCP_SERVER_STATE sLlcpServerState = LLCP_SERVER_IDLE;

static tNFA_HANDLE sLlcpConnLessConnectedHandle = 0;
static SyncEvent sNfaLlcpConnLessReadEvent;
static tNFA_HANDLE sLlcpConnLessHandle = 0;
static bool sRfEnabled;
static bool bClientReadState = FALSE;
static bool bServerReadState = FALSE;
static UINT8 bDestSap = 0x00;
static UINT8 bLlcpReadData[LLCP_MAX_DATA_SIZE];
static uint32_t dwLlcpReadLength = 0x00;
static bool blMoreDataRemaining = FALSE;
static SyncEvent sNfaLlcpSdpEvt;

static UINT8 bLlcpClientReadData[LLCP_MAX_DATA_SIZE];
static UINT32 dwLlcpClientReadLength = 0x00;
static bool blClientDataRemaining = FALSE;


extern void nativeNfcTag_registerNdefTypeHandler ();
extern void nativeNfcTag_deregisterNdefTypeHandler ();
extern void startRfDiscovery (BOOLEAN isStart);
extern bool isDiscoveryStarted();
static void nfaLlcpClientCallback (tNFA_P2P_EVT LlcpEvent, tNFA_P2P_EVT_DATA *eventData);

INT32 nativeNfcLlcp_ConnLessRegisterClientCallback(nfcllcpConnlessClientCallback_t *clientCallback)
{
    tNFA_STATUS status = NFA_STATUS_FAILED;
    pthread_t llcpCleintRespThread;
    int ret = 1;
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
        /* Stop RF Discovery if we were polling */
        startRfDiscovery (FALSE);
    }

    {
        SyncEventGuard g (sNfaLlcpClientRegEvent);
        bClientReadState = FALSE;
        if(NFA_STATUS_OK != (status = NFA_P2pRegisterClient(NFA_P2P_LLINK_TYPE, nfaLlcpClientCallback)))
        {
            NXPLOG_API_E ("%s: fail to register client callback for LLCP", __FUNCTION__);
            if (sRfEnabled)
            {
                /*  Rollback to default */
                startRfDiscovery (TRUE);
                gSyncMutex.unlock();
                return status;
            }

        }
        sNfaLlcpClientRegEvent.wait();
    }

    sClientCallback = clientCallback;
    status = NFA_STATUS_OK;

    gSyncMutex.unlock();
    return status;
}

void nativeNfcLlcp_notifyClientActivated()
{
    if (nfcManager_isNfcActive())
    {
        if(sClientCallback&& (NULL != sClientCallback->onDeviceArrival))
        {
            bClientReadState = FALSE;
            sClientCallback->onDeviceArrival();
        }
    }
}

void nativeNfcLlcp_notifyServerActivated()
{
    if (nfcManager_isNfcActive())
    {
        if(sServerCallback&& (NULL != sServerCallback->onDeviceArrival))
        {
            bServerReadState = FALSE;
            sServerCallback->onDeviceArrival();
        }
    }
}

void nativeNfcLlcp_notifyServerDeactivated()
{
    if (nfcManager_isNfcActive())
    {
        if(sServerCallback&& (NULL != sServerCallback->onDeviceDeparture))
        {
            sServerCallback->onDeviceDeparture();
        }
    }
}

void nativeNfcLlcp_notifyClientDeactivated()
{
    if (nfcManager_isNfcActive())
    {
        if(sClientCallback&& (NULL != sClientCallback->onDeviceDeparture))
        {
            sClientCallback->onDeviceDeparture();
        }
    }
}

void nativeNfcLlcp_abortClientWaits()
{
    NXPLOG_API_D ("%s", __FUNCTION__);
    sLlcpConnLessClientHandle = 0;
    {
        SyncEventGuard g (sNfaLlcpClientRegEvent);
        sNfaLlcpClientRegEvent.notifyOne ();
    }

    NXPLOG_API_D ("%s exit", __FUNCTION__);
}

static void nativeNfcLlcp_doClientReadCompleted (tNFA_STATUS status)
{
    NXPLOG_API_D ("%s: status=0x%X", __FUNCTION__, status);

    SyncEventGuard g (sNfaLlcpConnLessReadEvent);
    bClientReadState = TRUE;
    sNfaLlcpConnLessReadEvent.notifyOne ();
}

static void nativeNfcLlcp_doServerReadCompleted (tNFA_STATUS status)
{
    NXPLOG_API_D ("%s: status=0x%X", __FUNCTION__, status);
    if(nfcManager_isNfcActive())
    {
        if(sServerCallback&& (NULL != sServerCallback->onMessageReceived))
        {
            sServerCallback->onMessageReceived();
        }
    }
    SyncEventGuard g (sNfaLlcpConnLessReadEvent);
    bServerReadState = TRUE;
    sNfaLlcpConnLessReadEvent.notifyOne ();
}

static void nfaLlcpClientCallback (tNFA_P2P_EVT LlcpEvent, tNFA_P2P_EVT_DATA *eventData)
{
    NXPLOG_API_D("%s: snepEvent= %u", __FUNCTION__, LlcpEvent);

    if(eventData != NULL)
    {
        switch (LlcpEvent)
        {
            case NFA_P2P_REG_CLIENT_EVT:
                NXPLOG_API_D ("%s: NFA_P2P_REG_CLIENT_EVT; Client Register Handle: 0x%04x\n", __FUNCTION__, eventData->reg_client.client_handle);
                sLlcpConnLessClientHandle = eventData->reg_client.client_handle;
                {
                    SyncEventGuard guard (sNfaLlcpClientRegEvent);
                    sNfaLlcpClientRegEvent.notifyOne ();
                }
                break;

            case NFA_P2P_ACTIVATED_EVT:
                NXPLOG_API_D ("%s: NFA_P2P_ACTIVATED_EVT; Client Activated Handle: 0x%04x\n", __FUNCTION__, eventData->activated.handle);
                nativeNfcTag_deregisterNdefTypeHandler ();
                if((eventData->activated.handle) &&
                    (sLlcpConnLessClientHandle == eventData->activated.handle))
                {
                    bDestSap = LLCP_CL_SAP_ID_DEFAULT;
                    sLlcpConnLessHandle = eventData->activated.handle;
                    sLlcpConnLessConnectedHandle = eventData->connected.conn_handle;
                    NXPLOG_API_D("nfaLlcpServerCallBack: remote sap ID 0x%04x\n ", bDestSap);
                    nativeNfcLlcp_notifyClientActivated();
                }
                break;

            case NFA_P2P_DEACTIVATED_EVT:
                NXPLOG_API_D ("%s: NFA_P2P_DEACTIVATED_EVT: Client Deactivated Handle: 0x%04x\n", __FUNCTION__, eventData->deactivated.handle);
                if((eventData->deactivated.handle) &&
                       (sLlcpConnLessClientHandle == eventData->deactivated.handle))
                {
                    nativeNfcLlcp_notifyClientDeactivated();
                }
                nativeNfcLlcp_abortClientWaits();
                nativeNfcTag_registerNdefTypeHandler();
                break;

            case NFA_P2P_DISC_EVT:
                NXPLOG_API_D ("%s: NFA_SNEP_DISC_EVT: Client Connection/Register Handle: 0x%04x\n", __FUNCTION__, eventData->disc.handle);
                {
                    nativeNfcLlcp_abortClientWaits();
                }
                break;
            case NFA_P2P_DATA_EVT:
                NXPLOG_API_D ("%s: NFA_P2P_DATA_EVT: Handle: 0x%04x Remote SAP: 0x%04x\n", __FUNCTION__,
                        eventData->data.handle, eventData->data.remote_sap);
                bDestSap = eventData->data.remote_sap;
                sLlcpConnLessHandle = eventData->data.handle;
                NXPLOG_API_D("nfaLlcpServerCallBack: remote sap ID 0x%04x\n ", bDestSap);
                /* Chekthe Data event */
                nativeNfcLlcp_doClientReadCompleted(NFA_STATUS_OK);
                break;
            case NFA_P2P_CONGEST_EVT:
                NXPLOG_API_D ("%s: NFA_SNEP_ALLOC_BUFF_EVT: Handle: 0x%04x\n", __FUNCTION__, eventData->congest.handle);
                break;
            case NFA_P2P_SDP_EVT:
                    bDestSap = eventData->sdp.remote_sap;
                    sNfaLlcpSdpEvt.notifyOne();
                    NXPLOG_API_D ("nfaLlcpServerCallBack: 0x%04x\n", bDestSap);
                break;
            default:
                NXPLOG_API_D ("%s: unknown event 0x%X\n", LlcpEvent);
                break;
        }
    }
}

/**
 * LLCP Server callback from middleware
 */
void nfaLlcpServerCallBack(tNFA_P2P_EVT eP2pEvent,tNFA_P2P_EVT_DATA *psP2pEventData)
{
    NXPLOG_API_D("MwIf>%s:Enter: Event = 0x%x",__FUNCTION__,eP2pEvent);
    if(psP2pEventData != NULL)
    {
        switch (eP2pEvent)
        {
            case NFA_P2P_REG_SERVER_EVT:
            {
                sLlcpConnLessServerHandle = psP2pEventData->reg_server.server_handle;
                {
                    SyncEventGuard guard (sNfaLlcpServerRegEvent);
                    sNfaLlcpServerRegEvent.notifyOne ();
                }
            }
            break;
            case NFA_P2P_ACTIVATED_EVT:
            {
                NXPLOG_API_D("nfaLlcpServerCallBack: P2P Activated !! \n");
                if((psP2pEventData->activated.handle) &&
                (sLlcpConnLessServerHandle == psP2pEventData->activated.handle))
            {
                sLlcpConnLessHandle = psP2pEventData->activated.handle;
                sLlcpConnLessConnectedHandle = psP2pEventData->connected.conn_handle;
                NXPLOG_API_D("nfaLlcpServerCallBack: remote sap ID 0x%04x\n ", bDestSap);
                nativeNfcLlcp_notifyServerActivated();
            }
            }
            break;
            case NFA_P2P_DEACTIVATED_EVT:
            {
                NXPLOG_API_D("nfaLlcpServerCallBack P2P Deactivated !! \n");
                nativeNfcLlcp_notifyServerDeactivated();
            }
            break;
            case NFA_P2P_CONNECTED_EVT:
            {
                NXPLOG_API_D("nfaLlcpServerCallBack: P2P Connected Event \n ");
            }
            break;
            case NFA_P2P_DISC_EVT:
            {
                NXPLOG_API_D("nfaLlcpServerCallBack: P2P Discovery Event \n ");
            }
            break;
            case NFA_P2P_DATA_EVT:
            {
                NXPLOG_API_D("nfaLlcpServerCallBack: P2P Data Event ");
                bDestSap = psP2pEventData->data.remote_sap;
                sLlcpConnLessHandle = psP2pEventData->data.handle;
                NXPLOG_API_D("nfaLlcpServerCallBack: remote sap ID 0x%04x\n ", bDestSap);
                nativeNfcLlcp_doServerReadCompleted(NFA_STATUS_OK);
            }
            break;
            case NFA_P2P_CONGEST_EVT:
            {
                NXPLOG_API_D("nfaLlcpServerCallBack: P2P Link Congestion Event \n");
            }
            break;
            case NFA_P2P_SDP_EVT:
                bDestSap = psP2pEventData->sdp.remote_sap;
                sNfaLlcpSdpEvt.notifyOne();
                NXPLOG_API_D ("nfaLlcpServerCallBack: 0x%04x\n", bDestSap);
            break;
            default:
                NXPLOG_API_D("P2P Unknown Event");
            break;
        }

    }

     NXPLOG_API_D("MwIf>%s:Exit",__FUNCTION__);
    return;
}

INT32 nativeNfcLlcp_ConnLessStartServer(nfcllcpConnlessServerCallback_t *serverCallback)
 {
    tNFA_STATUS status = NFA_STATUS_OK;
    int ret;
    pthread_t llcpRespThread;
    bServerReadState = FALSE;
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
    if (sLlcpServerState == LLCP_SERVER_STARTED && serverCallback == sServerCallback)
    {
        NXPLOG_API_D ("%s: alread started!", __FUNCTION__);
        gSyncMutex.unlock();
        return NFA_STATUS_OK;
    }
    if (sLlcpServerState != LLCP_SERVER_IDLE)
    {
        NXPLOG_API_E ("%s: Server is started or busy. State = 0x%X", __FUNCTION__, sLlcpServerState);
        gSyncMutex.unlock();
        return NFA_STATUS_FAILED;
    }
    sServerCallback = serverCallback;
    sLlcpServerState = LLCP_SERVER_STARTING;
    sRfEnabled = isDiscoveryStarted();
    if (sRfEnabled)
    {
        /* Stop RF Discovery if we were polling */
        startRfDiscovery (FALSE);
    }
    SyncEventGuard guard (sNfaLlcpServerRegEvent);
    if(NFA_STATUS_OK != NFA_P2pRegisterServer ( LLCP_CL_SAP_ID_DEFAULT,
                                    NFA_P2P_LLINK_TYPE,
                                    (char *)LLCP_SERVER_NAME,
                                    nfaLlcpServerCallBack))
    {
        status = NFA_STATUS_FAILED;
        sLlcpServerState = LLCP_SERVER_IDLE;
        sServerCallback = NULL;
        if (sRfEnabled)
        {
            /*  Rollback to default */
            startRfDiscovery (TRUE);
            gSyncMutex.unlock();
            return status;
        }
    }
    sNfaLlcpServerRegEvent.wait();

    gSyncMutex.unlock();
    return status;


 }


void nativeNfcLlcp_ConnLessStopServer()
{
    NXPLOG_API_D ("%s: enter\n", __FUNCTION__);
    NFA_P2pDeregister(sLlcpConnLessServerHandle);
    sLlcpConnLessServerHandle = NULL;
    bDestSap = 0x00;
    dwLlcpReadLength = 0x00;
    blMoreDataRemaining = 0x00;
    bLlcpReadData[LLCP_MAX_DATA_SIZE] = {0};
    sLlcpServerState = LLCP_SERVER_IDLE;
    sServerCallback = NULL;
    NXPLOG_API_D ("%s: exit\n", __FUNCTION__);
}

void nativeNfcLlcp_ConnLessDeregisterClientCallback()
{
    NXPLOG_API_D ("%s: enter\n", __FUNCTION__);
    NFA_P2pDeregister(sLlcpConnLessClientHandle);
    sLlcpConnLessClientHandle = NULL;
    bDestSap = 0x00;
    dwLlcpReadLength = 0x00;
    blMoreDataRemaining = 0x00;
    bLlcpReadData[LLCP_MAX_DATA_SIZE] = {0};
    sClientCallback = NULL;
    NXPLOG_API_D ("%s: exit\n", __FUNCTION__);
}


 INT32 nativeNfcLlcp_ConnLessReceiveMessage(UINT8* msg, UINT32 *length)
 {
        NXPLOG_API_D ("%s: enter\n", __FUNCTION__);
        if(msg == NULL || length == NULL){
            NXPLOG_API_E ("%s: Invalid buffer or length", __FUNCTION__);
            return NFA_STATUS_FAILED;
        }

        NXPLOG_API_D("nfaLlcpServerCallBack: remote sap ID 0x%04x\n ", bDestSap);
        if( (bServerReadState == FALSE) && (bClientReadState == FALSE) )
        {
            sNfaLlcpConnLessReadEvent.wait();
        }

        if(NFA_STATUS_OK != NFA_P2pReadUI ((size_t)sLlcpConnLessHandle,
                     LLCP_MAX_DATA_SIZE,
                     &bDestSap,
                     &dwLlcpReadLength,
                     &bLlcpReadData[0],
                     &blMoreDataRemaining))
        {
            NXPLOG_API_D ("%s: send response failed.", __FUNCTION__);
            return NFA_STATUS_FAILED;
        }
        else
        {
            memcpy(msg,bLlcpReadData, dwLlcpReadLength);
            *length = dwLlcpReadLength;
            NXPLOG_API_D ("%s: exit\n", __FUNCTION__);
            bServerReadState = FALSE;
            bClientReadState = FALSE;
            return NFA_STATUS_OK;
        }

 }

 INT32 nativeNfcLlcp_ConnLessSendMessage(UINT8* msg, UINT32 length)
 {
     tNFA_STATUS bNfaStatus;
     NXPLOG_API_D ("%s: enter\n", __FUNCTION__);
    if(msg == NULL || length <= 0){
        NXPLOG_API_E ("%s: Invalid buffer or length", __FUNCTION__);
        return NFA_STATUS_FAILED;
    }

    NXPLOG_API_D("nfaLlcpServerCallBack: remote sap ID 0x%04x\n ", bDestSap);

    bNfaStatus = NFA_P2pSendUI(sLlcpConnLessHandle,
                           bDestSap,
                           length,
                           msg);
    if(bNfaStatus != NFA_STATUS_OK)
    {
        NXPLOG_API_E ("%s: Error in send message\n", __FUNCTION__);
        return  NFA_STATUS_FAILED;
    }
     NXPLOG_API_D ("%s: exit\n", __FUNCTION__);
     return NFA_STATUS_OK;
 }
