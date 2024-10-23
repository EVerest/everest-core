/******************************************************************************
 *
 *  Copyright 2015-2021 NXP
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
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "linux_nfc_api.h"
#include "tools.h"
typedef enum eDevState
{
    eDevState_NONE,
    eDevState_WAIT_ARRIVAL,
    eDevState_PRESENT,
    eDevState_WAIT_DEPARTURE,
    eDevState_DEPARTED,
    eDevState_EXIT
}eDevState;
typedef enum eSnepClientState
{
    eSnepClientState_WAIT_OFF,
    eSnepClientState_OFF,
    eSnepClientState_WAIT_READY,
    eSnepClientState_READY,
    eSnepClientState_EXIT
}eSnepClientState;
typedef enum eHCEState
{
    eHCEState_NONE,
    eHCEState_WAIT_DATA,
    eHCEState_DATA_RECEIVED,
    eHCEState_EXIT
}eHCEState;
typedef enum eDevType
{
    eDevType_NONE,
    eDevType_TAG,
    eDevType_P2P,
    eDevType_READER
}eDevType;
typedef enum T4T_NDEF_EMU_state_t
{
    Ready,
    NDEF_Application_Selected,
    CC_Selected,
    NDEF_Selected
} T4T_NDEF_EMU_state_t;
static void* g_ThreadHandle = NULL;
static void* g_devLock = NULL;
static void* g_SnepClientLock = NULL;
static void* g_HCELock = NULL;
static eDevState g_DevState = eDevState_NONE;
static eDevType g_Dev_Type = eDevType_NONE;
static eSnepClientState g_SnepClientState = eSnepClientState_OFF;
static eHCEState g_HCEState = eHCEState_NONE;
static nfc_tag_info_t g_TagInfo;
static nfcTagCallback_t g_TagCB;
static nfcHostCardEmulationCallback_t g_HceCB;
static nfcSnepServerCallback_t g_SnepServerCB;
static nfcSnepClientCallback_t g_SnepClientCB;
unsigned char *HCE_data = NULL;
unsigned int HCE_dataLenght = 0x00;
const unsigned char T4T_NDEF_EMU_APP_Select[] = {0x00,0xA4,0x04,0x00,0x07,0xD2,0x76,0x00,0x00,0x85,0x01,0x01};
const unsigned char T4T_NDEF_EMU_CC[] = {0x00, 0x0F, 0x20, 0x00, 0xFF, 0x00, 0xFF, 0x04, 0x06, 0xE1, 0x04, 0x00, 0xFF, 0x00, 0xFF};
const unsigned char T4T_NDEF_EMU_CC_Select[] = {0x00,0xA4,0x00,0x0C,0x02,0xE1,0x03};
const unsigned char T4T_NDEF_EMU_NDEF_Select[] = {0x00,0xA4,0x00,0x0C,0x02,0xE1,0x04};
const unsigned char T4T_NDEF_EMU_Read[] = {0x00,0xB0};
const unsigned char T4T_NDEF_EMU_OK[] = {0x90, 0x00};
const unsigned char T4T_NDEF_EMU_NOK[] = {0x6A, 0x82};
unsigned char *pT4T_NdefRecord = NULL;
unsigned short T4T_NdefRecord_size = 0;

typedef void T4T_NDEF_EMU_Callback_t (unsigned char*, unsigned short);
static T4T_NDEF_EMU_state_t eT4T_NDEF_EMU_State = Ready;
static T4T_NDEF_EMU_Callback_t *pT4T_NDEF_EMU_PushCb = NULL;
void help(int mode);
int InitEnv();
int LookForTag(char** args, int args_len, char* tag, char** data, int format);
/********************************** HCE **********************************/
static void T4T_NDEF_EMU_FillRsp (unsigned char *pRsp, unsigned short offset, unsigned char length)
{
    if (offset == 0)
    {
        pRsp[0] = (T4T_NdefRecord_size & 0xFF00) >> 8;
        pRsp[1] = (T4T_NdefRecord_size & 0x00FF);
        memcpy(&pRsp[2], &pT4T_NdefRecord[0], length-2);
    }
    else if (offset == 1)
    {
        pRsp[0] = (T4T_NdefRecord_size & 0x00FF);
        memcpy(&pRsp[1], &pT4T_NdefRecord[0], length-1);
    }
    else
    {
        memcpy(pRsp, &pT4T_NdefRecord[offset-2], length);
    }
    /* Did we reached the end of NDEF record ?*/
    if ((offset + length) >= (T4T_NdefRecord_size + 2))
    {
        /* Notify application of the NDEF send */
        if(pT4T_NDEF_EMU_PushCb != NULL) pT4T_NDEF_EMU_PushCb(pT4T_NdefRecord, T4T_NdefRecord_size);
    }
}
void T4T_NDEF_EMU_SetRecord(unsigned char *pRecord, unsigned short Record_size, T4T_NDEF_EMU_Callback_t *cb)
{
    pT4T_NdefRecord = pRecord;
    T4T_NdefRecord_size = Record_size;
    pT4T_NDEF_EMU_PushCb =  cb;
}
void T4T_NDEF_EMU_Reset(void)
{
    eT4T_NDEF_EMU_State = Ready;
}
void T4T_NDEF_EMU_Next(unsigned char *pCmd, unsigned short Cmd_size, unsigned char *pRsp, unsigned short *pRsp_size)
{
    unsigned char eStatus = 0x00;
    if (!memcmp(pCmd, T4T_NDEF_EMU_APP_Select, sizeof(T4T_NDEF_EMU_APP_Select)))
    {
        *pRsp_size = 0;
        eStatus = 0x01;
        eT4T_NDEF_EMU_State = NDEF_Application_Selected;
    }
    else if (!memcmp(pCmd, T4T_NDEF_EMU_CC_Select, sizeof(T4T_NDEF_EMU_CC_Select)))
    {
        if(eT4T_NDEF_EMU_State == NDEF_Application_Selected)
        {
            *pRsp_size = 0;
            eStatus = 0x01;
            eT4T_NDEF_EMU_State = CC_Selected;
        }
    }
    else if (!memcmp(pCmd, T4T_NDEF_EMU_NDEF_Select, sizeof(T4T_NDEF_EMU_NDEF_Select)))
    {
        *pRsp_size = 0;
        eStatus = 0x01;
        eT4T_NDEF_EMU_State = NDEF_Selected;
    }
    else if (!memcmp(pCmd, T4T_NDEF_EMU_Read, sizeof(T4T_NDEF_EMU_Read)))
    {
        if(eT4T_NDEF_EMU_State == CC_Selected)
        {
            memcpy(pRsp, T4T_NDEF_EMU_CC, sizeof(T4T_NDEF_EMU_CC));
            *pRsp_size = sizeof(T4T_NDEF_EMU_CC);
            eStatus = 0x01;
        }
        else if (eT4T_NDEF_EMU_State == NDEF_Selected)
        {
            unsigned short offset = (pCmd[2] << 8) + pCmd[3];
            unsigned char length = pCmd[4];
            if(length <= (T4T_NdefRecord_size + offset + 2))
            {
                T4T_NDEF_EMU_FillRsp(pRsp, offset, length);
                *pRsp_size = length;
                eStatus = 0x01;
            }
        }
    }
    if (eStatus == 0x01)
    {
        memcpy(&pRsp[*pRsp_size], T4T_NDEF_EMU_OK, sizeof(T4T_NDEF_EMU_OK));
        *pRsp_size += sizeof(T4T_NDEF_EMU_OK);
    } 
    else
    {
        memcpy(pRsp, T4T_NDEF_EMU_NOK, sizeof(T4T_NDEF_EMU_NOK));
        *pRsp_size = sizeof(T4T_NDEF_EMU_NOK);
        T4T_NDEF_EMU_Reset();
    }
}
/********************************** CallBack **********************************/
void onDataReceived(unsigned char *data, unsigned int data_length)
{
    framework_LockMutex(g_HCELock);
    
    HCE_dataLenght = data_length;
    HCE_data = malloc(HCE_dataLenght * sizeof(unsigned char));
    memcpy(HCE_data, data, data_length);
    
    if(eHCEState_NONE == g_HCEState)
    {
        g_HCEState = eHCEState_DATA_RECEIVED;
    }
    else if (eHCEState_WAIT_DATA == g_HCEState)
    {
        g_HCEState = eHCEState_DATA_RECEIVED;
        framework_NotifyMutex(g_HCELock, 0);
    }
    
    framework_UnlockMutex(g_HCELock);
}
void onHostCardEmulationActivated(unsigned char mode)
{
    framework_LockMutex(g_devLock);
    
    T4T_NDEF_EMU_Reset();
    
    if(eDevState_WAIT_ARRIVAL == g_DevState)
    {
        printf("\tNFC Reader Found, mode=0x%.2x\n\n", mode);
        g_DevState = eDevState_PRESENT;
        g_Dev_Type = eDevType_READER;
        framework_NotifyMutex(g_devLock, 0);
    }
    else if(eDevState_WAIT_DEPARTURE == g_DevState)
    {
        g_DevState = eDevState_PRESENT;
        g_Dev_Type = eDevType_READER;
        framework_NotifyMutex(g_devLock, 0);
    }
    else if(eDevState_EXIT == g_DevState)
    {
        g_DevState = eDevState_DEPARTED;
        g_Dev_Type = eDevType_NONE;
        framework_NotifyMutex(g_devLock, 0);
    }
    else
    {
        g_DevState = eDevState_PRESENT;
        g_Dev_Type = eDevType_READER;
    }
    framework_UnlockMutex(g_devLock);
}
void onHostCardEmulationDeactivated()
{
    framework_LockMutex(g_devLock);
    
    if(eDevState_WAIT_DEPARTURE == g_DevState)
    {
        printf("\tNFC Reader Lost\n\n");
        g_DevState = eDevState_DEPARTED;
        g_Dev_Type = eDevType_NONE;
        framework_NotifyMutex(g_devLock, 0);
    }
    else if(eDevState_PRESENT == g_DevState)
    {
        printf("\tNFC Reader Lost\n\n");
        g_DevState = eDevState_DEPARTED;
        g_Dev_Type = eDevType_NONE;
    }
    else if(eDevState_WAIT_ARRIVAL == g_DevState)
    {
    }
    else if(eDevState_EXIT == g_DevState)
    {
    }
    else
    {
        g_DevState = eDevState_DEPARTED;
        g_Dev_Type = eDevType_NONE;
    }
    framework_UnlockMutex(g_devLock);
    
    framework_LockMutex(g_HCELock);
    if(eHCEState_WAIT_DATA == g_HCEState)
    {
        g_HCEState = eHCEState_NONE;
        framework_NotifyMutex(g_HCELock, 0x00);
    }
    else if(eHCEState_EXIT == g_HCEState)
    {
        
    }
    else
    {
        g_HCEState = eHCEState_NONE;
    }
    framework_UnlockMutex(g_HCELock);
}
 
void onTagArrival(nfc_tag_info_t *pTagInfo)
{
    framework_LockMutex(g_devLock);
    
    if(eDevState_WAIT_ARRIVAL == g_DevState)
    {    
        printf("\tNFC Tag Found\n\n");
        memcpy(&g_TagInfo, pTagInfo, sizeof(nfc_tag_info_t));
        g_DevState = eDevState_PRESENT;
        g_Dev_Type = eDevType_TAG;
        framework_NotifyMutex(g_devLock, 0);
    }
    else if(eDevState_WAIT_DEPARTURE == g_DevState)
    {    
        memcpy(&g_TagInfo, pTagInfo, sizeof(nfc_tag_info_t));
        g_DevState = eDevState_PRESENT;
        g_Dev_Type = eDevType_TAG;
        framework_NotifyMutex(g_devLock, 0);
    }
    else if(eDevState_EXIT == g_DevState)
    {
        g_DevState = eDevState_DEPARTED;
        g_Dev_Type = eDevType_NONE;
        framework_NotifyMutex(g_devLock, 0);
    }
    else
    {
        g_DevState = eDevState_PRESENT;
        g_Dev_Type = eDevType_TAG;
    }
    framework_UnlockMutex(g_devLock);
}
void onTagDeparture(void)
{    
    framework_LockMutex(g_devLock);
    
    
    if(eDevState_WAIT_DEPARTURE == g_DevState)
    {
        printf("\tNFC Tag Lost\n\n");
        g_DevState = eDevState_DEPARTED;
        g_Dev_Type = eDevType_NONE;
        framework_NotifyMutex(g_devLock, 0);
    }
    else if(eDevState_WAIT_ARRIVAL == g_DevState)
    {
    }    
    else if(eDevState_EXIT == g_DevState)
    {
    }
    else
    {
        g_DevState = eDevState_DEPARTED;
        g_Dev_Type = eDevType_NONE;
    }
    framework_UnlockMutex(g_devLock);
}
void onDeviceArrival (void)
{
    framework_LockMutex(g_devLock);
    
    switch(g_DevState)
    {
        case eDevState_WAIT_DEPARTURE:
        {
            g_DevState = eDevState_PRESENT;
            g_Dev_Type = eDevType_P2P;
            framework_NotifyMutex(g_devLock, 0);
        } break;
        case eDevState_EXIT:
        {
            g_Dev_Type = eDevType_P2P;
        } break;
        case eDevState_NONE:
        {
            g_DevState = eDevState_PRESENT;
            g_Dev_Type = eDevType_P2P;
        } break;
        case eDevState_WAIT_ARRIVAL:
        {
            g_DevState = eDevState_PRESENT;
            g_Dev_Type = eDevType_P2P;
            framework_NotifyMutex(g_devLock, 0);
        } break;
        case eDevState_PRESENT:
        {
            g_Dev_Type = eDevType_P2P;
        } break;
        case eDevState_DEPARTED:
        {
            g_Dev_Type = eDevType_P2P;
            g_DevState = eDevState_PRESENT;
        } break;
    }
    
    framework_UnlockMutex(g_devLock);
}

void onDeviceDeparture (void)
{
    framework_LockMutex(g_devLock);
    
    switch(g_DevState)
    {
        case eDevState_WAIT_DEPARTURE:
        {
            g_DevState = eDevState_DEPARTED;
            g_Dev_Type = eDevType_NONE;
            framework_NotifyMutex(g_devLock, 0);
        } break;
        case eDevState_EXIT:
        {
            g_Dev_Type = eDevType_NONE;
        } break;
        case eDevState_NONE:
        {
            g_Dev_Type = eDevType_NONE;
        } break;
        case eDevState_WAIT_ARRIVAL:
        {
            g_Dev_Type = eDevType_NONE;
        } break;
        case eDevState_PRESENT:
        {
            g_Dev_Type = eDevType_NONE;
            g_DevState = eDevState_DEPARTED;
        } break;
        case eDevState_DEPARTED:
        {
            g_Dev_Type = eDevType_NONE;
        } break;
    }
    framework_UnlockMutex(g_devLock);
    
    framework_LockMutex(g_SnepClientLock);
    
    switch(g_SnepClientState)
    {
        case eSnepClientState_WAIT_OFF:
        {
            g_SnepClientState = eSnepClientState_OFF;
            framework_NotifyMutex(g_SnepClientLock, 0);
        } break;
        case eSnepClientState_OFF:
        {
        } break;
        case eSnepClientState_WAIT_READY:
        {
            g_SnepClientState = eSnepClientState_OFF;
            framework_NotifyMutex(g_SnepClientLock, 0);
        } break;
        case eSnepClientState_READY:
        {
            g_SnepClientState = eSnepClientState_OFF;
        } break;
        case eSnepClientState_EXIT:
        {
        } break;
    }
    
    framework_UnlockMutex(g_SnepClientLock);
}

void onMessageReceived(unsigned char *message, unsigned int length)
{
    unsigned int i = 0x00;
    printf("\n\t\tNDEF Message Received : \n");
    PrintNDEFContent(NULL, NULL, message, length);
}
void onSnepClientReady()
{
    framework_LockMutex(g_devLock);
    
    switch(g_DevState)
    {
        case eDevState_WAIT_DEPARTURE:
        {
            g_DevState = eDevState_PRESENT;
            g_Dev_Type = eDevType_P2P;
            framework_NotifyMutex(g_devLock, 0);
        } break;
        case eDevState_EXIT:
        {
            g_Dev_Type = eDevType_P2P;
        } break;
        case eDevState_NONE:
        {
            g_DevState = eDevState_PRESENT;
            g_Dev_Type = eDevType_P2P;
        } break;
        case eDevState_WAIT_ARRIVAL:
        {
            g_DevState = eDevState_PRESENT;
            g_Dev_Type = eDevType_P2P;
            framework_NotifyMutex(g_devLock, 0);
        } break;
        case eDevState_PRESENT:
        {
            g_Dev_Type = eDevType_P2P;
        } break;
        case eDevState_DEPARTED:
        {
            g_Dev_Type = eDevType_P2P;
            g_DevState = eDevState_PRESENT;
        } break;
    }
    framework_UnlockMutex(g_devLock);
    
    framework_LockMutex(g_SnepClientLock);
    
    switch(g_SnepClientState)
    {
        case eSnepClientState_WAIT_OFF:
        {
            g_SnepClientState = eSnepClientState_READY;
            framework_NotifyMutex(g_SnepClientLock, 0);
        } break;
        case eSnepClientState_OFF:
        {
            g_SnepClientState = eSnepClientState_READY;
        } break;
        case eSnepClientState_WAIT_READY:
        {
            g_SnepClientState = eSnepClientState_READY;
            framework_NotifyMutex(g_SnepClientLock, 0);
        } break;
        case eSnepClientState_READY:
        {
        } break;
        case eSnepClientState_EXIT:
        {
        } break;
    }
    
    framework_UnlockMutex(g_SnepClientLock);
}

void onSnepClientClosed()
{
    framework_LockMutex(g_devLock);
    
    switch(g_DevState)
    {
        case eDevState_WAIT_DEPARTURE:
        {
            g_DevState = eDevState_DEPARTED;
            g_Dev_Type = eDevType_NONE;
            framework_NotifyMutex(g_devLock, 0);
        } break;
        case eDevState_EXIT:
        {
            g_Dev_Type = eDevType_NONE;
        } break;
        case eDevState_NONE:
        {
            g_Dev_Type = eDevType_NONE;
        } break;
        case eDevState_WAIT_ARRIVAL:
        {
            g_Dev_Type = eDevType_NONE;
        } break;
        case eDevState_PRESENT:
        {
            g_Dev_Type = eDevType_NONE;
            g_DevState = eDevState_DEPARTED;
        } break;
        case eDevState_DEPARTED:
        {
            g_Dev_Type = eDevType_NONE;
        } break;
    }
    framework_UnlockMutex(g_devLock);
    
    framework_LockMutex(g_SnepClientLock);
    
    switch(g_SnepClientState)
    {
        case eSnepClientState_WAIT_OFF:
        {
            g_SnepClientState = eSnepClientState_OFF;
            framework_NotifyMutex(g_SnepClientLock, 0);
        } break;
        case eSnepClientState_OFF:
        {
        } break;
        case eSnepClientState_WAIT_READY:
        {
            g_SnepClientState = eSnepClientState_OFF;
            framework_NotifyMutex(g_SnepClientLock, 0);
        } break;
        case eSnepClientState_READY:
        {
            g_SnepClientState = eSnepClientState_OFF;
        } break;
        case eSnepClientState_EXIT:
        {
        } break;
    }
    
    framework_UnlockMutex(g_SnepClientLock);
}
 
int InitMode(int tag, int p2p, int hce)
{
    int res = 0x00;
    
    InitializeLogLevel();
    g_TagCB.onTagArrival = onTagArrival;
    g_TagCB.onTagDeparture = onTagDeparture;
        
    g_SnepServerCB.onDeviceArrival = onDeviceArrival;
    g_SnepServerCB.onDeviceDeparture = onDeviceDeparture;
    g_SnepServerCB.onMessageReceived = onMessageReceived;
    
    g_SnepClientCB.onDeviceArrival = onSnepClientReady;
    g_SnepClientCB.onDeviceDeparture = onSnepClientClosed;
        
    g_HceCB.onDataReceived = onDataReceived;
    g_HceCB.onHostCardEmulationActivated = onHostCardEmulationActivated;
    g_HceCB.onHostCardEmulationDeactivated = onHostCardEmulationDeactivated;
    
    if(0x00 == res)
    {
        res = doInitialize();
        if(0x00 != res)
        {
            printf("NfcService Init Failed\n");
        }
    }
    
    if(0x00 == res)
    {
        if(0x01 == tag)
        {
            registerTagCallback(&g_TagCB);
        }
#ifdef SNEP_ENABLED
        if(0x01 == p2p)
        {
            res = nfcSnep_registerClientCallback(&g_SnepClientCB);
            if(0x00 != res)
            {
                printf("SNEP Client Register Callback Failed\n");
            }
        }
#endif
    }
    
    if(0x00 == res && 0x01 == hce)
    {
        nfcHce_registerHceCallback(&g_HceCB);
    }
    if(0x00 == res)
    {
        doEnableDiscovery(DEFAULT_NFA_TECH_MASK, 0x00, hce, 0);
        if(0x01 == p2p)
        {
#ifdef SNEP_ENABLED
            res = nfcSnep_startServer(&g_SnepServerCB);
            if(0x00 != res)
            {
                printf("Start SNEP Server Failed\n");
            }
#endif
        }
    }
    
    return res;
}

void DeinitPollMode()
{
    nfcSnep_stopServer();
    disableDiscovery();
    
    nfcSnep_deregisterClientCallback();
    deregisterTagCallback();
    
    nfcHce_deregisterHceCallback();
    
    return;
}

int SnepPush(unsigned char* msgToPush, unsigned int len)
{
    int res = 0x00;
    
    framework_LockMutex(g_devLock);
    framework_LockMutex(g_SnepClientLock);
    
    if(eSnepClientState_READY != g_SnepClientState && eSnepClientState_EXIT!= g_SnepClientState && eDevState_PRESENT == g_DevState)
    {
        framework_UnlockMutex(g_devLock);
        g_SnepClientState = eSnepClientState_WAIT_READY;
        framework_WaitMutex(g_SnepClientLock, 0);
    }
    else
    {
        framework_UnlockMutex(g_devLock);
    }
    
    if(eSnepClientState_READY == g_SnepClientState)
    {
        framework_UnlockMutex(g_SnepClientLock);
        res = nfcSnep_putMessage(msgToPush, len);
        if(0x00 != res)
        {
            printf("\t\tPush Failed\n");
        }
        else
        {
            printf("\t\tPush successful\n");
        }
    }
    else
    {
        framework_UnlockMutex(g_SnepClientLock);
    }
    
    return res;
}

int WriteTag(nfc_tag_info_t TagInfo, unsigned char* msgToPush, unsigned int len)
{
    int res = 0x00;
    
    res = nfcTag_writeNdef(TagInfo.handle, msgToPush, len);
    if(0x00 != res)
    {
        res = 0xFF;
    }
    else
    {
        res = 0x00;
    }
    return res;
}

void PrintfNDEFInfo(ndef_info_t pNDEFinfo)
{
    if(0x01 == pNDEFinfo.is_ndef)
    {
        printf("\t\tRecord Found :\n");
        printf("\t\t\t\tNDEF Content Max size :     '%d bytes'\n", pNDEFinfo.max_ndef_length);
        printf("\t\t\t\tNDEF Actual Content size :     '%d bytes'\n", pNDEFinfo.current_ndef_length);
        if(0x01 == pNDEFinfo.is_writable)
        {
            printf("\t\t\t\tReadOnly :                      'FALSE'\n");
        }
        else
        {
            printf("\t\t\t\tReadOnly :                         'TRUE'\n");
        }
    }
    else    
    {
        printf("\t\tNo Record found\n");
    }
}

void open_uri (const char* uri)
{
    char *temp = malloc(strlen("xdg-open ") + strlen(uri) + 1);
    if (temp != NULL)
    {
        strcpy(temp, "xdg-open ");
        strcat(temp, uri);
        strcat(temp, "&");
        printf("\t\t- Opening URI in web browser ...\n");
        system(temp);
        free(temp);
    }
}

void PrintNDEFContent(nfc_tag_info_t* TagInfo, ndef_info_t* NDEFinfo, unsigned char* ndefRaw, unsigned int ndefRawLen)
{
    unsigned char* NDEFContent = NULL;
    nfc_friendly_type_t lNDEFType = NDEF_FRIENDLY_TYPE_OTHER;
    unsigned int res = 0x00;
    unsigned int i = 0x00;
    unsigned int langCode_len;
    char* LanguageCode = NULL;
    char* TextContent = NULL;
    char* URLContent = NULL;
    nfc_handover_select_t HandoverSelectContent;
    nfc_handover_request_t HandoverRequestContent;
    if(NULL != NDEFinfo)
    {
        ndefRawLen = NDEFinfo->current_ndef_length;
        NDEFContent = malloc(ndefRawLen * sizeof(unsigned char));
        memset(NDEFContent,0x0,ndefRawLen * sizeof(unsigned char));
        res = nfcTag_readNdef(TagInfo->handle, NDEFContent, ndefRawLen, &lNDEFType);
    }
    else if (NULL != ndefRaw && 0x00 != ndefRawLen)
    {
        NDEFContent = malloc(ndefRawLen * sizeof(unsigned char));
        memcpy(NDEFContent, ndefRaw, ndefRawLen);
        res = ndefRawLen;
        if((NDEFContent[0] & 0x7) == NDEF_TNF_WELLKNOWN && 0x55 == NDEFContent[3])
        {
            lNDEFType = NDEF_FRIENDLY_TYPE_URL;
        }
        if((NDEFContent[0] & 0x7) == NDEF_TNF_WELLKNOWN && 0x54 == NDEFContent[3])
        {
            lNDEFType = NDEF_FRIENDLY_TYPE_TEXT;
        }
    }
    else
    {
        printf("\t\t\t\tError : Invalid Parameters\n");
    }
    
    if(res != ndefRawLen)
    {
        printf("\t\t\t\tRead NDEF Content Failed\n");
    }
    else
    {
        switch(lNDEFType)
        {
            case NDEF_FRIENDLY_TYPE_TEXT:
            {
                TextContent = malloc(res * sizeof(char));
                langCode_len = ndef_readLanguageCode(NDEFContent, res, TextContent, res);
                if(0x00 <= langCode_len)
                {
                    LanguageCode = malloc((langCode_len + 1) * sizeof(char));
                    memcpy(LanguageCode, TextContent, langCode_len);
                    LanguageCode[langCode_len] = '\0'; /* Add extra character for proper display */
                    res = ndef_readText(NDEFContent, res, TextContent, res);
                    if(0x00 <= res)
                    {
                        TextContent[res] = '\0'; /* Add extra character for proper display */
                        printf("\t\t\t\tType :                 'Text'\n");
                        printf("\t\t\t\tLang :                 '%s'\n", LanguageCode);
                        printf("\t\t\t\tText :                 '%s'\n\n", TextContent);
                    }
                    else
                    {
                        printf("\t\t\t\tRead NDEF Text Error\n");
                    }
                }
                if(NULL != TextContent)
                {
                    free(TextContent);
                    TextContent = NULL;
                }
            } break;
            case NDEF_FRIENDLY_TYPE_URL:
            {
                /*NOTE : + 27 = Max prefix lenght*/
                URLContent = malloc(res * sizeof(unsigned char) + 27 );
                memset(URLContent, 0x00, res * sizeof(unsigned char) + 27);
                res = ndef_readUrl(NDEFContent, res, URLContent, res + 27);
                if(0x00 <= res)
                {
                    printf("\t\t\t\tType :                 'URI'\n");
                    printf("\t\t\t\t URI :                 '%s'\n\n", URLContent);
                    /*NOTE: open url in browser*/
                    /*open_uri(URLContent);*/
                }
                else
                {
                    printf("                Read NDEF URL Error\n");
                }
                if(NULL != URLContent)
                {
                    free(URLContent);
                    URLContent = NULL;
                }
            } break;
            case NDEF_FRIENDLY_TYPE_HS:
            {
                res = ndef_readHandoverSelectInfo(NDEFContent, res, &HandoverSelectContent);
                if(0x00 <= res)
                {
                    printf("\n\t\tHandover Select : \n");
                    
                    printf("\t\tBluetooth : \n\t\t\t\tPower state : ");
                    switch(HandoverSelectContent.bluetooth.power_state)
                    {
                        case HANDOVER_CPS_INACTIVE:
                        {
                            printf(" 'Inactive'\n");
                        } break;
                        case HANDOVER_CPS_ACTIVE:
                        {
                            printf(" 'Active'\n");
                        } break;
                        case HANDOVER_CPS_ACTIVATING:
                        {
                            printf(" 'Activating'\n");
                        } break;
                        case HANDOVER_CPS_UNKNOWN:
                        {
                            printf(" 'Unknown'\n");
                        } break;
                        default:
                        {
                            printf(" 'Unknown'\n");
                        } break;
                    }
                    if(HANDOVER_TYPE_BT == HandoverSelectContent.bluetooth.type)
                    {
                        printf("\t\t\t\tType :         'BT'\n");
                    }
                    else if(HANDOVER_TYPE_BLE == HandoverSelectContent.bluetooth.type)
                    {
                        printf("\t\t\t\tType :         'BLE'\n");
                    }
                    else
                    {
                        printf("\t\t\t\tType :            'Unknown'\n");
                    }
                    printf("\t\t\t\tAddress :      '");
                    for(i = 0x00; i < 6; i++)
                    {
                        printf("%02X ", HandoverSelectContent.bluetooth.address[i]);
                    }
                    printf("'\n\t\t\t\tDevice Name :  '");
                    for(i = 0x00; i < HandoverSelectContent.bluetooth.device_name_length; i++)    
                    {
                        printf("%c ", HandoverSelectContent.bluetooth.device_name[i]);
                    }
                    printf("'\n\t\t\t\tNDEF Record :     \n\t\t\t\t");
                    for(i = 0x01; i < HandoverSelectContent.bluetooth.ndef_length+1; i++)
                    {
                        printf("%02X ", HandoverSelectContent.bluetooth.ndef[i]);
                        if(i%8 == 0)
                        {
                            printf("\n\t\t\t\t");
                        }
                    }
                    printf("\n\t\tWIFI : \n\t\t\t\tPower state : ");
                    switch(HandoverSelectContent.wifi.power_state)
                    {
                        case HANDOVER_CPS_INACTIVE:
                        {
                            printf(" 'Inactive'\n");
                        } break;
                        case HANDOVER_CPS_ACTIVE:
                        {
                            printf(" 'Active'\n");
                        } break;
                        case HANDOVER_CPS_ACTIVATING:
                        {
                            printf(" 'Activating'\n");
                        } break;
                        case HANDOVER_CPS_UNKNOWN:
                        {
                            printf(" 'Unknown'\n");
                        } break;
                        default:
                        {
                            printf(" 'Unknown'\n");
                        } break;
                    }
                    
                    printf("\t\t\t\tSSID :         '");
                    for(i = 0x01; i < HandoverSelectContent.wifi.ssid_length+1; i++)
                    {
                        printf("%02X ", HandoverSelectContent.wifi.ssid[i]);
                        if(i%30 == 0)
                        {
                            printf("\n");
                        }
                    }
                    printf("'\n\t\t\t\tKey :          '");
                    for(i = 0x01; i < HandoverSelectContent.wifi.key_length+1; i++)
                    {
                        printf("%02X ", HandoverSelectContent.wifi.key[i]);
                        if(i%30 == 0)
                        {
                            printf("\n");
                        }
                    }                
                    printf("'\n\t\t\t\tNDEF Record : \n");
                    for(i = 0x01; i < HandoverSelectContent.wifi.ndef_length+1; i++)
                    {
                        printf("%02X ", HandoverSelectContent.wifi.ndef[i]);
                        if(i%30 == 0)
                        {
                            printf("\n");
                        }
                    }
                    printf("\n");
                }
                else
                {
                    printf("\n\t\tRead NDEF Handover Select Failed\n");
                }
                
            } break;
            case NDEF_FRIENDLY_TYPE_HR:
            {
                res = ndef_readHandoverRequestInfo(NDEFContent, res, &HandoverRequestContent);
                if(0x00 <= res)
                {
                    printf("\n\t\tHandover Request : \n");
                    printf("\t\tBluetooth : \n\t\t\t\tPower state : ");
                    switch(HandoverRequestContent.bluetooth.power_state)
                    {
                        case HANDOVER_CPS_INACTIVE:
                        {
                            printf(" 'Inactive'\n");
                        } break;
                        case HANDOVER_CPS_ACTIVE:
                        {
                            printf(" 'Active'\n");
                        } break;
                        case HANDOVER_CPS_ACTIVATING:
                        {
                            printf(" 'Activating'\n");
                        } break;
                        case HANDOVER_CPS_UNKNOWN:
                        {
                            printf(" 'Unknown'\n");
                        } break;
                        default:
                        {
                            printf(" 'Unknown'\n");
                        } break;
                    }
                    if(HANDOVER_TYPE_BT == HandoverRequestContent.bluetooth.type)
                    {
                        printf("\t\t\t\tType :         'BT'\n");
                    }
                    else if(HANDOVER_TYPE_BLE == HandoverRequestContent.bluetooth.type)
                    {
                        printf("\t\t\t\tType :         'BLE'\n");
                    }
                    else
                    {
                        printf("\t\t\t\tType :            'Unknown'\n");
                    }
                    printf("\t\t\t\tAddress :      '");
                    for(i = 0x00; i < 6; i++)
                    {
                        printf("%02X ", HandoverRequestContent.bluetooth.address[i]);
                    }
                    printf("'\n\t\t\t\tDevice Name :  '");
                    for(i = 0x00; i < HandoverRequestContent.bluetooth.device_name_length; i++)    
                    {
                        printf("%c ", HandoverRequestContent.bluetooth.device_name[i]);
                    }
                    printf("'\n\t\t\t\tNDEF Record :     \n\t\t\t\t");
                    for(i = 0x01; i < HandoverRequestContent.bluetooth.ndef_length+1; i++)
                    {
                        printf("%02X ", HandoverRequestContent.bluetooth.ndef[i]);
                        if(i%8 == 0)
                        {
                            printf("\n\t\t\t\t");
                        }
                    }
                    printf("\n\t\t\t\tWIFI :         'Has WIFI Request : %X '", HandoverRequestContent.wifi.has_wifi);
                    printf("\n\t\t\t\tNDEF Record :     \n\t\t\t\t");
                    for(i = 0x01; i < HandoverRequestContent.wifi.ndef_length+1; i++)
                    {
                        printf("%02X ", HandoverRequestContent.wifi.ndef[i]);
                        if(i%8 == 0)
                        {
                            printf("\n\t\t\t\t");
                        }
                    }
                    printf("\n");
                }
                else
                {
                    printf("\n\t\tRead NDEF Handover Request Failed\n");
                }
            } break;
            case NDEF_FRIENDLY_TYPE_OTHER:
            {
                switch(NDEFContent[0] & 0x7)
                {
                    case NDEF_TNF_EMPTY:
                    {
                        printf("\n\t\tTNF Empty\n");
                    } break;
                    case NDEF_TNF_WELLKNOWN:
                    {
                        printf("\n\t\tTNF Well Known\n");
                    } break;
                    case NDEF_TNF_MEDIA:
                    {
                        printf("\n\t\tTNF Media\n\n");
                        printf("\t\t\tType : ");
                        for(i = 0x00; i < NDEFContent[1]; i++)
                        {
                            printf("%c", NDEFContent[3 + i]);
                        }
                        printf("\n\t\t\tData : ");
                        for(i = 0x00; i < NDEFContent[2]; i++)
                         {
                            printf("%c", NDEFContent[3 + NDEFContent[1] + i]);
                            if('\n' == NDEFContent[3 + NDEFContent[1] + i])
                            {
                                printf("\t\t\t");
                            }
                        }
                        printf("\n");
                        
                    } break;
                    case NDEF_TNF_URI:
                    {
                        printf("\n\t\tTNF URI\n");
                    } break;
                    case NDEF_TNF_EXT:
                    {
                        printf("\n\t\tTNF External\n\n");
                        printf("\t\t\tType : ");
                        for(i = 0x00; i < NDEFContent[1]; i++)
                        {
                            printf("%c", NDEFContent[3 + i]);
                        }
                        printf("\n\t\t\tData : ");
                        for(i = 0x00; i < NDEFContent[2]; i++)
                         {
                            printf("%c", NDEFContent[3 + NDEFContent[1] + i]);
                            if('\n' == NDEFContent[3 + NDEFContent[1] + i])
                            {
                                printf("\t\t\t");
                            }
                        }
                        printf("\n");
                    } break;
                    case NDEF_TNF_UNKNOWN:
                    {
                        printf("\n\t\tTNF Unknown\n");
                    } break;
                    case NDEF_TNF_UNCHANGED:
                    {
                        printf("\n\t\tTNF Unchanged\n");
                    } break;
                    default:
                    {
                        printf("\n\t\tTNF Other\n");
                    } break;
                }
            } break;
            default:
            {
            } break;
        }
        printf("\n\t\t%d bytes of NDEF data received :\n\t\t", ndefRawLen);
        for(i = 0x00; i < ndefRawLen; i++)
        {
            printf("%02X ", NDEFContent[i]);
            if(i%30 == 0 && 0x00 != i)
            {
                printf("\n\t\t");
            }
        }
        printf("\n\n");
    }
    
    if(NULL != NDEFContent)
    {
        free(NDEFContent);
        NDEFContent = NULL;
    }
}

/* mode=1 => poll, mode=2 => push, mode=3 => write, mode=4 => HCE */
int WaitDeviceArrival(int mode, unsigned char* msgToSend, unsigned int len)
{
    int res = 0x00;
    unsigned int i = 0x00;
    int block = 0x01;
    unsigned char key[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    ndef_info_t NDEFinfo;
    eDevType DevTypeBck = eDevType_NONE;
    unsigned char MifareAuthCmd[] = {0x60U, 0x00 /*block*/, 0x02, 0x02, 0x02, 0x02, 0x00 /*key*/, 0x00 /*key*/, 0x00 /*key*/, 0x00 /*key*/ , 0x00 /*key*/, 0x00 /*key*/};
    unsigned char MifareAuthResp[255];
    unsigned char MifareReadCmd[] = {0x30U,  /*block*/ 0x00};
    unsigned char MifareWriteCmd[] = {0xA0U,  /*block*/ 0x00, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
    unsigned char MifareResp[255];

    unsigned char HCEReponse[255];
    short unsigned int HCEResponseLen = 0x00;
    int tag_count=0;
    int num_tags = 0;

    nfc_tag_info_t TagInfo;

    MifareAuthCmd[1] = block;
    memcpy(&MifareAuthCmd[6], key, 6);
    MifareReadCmd[1] = block;
    MifareWriteCmd[1] = block;
    
    do
    {
        framework_LockMutex(g_devLock);
        if(eDevState_EXIT == g_DevState)
        {
            framework_UnlockMutex(g_devLock);
            break;
        }
        
        else if(eDevState_PRESENT != g_DevState)
        {
               if(tag_count == 0) printf("Waiting for a Tag/Device...\n\n");
            g_DevState = eDevState_WAIT_ARRIVAL;
            framework_WaitMutex(g_devLock, 0);
        }
        
        if(eDevState_EXIT == g_DevState)
        {
            framework_UnlockMutex(g_devLock);
            break;
        }
        
        if(eDevState_PRESENT == g_DevState)
        {
            DevTypeBck = g_Dev_Type;
            if(eDevType_TAG == g_Dev_Type)
            {
                memcpy(&TagInfo, &g_TagInfo, sizeof(nfc_tag_info_t));
                framework_UnlockMutex(g_devLock);
                printf("        Type : ");
                switch (TagInfo.technology)
                {
                    case TARGET_TYPE_UNKNOWN:
                    {
                        printf("        'Type Unknown'\n");
                    } break;
                    case TARGET_TYPE_ISO14443_3A:
                    {
                        printf("        'Type A'\n");
                    } break;
                    case TARGET_TYPE_ISO14443_3B:
                    {
                        printf("        'Type 4B'\n");
                    } break;
                    case TARGET_TYPE_ISO14443_4:
                    {
                        printf("        'Type 4A'\n");
                    } break;
                    case TARGET_TYPE_FELICA:
                    {
                        printf("        'Type F'\n");
                    } break;
                    case TARGET_TYPE_ISO15693:
                    {
                        printf("        'Type V'\n");
                    } break;
                    case TARGET_TYPE_NDEF:
                    {
                        printf("        'Type NDEF'\n");
                    } break;
                    case TARGET_TYPE_NDEF_FORMATABLE:
                    {
                        printf("        'Type Formatable'\n");
                    } break;
                    case TARGET_TYPE_MIFARE_CLASSIC:
                    {
                        printf("        'Type A - Mifare Classic'\n");
                    } break;
                    case TARGET_TYPE_MIFARE_UL:
                    {
                        printf("        'Type A - Mifare Ul'\n");
                    } break;
                    case TARGET_TYPE_KOVIO_BARCODE:
                    {
                        printf("        'Type A - Kovio Barcode'\n");
                    } break;
                    case TARGET_TYPE_ISO14443_3A_3B:
                    {
                        printf("        'Type A/B'\n");
                    } break;
                    default:
                    {
                        printf("        'Type %d (Unknown or not supported)'\n", TagInfo.technology);
                    } break;
                }
                /*32 is max UID len (Kovio tags)*/
                if((0x00 != TagInfo.uid_length) && (32 >= TagInfo.uid_length))
                {
                    if(4 == TagInfo.uid_length || 7 == TagInfo.uid_length || 10 == TagInfo.uid_length)
                    {
                        printf("        NFCID1 :    \t'");
                    }
                    else if(8 == TagInfo.uid_length)
                    {
                        printf("        NFCID2 :    \t'");
                    }
                    else
                    {
                        printf("        UID :       \t'");
                    }
                    
                    for(i = 0x00; i < TagInfo.uid_length; i++)
                    {
                        printf("%02X ", (unsigned char) TagInfo.uid[i]);
                    }
                    printf("'\n");
                }
                res = nfcTag_isNdef(TagInfo.handle, &NDEFinfo);
                if(0x01 == res)
                {
                    PrintfNDEFInfo(NDEFinfo);
                    PrintNDEFContent(&TagInfo, &NDEFinfo, NULL, 0x00);
                }
                else
                {
                    printf("\t\tNDEF Content : NO, mode=%d, tech=%d\n", mode, TagInfo.technology);

                    if(0x03 == mode)
                    {
                         printf("\n\tFormating tag to NDEF prior to write ...\n");
                        if(nfcTag_isFormatable(TagInfo.handle))
                        {
                            if(nfcTag_formatTag(TagInfo.handle) == 0x00)
                            {
                                  printf("\tTag formating succeed\n");
                            }
                            else
                            {
                                  printf("\tTag formating failed\n");
                            }
                        }
                        else
                        {
                              printf("\tTag is not formatable\n");
                        }
                    }
                    else if(TARGET_TYPE_MIFARE_CLASSIC == TagInfo.technology)
                    {
                        memset(MifareAuthResp, 0x00, 255);
                        memset(MifareResp, 0x00, 255);
                        res = nfcTag_transceive(TagInfo.handle, MifareAuthCmd, 12, MifareAuthResp, 255, 500);
                        if(0x00 == res)
                        {
                            printf("\n\t\tRAW Tag transceive failed\n");
                        }
                        else
                        {
                            printf("\n\t\tMifare Authenticate command sent\n\t\tResponse : \n\t\t");
                            for(i = 0x00; i < (unsigned int) res; i++)
                            {
                                printf("%02X ", MifareAuthResp[i]);
                            }
                            printf("\n");

                            res = nfcTag_transceive(TagInfo.handle, MifareReadCmd, 2, MifareResp, 255, 500);
                            if(0x00 == res)
                            {
                                printf("\n\t\tRAW Tag transceive failed\n");
                            }
                            else
                            {
                                printf("\n\t\tMifare Read command sent\n\t\tResponse : \n\t\t");
                                for(i = 0x00; i < (unsigned int)res; i++)
                                {
                                    printf("%02X ", MifareResp[i]);
                                }
                                printf("\n\n");

                                res = nfcTag_transceive(TagInfo.handle, MifareWriteCmd, sizeof(MifareWriteCmd), MifareResp, 255, 500);
                                if(0x00 == res)
                                {
                                    printf("\n\t\tRAW Tag transceive failed\n");
                                }
                                else
                                {
                                    printf("\n\t\tMifare Write command sent\n\t\tResponse : \n\t\t");
                                    for(i = 0x00; i < (unsigned int)res; i++)
                                    {
                                        printf("%02X ", MifareResp[i]);
                                    }
                                    printf("\n\n");
                                }
                            }
                        }
                    }
                    else if(TARGET_TYPE_MIFARE_UL == TagInfo.technology)
                    {
                        printf("\n\tMIFARE UL card\n");
                        printf("\t\tMifare Read command: ");
                        for(i = 0x00; i < (unsigned int) sizeof(MifareReadCmd) ; i++)
                        {
                            printf("%02X ", MifareReadCmd[i]);
                        }
                        printf("\n");
                        res = nfcTag_transceive(TagInfo.handle, MifareReadCmd, sizeof(MifareReadCmd), MifareResp, 16, 500);
                        if(0x00 == res)
                        {
                            printf("\n\t\tRAW Tag transceive failed\n");
                        }
                        else
                        {
                            printf("\n\t\tMifare Read command sent\n\t\tResponse : \n\t\t");
                            for(i = 0x00; i < (unsigned int)res; i++)
                            {
                                printf("%02X ", MifareResp[i]);
                            }
                            printf("\n\n");
                        }
                    }
                    else
                    {
                        printf("\n\tNot a MIFARE card\n");
                    }
                }
                if(0x03 == mode)
                {
                    res = WriteTag(TagInfo, msgToSend, len);
                    if(0x00 == res)
                    {
                        printf("\tWrite Tag OK\n\tRead back data\n");
                        res = nfcTag_isNdef(TagInfo.handle, &NDEFinfo);
                        if(0x01 == res)
                        {
                            PrintfNDEFInfo(NDEFinfo);
                            PrintNDEFContent(&TagInfo, &NDEFinfo, NULL, 0x00);
                        }
                    }
                    else
                    {
                        printf("\tWrite Tag Failed\n");
                    }
                }
                num_tags = getNumTags();
                if(num_tags > 1)
                {
                    tag_count++;
                    if (tag_count < num_tags)
                    {
                        printf("\tMultiple tags found, selecting next tag...\n");
                        selectNextTag();
                    }
                    else
                    {
                        tag_count = 0;
                    }
                }
                 framework_LockMutex(g_devLock);
            }
            else if(eDevType_P2P == g_Dev_Type)/*P2P Detected*/
            {
                framework_UnlockMutex(g_devLock);
                 printf("\tDevice Found\n");
                
                if(2 == mode)
                {
                    SnepPush(msgToSend, len);
                }
                
                framework_LockMutex(g_SnepClientLock);
    
                if(eSnepClientState_READY == g_SnepClientState)
                {
                    g_SnepClientState = eSnepClientState_WAIT_OFF;
                    framework_WaitMutex(g_SnepClientLock, 0);
                }
                
                framework_UnlockMutex(g_SnepClientLock);
                framework_LockMutex(g_devLock);
        
            }
            else if(eDevType_READER == g_Dev_Type)
            {                
                framework_LockMutex(g_HCELock);
                do
                {
                    framework_UnlockMutex(g_devLock);
                
                    if(eHCEState_NONE == g_HCEState)
                    {
                        g_HCEState = eHCEState_WAIT_DATA;
                        framework_WaitMutex(g_HCELock, 0x00);
                    }
                    
                    if(eHCEState_DATA_RECEIVED == g_HCEState)
                    {
                        g_HCEState = eHCEState_NONE;
                        
                        if(HCE_data != NULL)
                        {
                            printf("\t\tReceived data from remote device : \n\t\t");
                            
                            for(i = 0x00; i < HCE_dataLenght; i++)
                            {
                                printf("%02X ", HCE_data[i]);
                            }
                            
                            /*Call HCE response builder*/
                            T4T_NDEF_EMU_Next(HCE_data, HCE_dataLenght, HCEReponse, &HCEResponseLen);
                            free(HCE_data);
                            HCE_dataLenght = 0x00;
                            HCE_data = NULL;
                        }
                        framework_UnlockMutex(g_HCELock);
                        res = nfcHce_sendCommand(HCEReponse, HCEResponseLen);
                        framework_LockMutex(g_HCELock);
                        if(0x00 == res)
                        {
                            printf("\n\n\t\tResponse sent : \n\t\t");
                            for(i = 0x00; i < HCEResponseLen; i++)
                            {
                                printf("%02X ", HCEReponse[i]);
                            }
                            printf("\n\n");
                        }
                        else
                        {
                            printf("\n\n\t\tFailed to send response\n\n");
                        }
                    }
                    framework_LockMutex(g_devLock);
                }while(eDevState_PRESENT == g_DevState);
                framework_UnlockMutex(g_HCELock);
            }
            else
            {
                framework_UnlockMutex(g_devLock);
                break;
            }
            
            if(eDevState_PRESENT == g_DevState)
            {
                g_DevState = eDevState_WAIT_DEPARTURE;
                framework_WaitMutex(g_devLock, 0);
                if(eDevType_P2P == DevTypeBck)
                {
                    printf("\tDevice Lost\n\n");
                }
                DevTypeBck = eDevType_NONE;
            }
            else if(eDevType_P2P == DevTypeBck)
            {
                printf("\tDevice Lost\n\n");
            }
        }
        
        framework_UnlockMutex(g_devLock);
    }while(0x01);
    
    return res;
}

void strtolower(char * string) 
{
    unsigned int i = 0x00;
    
    for(i = 0; i < strlen(string); i++) 
    {
        string[i] = tolower(string[i]);
    }
}

char* strRemovceChar(const char* str, char car)
{
    unsigned int i = 0x00;
    unsigned int index = 0x00;
    char * dest = (char*)malloc((strlen(str) + 1) * sizeof(char));
    
    for(i = 0x00; i < strlen(str); i++)
    {
        if(str[i] != car)
        {
            dest[index++] = str[i];
        }
    }
    dest[index] = '\0';
    return dest;
}

int convertParamtoBuffer(char* param, unsigned char** outBuffer, unsigned int* outBufferLen)
{
    int res = 0x00;
    unsigned int i = 0x00;
    int index = 0x00;
    char atoiBuf[3];
    atoiBuf[2] = '\0';
    
    if(NULL == param || NULL == outBuffer || NULL == outBufferLen)    
    {
        printf("Parameter Error\n");
        res = 0xFF;
    }
    
    if(0x00 == res)
    {
        param = strRemovceChar(param, ' ');
    }
    
    if(0x00 == res)
    {
        if(0x00 == strlen(param) % 2)
        {
            *outBufferLen = strlen(param) / 2;
            
            *outBuffer = (unsigned char*) malloc((*outBufferLen) * sizeof(unsigned char));
            if(NULL != *outBuffer)
            {
                for(i = 0x00; i < ((*outBufferLen) * 2); i = i + 2)
                {
                    atoiBuf[0] = param[i];
                    atoiBuf[1] = param[i + 1];
                    (*outBuffer)[index++] = strtol(atoiBuf, NULL, 16);
                }
            }
            else
            {
                printf("Memory Allocation Failed\n");
                res = 0xFF;
            }
        }
        else
        {
            printf("Invalid NDEF Message Param\n");
        }
        free(param);
    }
        
    return res;
}

int BuildNDEFMessage(int arg_len, char** arg, unsigned char** outNDEFBuffer, unsigned int* outNDEFBufferLen)
{
    int res = 0x00;
    nfc_handover_cps_t cps = HANDOVER_CPS_UNKNOWN;
    unsigned char* ndef_msg = NULL;
    unsigned int ndef_msg_len = 0x00;
    char *type = NULL;
    char *uri = NULL;
    char *text = NULL;
    char* lang = NULL;
    char* mime_type = NULL;
    char* mime_data = NULL;
    char* carrier_state = NULL;
    char* carrier_name = NULL;
    char* carrier_data = NULL;
    
    if(0xFF == LookForTag(arg, arg_len, "-t", &type, 0x00) && 0xFF == LookForTag(arg, arg_len, "--type", &type, 0x01))
    {
        res = 0xFF;
        printf("Record type missing (-t)\n");
        help(0x02);
    }
    if(0x00 == res)
    {
        strtolower(type);
        if(0x00 == strcmp(type, "uri"))
        {
            if(0x00 == LookForTag(arg, arg_len, "-u", &uri, 0x00) || 0x00 == LookForTag(arg, arg_len, "--uri", &uri, 0x01))
            {
                *outNDEFBufferLen = strlen(uri) + 30; /*TODO : replace 30 by URI NDEF message header*/
                *outNDEFBuffer = (unsigned char*) malloc(*outNDEFBufferLen * sizeof(unsigned char));
                res = ndef_createUri(uri, *outNDEFBuffer, *outNDEFBufferLen);
                if(0x00 >= res)
                {
                    printf("Failed to build URI NDEF Message\n");
                }
                else
                {
                    *outNDEFBufferLen = res;
                    res = 0x00;
                }
            }
            else
            {
                printf("URI Missing (-u)\n");
                help(0x02);
                res = 0xFF;
            }
        }
        else if(0x00 == strcmp(type, "text"))
        {
            if(0xFF == LookForTag(arg, arg_len, "-r", &text, 0x00) && 0xFF == LookForTag(arg, arg_len, "--rep", &text, 0x01))
            {
                printf("Representation missing (-r)\n");
                res = 0xFF;
            }
            
            if(0xFF == LookForTag(arg, arg_len, "-l", &lang, 0x00) && 0xFF == LookForTag(arg, arg_len, "--lang", &lang, 0x01))
            {
                printf("Language missing (-l)\n");
                res = 0xFF;
            }
            if(0x00 == res)
            {
                *outNDEFBufferLen = strlen(text) + strlen(lang) + 30; /*TODO : replace 30 by TEXT NDEF message header*/
                *outNDEFBuffer = (unsigned char*) malloc(*outNDEFBufferLen * sizeof(unsigned char));
                res = ndef_createText(lang, text, *outNDEFBuffer, *outNDEFBufferLen);
                if(0x00 >= res)
                {
                    printf("Failed to build TEXT NDEF Message\n");
                }
                else
                {
                    *outNDEFBufferLen = res;
                    res = 0x00;
                }    
            }
            else
            {
                help(0x02);
            }
        }
        else if(0x00 == strcmp(type, "mime"))
        {
            if(0xFF == LookForTag(arg, arg_len, "-m", &mime_type, 0x00) && 0xFF == LookForTag(arg, arg_len, "--mime", &mime_type, 0x01))
            {
                printf("Mime-type missing (-m)\n");
                res = 0xFF;
            }
            if(0xFF == LookForTag(arg, arg_len, "-d", &mime_data, 0x00) && 0xFF == LookForTag(arg, arg_len, "--data", &mime_data, 0x01))
            {
                printf("NDEF Data missing (-d)\n");
                res = 0xFF;
            }
            if(0x00 == res)
            {
                *outNDEFBufferLen = strlen(mime_data) +  strlen(mime_type) + 30; /*TODO : replace 30 by MIME NDEF message header*/
                *outNDEFBuffer = (unsigned char*) malloc(*outNDEFBufferLen * sizeof(unsigned char));
                
                res = convertParamtoBuffer(mime_data, &ndef_msg, &ndef_msg_len);
                
                if(0x00 == res)
                {
                    res = ndef_createMime(mime_type, ndef_msg, ndef_msg_len, *outNDEFBuffer, *outNDEFBufferLen);
                    if(0x00 >= res)
                    {
                        printf("Failed to build MIME NDEF Message\n");
                    }
                    else
                    {
                        *outNDEFBufferLen = res;
                        res = 0x00;
                    }
                }
            }
            else
            {
                help(0x02);
            }
        }
        else if(0x00 == strcmp(type, "hs"))
        {
            
            if(0xFF == LookForTag(arg, arg_len, "-cs", &carrier_state, 0x00) && 0xFF == LookForTag(arg, arg_len, "--carrierState", &carrier_state, 0x01))
            {
                printf("Carrier Power State missing (-cs)\n");
                res = 0xFF;
            }
            if(0xFF == LookForTag(arg, arg_len, "-cn", &carrier_name, 0x00) && 0xFF == LookForTag(arg, arg_len, "--carrierName", &carrier_name, 0x01))
            {
                printf("Carrier Reference Name missing (-cn)\n");
                res = 0xFF;
            }
            
            if(0xFF == LookForTag(arg, arg_len, "-d", &carrier_data, 0x00) && 0xFF == LookForTag(arg, arg_len, "--data", &carrier_data, 0x01))
            {
                printf("NDEF Data missing (-d)\n");
                res = 0xFF;
            }
            
            if(0x00 == res)
            {
                *outNDEFBufferLen = strlen(carrier_name) + strlen(carrier_data) + 30;  /*TODO : replace 30 by HS NDEF message header*/
                *outNDEFBuffer = (unsigned char*) malloc(*outNDEFBufferLen * sizeof(unsigned char));
                
                strtolower(carrier_state);
                
                if(0x00 == strcmp(carrier_state, "inactive"))
                {
                    cps = HANDOVER_CPS_INACTIVE;
                }
                else if(0x00 == strcmp(carrier_state, "active"))
                {
                    cps = HANDOVER_CPS_ACTIVE;
                }
                else if(0x00 == strcmp(carrier_state, "activating"))
                {
                    cps = HANDOVER_CPS_ACTIVATING;
                }
                else
                {
                    printf("Error : unknown carrier power state %s\n", carrier_state);
                    res = 0xFF;
                }
                
                if(0x00 == res)
                {
                    res = convertParamtoBuffer(carrier_data, &ndef_msg, &ndef_msg_len);
                }
                
                if(0x00 == res)
                {
                    res = ndef_createHandoverSelect(cps, carrier_name, ndef_msg, ndef_msg_len, *outNDEFBuffer, *outNDEFBufferLen);
                    if(0x00 >= res)
                    {
                        printf("Failed to build handover select message\n");
                    }
                    else
                    {
                        *outNDEFBufferLen = res;
                        res = 0x00;
                    }
                }
            }
            else
            {
                help(0x02);
            }
        }
        else
        {
            printf("NDEF Type %s not supported\n", type);
            res = 0xFF;
        }
    }
    
    if(NULL != ndef_msg)
    {
        free(ndef_msg);
        ndef_msg = NULL;
        ndef_msg_len = 0x00;
    }
    
    return res;
}

/* if data = NULL this tag is not followed by dataStr : for example -h --help
if format = 0 tag format -t "text" if format=1 tag format : --type=text */
int LookForTag(char** args, int args_len, char* tag, char** data, int format)
{
    int res = 0xFF;
    int i = 0x00;
    int found = 0xFF;
    
    for(i = 0x00; i < args_len; i++)
    {
        found = 0xFF;
        strtolower(args[i]);
        if(0x00 == format)
        {
            found = strcmp(args[i], tag);
        }
        else
        {
            found = strncmp(args[i], tag, strlen(tag));
        }
        
        if(0x00 == found)
        {
            if(NULL != data)
            {
                if(0x00 == format)
                {
                    if(i < (args_len - 1))
                    {
                        *data = args[i + 1];
                        res = 0x00;
                        break;
                    }
                    else
                    {
                        printf("Argument missing after %s\n", tag);
                    }
                }
                else
                {
                    *data = &args[i][strlen(tag) + 1]; /* +1 to remove '='*/
                    res = 0x00;
                    break;
                }
            }
            else
            {
                res = 0x00;
                break;
            }
        }
    }
    
    return res;
}
 
void cmd_poll(int arg_len, char** arg)
{
    int res = 0x00;
    
    printf("#########################################################################################\n");
    printf("##                                       NFC demo                                      ##\n");
    printf("#########################################################################################\n");
    printf("##                                 Poll mode activated                                 ##\n");
    printf("#########################################################################################\n");
    
    InitEnv();
    if(0x00 == LookForTag(arg, arg_len, "-h", NULL, 0x00) || 0x00 == LookForTag(arg, arg_len, "--help", NULL, 0x01))
    {
        help(0x01);
    }
    else
    {
        res = InitMode(0x01, 0x01, 0x00);
        
        if(0x00 == res)
        {
            WaitDeviceArrival(0x01, NULL , 0x00);
        }
    
        DeinitPollMode();
    }
    
    printf("Leaving ...\n");
}
 
void cmd_push(int arg_len, char** arg)
{
    int res = 0x00;
    unsigned char * NDEFMsg = NULL;
    unsigned int NDEFMsgLen = 0x00;
    
    printf("#########################################################################################\n");
    printf("##                                       NFC demo                                      ##\n");
    printf("#########################################################################################\n");
    printf("##                                 Push mode activated                                 ##\n");
    printf("#########################################################################################\n");
    
    InitEnv();
    
    if(0x00 == LookForTag(arg, arg_len, "-h", NULL, 0x00) || 0x00 == LookForTag(arg, arg_len, "--help", NULL, 0x01))
    {
        help(0x02);
    }
    else
    {
        res = InitMode(0x01, 0x01, 0x00);
        
        if(0x00 == res)
        {
            res = BuildNDEFMessage(arg_len, arg, &NDEFMsg, &NDEFMsgLen);
        }
        
        if(0x00 == res)
        {
            WaitDeviceArrival(0x02, NDEFMsg, NDEFMsgLen);
        }
        
        if(NULL != NDEFMsg)
        {
            free(NDEFMsg);
            NDEFMsg = NULL;
            NDEFMsgLen = 0x00;
        }
        
        DeinitPollMode();
    }
    
    printf("Leaving ...\n");
}

void cmd_share(int arg_len, char** arg)
{
    int res = 0x00;
    unsigned char * NDEFMsg = NULL;
    unsigned int NDEFMsgLen = 0x00;
    
    printf("#########################################################################################\n");
    printf("##                                       NFC demo                                      ##\n");
    printf("#########################################################################################\n");
    printf("##                                 Share mode activated                                ##\n");
    printf("#########################################################################################\n");
    
    InitEnv();
    
    if(0x00 == LookForTag(arg, arg_len, "-h", NULL, 0x00) || 0x00 == LookForTag(arg, arg_len, "--help", NULL, 0x01))
    {
        help(0x02);
    }
    else
    {
        res = InitMode(0x00, 0x00, 0x01);
        
        if(0x00 == res)
        {
            res = BuildNDEFMessage(arg_len, arg, &NDEFMsg, &NDEFMsgLen);
        }
        
        if(0x00 == res)
        {
            T4T_NDEF_EMU_SetRecord(NDEFMsg, NDEFMsgLen, NULL);
        }
        
        if(0x00 == res)
        {
            WaitDeviceArrival(0x04, NDEFMsg, NDEFMsgLen);
        }
        
        if(NULL != NDEFMsg)
        {
            free(NDEFMsg);
            NDEFMsg = NULL;
            NDEFMsgLen = 0x00;
        }
        
        DeinitPollMode();
    }
    
    printf("Leaving ...\n");
}
void cmd_write(int arg_len, char** arg)
{
    int res = 0x00;
    unsigned char * NDEFMsg = NULL;
    unsigned int NDEFMsgLen = 0x00;
    
    printf("#########################################################################################\n");
    printf("##                                       NFC demo                                      ##\n");
    printf("#########################################################################################\n");
    printf("##                                 Write mode activated                                ##\n");
    printf("#########################################################################################\n");
    
    InitEnv();
    
    if(0x00 == LookForTag(arg, arg_len, "-h", NULL, 0x00) || 0x00 == LookForTag(arg, arg_len, "--help", NULL, 0x01))
    {
        help(0x02);
    }
    else
    {
        res = InitMode(0x01, 0x01, 0x00);
        
        if(0x00 == res)
        {
            res = BuildNDEFMessage(arg_len, arg, &NDEFMsg, &NDEFMsgLen);
        }
        
        if(0x00 == res)
        {
            WaitDeviceArrival(0x03, NDEFMsg, NDEFMsgLen);
        }
        
        if(NULL != NDEFMsg)
        {
            free(NDEFMsg);
            NDEFMsg = NULL;
            NDEFMsgLen = 0x00;
        }
        
        DeinitPollMode();
    }
    
    printf("Leaving ...\n");
}

void* ExitThread(void* pContext)
{
    printf("                              ... press enter to quit ...\n\n");
    
    getchar();
    
    framework_LockMutex(g_SnepClientLock);
    
    if(eSnepClientState_WAIT_OFF == g_SnepClientState || eSnepClientState_WAIT_READY == g_SnepClientState)
    {
        g_SnepClientState = eSnepClientState_EXIT;
        framework_NotifyMutex(g_SnepClientLock, 0);
    }
    else
    {
        g_SnepClientState = eSnepClientState_EXIT;
    }
    framework_UnlockMutex(g_SnepClientLock);
    
    framework_LockMutex(g_devLock);
    
    if(eDevState_WAIT_ARRIVAL == g_DevState || eDevState_WAIT_DEPARTURE == g_DevState)
    {
        g_DevState = eDevState_EXIT;
        framework_NotifyMutex(g_devLock, 0);
    }
    else
    {
        g_DevState = eDevState_EXIT;
    }
    
    framework_UnlockMutex(g_devLock);
    return NULL;
}

int InitEnv()
{
    eResult tool_res = FRAMEWORK_SUCCESS;
    int res = 0x00;
    
    tool_res = framework_CreateMutex(&g_devLock);
    if(FRAMEWORK_SUCCESS != tool_res)
    {
        res = 0xFF;
    }
    
    if(0x00 == res)
    {
        tool_res = framework_CreateMutex(&g_SnepClientLock);
        if(FRAMEWORK_SUCCESS != tool_res)
        {
            res = 0xFF;
        }
     }
     
     if(0x00 == res)
    {
        tool_res = framework_CreateMutex(&g_HCELock);
        if(FRAMEWORK_SUCCESS != tool_res)
        {
            res = 0xFF;
        }
     }
     if(0x00 == res)
    {
        tool_res = framework_CreateThread(&g_ThreadHandle, ExitThread, NULL);
        if(FRAMEWORK_SUCCESS != tool_res)
        {
            res = 0xFF;
        }
    }
    return res;
}

int CleanEnv()
{
    if(NULL != g_ThreadHandle)
    {
        framework_JoinThread(g_ThreadHandle);
        framework_DeleteThread(g_ThreadHandle);
        g_ThreadHandle = NULL;
    }
        
    if(NULL != g_devLock)
    {
        framework_DeleteMutex(g_devLock);
        g_devLock = NULL;
    }
    
    if(NULL != g_SnepClientLock)
    {
        framework_DeleteMutex(g_SnepClientLock);
        g_SnepClientLock = NULL;
    }
    if(NULL != g_HCELock)
    {
        framework_DeleteMutex(g_HCELock);
        g_HCELock = NULL;
    }
    return 0x00;
}
 
int main(int argc, char ** argv)
{
    if (argc<2)
    {
        printf("Missing argument\n");
        help(0x00);
    }
    else if (strcmp(argv[1],"poll") == 0)
    {
        cmd_poll(argc - 2, &argv[2]);
    }
    else if(strcmp(argv[1],"write") == 0)
    {
        cmd_write(argc - 2, &argv[2]);
    }
    else if(strcmp(argv[1],"push") == 0)
    {
        cmd_push(argc - 2, &argv[2]);
    }
    else if(strcmp(argv[1],"share") == 0)
    {
        cmd_share(argc - 2, &argv[2]);
    }
    else
    {
        help(0x00);
    }
    printf("\n");
    
    CleanEnv();
    
    return 0;
}
 
void help(int mode)
{
    printf("\nCOMMAND: \n");
    printf("\tpoll\tPolling mode \t e.g. <nfcDemoApp poll >\n");
    printf("\twrite\tWrite tag \t e.g. <nfcDemoApp write --type=Text -l en -r \"Test\">\n");
    printf("\tshare\tTag Emulation Mode \t e.g. <nfcDemoApp share -t URI -u http://www.nxp.com>\n");
    printf("\tpush\tPush to device \t e.g. <nfcDemoApp push -t URI -u http://www.nxp.com>\n");
    printf("\t    \t               \t e.g. <nfcDemoApp push --type=mime -m \"application/vnd.bluetooth.ep.oob\" -d \"2200AC597405AF1C0E0947616C617879204E6F74652033040D0C024005031E110B11\">\n");
    printf("\n");
    printf("Help Options:\n");
    printf("-h, --help                       Show help options\n");
    printf("\n");
    
    if(0x01 == mode)
    {
        printf("No Application Options for poll mode\n");
    }
    if(0x02 == mode)
    {
        printf("Supported NDEF Types : \n");
        printf("\t URI\n");
        printf("\t TEXT\n");
        printf("\t MIME\n");
        printf("\t HS (handover select)\n");
        printf("\n");
        
        printf("Application Options : \n");
        printf("\t-l,  --lang=en                   Language\n");
        printf("\t-m,  --mime=audio/mp3            Mime-type\n");
        printf("\t-r,  --rep=sample text           Representation\n");
        printf("\t-t,  --type=Text                 Record type (Text, URI...\n");
        printf("\t-u,  --uri=http://www.nxp.com    URI\n");
        printf("\t-d,  --data=00223344565667788    NDEF Data (for type MIME & Handover select)\n");
        printf("\t-cs, --carrierState=active       Carrier State (for type Handover select)\n");
        printf("\t-cn, --carrierName=ble           Carrier Reference Name (for type Handover select)\n");
    }
    printf("\n");
}
