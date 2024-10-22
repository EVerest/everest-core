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

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <log/log.h>
#include <nfc_api.h>
#include <nfc_int.h>
#include <phNfcCompId.h>
#include <phNxpExtns_MifareStd.h>
#include <rw_api.h>

using android::base::StringPrintf;

extern bool nfc_debug_enabled;

phNxpExtns_Context_t gphNxpExtns_Context;
phNciNfc_TransceiveInfo_t tNciTranscvInfo;
phFriNfc_sNdefSmtCrdFmt_t* NdefSmtCrdFmt = NULL;
phFriNfc_NdefMap_t* NdefMap = NULL;
phLibNfc_NdefInfo_t NdefInfo;
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
pthread_mutex_t SharedDataMutex = PTHREAD_MUTEX_INITIALIZER;
#endif
uint8_t current_key[PHLIBNFC_MFC_AUTHKEYLEN] = {0};
phNci_mfc_auth_cmd_t gAuthCmdBuf;
static NFCSTATUS phNciNfc_SendMfReq(phNciNfc_TransceiveInfo_t tTranscvInfo,
                                    uint8_t* buff, uint16_t* buffSz);
static NFCSTATUS phLibNfc_SendRawCmd(
    phNfc_sTransceiveInfo_t* pTransceiveInfo,
    pphNciNfc_TransceiveInfo_t pMappedTranscvIf);
static NFCSTATUS phLibNfc_SendWrt16Cmd(
    phNfc_sTransceiveInfo_t* pTransceiveInfo,
    pphNciNfc_TransceiveInfo_t pMappedTranscvIf);
static NFCSTATUS phLibNfc_SendAuthCmd(
    phNfc_sTransceiveInfo_t* pTransceiveInfo,
    phNciNfc_TransceiveInfo_t* tNciTranscvInfo) __attribute__((unused));
static NFCSTATUS phLibNfc_MapCmds(phNciNfc_RFDevType_t RemDevType,
                                  phNfc_sTransceiveInfo_t* pTransceiveInfo,
                                  pphNciNfc_TransceiveInfo_t pMappedTranscvIf);
static NFCSTATUS phLibNfc_MifareMap(
    phNfc_sTransceiveInfo_t* pTransceiveInfo,
    pphNciNfc_TransceiveInfo_t pMappedTranscvIf);
static NFCSTATUS phLibNfc_ChkAuthCmdMFC(
    phNfc_sTransceiveInfo_t* pTransceiveInfo, uint8_t* bKey);
static NFCSTATUS phLibNfc_GetKeyNumberMFC(uint8_t* buffer, uint8_t* bKey);
static void phLibNfc_CalSectorAddress(uint8_t* Sector_Address);
static NFCSTATUS phNciNfc_MfCreateAuthCmdHdr(
    phNciNfc_TransceiveInfo_t tTranscvInfo, uint8_t bBlockAddr, uint8_t* buff,
    uint16_t* buffSz);
static NFCSTATUS phNciNfc_MfCreateXchgDataHdr(
    phNciNfc_TransceiveInfo_t tTranscvInfo, uint8_t* buff, uint16_t* buffSz);
static NFCSTATUS phLibNfc_SendWrt16CmdPayload(
    phNfc_sTransceiveInfo_t* pTransceiveInfo,
    pphNciNfc_TransceiveInfo_t pMappedTranscvIf);
static NFCSTATUS phNciNfc_RecvMfResp(phNciNfc_Buff_t* RspBuffInfo,
                                     NFCSTATUS wStatus);
static NFCSTATUS nativeNfcExtns_doTransceive(uint8_t* buff, uint16_t buffSz);
static NFCSTATUS phFriNfc_NdefSmtCrd_Reset__(
    phFriNfc_sNdefSmtCrdFmt_t* NdefSmtCrdFmt, uint8_t* SendRecvBuffer,
    uint16_t* SendRecvBuffLen);
static NFCSTATUS phFriNfc_ValidateParams(uint8_t* PacketData,
                                         uint32_t* PacketDataLength,
                                         uint8_t Offset,
                                         phFriNfc_NdefMap_t* pNdefMap,
                                         uint8_t bNdefReq);
static void Mfc_FormatNdef_Completion_Routine(void* NdefCtxt, NFCSTATUS status);
static void Mfc_WriteNdef_Completion_Routine(void* NdefCtxt, NFCSTATUS status);
static void Mfc_ReadNdef_Completion_Routine(void* NdefCtxt, NFCSTATUS status);
static void Mfc_CheckNdef_Completion_Routine(void* NdefCtxt, NFCSTATUS status);

/*******************************************************************************
**
** Function         phNxpExtns_MfcModuleDeInit
**
** Description      It Deinitializes the Mifare module.
**
**                  Frees all the memory occupied by Mifare module
**
** Returns:
**                  NFCSTATUS_SUCCESS - if successfully deinitialize
**                  NFCSTATUS_FAILED  - otherwise
**
*******************************************************************************/
NFCSTATUS phNxpExtns_MfcModuleDeInit(void) {
  NFCSTATUS status = NFCSTATUS_FAILED;

  if (NdefMap != NULL) {
    if (NdefMap->psRemoteDevInfo != NULL) {
      free(NdefMap->psRemoteDevInfo);
      NdefMap->psRemoteDevInfo = NULL;
    }
    if (NdefMap->SendRecvBuf != NULL) {
      free(NdefMap->SendRecvBuf);
      NdefMap->SendRecvBuf = NULL;
    }
    if (NdefMap->SendRecvLength != NULL) {
      free(NdefMap->SendRecvLength);
      NdefMap->SendRecvLength = NULL;
    }
    if (NdefMap->DataCount != NULL) {
      free(NdefMap->DataCount);
      NdefMap->DataCount = NULL;
    }
    if (NdefMap->pTransceiveInfo != NULL) {
      if (NdefMap->pTransceiveInfo->sSendData.buffer != NULL) {
        free(NdefMap->pTransceiveInfo->sSendData.buffer);
        NdefMap->pTransceiveInfo->sSendData.buffer = NULL;
      }
      if (NdefMap->pTransceiveInfo->sRecvData.buffer != NULL) {
        free(NdefMap->pTransceiveInfo->sRecvData.buffer);
        NdefMap->pTransceiveInfo->sRecvData.buffer = NULL;
      }
      free(NdefMap->pTransceiveInfo);
      NdefMap->pTransceiveInfo = NULL;
    }

    free(NdefMap);
    NdefMap = NULL;
  }

  if (tNciTranscvInfo.tSendData.pBuff != NULL) {
    free(tNciTranscvInfo.tSendData.pBuff);
    tNciTranscvInfo.tSendData.pBuff = NULL;
  }

  if (NdefSmtCrdFmt != NULL) {
    free(NdefSmtCrdFmt);
    NdefSmtCrdFmt = NULL;
  }
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
  pthread_mutex_lock(&SharedDataMutex);
#endif
  if (NULL != NdefInfo.psUpperNdefMsg) {
    free(NdefInfo.psUpperNdefMsg);
    NdefInfo.psUpperNdefMsg = NULL;
  }
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
  pthread_mutex_unlock(&SharedDataMutex);
#endif
  if (NULL != gAuthCmdBuf.pauth_cmd) {
    if (NULL != gAuthCmdBuf.pauth_cmd->buffer) {
      free(gAuthCmdBuf.pauth_cmd->buffer);
      gAuthCmdBuf.pauth_cmd->buffer = NULL;
    }
    free(gAuthCmdBuf.pauth_cmd);
    gAuthCmdBuf.pauth_cmd = NULL;
  }
  status = NFCSTATUS_SUCCESS;
  return status;
}

/*******************************************************************************
**
** Function         phNxpExtns_MfcModuleInit
**
** Description      It Initializes the memroy and global variables related
**                  to Mifare module.
**
**                  Reset all the global variables and allocate memory for
*Mifare module
**
** Returns:
**                  NFCSTATUS_SUCCESS - if successfully deinitialize
**                  NFCSTATUS_FAILED  - otherwise
**
*******************************************************************************/
NFCSTATUS phNxpExtns_MfcModuleInit(void) {
  NFCSTATUS status = NFCSTATUS_FAILED;
  gphNxpExtns_Context.writecmdFlag = false;
  gphNxpExtns_Context.RawWriteCallBack = false;
  gphNxpExtns_Context.CallBackCtxt = NULL;
  gphNxpExtns_Context.CallBackMifare = NULL;
  gphNxpExtns_Context.ExtnsConnect = false;
  gphNxpExtns_Context.ExtnsDeactivate = false;
  gphNxpExtns_Context.ExtnsCallBack = false;

  NdefMap = (phFriNfc_NdefMap_t*)malloc(sizeof(phFriNfc_NdefMap_t));
  if (NULL == NdefMap) {
    goto clean_and_return;
  }
  memset(NdefMap, 0, sizeof(phFriNfc_NdefMap_t));

  NdefMap->psRemoteDevInfo = (phLibNfc_sRemoteDevInformation_t*)malloc(
      sizeof(phLibNfc_sRemoteDevInformation_t));
  if (NULL == NdefMap->psRemoteDevInfo) {
    goto clean_and_return;
  }
  memset(NdefMap->psRemoteDevInfo, 0, sizeof(phLibNfc_sRemoteDevInformation_t));

  NdefMap->SendRecvBuf = (uint8_t*)malloc((uint32_t)(MAX_BUFF_SIZE * 2));
  if (NULL == NdefMap->SendRecvBuf) {
    goto clean_and_return;
  }
  memset(NdefMap->SendRecvBuf, 0, (MAX_BUFF_SIZE * 2));

  NdefMap->SendRecvLength = (uint16_t*)malloc(sizeof(uint16_t));
  if (NULL == NdefMap->SendRecvLength) {
    goto clean_and_return;
  }
  memset(NdefMap->SendRecvLength, 0, sizeof(uint16_t));

  NdefMap->DataCount = (uint16_t*)malloc(sizeof(uint16_t));
  if (NULL == NdefMap->DataCount) {
    goto clean_and_return;
  }
  memset(NdefMap->DataCount, 0, sizeof(uint16_t));

  NdefMap->pTransceiveInfo =
      (phNfc_sTransceiveInfo_t*)malloc(sizeof(phNfc_sTransceiveInfo_t));
  if (NULL == NdefMap->pTransceiveInfo) {
    goto clean_and_return;
  }
  memset(NdefMap->pTransceiveInfo, 0, sizeof(phNfc_sTransceiveInfo_t));

  tNciTranscvInfo.tSendData.pBuff = (uint8_t*)malloc((uint32_t)MAX_BUFF_SIZE);
  if (NULL == tNciTranscvInfo.tSendData.pBuff) {
    goto clean_and_return;
  }
  memset(tNciTranscvInfo.tSendData.pBuff, 0, MAX_BUFF_SIZE);

  NdefMap->pTransceiveInfo->sSendData.buffer =
      (uint8_t*)malloc((uint32_t)MAX_BUFF_SIZE);
  if (NdefMap->pTransceiveInfo->sSendData.buffer == NULL) {
    goto clean_and_return;
  }
  memset(NdefMap->pTransceiveInfo->sSendData.buffer, 0, MAX_BUFF_SIZE);
  NdefMap->pTransceiveInfo->sSendData.length = MAX_BUFF_SIZE;

  NdefMap->pTransceiveInfo->sRecvData.buffer = (uint8_t*)malloc(
      (uint32_t)MAX_BUFF_SIZE); /* size should be same as sRecvData */
  if (NdefMap->pTransceiveInfo->sRecvData.buffer == NULL) {
    goto clean_and_return;
  }
  memset(NdefMap->pTransceiveInfo->sRecvData.buffer, 0, MAX_BUFF_SIZE);
  NdefMap->pTransceiveInfo->sRecvData.length = MAX_BUFF_SIZE;

  NdefSmtCrdFmt =
      (phFriNfc_sNdefSmtCrdFmt_t*)malloc(sizeof(phFriNfc_sNdefSmtCrdFmt_t));
  if (NdefSmtCrdFmt == NULL) {
    goto clean_and_return;
  }
  memset(NdefSmtCrdFmt, 0, sizeof(phFriNfc_sNdefSmtCrdFmt_t));
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
  pthread_mutex_lock(&SharedDataMutex);
#endif
  NdefInfo.psUpperNdefMsg = (phNfc_sData_t*)malloc(sizeof(phNfc_sData_t));
  if (NULL == NdefInfo.psUpperNdefMsg) {
    goto clean_and_return;
  }
  memset(NdefInfo.psUpperNdefMsg, 0, sizeof(phNfc_sData_t));
  memset(&gAuthCmdBuf, 0, sizeof(phNci_mfc_auth_cmd_t));
  gAuthCmdBuf.pauth_cmd = (phNfc_sData_t*)malloc(sizeof(phNfc_sData_t));
  if (NULL == gAuthCmdBuf.pauth_cmd) {
    goto clean_and_return;
  }
  gAuthCmdBuf.pauth_cmd->buffer = (uint8_t*)malloc((uint32_t)NCI_MAX_DATA_LEN);
  if (NULL == gAuthCmdBuf.pauth_cmd->buffer) {
    goto clean_and_return;
  }
  status = NFCSTATUS_SUCCESS;

clean_and_return:
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
  pthread_mutex_unlock(&SharedDataMutex);
#endif
  if (status != NFCSTATUS_SUCCESS) {
    LOG(ERROR) << StringPrintf("CRIT: Memory Allocation failed for MFC!");
    phNxpExtns_MfcModuleDeInit();
  }
  return status;
}

/*******************************************************************************
**
** Function         Mfc_CheckNdef
**
** Description      It triggers NDEF detection for Mifare Classic Tag.
**
**
** Returns          NFCSTATUS_SUCCESS - if successfully initiated
**                  NFCSTATUS_FAILED  - otherwise
**
*******************************************************************************/
NFCSTATUS Mfc_CheckNdef(void) {
  NFCSTATUS status = NFCSTATUS_FAILED;

  EXTNS_SetCallBackFlag(false);
  /* Set Completion Routine for CheckNdef */
  NdefMap->CompletionRoutine[0].CompletionRoutine =
      Mfc_CheckNdef_Completion_Routine;

  gphNxpExtns_Context.CallBackMifare = phFriNfc_MifareStdMap_Process;
  gphNxpExtns_Context.CallBackCtxt = NdefMap;
  status = phFriNfc_MifareStdMap_H_Reset(NdefMap);
  if (NFCSTATUS_SUCCESS == status) {
    status = phFriNfc_MifareStdMap_ChkNdef(NdefMap);
    if (status == NFCSTATUS_PENDING) {
      status = NFCSTATUS_SUCCESS;
    }
  }
  if (status != NFCSTATUS_SUCCESS) {
    status = NFCSTATUS_FAILED;
  }

  return status;
}

/*******************************************************************************
**
** Function         Mfc_CheckNdef_Completion_Routine
**
** Description      Notify NDEF detection for Mifare Classic Tag to JNI
**
**                  Upon completion of NDEF detection, a
**                  NFA_NDEF_DETECT_EVT will be sent, to notify the application
**                  of the NDEF attributes (NDEF total memory size, current
**                  size, etc.).
**
** Returns:         void
**
*******************************************************************************/
static void Mfc_CheckNdef_Completion_Routine(void* NdefCtxt, NFCSTATUS status) {
  (void)NdefCtxt;
  tNFA_CONN_EVT_DATA conn_evt_data;

  conn_evt_data.ndef_detect.status = status;
  if (NFCSTATUS_SUCCESS == status) {
    /* NDef Tag Detected */
    conn_evt_data.ndef_detect.protocol = NFC_PROTOCOL_MIFARE;
    phFrinfc_MifareClassic_GetContainerSize(
        NdefMap, (uint32_t*)&(conn_evt_data.ndef_detect.max_size),
        (uint32_t*)&(conn_evt_data.ndef_detect.cur_size));
    NdefInfo.NdefLength = conn_evt_data.ndef_detect.max_size;
    /* update local flags */
    NdefInfo.is_ndef = 1;
    NdefInfo.NdefActualSize = conn_evt_data.ndef_detect.cur_size;
    if (PH_NDEFMAP_CARD_STATE_READ_ONLY == NdefMap->CardState) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("Mfc_CheckNdef_Completion_Routine : READ_ONLY_CARD");
      conn_evt_data.ndef_detect.flags = RW_NDEF_FL_READ_ONLY;
    } else {
      conn_evt_data.ndef_detect.flags =
          RW_NDEF_FL_SUPPORTED | RW_NDEF_FL_FORMATED;
    }
  } else {
    /* NDEF Detection failed for other reasons */
    conn_evt_data.ndef_detect.cur_size = 0;
    conn_evt_data.ndef_detect.max_size = 0;
    conn_evt_data.ndef_detect.flags = RW_NDEF_FL_UNKNOWN;

    /* update local flags */
    NdefInfo.is_ndef = 0;
    NdefInfo.NdefActualSize = conn_evt_data.ndef_detect.cur_size;
  }
  (*gphNxpExtns_Context.p_conn_cback)(NFA_NDEF_DETECT_EVT, &conn_evt_data);

  return;
}
/*******************************************************************************
**
** Function         Mfc_ReadNdef_Completion_Routine
**
** Description      Notify NDEF read completion for Mifare Classic Tag to JNI
**
**                  Upon completion of NDEF read, a
**                  NFA_READ_CPLT_EVT will be sent, to notify the application
**                  with the NDEF data and status
**
** Returns:         void
**
*******************************************************************************/
static void Mfc_ReadNdef_Completion_Routine(void* NdefCtxt, NFCSTATUS status) {
  (void)NdefCtxt;
  tNFA_CONN_EVT_DATA conn_evt_data;
  tNFA_NDEF_EVT_DATA p_data;

  conn_evt_data.status = status;
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
  pthread_mutex_lock(&SharedDataMutex);
#endif
  if (NFCSTATUS_SUCCESS == status) {
    p_data.ndef_data.len = NdefInfo.psUpperNdefMsg->length;
    p_data.ndef_data.p_data = NdefInfo.psUpperNdefMsg->buffer;
    (*gphNxpExtns_Context.p_ndef_cback)(NFA_NDEF_DATA_EVT, &p_data);
  } else {
  }

  (*gphNxpExtns_Context.p_conn_cback)(NFA_READ_CPLT_EVT, &conn_evt_data);

  if (NdefInfo.psUpperNdefMsg->buffer != NULL) {
    free(NdefInfo.psUpperNdefMsg->buffer);
    NdefInfo.psUpperNdefMsg->buffer = NULL;
  }
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
  pthread_mutex_unlock(&SharedDataMutex);
#endif
  return;
}

/*******************************************************************************
**
** Function         Mfc_WriteNdef_Completion_Routine
**
** Description      Notify NDEF write completion for Mifare Classic Tag to JNI
**
**                  Upon completion of NDEF write, a
**                  NFA_WRITE_CPLT_EVT will be sent along with status
**
** Returns:         void
**
*******************************************************************************/
static void Mfc_WriteNdef_Completion_Routine(void* NdefCtxt, NFCSTATUS status) {
  (void)NdefCtxt;
  tNFA_CONN_EVT_DATA conn_evt_data;

  conn_evt_data.status = status;
  (*gphNxpExtns_Context.p_conn_cback)(NFA_WRITE_CPLT_EVT, &conn_evt_data);

  return;
}

/*******************************************************************************
**
** Function         Mfc_FormatNdef_Completion_Routine
**
** Description      Notify NDEF format completion for Mifare Classic Tag to JNI
**
**                  Upon completion of NDEF format, a
**                  NFA_FORMAT_CPLT_EVT will be sent along with status
**
** Returns:         void
**
*******************************************************************************/
static void Mfc_FormatNdef_Completion_Routine(void* NdefCtxt,
                                              NFCSTATUS status) {
  (void)NdefCtxt;
  tNFA_CONN_EVT_DATA conn_evt_data;

  conn_evt_data.status = status;
  (*gphNxpExtns_Context.p_conn_cback)(NFA_FORMAT_CPLT_EVT, &conn_evt_data);

  return;
}

/*******************************************************************************
**
** Function          phFriNfc_ValidateParams
**
** Description      This function is a common function which validates NdefRd
**                  and NdefWr parameters.
**
** Returns          NFCSTATUS_SUCCESS  - All the params are valid
**                  NFCSTATUS_FAILED   - otherwise
**
*******************************************************************************/
static NFCSTATUS phFriNfc_ValidateParams(uint8_t* PacketData,
                                         uint32_t* PacketDataLength,
                                         uint8_t Offset,
                                         phFriNfc_NdefMap_t* pNdefMap,
                                         uint8_t bNdefReq) {
  if ((pNdefMap == NULL) || (PacketData == NULL) ||
      (PacketDataLength == NULL)) {
    return NFCSTATUS_FAILED;
  }

  if (pNdefMap->CardState == PH_NDEFMAP_CARD_STATE_INVALID) {
    return NFCSTATUS_FAILED;
  }

  if (bNdefReq == PH_FRINFC_NDEF_READ_REQ) {
    if ((Offset != PH_FRINFC_NDEFMAP_SEEK_CUR) &&
        (Offset != PH_FRINFC_NDEFMAP_SEEK_BEGIN)) {
      return NFCSTATUS_FAILED;
    }
    if (pNdefMap->CardState == PH_NDEFMAP_CARD_STATE_INITIALIZED) {
      pNdefMap->NumOfBytesRead = PacketDataLength;
      *pNdefMap->NumOfBytesRead = 0;
      return NFCSTATUS_EOF_NDEF_CONTAINER_REACHED;
    }
    if ((pNdefMap->PrevOperation == PH_FRINFC_NDEFMAP_WRITE_OPE) &&
        (Offset != PH_FRINFC_NDEFMAP_SEEK_BEGIN)) {
      return NFCSTATUS_FAILED; /* return INVALID_DEVICE_REQUEST */
    }
    if (Offset == PH_FRINFC_NDEFMAP_SEEK_BEGIN) {
      pNdefMap->ApduBuffIndex = 0;
      *pNdefMap->DataCount = 0;
    } else if ((pNdefMap->bPrevReadMode == PH_FRINFC_NDEFMAP_SEEK_BEGIN) ||
               (pNdefMap->bPrevReadMode == PH_FRINFC_NDEFMAP_SEEK_CUR)) {
    } else {
      return NFCSTATUS_FAILED;
    }
  } else if (bNdefReq == PH_FRINFC_NDEF_WRITE_REQ) {
    if (pNdefMap->CardState == PH_NDEFMAP_CARD_STATE_READ_ONLY) {
      pNdefMap->WrNdefPacketLength = PacketDataLength;
      *pNdefMap->WrNdefPacketLength = 0x00;
      return NFCSTATUS_NOT_ALLOWED;
    }
  }

  return NFCSTATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         Mfc_SetRdOnly_Completion_Routine
**
** Description      Notify NDEF read only completion for Mifare Classic Tag to
*JNI
**
**                  Upon completion of NDEF format, a
**                  NFA_SET_TAG_RO_EVT will be sent along with status
**
** Returns:         void
**
*******************************************************************************/
static void Mfc_SetRdOnly_Completion_Routine(void* NdefCtxt, NFCSTATUS status) {
  (void)NdefCtxt;
  tNFA_CONN_EVT_DATA conn_evt_data;
  LOG(ERROR) << StringPrintf("%s status = 0x%x", __func__, status);
  conn_evt_data.status = status;
  (*gphNxpExtns_Context.p_conn_cback)(NFA_SET_TAG_RO_EVT, &conn_evt_data);

  return;
}

/*******************************************************************************
**
** Function        Mfc_SetReadOnly
**
**
** Description:    It triggers ConvertToReadOnly  for Mifare Classic Tag.
**
** Returns:
**                  NFCSTATUS_SUCCESS if successfully initiated
**                  NFCSTATUS_FAILED otherwise
**
*******************************************************************************/
NFCSTATUS Mfc_SetReadOnly(uint8_t* secrtkey, uint8_t len) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s Entering ", __func__);
  NFCSTATUS status = NFCSTATUS_FAILED;
  uint8_t mif_secrete_key[6] = {0};
  uint8_t id = 0;
  EXTNS_SetCallBackFlag(false);
  memcpy(mif_secrete_key, secrtkey, len);
  gphNxpExtns_Context.CallBackMifare = phFriNfc_MifareStdMap_Process;
  gphNxpExtns_Context.CallBackCtxt = NdefMap;
  for (id = 0; id < len; id++) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("secrtkey[%d] = 0x%x", id, secrtkey[id]);
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("mif_secrete_key[%d] = 0x%x", id, mif_secrete_key[id]);
  }
  /* Set Completion Routine for ReadNdef */
  NdefMap->CompletionRoutine[0].CompletionRoutine =
      Mfc_SetRdOnly_Completion_Routine;
  if (NdefInfo.is_ndef == 0) {
    status = NFCSTATUS_NON_NDEF_COMPLIANT;
    goto Mfc_SetRdOnly;
  } else if ((NdefInfo.is_ndef == 1) && (NdefInfo.NdefActualSize == 0)) {
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
    pthread_mutex_lock(&SharedDataMutex);
#endif
    NdefInfo.psUpperNdefMsg->length = NdefInfo.NdefActualSize;
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
    pthread_mutex_unlock(&SharedDataMutex);
#endif
    status = NFCSTATUS_SUCCESS;
    goto Mfc_SetRdOnly;
  } else {
    status = phFriNfc_MifareStdMap_ConvertToReadOnly(NdefMap, mif_secrete_key);
  }
  if (NFCSTATUS_PENDING == status) {
    status = NFCSTATUS_SUCCESS;
  }

Mfc_SetRdOnly:
  return status;
}

/*******************************************************************************
**
** Function         Mfc_ReadNdef
**
** Description      It triggers receiving of the NDEF message from Mifare
*Classic Tag.
**
**
** Returns:
**                  NFCSTATUS_SUCCESS - if successfully initiated
**                  NFCSTATUS_FAILED  - otherwise
**
*******************************************************************************/
NFCSTATUS Mfc_ReadNdef(void) {
  NFCSTATUS status = NFCSTATUS_FAILED;
  uint8_t* PacketData = NULL;
  uint32_t* PacketDataLength = NULL;
  phLibNfc_Ndef_EOffset_t Offset;

  EXTNS_SetCallBackFlag(false);

  Offset = phLibNfc_Ndef_EBegin;

  gphNxpExtns_Context.CallBackMifare = phFriNfc_MifareStdMap_Process;
  gphNxpExtns_Context.CallBackCtxt = NdefMap;
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
  pthread_mutex_lock(&SharedDataMutex);
#endif
  if (NdefInfo.is_ndef == 0) {
    status = NFCSTATUS_NON_NDEF_COMPLIANT;
    goto Mfc_RdNdefEnd;
  } else if ((NdefInfo.is_ndef == 1) && (NdefInfo.NdefActualSize == 0)) {
    NdefInfo.psUpperNdefMsg->length = NdefInfo.NdefActualSize;
    status = NFCSTATUS_SUCCESS;
    goto Mfc_RdNdefEnd;
  } else {
    NdefInfo.psUpperNdefMsg->buffer = (uint8_t*)malloc(NdefInfo.NdefActualSize);
    if (NULL == NdefInfo.psUpperNdefMsg->buffer) {
      goto Mfc_RdNdefEnd;
    }
    NdefInfo.psUpperNdefMsg->length = NdefInfo.NdefActualSize;

    /* Set Completion Routine for ReadNdef */
    NdefMap->CompletionRoutine[1].CompletionRoutine =
        Mfc_ReadNdef_Completion_Routine;
    NdefInfo.NdefContinueRead = (uint8_t)(PH_FRINFC_NDEFMAP_SEEK_BEGIN);
  }

  PacketData = NdefInfo.psUpperNdefMsg->buffer;
  PacketDataLength = (uint32_t*)&(NdefInfo.psUpperNdefMsg->length);
  NdefMap->bCurrReadMode = Offset;
  status = phFriNfc_ValidateParams(PacketData, PacketDataLength, Offset,
                                   NdefMap, PH_FRINFC_NDEF_READ_REQ);
  if (status != NFCSTATUS_SUCCESS) {
    goto Mfc_RdNdefEnd;
  }

  status = phFriNfc_MifareStdMap_RdNdef(NdefMap, PacketData, PacketDataLength,
                                        Offset);

  if (NFCSTATUS_INSUFFICIENT_STORAGE == status) {
    NdefInfo.psUpperNdefMsg->length = 0x00;
    status = NFCSTATUS_SUCCESS;
  }

  if (NFCSTATUS_PENDING == status) {
    status = NFCSTATUS_SUCCESS;
  }

Mfc_RdNdefEnd:
  if (status != NFCSTATUS_SUCCESS) {
    if (NULL != NdefInfo.psUpperNdefMsg->buffer) {
      free(NdefInfo.psUpperNdefMsg->buffer);
      NdefInfo.psUpperNdefMsg->buffer = NULL;
    }
    status = NFCSTATUS_FAILED;
  }
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
  pthread_mutex_unlock(&SharedDataMutex);
#endif
  return status;
}
/*******************************************************************************
**
** Function         Mfc_PresenceCheck
**
** Description      It triggers receiving of the NDEF message from Mifare
*Classic Tag.
**
**
** Returns:
**                  NFCSTATUS_SUCCESS - if successfully initiated
**                  NFCSTATUS_FAILED  - otherwise
**
*******************************************************************************/
NFCSTATUS Mfc_PresenceCheck(void) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  if (gAuthCmdBuf.auth_status == true) {
    EXTNS_SetCallBackFlag(false);
    status = nativeNfcExtns_doTransceive(gAuthCmdBuf.pauth_cmd->buffer,
                                         gAuthCmdBuf.pauth_cmd->length);
    if (status != NFCSTATUS_PENDING) {
      gAuthCmdBuf.auth_sent = false;
      status = NFCSTATUS_FAILED;
    } else {
      gAuthCmdBuf.auth_sent = true;
      status = NFCSTATUS_SUCCESS;
    }
  } else {
    status = NFCSTATUS_NOT_ALLOWED;
  }
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s status = 0x%x", __func__, status);
  return status;
}
/*******************************************************************************
**
** Function         Mfc_WriteNdef
**
** Description      It triggers the NDEF data write to Mifare Classic Tag.
**
**
** Returns:
**                  NFCSTATUS_SUCCESS - if successfully initiated
**                  NFCSTATUS_FAILED  - otherwise
**
*******************************************************************************/
NFCSTATUS Mfc_WriteNdef(uint8_t* p_data, uint32_t len) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  uint8_t* PacketData = NULL;
  uint32_t* PacketDataLength = NULL;

  if (p_data == NULL || len == 0) {
    LOG(ERROR) << StringPrintf("MFC Error: Invalid Parameters to Ndef Write");
    status = NFCSTATUS_FAILED;
    goto Mfc_WrNdefEnd;
  }

  EXTNS_SetCallBackFlag(false);
  gphNxpExtns_Context.CallBackMifare = phFriNfc_MifareStdMap_Process;
  gphNxpExtns_Context.CallBackCtxt = NdefMap;
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
  pthread_mutex_lock(&SharedDataMutex);
#endif
  if (NdefInfo.is_ndef == PH_LIBNFC_INTERNAL_CHK_NDEF_NOT_DONE) {
    status = NFCSTATUS_REJECTED;
    goto Mfc_WrNdefEnd;
  } else if (NdefInfo.is_ndef == 0) {
    status = NFCSTATUS_NON_NDEF_COMPLIANT;
    goto Mfc_WrNdefEnd;
  } else if (len > NdefInfo.NdefLength) {
    status = NFCSTATUS_NOT_ENOUGH_MEMORY;
    goto Mfc_WrNdefEnd;
  } else {
    NdefInfo.psUpperNdefMsg->buffer = p_data;
    NdefInfo.psUpperNdefMsg->length = len;

    NdefInfo.AppWrLength = len;
    NdefMap->CompletionRoutine[2].CompletionRoutine =
        Mfc_WriteNdef_Completion_Routine;
    if (0 == len) {
      /* TODO: Erase the Tag */
    } else {
      NdefMap->ApduBuffIndex = 0x00;
      *NdefMap->DataCount = 0x00;
      PacketData = NdefInfo.psUpperNdefMsg->buffer;
      PacketDataLength = &(NdefInfo.dwWrLength);
      NdefMap->WrNdefPacketLength = PacketDataLength;
      NdefInfo.dwWrLength = len;

      status = phFriNfc_ValidateParams(PacketData, PacketDataLength, 0, NdefMap,
                                       PH_FRINFC_NDEF_WRITE_REQ);
      if (status != NFCSTATUS_SUCCESS) {
        goto Mfc_WrNdefEnd;
      }

      status = phFriNfc_MifareStdMap_WrNdef(
          NdefMap, PacketData, PacketDataLength, PH_FRINFC_NDEFMAP_SEEK_BEGIN);

      if (status == NFCSTATUS_PENDING) {
        status = NFCSTATUS_SUCCESS;
      }
    }
  }

Mfc_WrNdefEnd:
#if (NFC_NXP_NOT_OPEN_INCLUDED == TRUE)
  pthread_mutex_unlock(&SharedDataMutex);
#endif
  if (status != NFCSTATUS_SUCCESS) {
    status = NFCSTATUS_FAILED;
  }
  return status;
}
/*******************************************************************************
**
** Function          phFriNfc_NdefSmtCrd_Reset__
**
** Description      This function Resets the component instance to the initial
**                  state and initializes the internal variables.
**
** Returns          NFCSTATUS_SUCCESS
**
*******************************************************************************/
static NFCSTATUS phFriNfc_NdefSmtCrd_Reset__(
    phFriNfc_sNdefSmtCrdFmt_t* NdefSmtCrdFmt, uint8_t* SendRecvBuffer,
    uint16_t* SendRecvBuffLen) {
  //    NFCSTATUS status = NFCSTATUS_FAILED;                      /*commented to
  //    eliminate unused variable warning*/
  uint8_t index;

  /* Initialize the state to Init */
  NdefSmtCrdFmt->State = PH_FRINFC_SMTCRDFMT_STATE_RESET_INIT;

  for (index = 0; index < PH_FRINFC_SMTCRDFMT_CR; index++) {
    /* Initialize the NdefMap Completion Routine to Null */
    NdefSmtCrdFmt->CompletionRoutine[index].CompletionRoutine = NULL;
    /* Initialize the NdefMap Completion Routine context to Null  */
    NdefSmtCrdFmt->CompletionRoutine[index].Context = NULL;
  }

  /* Trx Buffer registered */
  NdefSmtCrdFmt->SendRecvBuf = SendRecvBuffer;

  /* Trx Buffer Size */
  NdefSmtCrdFmt->SendRecvLength = SendRecvBuffLen;

  /* Register Transfer Buffer Length */
  NdefSmtCrdFmt->SendLength = 0;

  /* Initialize the Format status flag*/
  NdefSmtCrdFmt->FmtProcStatus = 0;

  /* Reset the Card Type */
  NdefSmtCrdFmt->CardType = 0;

  /* Reset MapCompletion Info*/
  NdefSmtCrdFmt->SmtCrdFmtCompletionInfo.CompletionRoutine = NULL;
  NdefSmtCrdFmt->SmtCrdFmtCompletionInfo.Context = NULL;

  phFriNfc_MfStd_Reset(NdefSmtCrdFmt);

  return NFCSTATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         Mfc_FormatNdef
**
** Description      It triggers the NDEF format of Mifare Classic Tag.
**
**
** Returns:
**                  NFCSTATUS_SUCCESS - if successfully initiated
**                  NFCSTATUS_FAILED  - otherwise
**
*******************************************************************************/
NFCSTATUS Mfc_FormatNdef(uint8_t* secretkey, uint8_t len) {
  NFCSTATUS status = NFCSTATUS_FAILED;
  uint8_t mif_std_key[PHLIBNFC_MFC_AUTHKEYLEN] = {0};
  //    static uint8_t   Index;
  //    /*commented to eliminate unused variable warning*/
  uint8_t sak = 0;

  EXTNS_SetCallBackFlag(false);

  if (len != PHLIBNFC_MFC_AUTHKEYLEN) return NFCSTATUS_FAILED;

  memcpy(mif_std_key, secretkey, len);
  memcpy(current_key, secretkey, len);

  if (NULL == NdefSmtCrdFmt || NULL == NdefMap ||
      NULL == NdefMap->SendRecvBuf) {
    goto Mfc_FormatEnd;
  }
  NdefSmtCrdFmt->pTransceiveInfo = NdefMap->pTransceiveInfo;

  gphNxpExtns_Context.CallBackMifare = phFriNfc_MfStd_Process;
  gphNxpExtns_Context.CallBackCtxt = NdefSmtCrdFmt;

  NdefInfo.NdefSendRecvLen = NDEF_SENDRCV_BUF_LEN;
  phFriNfc_NdefSmtCrd_Reset__(NdefSmtCrdFmt, NdefMap->SendRecvBuf,
                              &(NdefInfo.NdefSendRecvLen));

  /* Register Callbacks */
  NdefSmtCrdFmt->CompletionRoutine[0].CompletionRoutine =
      Mfc_FormatNdef_Completion_Routine;
  NdefSmtCrdFmt->CompletionRoutine[1].CompletionRoutine =
      Mfc_FormatNdef_Completion_Routine;
  NdefSmtCrdFmt->psRemoteDevInfo = NdefMap->psRemoteDevInfo;
  sak = NdefSmtCrdFmt->psRemoteDevInfo->RemoteDevInfo.Iso14443A_Info.Sak;

  if ((0x08 == (sak & 0x18)) || (0x18 == (sak & 0x18)) || (0x01 == sak)) {
    NdefSmtCrdFmt->CardType = (uint8_t)(
        ((sak & 0x18) == 0x08)
            ? PH_FRINFC_SMTCRDFMT_MFSTD_1K_CRD
            : (((sak & 0x19) == 0x19) ? PH_FRINFC_SMTCRDFMT_MFSTD_2K_CRD
                                      : PH_FRINFC_SMTCRDFMT_MFSTD_4K_CRD));
    status = phFriNfc_MfStd_Format(NdefSmtCrdFmt, mif_std_key);
  }

  if (NFCSTATUS_PENDING == status) {
    status = NFCSTATUS_SUCCESS;
  }

Mfc_FormatEnd:
  if (status != NFCSTATUS_SUCCESS) {
    status = NFCSTATUS_FAILED;
  }

  return status;
}

/*******************************************************************************
**
** Function         phNxNciExtns_MifareStd_Reconnect
**
** Description      This function sends the deactivate command to NFCC for
*Mifare
**
**
** Returns:
**                  NFCSTATUS_PENDING - if successfully initiated
**                  NFCSTATUS_FAILED  - otherwise
**
*******************************************************************************/
NFCSTATUS phNxNciExtns_MifareStd_Reconnect(void) {
  tNFA_STATUS status;

  EXTNS_SetDeactivateFlag(true);
  if (NFA_STATUS_OK !=
      (status = NFA_Deactivate(true))) /* deactivate to sleep state */
  {
    LOG(ERROR) << StringPrintf("%s: deactivate failed, status = %d", __func__,
                               status);
    return NFCSTATUS_FAILED;
  }

  return NFCSTATUS_PENDING;
}

/*******************************************************************************
**
** Function         Mfc_DeactivateCbackSelect
**
** Description      This function select the Mifare tag
**
**
** Returns:         void
**
*******************************************************************************/
void Mfc_DeactivateCbackSelect(void) {
  tNFA_STATUS status;

  EXTNS_SetConnectFlag(true);
  if (NFA_STATUS_OK !=
      (status = NFA_Select(0x01, phNciNfc_e_RfProtocolsMifCProtocol,
                           phNciNfc_e_RfInterfacesTagCmd_RF))) {
    LOG(ERROR) << StringPrintf("%s: NFA_Select failed, status = %d", __func__,
                               status);
  }

  return;
}

/*******************************************************************************
**
** Function         Mfc_ActivateCback
**
** Description      This function invoke the callback when receive the response
**
**
** Returns:         void
**
**
*******************************************************************************/
void Mfc_ActivateCback(void) {
  gphNxpExtns_Context.CallBackMifare(gphNxpExtns_Context.CallBackCtxt,
                                     NFCSTATUS_SUCCESS);
  return;
}

/*******************************************************************************
**
** Function         Mfc_Transceive
**
** Description      Sends raw frame to Mifare Classic Tag.
**
** Returns          NFCSTATUS_SUCCESS - if successfully initiated
**                  NFCSTATUS_FAILED  - otherwise
**
*******************************************************************************/
NFCSTATUS Mfc_Transceive(uint8_t* p_data, uint32_t len) {
  NFCSTATUS status = NFCSTATUS_FAILED;
  uint8_t i = 0x00;

  if (len == 0) {
    android_errorWriteLog(0x534e4554, "132082342");
    return status;
  }

  gphNxpExtns_Context.RawWriteCallBack = false;
  gphNxpExtns_Context.CallBackMifare = NULL;
  gphNxpExtns_Context.CallBackCtxt = NdefMap;

  EXTNS_SetCallBackFlag(true);
  if (p_data[0] == 0x60 || p_data[0] == 0x61) {
    if (len < 12) {
      android_errorWriteLog(0x534e4554, "125900276");
      return status;
    }
    NdefMap->Cmd.MfCmd = (phNfc_eMifareCmdList_t)p_data[0];

    NdefMap->SendRecvBuf[i++] = p_data[1];

    NdefMap->SendRecvBuf[i++] = p_data[6]; /* TODO, handle 7 byte UID */
    NdefMap->SendRecvBuf[i++] = p_data[7];
    NdefMap->SendRecvBuf[i++] = p_data[8];
    NdefMap->SendRecvBuf[i++] = p_data[9];
    NdefMap->SendRecvBuf[i++] = p_data[10];
    NdefMap->SendRecvBuf[i++] = p_data[11];

    status = phFriNfc_ExtnsTransceive(NdefMap->pTransceiveInfo, NdefMap->Cmd,
                                      NdefMap->SendRecvBuf, NdefMap->SendLength,
                                      NdefMap->SendRecvLength);
  } else if (p_data[0] == 0xA0) {
    EXTNS_SetCallBackFlag(false);
    NdefMap->Cmd.MfCmd = phNfc_eMifareWrite16;
    gphNxpExtns_Context.RawWriteCallBack = true;

    memcpy(NdefMap->SendRecvBuf, &p_data[1], len - 1);
    NdefMap->SendLength = len - 1;
    status = phFriNfc_ExtnsTransceive(NdefMap->pTransceiveInfo, NdefMap->Cmd,
                                      NdefMap->SendRecvBuf, NdefMap->SendLength,
                                      NdefMap->SendRecvLength);
  } else if ((p_data[0] == phNfc_eMifareInc) ||
             (p_data[0] == phNfc_eMifareDec)) {
    EXTNS_SetCallBackFlag(false);
    NdefMap->Cmd.MfCmd = (phNfc_eMifareCmdList_t)p_data[0];
    gphNxpExtns_Context.RawWriteCallBack = true;

    memcpy(NdefMap->SendRecvBuf, &p_data[1], len - 1);
    NdefMap->SendLength = len - 1;
    status = phFriNfc_ExtnsTransceive(NdefMap->pTransceiveInfo, NdefMap->Cmd,
                                      NdefMap->SendRecvBuf, NdefMap->SendLength,
                                      NdefMap->SendRecvLength);
  } else if (((p_data[0] == phNfc_eMifareTransfer) ||
              (p_data[0] == phNfc_eMifareRestore)) &&
             (len == 2)) {
    NdefMap->Cmd.MfCmd = (phNfc_eMifareCmdList_t)p_data[0];
    if (p_data[0] == phNfc_eMifareRestore) {
      EXTNS_SetCallBackFlag(false);
      gphNxpExtns_Context.RawWriteCallBack = true;
      memcpy(NdefMap->SendRecvBuf, &p_data[1], len - 1);
      NdefMap->SendLength = len - 1;
    } else {
      memcpy(NdefMap->SendRecvBuf, p_data, len);
      NdefMap->SendLength = len;
    }
    status = phFriNfc_ExtnsTransceive(NdefMap->pTransceiveInfo, NdefMap->Cmd,
                                      NdefMap->SendRecvBuf, NdefMap->SendLength,
                                      NdefMap->SendRecvLength);

  } else {
    NdefMap->Cmd.MfCmd = (phNfc_eMifareCmdList_t)phNfc_eMifareRaw;

    memcpy(NdefMap->SendRecvBuf, p_data, len);
    NdefMap->SendLength = len;
    status = phFriNfc_ExtnsTransceive(NdefMap->pTransceiveInfo, NdefMap->Cmd,
                                      NdefMap->SendRecvBuf, NdefMap->SendLength,
                                      NdefMap->SendRecvLength);
  }
  if (NFCSTATUS_PENDING == status) {
    status = NFCSTATUS_SUCCESS;
  } else {
    LOG(ERROR) << StringPrintf("ERROR: Mfc_Transceive = 0x%x", status);
  }

  return status;
}

/*******************************************************************************
**
** Function         nativeNfcExtns_doTransceive
**
** Description      Sends raw frame to BCM stack.
**
** Returns          NFCSTATUS_PENDING - if successfully initiated
**                  NFCSTATUS_FAILED  - otherwise
**
*******************************************************************************/
static NFCSTATUS nativeNfcExtns_doTransceive(uint8_t* buff, uint16_t buffSz) {
  NFCSTATUS wStatus = NFCSTATUS_PENDING;
  tNFA_STATUS status =
      NFA_SendRawFrame(buff, buffSz, NFA_DM_DEFAULT_PRESENCE_CHECK_START_DELAY);

  if (status != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: fail send; error=%d", __func__, status);
    wStatus = NFCSTATUS_FAILED;
  }

  return wStatus;
}

/*******************************************************************************
**
** Function          phNciNfc_RecvMfResp
**
** Description      This function shall be invoked as part of ReaderMgmt data
**                  exchange sequence handler on receiving response/data from
*NFCC
**
** Returns          NFCSTATUS_SUCCESS - Data Reception is successful
**                  NFCSTATUS_FAILED  - Data Reception failed
**
*******************************************************************************/
static NFCSTATUS phNciNfc_RecvMfResp(phNciNfc_Buff_t* RspBuffInfo,
                                     NFCSTATUS wStatus) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  uint16_t wPldDataSize = 0;
  phNciNfc_ExtnRespId_t RecvdExtnRspId = phNciNfc_e_InvalidRsp;
  if (NULL == RspBuffInfo) {
    status = NFCSTATUS_FAILED;
  } else {
    if ((0 == (RspBuffInfo->wLen)) || (PH_NCINFC_STATUS_OK != wStatus) ||
        (NULL == (RspBuffInfo->pBuff))) {
      status = NFCSTATUS_FAILED;
    } else {
      RecvdExtnRspId = (phNciNfc_ExtnRespId_t)RspBuffInfo->pBuff[0];

      switch (RecvdExtnRspId) {
        case phNciNfc_e_MfXchgDataRsp: {
          NFCSTATUS writeResponse = NFCSTATUS_SUCCESS;
          /* check the status byte */
          if (NFC_GetNCIVersion() == NCI_VERSION_2_0 &&
              (NdefMap->State == PH_FRINFC_NDEFMAP_STATE_WR_TLV ||
               NdefMap->State == PH_FRINFC_NDEFMAP_STATE_WRITE ||
               NdefMap->State == PH_FRINFC_NDEFMAP_STATE_WR_NDEF_LEN ||
               NdefMap->State == PH_FRINFC_NDEFMAP_STATE_INIT)) {
            uint8_t rspAck = RspBuffInfo->pBuff[RspBuffInfo->wLen - 2];
            uint8_t rspAckMask = ((RspBuffInfo->pBuff[RspBuffInfo->wLen - 1]) &
                                  MAX_NUM_VALID_BITS_FOR_ACK);
            NCI_CALCULATE_ACK(rspAck, rspAckMask);
            writeResponse =
                (rspAck == T2T_RSP_ACK) ? NFCSTATUS_SUCCESS : NFC_STATUS_FAILED;
          } else {
            writeResponse = RspBuffInfo->pBuff[RspBuffInfo->wLen - 1];
          }
          if (PH_NCINFC_STATUS_OK == writeResponse) {
            status = NFCSTATUS_SUCCESS;
            uint16_t wRecvDataSz = 0;

            /* DataLen = TotalRecvdLen - (sizeof(RspId) + sizeof(Status)) */
            wPldDataSize = ((RspBuffInfo->wLen) -
                            (PHNCINFC_EXTNID_SIZE + PHNCINFC_EXTNSTATUS_SIZE));
            wRecvDataSz = NCI_MAX_DATA_LEN;

            /* wPldDataSize = wPldDataSize-1; ==> ignoring the last status byte
             * appended with data */
            if ((wPldDataSize) <= wRecvDataSz) {
              /* Extract the data part from pBuff[2] & fill it to be sent to
               * upper layer */
              memcpy(NdefMap->SendRecvBuf, &(RspBuffInfo->pBuff[1]),
                     (wPldDataSize));
              /* update the number of bytes received from lower layer,excluding
               * the status byte */
              *(NdefMap->SendRecvLength) = wPldDataSize;
            } else {
              // TODO:- Map some status for remaining extra data received to be
              // sent back to caller??
              status = NFCSTATUS_FAILED;
            }
          } else {
            status = NFCSTATUS_FAILED;
          }
        } break;

        case phNciNfc_e_MfcAuthRsp: {
          /* check the status byte */
          if (PH_NCINFC_STATUS_OK == RspBuffInfo->pBuff[1]) {
            if (gAuthCmdBuf.auth_sent == true) {
              MfcPresenceCheckResult(NFCSTATUS_SUCCESS);
              return NFCSTATUS_SUCCESS;
            }
            gAuthCmdBuf.auth_status = true;
            status = NFCSTATUS_SUCCESS;
            if ((PHNCINFC_EXTNID_SIZE + PHNCINFC_EXTNSTATUS_SIZE) >
                RspBuffInfo->wLen) {
              android_errorWriteLog(0x534e4554, "126204073");
              return NFCSTATUS_FAILED;
            }
            /* DataLen = TotalRecvdLen - (sizeof(RspId) + sizeof(Status)) */
            wPldDataSize = ((RspBuffInfo->wLen) -
                            (PHNCINFC_EXTNID_SIZE + PHNCINFC_EXTNSTATUS_SIZE));

            /* Extract the data part from pBuff[2] & fill it to be sent to upper
             * layer */
            memcpy(NdefMap->SendRecvBuf, &(RspBuffInfo->pBuff[2]),
                   wPldDataSize);
            /* update the number of bytes received from lower layer,excluding
             * the status byte */
            *(NdefMap->SendRecvLength) = wPldDataSize;
          } else {
            if (gAuthCmdBuf.auth_sent == true) {
              gAuthCmdBuf.auth_status = false;
              MfcPresenceCheckResult(NFCSTATUS_FAILED);
              return NFCSTATUS_SUCCESS;
            } else {
              /* Reset the stored auth command buffer */
              memset(gAuthCmdBuf.pauth_cmd->buffer, 0, NCI_MAX_DATA_LEN);
              gAuthCmdBuf.pauth_cmd->length = 0;
              gAuthCmdBuf.auth_status = false;
            }
            status = NFCSTATUS_FAILED;
          }
        } break;

        default: { status = NFCSTATUS_FAILED; } break;
      }
    }
  }

  return status;
}

/*******************************************************************************
**
** Function         phLibNfc_SendWrt16CmdPayload
**
** Description      This function map the raw write cmd
**
** Returns          NFCSTATUS_SUCCESS            - Command framing done
**                  NFCSTATUS_INVALID_PARAMETER  - Otherwise
**
*******************************************************************************/
static NFCSTATUS phLibNfc_SendWrt16CmdPayload(
    phNfc_sTransceiveInfo_t* pTransceiveInfo,
    pphNciNfc_TransceiveInfo_t pMappedTranscvIf) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;

  if ((NULL != pTransceiveInfo->sSendData.buffer) &&
      (0 != (pTransceiveInfo->sSendData.length))) {
    memcpy(pMappedTranscvIf->tSendData.pBuff, pTransceiveInfo->sSendData.buffer,
           (pTransceiveInfo->sSendData.length));
    pMappedTranscvIf->tSendData.wLen = pTransceiveInfo->sSendData.length;
    pMappedTranscvIf->uCmd.T2TCmd = phNciNfc_eT2TRaw;
  } else {
    wStatus = NFCSTATUS_INVALID_PARAMETER;
  }

  if (gphNxpExtns_Context.RawWriteCallBack == true) {
    EXTNS_SetCallBackFlag(true);
    gphNxpExtns_Context.RawWriteCallBack = false;
  }

  return wStatus;
}

/*******************************************************************************
**
** Function         phLibNfc_SendIncDecCmdPayload
**
** Description      This function prepares the Increment/Decrement Value to be
**                  sent. This is called after sending the Increment/Decrement
**                  command is already sent and successfull
**
** Returns          NFCSTATUS_SUCCESS            - Payload framing done
**                  NFCSTATUS_INVALID_PARAMETER  - Otherwise
**
*******************************************************************************/
static NFCSTATUS phLibNfc_SendIncDecCmdPayload(
    phNfc_sTransceiveInfo_t* pTransceiveInfo,
    pphNciNfc_TransceiveInfo_t pMappedTranscvIf) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;

  if ((NULL != pTransceiveInfo->sSendData.buffer) &&
      (0 != (pTransceiveInfo->sSendData.length))) {
    memcpy(pMappedTranscvIf->tSendData.pBuff, pTransceiveInfo->sSendData.buffer,
           (pTransceiveInfo->sSendData.length));
    pMappedTranscvIf->tSendData.wLen = pTransceiveInfo->sSendData.length;
    pMappedTranscvIf->uCmd.T2TCmd = phNciNfc_eT2TRaw;
  } else {
    wStatus = NFCSTATUS_INVALID_PARAMETER;
  }

  if (gphNxpExtns_Context.RawWriteCallBack == true) {
    EXTNS_SetCallBackFlag(true);
    gphNxpExtns_Context.RawWriteCallBack = false;
  }

  return wStatus;
}

/*******************************************************************************
**
** Function         Mfc_RecvPacket
**
** Description      Decodes Mifare Classic Tag Response
**                  This is called from NFA_SendRaw Callback
**
** Returns:
**                  NFCSTATUS_SUCCESS - if successfully initiated
**                  NFCSTATUS_FAILED  - otherwise
**
*******************************************************************************/
NFCSTATUS Mfc_RecvPacket(uint8_t* buff, uint8_t buffSz) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  phNciNfc_Buff_t RspBuff;
  uint8_t* pcmd_buff;
  uint16_t buffSize;

  RspBuff.pBuff = buff;
  RspBuff.wLen = buffSz;
  status = phNciNfc_RecvMfResp(&RspBuff, status);
  if (true == gAuthCmdBuf.auth_sent) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s Mfc Check Presence in progress", __func__);
    gAuthCmdBuf.auth_sent = false;
    return status;
  }
  if (true == gphNxpExtns_Context.writecmdFlag &&
      (NFCSTATUS_SUCCESS == status)) {
    pcmd_buff = (uint8_t*)malloc((uint32_t)MAX_BUFF_SIZE);
    if (NULL == pcmd_buff) {
      return NFCSTATUS_FAILED;
    }
    buffSize = MAX_BUFF_SIZE;
    gphNxpExtns_Context.writecmdFlag = false;
    phLibNfc_SendWrt16CmdPayload(NdefMap->pTransceiveInfo, &tNciTranscvInfo);
    status = phNciNfc_SendMfReq(tNciTranscvInfo, pcmd_buff, &buffSize);
    if (NFCSTATUS_PENDING != status) {
      LOG(ERROR) << StringPrintf("ERROR : Mfc_RecvPacket: 0x%x", status);
    } else {
      status = NFCSTATUS_SUCCESS;
    }
    if (pcmd_buff != NULL) {
      free(pcmd_buff);
      pcmd_buff = NULL;
    }
  } else if (true == gphNxpExtns_Context.incrdecflag &&
             (NFCSTATUS_SUCCESS == status)) {
    pcmd_buff = (uint8_t*)malloc((uint32_t)MAX_BUFF_SIZE);
    if (NULL == pcmd_buff) {
      return NFCSTATUS_FAILED;
    }
    buffSize = MAX_BUFF_SIZE;
    gphNxpExtns_Context.incrdecflag = false;
    phLibNfc_SendIncDecCmdPayload(NdefMap->pTransceiveInfo, &tNciTranscvInfo);
    status = phNciNfc_SendMfReq(tNciTranscvInfo, pcmd_buff, &buffSize);
    if (NFCSTATUS_PENDING != status) {
      LOG(ERROR) << StringPrintf("ERROR : Mfc_RecvPacket: 0x%x", status);
    } else {
      status = NFCSTATUS_SUCCESS;
    }
    gphNxpExtns_Context.incrdecstatusflag = true;
    if (pcmd_buff != NULL) {
      free(pcmd_buff);
      pcmd_buff = NULL;
    }

  } else {
    if (gphNxpExtns_Context.CallBackMifare != NULL) {
      if ((gphNxpExtns_Context.incrdecstatusflag == true) && status == 0xB2) {
        gphNxpExtns_Context.incrdecstatusflag = false;
        status = NFCSTATUS_SUCCESS;
      }
      gphNxpExtns_Context.CallBackMifare(gphNxpExtns_Context.CallBackCtxt,
                                         status);
    }
  }

  return status;
}

/*******************************************************************************
**
** Function         phNciNfc_MfCreateXchgDataHdr
**
** Description      This function builds the payload header for mifare XchgData
**                  request to be sent to NFCC.
**
** Returns          NFCSTATUS_PENDING            - Command framing done
**                  NFCSTATUS_FAILED             - Otherwise
**
*******************************************************************************/
static NFCSTATUS phNciNfc_MfCreateXchgDataHdr(
    phNciNfc_TransceiveInfo_t tTranscvInfo, uint8_t* buff, uint16_t* buffSz)

{
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  uint8_t i = 0;

  buff[i++] = phNciNfc_e_MfRawDataXchgHdr;
  memcpy(&buff[i], tTranscvInfo.tSendData.pBuff, tTranscvInfo.tSendData.wLen);
  *buffSz = i + tTranscvInfo.tSendData.wLen;

  status = nativeNfcExtns_doTransceive(buff, (uint16_t)*buffSz);

  return status;
}

/*******************************************************************************
**
** Function         phNciNfc_MfCreateAuthCmdHdr
**
** Description      This function builds the payload header for mifare
**                  classic Authenticate command to be sent to NFCC.
**
** Returns          NFCSTATUS_PENDING            - Command framing done
**                  NFCSTATUS_FAILED             - Otherwise
**
*******************************************************************************/
static NFCSTATUS phNciNfc_MfCreateAuthCmdHdr(
    phNciNfc_TransceiveInfo_t tTranscvInfo, uint8_t bBlockAddr, uint8_t* buff,
    uint16_t* buffSz) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  //    pphNciNfc_RemoteDevInformation_t  pActivDev = NULL;
  //    /*commented to eliminate unused variable warning*/
  uint8_t bKey = 0x00;

  /*No need to check range of block address*/
  /*To check for Authenticate A or Authenticate B type command*/
  if (PHNCINFC_AUTHENTICATION_KEYB == tTranscvInfo.tSendData.pBuff[0]) {
    bKey = bKey | PHNCINFC_ENABLE_KEY_B;
  }

  /*TO Do last 4 bits of Key to be set based of firmware implementation*/
  /*this value is hardcoded but based on firmware implementation change this
   * value*/
  bKey = (bKey | PHNCINFC_AUTHENTICATION_KEY);

  bKey |= tTranscvInfo.tSendData.pBuff[2];

  /*For authentication extension no need to copy tSendData buffer of
   * tTranscvInfo */
  tTranscvInfo.tSendData.wLen = 0x00;

  buff[0] = phNciNfc_e_MfcAuthReq;
  buff[1] = bBlockAddr;
  buff[2] = bKey;

  *buffSz = 0x03;
  if (bKey & PH_NCINFC_MIFARECLASSIC_EMBEDDED_KEY) {
    memcpy(&buff[3], &tTranscvInfo.tSendData.pBuff[3], PHLIBNFC_MFC_AUTHKEYLEN);
    *buffSz += PHLIBNFC_MFC_AUTHKEYLEN;
  }
  /* Store the auth command buffer to use further for presence check */
  if (gAuthCmdBuf.pauth_cmd != NULL) {
    memset(gAuthCmdBuf.pauth_cmd->buffer, 0, NCI_MAX_DATA_LEN);
    gAuthCmdBuf.pauth_cmd->length = *buffSz;
    memcpy(gAuthCmdBuf.pauth_cmd->buffer, buff, *buffSz);
  }
  status = nativeNfcExtns_doTransceive(buff, (uint16_t)*buffSz);

  return status;
}

/*******************************************************************************
**
** Function         phNciNfc_SendMfReq
**
** Description      This function shall be invoked as part of ReaderMgmt data
**                  exchange sequence handler.
**                  It shall send the request packet to NFCC.
**
** Returns          NFCSTATUS_PENDING  - Send request is Pending
**                  NFCSTATUS_FAILED   - otherwise
**
*******************************************************************************/
static NFCSTATUS phNciNfc_SendMfReq(phNciNfc_TransceiveInfo_t tTranscvInfo,
                                    uint8_t* buff, uint16_t* buffSz) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  switch (tTranscvInfo.uCmd.T2TCmd) {
    case phNciNfc_eT2TRaw: {
      status = phNciNfc_MfCreateXchgDataHdr(tTranscvInfo, buff, buffSz);
    } break;
    case phNciNfc_eT2TAuth: {
      status = phNciNfc_MfCreateAuthCmdHdr(tTranscvInfo, (tTranscvInfo.bAddr),
                                           buff, buffSz);
    } break;
    default: {
      status = NFCSTATUS_FAILED;
      break;
    }
  }

  return status;
}

/*******************************************************************************
**
** Function         phLibNfc_CalSectorAddress
**
** Description      This function update the sector address for Mifare classic
**
** Returns          none
**
*******************************************************************************/
static void phLibNfc_CalSectorAddress(uint8_t* Sector_Address) {
  uint8_t BlockNumber = 0x00;

  if (NULL != Sector_Address) {
    BlockNumber = *Sector_Address;
    if (BlockNumber >= PHLIBNFC_MIFARESTD4K_BLK128) {
      *Sector_Address = (uint8_t)(PHLIBNFC_MIFARESTD_SECTOR_NO32 +
                                  ((BlockNumber - PHLIBNFC_MIFARESTD4K_BLK128) /
                                   PHLIBNFC_MIFARESTD_BLOCK_BYTES));
    } else {
      *Sector_Address = BlockNumber / PHLIBNFC_NO_OF_BLKPERSECTOR;
    }
  } else {
  }

  return;
}

/*******************************************************************************
**
** Function         phLibNfc_GetKeyNumberMFC
**
** Description      This function find key number based on authentication
*command
**
** Returns          NFCSTATUS_SUCCESS  - If found the key number
**                  NFCSTATUS_FAILED   - otherwise
**
*******************************************************************************/
static NFCSTATUS phLibNfc_GetKeyNumberMFC(uint8_t* buffer, uint8_t* bKey) {
  int32_t sdwStat = 0X00;
  NFCSTATUS wStatus = NFCSTATUS_INVALID_PARAMETER;

  uint8_t bIndex = 0x00;
  uint8_t bNoOfKeys = 0x00;

#if PHLIBNFC_NXPETENSION_CONFIGURE_MFKEYS
  uint8_t aMfc_keys[NXP_NUMBER_OF_MFC_KEYS][NXP_MFC_KEY_SIZE] = NXP_MFC_KEYS;
#else
  uint8_t aMfc_keys[1][1] = {{0x00}};
#endif

  if (NULL != bKey && NULL != buffer) {
    bNoOfKeys = sizeof(aMfc_keys) / NXP_MFC_KEY_SIZE;
    /* Traverse through the keys stored to determine whether keys is preloaded
     * key */
    for (bIndex = 0; bIndex < bNoOfKeys; bIndex++) {
      /* Check passed key is NDEF key */
      sdwStat = memcmp(&buffer[PHLIBNFC_MFCUIDLEN_INAUTHCMD], aMfc_keys[bIndex],
                       PHLIBNFC_MFC_AUTHKEYLEN);
      if (!sdwStat) {
        LOG(ERROR) << StringPrintf(
            "Mifare : phLibNfc_GetKeyNumberMFC Key found");
        *bKey = bIndex;
        wStatus = NFCSTATUS_SUCCESS;
        break;
      }
    }
    LOG(ERROR) << StringPrintf(
        "Mifare : phLibNfc_GetKeyNumberMFC returning = 0x%x Key = 0x%x",
        wStatus, *bKey);
  } else {
    wStatus = NFCSTATUS_FAILED;
    LOG(ERROR) << StringPrintf(
        "Mifare : phLibNfc_GetKeyNumberMFC returning = 0x%x", wStatus);
  }

  return wStatus;
}

/*******************************************************************************
**
** Function         phLibNfc_ChkAuthCmdMFC
**
** Description      This function Check Authentication command send is proper or
*not
**
** Returns          NFCSTATUS_SUCCESS  - Authenticate command is proper
**                  NFCSTATUS_FAILED   - otherwise
**
*******************************************************************************/
static NFCSTATUS phLibNfc_ChkAuthCmdMFC(
    phNfc_sTransceiveInfo_t* pTransceiveInfo, uint8_t* bKey) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;

  if (NULL != pTransceiveInfo && NULL != pTransceiveInfo->sSendData.buffer &&
      0 != pTransceiveInfo->sSendData.length && NULL != bKey) {
    if ((pTransceiveInfo->cmd.MfCmd == phNfc_eMifareAuthentA ||
         pTransceiveInfo->cmd.MfCmd == phNfc_eMifareAuthentB)) {
      wStatus =
          phLibNfc_GetKeyNumberMFC(pTransceiveInfo->sSendData.buffer, bKey);
    } else {
      wStatus = NFCSTATUS_FAILED;
    }
  } else {
    wStatus = NFCSTATUS_FAILED;
  }
  return wStatus;
}

/*******************************************************************************
**
** Function         phLibNfc_MifareMap
**
** Description      Mifare Mapping Utility function
**
** Returns          NFCSTATUS_SUCCESS             - Mapping is proper
**                  NFCSTATUS_INVALID_PARAMETER   - otherwise
**
*******************************************************************************/
static NFCSTATUS phLibNfc_MifareMap(
    phNfc_sTransceiveInfo_t* pTransceiveInfo,
    pphNciNfc_TransceiveInfo_t pMappedTranscvIf) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  uint8_t bBuffIdx = 0;
  uint8_t bSectorNumber;
  uint8_t bKey = 0;

  switch (pTransceiveInfo->cmd.MfCmd) {
    case phNfc_eMifareRead16: {
      if ((NULL != pTransceiveInfo->sRecvData.buffer) &&
          (0 != (pTransceiveInfo->sRecvData.length))) {
        pMappedTranscvIf->tSendData.pBuff[bBuffIdx++] = phNfc_eMifareRead16;
        pMappedTranscvIf->tSendData.pBuff[bBuffIdx++] = pTransceiveInfo->addr;
        pMappedTranscvIf->tSendData.wLen = bBuffIdx;
        pMappedTranscvIf->uCmd.T2TCmd = phNciNfc_eT2TRaw;
      } else {
        status = NFCSTATUS_INVALID_PARAMETER;
      }
    } break;

    case phNfc_eMifareWrite16: {
      if ((NULL != pTransceiveInfo->sSendData.buffer) &&
          (0 != (pTransceiveInfo->sSendData.length))) {
        pMappedTranscvIf->tSendData.pBuff[bBuffIdx++] = phNfc_eMifareWrite16;
        pMappedTranscvIf->tSendData.pBuff[bBuffIdx++] = pTransceiveInfo->addr;
        memcpy(&(pMappedTranscvIf->tSendData.pBuff[bBuffIdx]),
               pTransceiveInfo->sSendData.buffer,
               (pTransceiveInfo->sSendData.length));
        pMappedTranscvIf->tSendData.wLen =
            bBuffIdx + pTransceiveInfo->sSendData.length;
        pMappedTranscvIf->uCmd.T2TCmd = phNciNfc_eT2TRaw;
      } else {
        status = NFCSTATUS_INVALID_PARAMETER;
      }
    } break;

    case phNfc_eMifareAuthentA:
    case phNfc_eMifareAuthentB: {
      if ((NULL != pTransceiveInfo->sSendData.buffer) &&
          (0 != (pTransceiveInfo->sSendData.length)) &&
          (NULL != pTransceiveInfo->sRecvData.buffer) &&
          (0 != (pTransceiveInfo->sRecvData.length))) {
        status = phLibNfc_ChkAuthCmdMFC(pTransceiveInfo, &bKey);
        if (NFCSTATUS_FAILED != status) {
          bSectorNumber = pTransceiveInfo->addr;
          phLibNfc_CalSectorAddress(&bSectorNumber);
          /*For creating extension command header pTransceiveInfo's MfCmd get
           * used*/
          pMappedTranscvIf->tSendData.pBuff[bBuffIdx++] =
              pTransceiveInfo->cmd.MfCmd;
          pMappedTranscvIf->tSendData.pBuff[bBuffIdx++] = bSectorNumber;
          pMappedTranscvIf->uCmd.T2TCmd = phNciNfc_eT2TAuth;
          pMappedTranscvIf->bAddr = bSectorNumber;
          pMappedTranscvIf->bNumBlock = pTransceiveInfo->NumBlock;
          if (NFCSTATUS_SUCCESS == status) {
            pMappedTranscvIf->tSendData.pBuff[bBuffIdx++] = bKey;
            (pMappedTranscvIf->tSendData.wLen) = (uint16_t)(bBuffIdx);

          } else if (NFCSTATUS_INVALID_PARAMETER == status) {
            bKey = bKey | PH_NCINFC_MIFARECLASSIC_EMBEDDED_KEY;
            pMappedTranscvIf->tSendData.pBuff[bBuffIdx++] = bKey;
            memcpy(&pMappedTranscvIf->tSendData.pBuff[bBuffIdx],
                   &pTransceiveInfo->sSendData
                        .buffer[PHLIBNFC_MFCUIDLEN_INAUTHCMD],
                   PHLIBNFC_MFC_AUTHKEYLEN);

            (pMappedTranscvIf->tSendData.wLen) =
                (uint16_t)(bBuffIdx + PHLIBNFC_MFC_AUTHKEYLEN);
            status = NFCSTATUS_SUCCESS;
          } else {
            /* do nothing */
          }
        }
      } else {
        status = NFCSTATUS_INVALID_PARAMETER;
      }
    } break;

    case phNfc_eMifareRaw: {
    } break;

    default: {
      status = NFCSTATUS_INVALID_PARAMETER;
      break;
    }
  }

  return status;
}

/*******************************************************************************
**
** Function         phLibNfc_MapCmds
**
** Description      This function maps the command request from libnfc level to
*nci level
**
** Returns          NFCSTATUS_SUCCESS           - Mapping of command is
*successful
**                  NFCSTATUS_INVALID_PARAMETER - One or more of the supplied
**                  parameters could not be interpreted properly
**
*******************************************************************************/
static NFCSTATUS phLibNfc_MapCmds(phNciNfc_RFDevType_t RemDevType,
                                  phNfc_sTransceiveInfo_t* pTransceiveInfo,
                                  pphNciNfc_TransceiveInfo_t pMappedTranscvIf) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;

  if ((NULL == pTransceiveInfo) || (NULL == pMappedTranscvIf)) {
    return NFCSTATUS_FAILED;
  }
  switch (RemDevType) {
    case phNciNfc_eMifare1k_PICC:
    case phNciNfc_eMifare4k_PICC: {
      status = phLibNfc_MifareMap(pTransceiveInfo, pMappedTranscvIf);
      break;
    }
    default: { break; }
  }

  return status;
}

/*******************************************************************************
**
** Function         phLibNfc_SendAuthCmd
**
** Description      This function Send authentication command to NFCC
**
** Returns          NFCSTATUS_SUCCESS           - Parameters are proper
**                  NFCSTATUS_INVALID_PARAMETER - Otherwise
**
*******************************************************************************/
static NFCSTATUS phLibNfc_SendAuthCmd(
    phNfc_sTransceiveInfo_t* pTransceiveInfo,
    phNciNfc_TransceiveInfo_t* tNciTranscvInfo) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;

  wStatus = phLibNfc_MapCmds(phNciNfc_eMifare1k_PICC, pTransceiveInfo,
                             tNciTranscvInfo);

  return wStatus;
}

/*******************************************************************************
**
** Function         phLibNfc_SendWrt16Cmd
**
** Description      This function maps Mifarewirte16 commands
**
** Returns          NFCSTATUS_SUCCESS           - Parameters are mapped
**                  NFCSTATUS_INVALID_PARAMETER - Otherwise
**
*******************************************************************************/
static NFCSTATUS phLibNfc_SendWrt16Cmd(
    phNfc_sTransceiveInfo_t* pTransceiveInfo,
    pphNciNfc_TransceiveInfo_t pMappedTranscvIf) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;
  uint8_t bBuffIdx = 0x00;

  if ((NULL != pTransceiveInfo->sSendData.buffer) &&
      (0 != (pTransceiveInfo->sSendData.length))) {
    pMappedTranscvIf->tSendData.pBuff[bBuffIdx++] = phNfc_eMifareWrite16;
    pMappedTranscvIf->tSendData.pBuff[bBuffIdx++] = pTransceiveInfo->addr;
    pMappedTranscvIf->tSendData.wLen = bBuffIdx;
    pMappedTranscvIf->uCmd.T2TCmd = phNciNfc_eT2TRaw;
  } else {
    wStatus = NFCSTATUS_INVALID_PARAMETER;
  }

  return wStatus;
}

/*******************************************************************************
**
** Function         phLibNfc_SendIncDecCmd
**
** Description      This function prepares the Increment/Decrement command
**                  to be sent, increment/decrement value is sent separately
**
** Returns          NFCSTATUS_SUCCESS           - Params are mapped
**                  NFCSTATUS_INVALID_PARAMETER - Otherwise
**
*******************************************************************************/
static NFCSTATUS phLibNfc_SendIncDecCmd(
    phNfc_sTransceiveInfo_t* pTransceiveInfo,
    pphNciNfc_TransceiveInfo_t pMappedTranscvIf, uint8_t IncDecCmd) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;
  uint8_t bBuffIdx = 0x00;

  if ((NULL != pTransceiveInfo->sSendData.buffer) &&
      (0 != (pTransceiveInfo->sSendData.length))) {
    pMappedTranscvIf->tSendData.pBuff[bBuffIdx++] = IncDecCmd;
    pMappedTranscvIf->tSendData.pBuff[bBuffIdx++] = pTransceiveInfo->addr;
    pMappedTranscvIf->tSendData.wLen = bBuffIdx;
    pMappedTranscvIf->uCmd.T2TCmd = phNciNfc_eT2TRaw;
  } else {
    wStatus = NFCSTATUS_INVALID_PARAMETER;
  }

  return wStatus;
}

/*******************************************************************************
**
** Function         phLibNfc_SendRawCmd
**
** Description      This function maps Mifare raw command
**
** Returns          NFCSTATUS_SUCCESS           - Parameters are mapped
**                  NFCSTATUS_INVALID_PARAMETER - Otherwise
**
*******************************************************************************/
static NFCSTATUS phLibNfc_SendRawCmd(
    phNfc_sTransceiveInfo_t* pTransceiveInfo,
    pphNciNfc_TransceiveInfo_t pMappedTranscvIf) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;
  //    uint8_t bBuffIdx = 0x00;                                  /*commented to
  //    eliminate unused variable warning*/

  if ((NULL != pTransceiveInfo->sSendData.buffer) &&
      (0 != (pTransceiveInfo->sSendData.length))) {
    memcpy(pMappedTranscvIf->tSendData.pBuff, pTransceiveInfo->sSendData.buffer,
           (pTransceiveInfo->sSendData.length));
    pMappedTranscvIf->tSendData.wLen = pTransceiveInfo->sSendData.length;
    pMappedTranscvIf->uCmd.T2TCmd = phNciNfc_eT2TRaw;
  } else {
    wStatus = NFCSTATUS_INVALID_PARAMETER;
  }

  return wStatus;
}

/*******************************************************************************
**
** Function         phFriNfc_ExtnsTransceive
**
** Description      This function maps Mifare raw command and send it to NFCC
**
** Returns          NFCSTATUS_PENDING           - Operation successful
**                  NFCSTATUS_INVALID_PARAMETER - Otherwise
**
*******************************************************************************/
NFCSTATUS phFriNfc_ExtnsTransceive(phNfc_sTransceiveInfo_t* pTransceiveInfo,
                                   phNfc_uCmdList_t Cmd, uint8_t* SendRecvBuf,
                                   uint16_t SendLength,
                                   uint16_t* SendRecvLength) {
  (void)SendRecvLength;
  NFCSTATUS status = NFCSTATUS_FAILED;
  uint8_t* buff = NULL;
  uint16_t buffSz = 0;
  uint8_t i = 0;
  uint32_t length = SendLength;
  uint8_t restore_payload[] = {
      0x00, 0x00, 0x00, 0x00,
  };

  if (SendLength == 0) {
    android_errorWriteLog(0x534e4554, "132083376");
    return status;
  }

  buff = (uint8_t*)malloc((uint32_t)MAX_BUFF_SIZE);
  if (NULL == buff) {
    return status;
  }

  pTransceiveInfo->cmd = Cmd;

  if ((Cmd.MfCmd == phNfc_eMifareAuthentA) ||
      (Cmd.MfCmd == phNfc_eMifareAuthentB)) {
    pTransceiveInfo->addr = SendRecvBuf[i++];
    pTransceiveInfo->sSendData.buffer[4] = SendRecvBuf[i++];
    pTransceiveInfo->sSendData.buffer[5] = SendRecvBuf[i++];
    pTransceiveInfo->sSendData.buffer[6] = SendRecvBuf[i++];
    pTransceiveInfo->sSendData.buffer[7] = SendRecvBuf[i++];
    pTransceiveInfo->sSendData.buffer[8] = SendRecvBuf[i++];
    pTransceiveInfo->sSendData.buffer[9] = SendRecvBuf[i++];

    pTransceiveInfo->cmd.MfCmd = Cmd.MfCmd;

    pTransceiveInfo->sSendData.length = length;
    pTransceiveInfo->sRecvData.length = MAX_BUFF_SIZE;
    status = phLibNfc_MifareMap(pTransceiveInfo, &tNciTranscvInfo);
  } else if (Cmd.MfCmd == phNfc_eMifareWrite16) {
    pTransceiveInfo->addr = SendRecvBuf[i++];
    length = SendLength - i;
    memcpy(pTransceiveInfo->sSendData.buffer, &SendRecvBuf[i], length);
    pTransceiveInfo->sSendData.length = length;
    pTransceiveInfo->sRecvData.length = MAX_BUFF_SIZE;

    gphNxpExtns_Context.writecmdFlag = true;

    status = phLibNfc_SendWrt16Cmd(pTransceiveInfo, &tNciTranscvInfo);
  } else if ((Cmd.MfCmd == phNfc_eMifareInc) ||
             (Cmd.MfCmd == phNfc_eMifareDec)) {
    pTransceiveInfo->addr = SendRecvBuf[i++];
    length = SendLength - i;
    memcpy(pTransceiveInfo->sSendData.buffer, &SendRecvBuf[i], length);
    pTransceiveInfo->sSendData.length = length;
    pTransceiveInfo->sRecvData.length = MAX_BUFF_SIZE;

    gphNxpExtns_Context.incrdecflag = true;

    status =
        phLibNfc_SendIncDecCmd(pTransceiveInfo, &tNciTranscvInfo, Cmd.MfCmd);

  } else if (Cmd.MfCmd == phNfc_eMifareRestore) {
    pTransceiveInfo->addr = SendRecvBuf[i++];
    length = SendLength - i;
    memcpy(pTransceiveInfo->sSendData.buffer, &restore_payload[0],
           sizeof(restore_payload));
    pTransceiveInfo->sSendData.length = length + sizeof(restore_payload);
    pTransceiveInfo->sRecvData.length = MAX_BUFF_SIZE;

    gphNxpExtns_Context.incrdecflag = true;

    status =
        phLibNfc_SendIncDecCmd(pTransceiveInfo, &tNciTranscvInfo, Cmd.MfCmd);

  } else if ((Cmd.MfCmd == phNfc_eMifareRaw) ||
             (Cmd.MfCmd == phNfc_eMifareTransfer)) {
    pTransceiveInfo->cmd.MfCmd = (phNfc_eMifareCmdList_t)phNciNfc_eT2TRaw;
    memcpy(pTransceiveInfo->sSendData.buffer, SendRecvBuf, length);
    pTransceiveInfo->sSendData.length = length;
    pTransceiveInfo->sRecvData.length = MAX_BUFF_SIZE;
    status = phLibNfc_SendRawCmd(pTransceiveInfo, &tNciTranscvInfo);
  } else {
    pTransceiveInfo->addr = SendRecvBuf[i++];
    length = SendLength - i;
    memcpy(pTransceiveInfo->sSendData.buffer, &SendRecvBuf[i], length);
    pTransceiveInfo->sSendData.length = length;
    pTransceiveInfo->sRecvData.length = MAX_BUFF_SIZE;
    status = phLibNfc_MifareMap(pTransceiveInfo, &tNciTranscvInfo);
  }

  if (NFCSTATUS_SUCCESS == status) {
    status = phNciNfc_SendMfReq(tNciTranscvInfo, buff, &buffSz);
    if (NFCSTATUS_PENDING != status) {
      LOG(ERROR) << StringPrintf("ERROR : phNciNfc_SendMfReq()");
    }
  } else {
    LOG(ERROR) << StringPrintf(" ERROR : Sending phNciNfc_SendMfReq");
  }
  if (buff != NULL) {
    free(buff);
    buff = NULL;
  }

  return status;
}
