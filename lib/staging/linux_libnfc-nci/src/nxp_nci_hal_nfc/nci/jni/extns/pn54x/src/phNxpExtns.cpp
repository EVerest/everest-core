/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/******************************************************************************
 *
 *  The original Work has been changed by NXP Semiconductors.
 *
 *  Copyright (C) 2020 NXP Semiconductors
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
#ifdef ESE_NFC_SYNCHRONIZATION
#include <linux/ese-nfc-sync.h>
#endif
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <phNxpExtns_MifareStd.h>

using android::base::StringPrintf;

extern phNxpExtns_Context_t gphNxpExtns_Context;
extern phFriNfc_NdefMap_t* NdefMap;
extern phNci_mfc_auth_cmd_t gAuthCmdBuf;

static NFCSTATUS phNxpExtns_ProcessSysMessage(phLibNfc_Message_t* msg);
static NFCSTATUS phNxpExtns_SendMsg(phLibNfc_Message_t* sysmsg);

#ifdef ESE_NFC_SYNCHRONIZATION
/* timing calculation structure*/
typedef struct time_cal {
  struct timeval tv1;
  struct timeval tv2;
} TimeCal;
static int fd_ese_nfc_sync; /*file descriptor to hold sync driver handle*/
#endif

/*******************************************************************************
**
** Function         EXTNS_Init
**
** Description      This function Initializes Mifare Classic Extns. Allocates
**                  required memory and initializes the Mifare Classic Vars
**
** Returns          NFCSTATUS_SUCCESS if successfully initialized
**                  NFCSTATUS_FAILED otherwise
**
*******************************************************************************/
NFCSTATUS EXTNS_Init(tNFA_DM_CBACK* p_nfa_dm_cback,
                     tNFA_CONN_CBACK* p_nfa_conn_cback) {
  NFCSTATUS status = NFCSTATUS_FAILED;

  /* Validate parameters */
  if ((!p_nfa_dm_cback) || (!p_nfa_conn_cback)) {
    LOG(ERROR) << StringPrintf("EXTNS_Init(): error null callback");
    goto clean_and_return;
  }

  gphNxpExtns_Context.p_dm_cback = p_nfa_dm_cback;
  gphNxpExtns_Context.p_conn_cback = p_nfa_conn_cback;

  if (NFCSTATUS_SUCCESS != phNxpExtns_MfcModuleInit()) {
    LOG(ERROR) << StringPrintf("ERROR: MFC Module Init Failed");
    goto clean_and_return;
  }
  gphNxpExtns_Context.Extns_status = EXTNS_STATUS_OPEN;

  status = NFCSTATUS_SUCCESS;
  return status;

clean_and_return:
  gphNxpExtns_Context.Extns_status = EXTNS_STATUS_CLOSE;
  return status;
}

/*******************************************************************************
**
** Function         EXTNS_Close
**
** Description      This function de-initializes Mifare Classic Extns.
**                  De-allocates memory
**
** Returns          None
**
*******************************************************************************/
void EXTNS_Close(void) {
  gphNxpExtns_Context.Extns_status = EXTNS_STATUS_CLOSE;
  phNxpExtns_MfcModuleDeInit();
  return;
}

/*******************************************************************************
**
** Function         EXTNS_GetStatus
**
** Description      This function Gets the Status of the Extns.
**
** Returns          None
**
*******************************************************************************/

phNxpExtns_Status EXTNS_GetStatus(void) {
  return gphNxpExtns_Context.Extns_status;
}

/*******************************************************************************
**
** Function         EXTNS_MfcCallBack
**
** Description      Decodes Mifare Classic Tag Response
**                  This is called from NFA_SendRaw Callback
**
** Returns:
**                  NFCSTATUS_SUCCESS if successfully initiated
**                  NFCSTATUS_FAILED otherwise
**
*******************************************************************************/
NFCSTATUS EXTNS_MfcCallBack(uint8_t* buf, uint32_t buflen) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  phLibNfc_Message_t msg;

  msg.eMsgType = PH_NXPEXTNS_RX_DATA;
  msg.pMsgData = buf;
  msg.Size = buflen;

  status = phNxpExtns_SendMsg(&msg);
  if (NFCSTATUS_SUCCESS != status) {
    LOG(ERROR) << StringPrintf("Error Sending msg to Extension Thread");
  }
  return status;
}

/*******************************************************************************
**
** Function         EXTNS_MfcCheckNDef
**
** Description      Performs NDEF detection for Mifare Classic Tag
**
**                  Upon successful completion of NDEF detection, a
**                  NFA_NDEF_DETECT_EVT will be sent, to notify the application
**                  of the NDEF attributes (NDEF total memory size, current
**                  size, etc.).
**
** Returns:
**                  NFCSTATUS_SUCCESS if successfully initiated
**                  NFCSTATUS_FAILED otherwise
**
*******************************************************************************/
NFCSTATUS EXTNS_MfcCheckNDef(void) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  phLibNfc_Message_t msg;

  msg.eMsgType = PH_NXPEXTNS_MIFARE_CHECK_NDEF;
  msg.pMsgData = NULL;
  msg.Size = 0;

  status = phNxpExtns_SendMsg(&msg);
  if (NFCSTATUS_SUCCESS != status) {
    LOG(ERROR) << StringPrintf("Error Sending msg to Extension Thread");
  }

  return status;
}

/*******************************************************************************
**
** Function         EXTNS_MfcReadNDef
**
** Description      Reads NDEF message from Mifare Classic Tag.
**
**                  Upon receiving the NDEF message, the message will be sent to
**                  the handler registered with
*EXTNS_MfcRegisterNDefTypeHandler.
**
** Returns:
**                  NFCSTATUS_SUCCESS if successfully initiated
**                  NFCSTATUS_FAILED otherwise
**
*******************************************************************************/
NFCSTATUS EXTNS_MfcReadNDef(void) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  phLibNfc_Message_t msg;

  msg.eMsgType = PH_NXPEXTNS_MIFARE_READ_NDEF;
  msg.pMsgData = NULL;
  msg.Size = 0;

  status = phNxpExtns_SendMsg(&msg);
  if (NFCSTATUS_SUCCESS != status) {
    LOG(ERROR) << StringPrintf("Error Sending msg to Extension Thread");
  }

  return status;
}
/*******************************************************************************
**
** Function         EXTNS_MfcPresenceCheck
**
** Description      Do the check presence for Mifare Classic Tag.
**
**
** Returns:
**                  NFCSTATUS_SUCCESS if successfully initiated
**                  NFCSTATUS_FAILED otherwise
**
*******************************************************************************/
NFCSTATUS EXTNS_MfcPresenceCheck(void) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  phLibNfc_Message_t msg;

  msg.eMsgType = PH_NXPEXTNS_MIFARE_PRESENCE_CHECK;
  msg.pMsgData = NULL;
  msg.Size = 0;

  gAuthCmdBuf.status = NFCSTATUS_FAILED;
  if (sem_init(&gAuthCmdBuf.semPresenceCheck, 0, 0) == -1) {
    LOG(ERROR) << StringPrintf("%s: semaphore creation failed (errno=%d)",
                               __func__, errno);
    return NFCSTATUS_FAILED;
  }

  status = phNxpExtns_SendMsg(&msg);
  if (NFCSTATUS_SUCCESS != status) {
    LOG(ERROR) << StringPrintf("Error Sending msg to Extension Thread");
    sem_destroy(&gAuthCmdBuf.semPresenceCheck);
  }

  return status;
}

/*******************************************************************************
**
** Function        EXTNS_MfcSetReadOnly
**
**
** Description:
**      Sets tag as read only.
**
**      When tag is set as read only, or if an error occurs, the app will be
**      notified with NFA_SET_TAG_RO_EVT.
**
** Returns:
**                  NFCSTATUS_SUCCESS if successfully initiated
**                  NFCSTATUS_FAILED otherwise
**
*******************************************************************************/
NFCSTATUS EXTNS_MfcSetReadOnly(uint8_t* key, uint8_t len) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  phLibNfc_Message_t msg;

  msg.eMsgType = PH_NXPEXTNS_MIFARE_READ_ONLY;
  msg.pMsgData = key;
  msg.Size = len;

  status = phNxpExtns_SendMsg(&msg);
  if (NFCSTATUS_SUCCESS != status) {
    LOG(ERROR) << StringPrintf("Error Sending msg to Extension Thread");
  }

  return status;
}

/*******************************************************************************
**
** Function         EXTNS_MfcWriteNDef
**
** Description      Writes NDEF data to Mifare Classic Tag.
**
**                  When the entire message has been written, or if an error
**                  occurs, the app will be notified with NFA_WRITE_CPLT_EVT.
**
**                  p_data needs to be persistent until NFA_WRITE_CPLT_EVT
**
** Returns:
**                  NFCSTATUS_SUCCESS if successfully initiated
**                  NFCSTATUS_FAILED otherwise
**
*******************************************************************************/
NFCSTATUS EXTNS_MfcWriteNDef(uint8_t* p_data, uint32_t len) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  phLibNfc_Message_t msg;

  msg.eMsgType = PH_NXPEXTNS_MIFARE_WRITE_NDEF;
  msg.pMsgData = p_data;
  msg.Size = len;

  status = phNxpExtns_SendMsg(&msg);
  if (NFCSTATUS_SUCCESS != status) {
    LOG(ERROR) << StringPrintf("Error Sending msg to Extension Thread");
  }

  return status;
}

/*****************************************************************************
**
** Function         EXTNS_MfcFormatTag
**
** Description      Formats Mifare Classic Tag.
**
**                  The NFA_RW_FORMAT_CPLT_EVT, status is used to
**                  indicate if tag is successfully formated or not
**
** Returns
**                  NFCSTATUS_SUCCESS if successfully initiated
**                  NFCSTATUS_FAILED otherwise
**
*****************************************************************************/
NFCSTATUS EXTNS_MfcFormatTag(uint8_t* key, uint8_t len) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  phLibNfc_Message_t msg;

  msg.eMsgType = PH_NXPEXTNS_MIFARE_FORMAT_NDEF;
  msg.pMsgData = key;
  msg.Size = len;

  status = phNxpExtns_SendMsg(&msg);
  if (NFCSTATUS_SUCCESS != status) {
    LOG(ERROR) << StringPrintf("Error Sending msg to Extension Thread");
  }

  return status;
}

/*****************************************************************************
**
** Function         EXTNS_MfcDisconnect
**
** Description      Disconnects Mifare Classic Tag.
**
** Returns
**                  NFCSTATUS_SUCCESS if successfully initiated
**                  NFCSTATUS_FAILED otherwise
**
*****************************************************************************/
NFCSTATUS EXTNS_MfcDisconnect(void) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  phLibNfc_Message_t msg;

  msg.eMsgType = PH_NXPEXTNS_DISCONNECT;
  msg.pMsgData = NULL;
  msg.Size = 0;

  status = phNxpExtns_SendMsg(&msg);
  if (NFCSTATUS_SUCCESS != status) {
    LOG(ERROR) << StringPrintf("Error Sending msg to Extension Thread");
  }

  return status;
}

/*****************************************************************************
**
** Function         EXTNS_MfcActivated
**
** Description      Activates Mifare Classic Tag.
**
** Returns
**                  NFCSTATUS_SUCCESS if successfully initiated
**                  NFCSTATUS_FAILED otherwise
**
*****************************************************************************/
NFCSTATUS EXTNS_MfcActivated(void) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  phLibNfc_Message_t msg;

  msg.eMsgType = PH_NXPEXTNS_ACTIVATED;
  msg.pMsgData = NULL;
  msg.Size = 0;

  status = phNxpExtns_SendMsg(&msg);
  if (NFCSTATUS_SUCCESS != status) {
    LOG(ERROR) << StringPrintf("Error Sending msg to Extension Thread");
  }

  return status;
}

/*******************************************************************************
**
** Function         EXTNS_MfcTransceive
**
** Description      Sends raw frame to Mifare Classic Tag.
**
** Returns          NFCSTATUS_SUCCESS if successfully initiated
**                  NFCSTATUS_FAILED otherwise
**
*******************************************************************************/
NFCSTATUS EXTNS_MfcTransceive(uint8_t* p_data, uint32_t len) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  phLibNfc_Message_t msg;

  msg.eMsgType = PH_NXPEXTNS_MIFARE_TRANSCEIVE;
  msg.pMsgData = p_data;
  msg.Size = len;

  status = phNxpExtns_SendMsg(&msg);
  if (NFCSTATUS_SUCCESS != status) {
    LOG(ERROR) << StringPrintf("Error Sending msg to Extension Thread");
  }

  return status;
}

/*******************************************************************************
**
** Function         EXTNS_MfcInit
**
** Description      This function is used to Init Mifare Classic Extns.
**                  This function should be called when the tag detected is
**                  Mifare Classic.
**
** Returns          NFCSTATUS_SUCCESS
**
*******************************************************************************/
NFCSTATUS EXTNS_MfcInit(tNFA_ACTIVATED& activationData) {
  tNFC_ACTIVATE_DEVT rfDetail = activationData.activate_ntf;

  NdefMap->psRemoteDevInfo->RemoteDevInfo.Iso14443A_Info.Sak =
      rfDetail.rf_tech_param.param.pa.sel_rsp;
  NdefMap->psRemoteDevInfo->RemoteDevInfo.Iso14443A_Info.AtqA[0] =
      rfDetail.rf_tech_param.param.pa.sens_res[0];
  NdefMap->psRemoteDevInfo->RemoteDevInfo.Iso14443A_Info.AtqA[1] =
      rfDetail.rf_tech_param.param.pa.sens_res[1];

  return NFCSTATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         phNxpExtns_ProcessSysMessage
**
** Description      Internal function to route the request from JNI and Callback
**                  from NFA_SendRawFrame to right function
**
** Returns          NFCSTATUS_SUCCESS if valid request
**                  NFCSTATUS_FAILED otherwise
**
*******************************************************************************/
static NFCSTATUS phNxpExtns_ProcessSysMessage(phLibNfc_Message_t* msg) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  if (gphNxpExtns_Context.Extns_status == EXTNS_STATUS_CLOSE) {
    return NFCSTATUS_FAILED;
  }

  switch (msg->eMsgType) {
    case PH_NXPEXTNS_RX_DATA:
      status = Mfc_RecvPacket((uint8_t*)msg->pMsgData, msg->Size);
      break;

    case PH_NXPEXTNS_MIFARE_CHECK_NDEF:
      pthread_mutex_init(&gAuthCmdBuf.syncmutex, NULL);
      pthread_mutex_lock(&gAuthCmdBuf.syncmutex);
      status = Mfc_CheckNdef();
      pthread_mutex_unlock(&gAuthCmdBuf.syncmutex);
      pthread_mutex_destroy(&gAuthCmdBuf.syncmutex);
      break;

    case PH_NXPEXTNS_MIFARE_READ_NDEF:
      status = Mfc_ReadNdef();
      break;

    case PH_NXPEXTNS_MIFARE_WRITE_NDEF:
      status = Mfc_WriteNdef((uint8_t*)msg->pMsgData, msg->Size);
      break;

    case PH_NXPEXTNS_MIFARE_FORMAT_NDEF:
      status = Mfc_FormatNdef((uint8_t*)msg->pMsgData, msg->Size);
      break;

    case PH_NXPEXTNS_DISCONNECT:
      Mfc_DeactivateCbackSelect();
      break;

    case PH_NXPEXTNS_ACTIVATED:
      Mfc_ActivateCback();
      break;

    case PH_NXPEXTNS_MIFARE_TRANSCEIVE:
      status = Mfc_Transceive((uint8_t*)msg->pMsgData, msg->Size);
      break;

    case PH_NXPEXTNS_MIFARE_READ_ONLY:
      status = Mfc_SetReadOnly((uint8_t*)msg->pMsgData, msg->Size);
      break;
    case PH_NXPEXTNS_MIFARE_PRESENCE_CHECK:
      pthread_mutex_init(&gAuthCmdBuf.syncmutex, NULL);
      pthread_mutex_lock(&gAuthCmdBuf.syncmutex);
      status = Mfc_PresenceCheck();
      pthread_mutex_unlock(&gAuthCmdBuf.syncmutex);
      pthread_mutex_destroy(&gAuthCmdBuf.syncmutex);
      break;
    default:
      status = NFCSTATUS_FAILED;
      LOG(ERROR) << StringPrintf("Illegal Command for Extension");
      break;
  }

  return status;
}

/*******************************************************************************
**
** Function         phNxpExtns_SendMsg
**
** Description      unlocks phNxpExtns_ProcessSysMessage with a valid message
**
** Returns          NFCSTATUS_SUCCESS if successfully initiated
**                  NFCSTATUS_FAILED otherwise
**
*******************************************************************************/
static NFCSTATUS phNxpExtns_SendMsg(phLibNfc_Message_t* sysmsg) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  status = phNxpExtns_ProcessSysMessage(sysmsg);

  return status;
}

/*******************************************************************************
**
** Function         EXTNS_MfcRegisterNDefTypeHandler
**
** Description      This function allows the applications to register for
**                  specific types of NDEF records.
**
**                  For records types which were not registered, the record will
**                  be sent to the default handler.
**
** Returns          NFCSTATUS_SUCCESS
**
*******************************************************************************/
NFCSTATUS
EXTNS_MfcRegisterNDefTypeHandler(tNFA_NDEF_CBACK* ndefHandlerCallback) {
  NFCSTATUS status = NFCSTATUS_FAILED;
  if (NULL != ndefHandlerCallback) {
    gphNxpExtns_Context.p_ndef_cback = ndefHandlerCallback;
    status = NFCSTATUS_SUCCESS;
  }

  return status;
}

/*******************************************************************************
**                     Synchronizing Functions                                **
**            Synchronizes Callback in JNI and MFC Extns                      **
*******************************************************************************/

bool_t EXTNS_GetConnectFlag(void) { return (gphNxpExtns_Context.ExtnsConnect); }

void EXTNS_SetConnectFlag(bool_t flagval) {
  gphNxpExtns_Context.ExtnsConnect = flagval;
}

bool_t EXTNS_GetDeactivateFlag(void) {
  return (gphNxpExtns_Context.ExtnsDeactivate);
}

void EXTNS_SetDeactivateFlag(bool_t flagval) {
  gphNxpExtns_Context.ExtnsDeactivate = flagval;
}

bool_t EXTNS_GetCallBackFlag(void) {
  return (gphNxpExtns_Context.ExtnsCallBack);
}

void EXTNS_SetCallBackFlag(bool_t flagval) {
  gphNxpExtns_Context.ExtnsCallBack = flagval;
}
NFCSTATUS EXTNS_GetPresenceCheckStatus(void) {
  struct timespec ts;

  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += 0;
  ts.tv_nsec += 100 * 1000 * 1000;  // 100 milisec
  if (ts.tv_nsec >= 1000 * 1000 * 1000) {
    ts.tv_sec += 1;
    ts.tv_nsec = ts.tv_nsec - (1000 * 1000 * 1000);
  }

  if (sem_timedwait(&gAuthCmdBuf.semPresenceCheck, &ts)) {
    LOG(ERROR) << StringPrintf("%s: failed to wait (errno=%d)", __func__,
                               errno);
    sem_destroy(&gAuthCmdBuf.semPresenceCheck);
    gAuthCmdBuf.auth_sent = false;
    return NFCSTATUS_FAILED;
  }
  if (sem_destroy(&gAuthCmdBuf.semPresenceCheck)) {
    LOG(ERROR) << StringPrintf(
        "%s: Failed to destroy check Presence semaphore (errno=%d)", __func__,
        errno);
  }
  return gAuthCmdBuf.status;
}

void MfcPresenceCheckResult(NFCSTATUS status) {
  gAuthCmdBuf.status = status;
  EXTNS_SetCallBackFlag(true);
  sem_post(&gAuthCmdBuf.semPresenceCheck);
}
void MfcResetPresenceCheckStatus(void) { gAuthCmdBuf.auth_sent = false; }
/*******************************************************************************
**
** Function         EXTNS_CheckMfcResponse
**
** Description      This function is called from JNI Transceive for Mifare
**                  Classic Tag status interpretation and to send the required
**                  status to application
**
** Returns          NFCSTATUS_SUCCESS
**                  NFCSTATUS_FAILED
**
*******************************************************************************/
NFCSTATUS EXTNS_CheckMfcResponse(uint8_t** sTransceiveData,
                                 uint32_t* sTransceiveDataLen) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  if (*sTransceiveDataLen == 3) {
    if ((*sTransceiveData)[0] == 0x10 && (*sTransceiveData)[1] != 0x0A) {
      LOG(ERROR) << StringPrintf("Mifare Error in payload response");
      *sTransceiveDataLen = 0x1;
      *sTransceiveData += 1;
      return NFCSTATUS_FAILED;
    }
  }
  if ((*sTransceiveData)[0] == 0x40) {
    *sTransceiveData += 1;
    *sTransceiveDataLen = 0x01;
    if ((*sTransceiveData)[0] == 0x03) {
      *sTransceiveDataLen = 0x00;
      status = NFCSTATUS_FAILED;
    }
  } else if ((*sTransceiveData)[0] == 0x10) {
    *sTransceiveData += 1;
    *sTransceiveDataLen = 0x10;
  }

  return status;
}
