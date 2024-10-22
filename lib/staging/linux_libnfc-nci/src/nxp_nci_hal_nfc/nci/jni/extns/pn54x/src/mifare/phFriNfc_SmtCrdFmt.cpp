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

/*
 * Smart Card Completion Routing component
 */

#include <phFriNfc.h>
#include <phFriNfc_MifStdFormat.h>
#include <phFriNfc_SmtCrdFmt.h>
#include <phNfcCompId.h>
#include <phNfcTypes.h>

/*******************************************************************************
**
** Function         phFriNfc_SmtCrdFmt_HCrHandler
**
** Description      This function is called to complete Completion Routine when
*gets error.
**
** Returns          none.
**
*******************************************************************************/
void phFriNfc_SmtCrdFmt_HCrHandler(phFriNfc_sNdefSmtCrdFmt_t* NdefSmtCrdFmt,
                                   NFCSTATUS Status) {
  /* set the state back to the Reset_Init state*/
  NdefSmtCrdFmt->State = PH_FRINFC_SMTCRDFMT_STATE_RESET_INIT;

  /* set the completion routine*/
  NdefSmtCrdFmt->CompletionRoutine[PH_FRINFC_SMTCRDFMT_CR_FORMAT]
      .CompletionRoutine(NdefSmtCrdFmt->CompletionRoutine->Context, Status);

  return;
}

/*******************************************************************************
**
** Function         phFriNfc_NdefSmtCrd_Reset
**
** Description      Resets the component instance to the initial state and
*initializes the
**                  internal variables.
**                  This function has to be called at the beginning, after
*creating an instance of
**                  phFriNfc_sNdefSmtCrdFmt_t.  Use this function to reset the
*instance and/or to switch
**                  to a different underlying card types.
**
** Returns          NFCSTATUS_SUCCESS if operation successful.
**                  NFCSTATUS_INVALID_PARAMETER if at least one parameter of the
*function is invalid.
**
*******************************************************************************/
NFCSTATUS
phFriNfc_NdefSmtCrd_Reset(phFriNfc_sNdefSmtCrdFmt_t* NdefSmtCrdFmt,
                          void* LowerDevice,
                          phHal_sRemoteDevInformation_t* psRemoteDevInfo,
                          uint8_t* SendRecvBuffer, uint16_t* SendRecvBuffLen) {
  NFCSTATUS result = NFCSTATUS_SUCCESS;
  uint8_t index;
  if ((SendRecvBuffLen == NULL) || (NdefSmtCrdFmt == NULL) ||
      (psRemoteDevInfo == NULL) || (SendRecvBuffer == NULL) ||
      (LowerDevice == NULL) || (*SendRecvBuffLen == 0) ||
      (*SendRecvBuffLen < PH_FRINFC_SMTCRDFMT_MAX_SEND_RECV_BUF_SIZE)) {
    result =
        PHNFCSTVAL(CID_FRI_NFC_NDEF_SMTCRDFMT, NFCSTATUS_INVALID_PARAMETER);
  } else {
    /* Initialize the state to Init */
    NdefSmtCrdFmt->State = PH_FRINFC_SMTCRDFMT_STATE_RESET_INIT;

    for (index = 0; index < PH_FRINFC_SMTCRDFMT_CR; index++) {
      /* Initialize the NdefMap Completion Routine to Null */
      NdefSmtCrdFmt->CompletionRoutine[index].CompletionRoutine = NULL;
      /* Initialize the NdefMap Completion Routine context to Null  */
      NdefSmtCrdFmt->CompletionRoutine[index].Context = NULL;
    }

    /* Lower Device(Always Overlapped HAL Struct initialized in application
     * is registered in NdefMap Lower Device)
     */
    NdefSmtCrdFmt->pTransceiveInfo = (phNfc_sTransceiveInfo*)LowerDevice;

    /* Remote Device info received from Manual Device Discovery is registered
     * here */
    NdefSmtCrdFmt->psRemoteDevInfo = psRemoteDevInfo;

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

    /* Reset Mifare Standard Container elements*/
    phFriNfc_MfStd_Reset(NdefSmtCrdFmt);
  }

  return (result);
}

/*******************************************************************************
**
** Function         phFriNfc_NdefSmtCrd_SetCR
**
** Description      This function allows the caller to set a Completion Routine
*(notifier).
**
** Returns          NFCSTATUS_SUCCESS if operation successful.
**                  NFCSTATUS_INVALID_PARAMETER if at least one parameter of the
*function is invalid.
**
*******************************************************************************/
NFCSTATUS phFriNfc_NdefSmtCrd_SetCR(phFriNfc_sNdefSmtCrdFmt_t* NdefSmtCrdFmt,
                                    uint8_t FunctionID,
                                    pphFriNfc_Cr_t CompletionRoutine,
                                    void* CompletionRoutineContext) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  if ((NdefSmtCrdFmt == NULL) || (FunctionID >= PH_FRINFC_SMTCRDFMT_CR) ||
      (CompletionRoutine == NULL) || (CompletionRoutineContext == NULL)) {
    status =
        PHNFCSTVAL(CID_FRI_NFC_NDEF_SMTCRDFMT, NFCSTATUS_INVALID_PARAMETER);
  } else {
    /* Register the application callback with the NdefMap Completion Routine */
    NdefSmtCrdFmt->CompletionRoutine[FunctionID].CompletionRoutine =
        CompletionRoutine;

    /* Register the application context with the NdefMap Completion Routine
     * context */
    NdefSmtCrdFmt->CompletionRoutine[FunctionID].Context =
        CompletionRoutineContext;
  }
  return status;
}

/*******************************************************************************
**
** Function         phFriNfc_NdefSmtCrd_Process
**
** Description      This function is called by the lower layer (OVR HAL)
**                  when an I/O operation has finished. The internal state
*machine decides
**                  whether to call into the lower device again or to complete
*the process
**                  by calling into the upper layer's completion routine, stored
*within this
**                  component's context (phFriNfc_sNdefSmtCrdFmt_t).
**
** Returns          none.
**
*******************************************************************************/
void phFriNfc_NdefSmtCrd_Process(void* Context, NFCSTATUS Status) {
  if (Context != NULL) {
    phFriNfc_sNdefSmtCrdFmt_t* NdefSmtCrdFmt =
        (phFriNfc_sNdefSmtCrdFmt_t*)Context;

    switch (NdefSmtCrdFmt->psRemoteDevInfo->RemDevType) {
      case phNfc_eMifare_PICC:
      case phNfc_eISO14443_3A_PICC:
        if ((NdefSmtCrdFmt->CardType == PH_FRINFC_SMTCRDFMT_MFSTD_1K_CRD) ||
            (NdefSmtCrdFmt->CardType == PH_FRINFC_SMTCRDFMT_MFSTD_4K_CRD) ||
            (NdefSmtCrdFmt->CardType == PH_FRINFC_SMTCRDFMT_MFSTD_2K_CRD)) {
          /* Remote device is Mifare Standard card */
          phFriNfc_MfStd_Process(NdefSmtCrdFmt, Status);

        } else {
          Status = PHNFCSTVAL(CID_FRI_NFC_NDEF_SMTCRDFMT,
                              NFCSTATUS_INVALID_REMOTE_DEVICE);
        }
        break;
      default:
        /* Remote device opmode not recognized.
         * Probably not NDEF compliant
         */
        Status = PHNFCSTVAL(CID_FRI_NFC_NDEF_SMTCRDFMT,
                            NFCSTATUS_INVALID_REMOTE_DEVICE);
        /* set the state back to the Reset_Init state*/
        NdefSmtCrdFmt->State = PH_FRINFC_SMTCRDFMT_STATE_RESET_INIT;

        /* set the completion routine*/
        NdefSmtCrdFmt->CompletionRoutine[PH_FRINFC_SMTCRDFMT_CR_INVALID_OPE]
            .CompletionRoutine(NdefSmtCrdFmt->CompletionRoutine->Context,
                               Status);
        break;
    }
  } else {
    Status =
        PHNFCSTVAL(CID_FRI_NFC_NDEF_SMTCRDFMT, NFCSTATUS_INVALID_PARAMETER);
    /* The control should not come here. As Context itself is NULL ,
     * Can't call the CR
     */
  }

  return;
}
