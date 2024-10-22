/******************************************************************************
 *
 *  Copyright 2020 NXP
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
#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <nfa_api.h>
#include <phNxpExtns.h>
#include <SyncEvent.h>
#include "nfc_config.h"

using android::base::StringPrintf;

#if (NFC_AGC_DEBUG_FEATURE == TRUE)

/* AGC Command Frame period in ms */
#define NFC_AGC_INTERFRAME_PERIOD 500U
#define NFC_AGC_RSSI_VAL_SIZE     0xFFU
#define NFC_AGC_RESP_WAIT_TIME    1000U

/*
 * Extns module status
 */

typedef enum { EXTNS_STATUS_OPEN = 0, EXTNS_STATUS_CLOSE } phNxpExtns_Status;

enum AgcState
{
    AgcStateOff = 0,
    AgcStateStarted = 1,
    AgcStateRunning = 2,
    AgcStateStopped = 3,
    AgcStateExit = AgcStateOff
};

typedef struct debugAgcEnable
{
    SyncEvent     debugAgcSyncEvt;
    SyncEvent     debugAgcStopEvt;
    tNFA_STATUS   debugAgcCmdStatus;
    uint8_t       debugAgcRspData[NFC_AGC_RSSI_VAL_SIZE];
    uint8_t       debugAgcRspLen;
    AgcState      debugAgcState; // flag to indicate agc ongoing, running or stopped.
    bool          debugAgcEnable; // config param
}debugAgcEnable_t;

static debugAgcEnable_t enableDebugAgc;
static void *enableAgcThread(void *arg);
void EXTNS_DebugAgcCfg(uint8_t rfState);
static void setAgcProcessState(AgcState state);
static AgcState getAgcProcessState();
static tNFA_STATUS sendAgcDebugCmd();

extern bool nfc_debug_enabled;

extern phNxpExtns_Status EXTNS_GetStatus(void);

/*******************************************************************************
**
** Function:        phNxpAgcDebug_Cfg
**
** Description:     Enable/Disable Dynamic RSSI feature.
**
** Returns:         None
**
*******************************************************************************/
void EXTNS_DebugAgcCfg(uint8_t rfState)
{
    unsigned long enableAgcDebug = 0;
    int retvalue = 0xFF;
    enableAgcDebug = NfcConfig::getUnsigned(NAME_NXP_AGC_DEBUG_ENABLE, 0x00);
    enableDebugAgc.debugAgcEnable = (bool) enableAgcDebug;
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s ,%lu:", __func__, enableAgcDebug);

    if(EXTNS_STATUS_CLOSE == EXTNS_GetStatus())
    {
        SyncEventGuard guard(enableDebugAgc.debugAgcStopEvt);
        enableDebugAgc.debugAgcStopEvt.notifyOne ();
        return;
    }

    if(enableDebugAgc.debugAgcEnable && rfState )
    {
        if (getAgcProcessState() == AgcStateOff)
        {
            pthread_t agcThread;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            retvalue = pthread_create(&agcThread, &attr, enableAgcThread, NULL);
            pthread_attr_destroy(&attr);
        }
    }
    else
    {
        if(!enableDebugAgc.debugAgcEnable)
        {
            DLOG_IF(INFO, nfc_debug_enabled)
                << StringPrintf("%s AgcDebug not enabled", __func__);
        }
        else
        {
            SyncEventGuard syncGuard(enableDebugAgc.debugAgcSyncEvt);
            enableDebugAgc.debugAgcSyncEvt.notifyOne ();
            SyncEventGuard stopGuard(enableDebugAgc.debugAgcStopEvt);
            enableDebugAgc.debugAgcStopEvt.notifyOne ();
        }
    }
}

void *enableAgcThread(void *arg)
{
    tNFA_STATUS status = NFA_STATUS_FAILED;

    setAgcProcessState(AgcStateStarted);

    while( getAgcProcessState())
    {
        if(getAgcProcessState() == AgcStateStopped)
        {
            break;
        }

        if(EXTNS_STATUS_CLOSE == EXTNS_GetStatus())
        {
            setAgcProcessState(AgcStateExit);
            break;
        }

        status = sendAgcDebugCmd();
        if(status == NFA_STATUS_OK)
        {
            DLOG_IF(INFO, nfc_debug_enabled)
                << StringPrintf("%s:  enable success exit", __func__);
        }
#if 1
        SyncEventGuard guard(enableDebugAgc.debugAgcStopEvt);
        bool stopWait = enableDebugAgc.debugAgcStopEvt.wait(NFC_AGC_INTERFRAME_PERIOD);
        if (stopWait)
        {
            setAgcProcessState(AgcStateExit);
            break;
        }
#else
        usleep((NFC_AGC_INTERFRAME_PERIOD*1000));
#endif
    }
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
    pthread_exit(NULL);
    return NULL;
}

/*******************************************************************************
 **
 ** Function:       setAgcProcessState
 **
 ** Description:    sets the AGC process to stop
 **
 ** Returns:        None .
 **
 *******************************************************************************/
void setAgcProcessState(AgcState state)
{
    enableDebugAgc.debugAgcState = state;
}

/*******************************************************************************
 **
 ** Function:       getAgcProcessState
 **
 ** Description:    returns the AGC process state.
 **
 ** Returns:        true/false .
 **
 *******************************************************************************/
static AgcState getAgcProcessState()
{
    return enableDebugAgc.debugAgcState;
}

/*******************************************************************************
 **
 ** Function:        printDataByte()
 **
 ** Description:     Prints the AGC values
 **
 ** Returns:         success/failure
 **
 *******************************************************************************/
static void printDataByte(uint16_t param_len, uint8_t *p_param)
{
    char print_buffer[param_len * 3 + 1];
    memset (print_buffer, 0, sizeof(print_buffer));
    for (int i = 3; i < param_len; i++)
    {
        snprintf(&print_buffer[i * 2], 3 ,"%02X", p_param[i]);
    }
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: AGC Dynamic RSSI values  = %s",__func__, print_buffer);
}

static void nfcManagerSetCbStatus(tNFA_STATUS status)
{
	enableDebugAgc.debugAgcCmdStatus = status;
}

static tNFA_STATUS nfcManagerGetCbStatus(void)
{
    return enableDebugAgc.debugAgcCmdStatus;
}

/*******************************************************************************
 **
 ** Function:        NxpResponse_EnableAGCDebug_Cb()
 **
 ** Description:     Cb to handle the response of AGC command
 **
 ** Returns:         success/failure
 **
 *******************************************************************************/
static void nfcManagerAgcDebugCb(uint8_t event, uint16_t param_len, uint8_t *p_param)
{
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: Received length data = 0x%x",__func__, param_len);
    if(param_len > 0)
    {
    	enableDebugAgc.debugAgcRspLen = param_len;
        memcpy(enableDebugAgc.debugAgcRspData, p_param,
                            enableDebugAgc.debugAgcRspLen);
        nfcManagerSetCbStatus(NFA_STATUS_OK);
    }
    else
    {
        nfcManagerSetCbStatus(NFA_STATUS_FAILED);
    }
    SyncEventGuard guard(enableDebugAgc.debugAgcSyncEvt);
    enableDebugAgc.debugAgcSyncEvt.notifyOne ();
}


/*******************************************************************************
 **
 ** Function:        sendAgcDebugCmd()
 **
 ** Description:     Sends the AGC Debug command.This enables dynamic RSSI
 **                  look up table filling for different "TX RF settings" and enables
 **                  MWdebug prints.
 **
 ** Returns:         success/failure
 **
 *******************************************************************************/
static tNFA_STATUS sendAgcDebugCmd()
{
    tNFA_STATUS status = NFA_STATUS_FAILED;
    uint8_t get_rssi_val[] = {0x2F, 0x33, 0x04, 0x40, 0x00, 0x40, 0xD8};

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: Enter",__func__);
    SyncEventGuard guard (enableDebugAgc.debugAgcSyncEvt);
    nfcManagerSetCbStatus(NFA_STATUS_FAILED);
	enableDebugAgc.debugAgcRspLen = 0;
    memset(enableDebugAgc.debugAgcRspData, 0, NFC_AGC_RSSI_VAL_SIZE);
    status = NFA_SendRawVsCommand(sizeof(get_rssi_val), get_rssi_val, nfcManagerAgcDebugCb);
    if (status == NFA_STATUS_OK)
    {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: Success NFA_SendRawVsCommand",__func__);
        enableDebugAgc.debugAgcSyncEvt.wait(NFC_AGC_RESP_WAIT_TIME); /* wait for callback */
    }
    else
    {    status = NFA_STATUS_FAILED;
         DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: Failed NFA_SendRawVsCommand", __func__);
    }
    status = nfcManagerGetCbStatus();
    if(status == NFA_STATUS_OK && enableDebugAgc.debugAgcRspLen > 0)
    {
        printDataByte(enableDebugAgc.debugAgcRspLen, enableDebugAgc.debugAgcRspData);
    }
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: Exit",__func__);
    return status;
}

#endif
