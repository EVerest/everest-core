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

#ifndef PHFRINFC_SMTCRDFMT_H
#define PHFRINFC_SMTCRDFMT_H

#include <phNfcTypes_Mapping.h>

/********************* Definitions and structures *****************************/

#define DESFIRE_FMT_EV1

#define PH_FRI_NFC_SMTCRDFMT_NFCSTATUS_FORMAT_ERROR 9 /* Format error */
#define PH_FRINFC_SMTCRDFMT_MSTD_DEFAULT_KEYA_OR_KEYB \
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } /* Default Key */
#define PH_FRINFC_SMTCRDFMT_MSTD_MADSECT_KEYA \
  { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 } /* Key A */
#define PH_FRINFC_SMTCRDFMT_NFCFORUMSECT_KEYA \
  { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 } /* NFC Forum Key */
#define PH_FRINFC_SMTCRDFMT_MSTD_MADSECT_ACCESSBITS \
  { 0x78, 0x77, 0x88 } /* Access bits */
#define PH_FRINFC_SMTCRDFMT_MSTD_NFCFORUM_ACCESSBITS \
  { 0x7F, 0x07, 0x88 }                               /* NFC Forum access bits */
#define PH_FRINFC_SMTCRDFMT_MAX_TLV_TYPE_SUPPORTED 1 /* TLV support */
#define PH_FRINFC_SMTCRDFMT_MAX_SEND_RECV_BUF_SIZE 252 /* Buffer size */
#define PH_FRINFC_SMTCRDFMT_STATE_RESET_INIT 1         /* Format state */

/*
 * Enum definition contains Tag Types
 */
enum {
  PH_FRINFC_SMTCRDFMT_MIFARE_UL_CARD,
  PH_FRINFC_SMTCRDFMT_ISO14443_4A_CARD,
  PH_FRINFC_SMTCRDFMT_MFSTD_1K_CRD,
  PH_FRINFC_SMTCRDFMT_MFSTD_2K_CRD,
  PH_FRINFC_SMTCRDFMT_MFSTD_4K_CRD,
  PH_FRINFC_SMTCRDFMT_TOPAZ_CARD
};

#define PH_FRINFC_SMTCRDFMT_CR_FORMAT 0 /* Index for phFriNfc_SmtCrd_Format */
#define PH_FRINFC_SMTCRDFMT_CR_INVALID_OPE \
  1                              /* Index for Unknown States/Operations */
#define PH_FRINFC_SMTCRDFMT_CR 2 /* Number of completion routines */

/*
 * Mifare Std Additional Information Structure
 */
typedef struct phFriNfc_MfStd_AddInfo {
  uint8_t Default_KeyA_OR_B[6];  /* Stores the Default KeyA and KeyB values */
  uint8_t MADSect_KeyA[6];       /* Key A of MAD sector */
  uint8_t NFCForumSect_KeyA[6];  /* Key A of NFC Forum Sector sector */
  uint8_t MADSect_AccessBits[3]; /* Access Bits of MAD sector */
  uint8_t NFCForumSect_AccessBits[3]; /* Access Bits of NFC Forum sector */
  uint8_t ScrtKeyB[6];    /* Secret key B to given by the application */
  uint8_t AuthState;      /* Status of the different authentication */
  uint16_t CurrentBlock;  /* Stores the current block */
  uint8_t NoOfDevices;    /* Stores the current block */
  uint8_t SectCompl[40];  /* Store the compliant sectors */
  uint8_t WrMADBlkFlag;   /* Flag to know that MAD sector */
  uint8_t MADSectBlk[80]; /* Fill the MAD sector blocks */
  uint8_t UpdMADBlk;      /* Fill the MAD sector blocks */
} phFriNfc_MfStd_AddInfo_t;

/*
 *  Ndef Mifare Std Additional Information Structure
 */
typedef struct phFriNfc_sNdefSmtCrdFmt_AddInfo {
  phFriNfc_MfStd_AddInfo_t
      MfStdInfo; /* Mifare Std Additional Information Structure */

} phFriNfc_sNdefSmtCrdFmt_AddInfo_t;

/*
 *  Context information Structure
 */
typedef struct phFriNfc_sNdefSmtCrdFmt {
  phNfc_sTransceiveInfo_t*
      pTransceiveInfo; /* Pointer to the Transceive information */
  phHal_sRemoteDevInformation_t*
      psRemoteDevInfo; /* Pointer to the Remote Device Information */
  uint8_t CardType;    /* Stores the type of the smart card */
  uint8_t State;       /* The state of the operation */
  uint8_t CardState;   /* Stores the card state */
  phFriNfc_CplRt_t CompletionRoutine
      [PH_FRINFC_SMTCRDFMT_CR];             /* Completion Routine Context */
  phFriNfc_CplRt_t SmtCrdFmtCompletionInfo; /* Holds the completion routine
                                               informations of the Smart Card
                                               Formatting Layer */
  phHal_uCmdList_t Cmd;     /* Holds the Command Type(read/write) */
  uint16_t* SendRecvLength; /* Holds the length of the received data */
  uint8_t* SendRecvBuf;     /* Holds the ack of some intial commands */
  uint16_t SendLength;      /* Holds the length of the data to be sent */
  NFCSTATUS
  FmtProcStatus; /* Stores the output/result of the format procedure */
  phFriNfc_sNdefSmtCrdFmt_AddInfo_t AddInfo; /* Stores Additional Information
                                                needed to format the different
                                                types of tags*/
  uint8_t TLVMsg[PH_FRINFC_SMTCRDFMT_MAX_TLV_TYPE_SUPPORTED]
                [8]; /* Stores NDEF message TLV */
} phFriNfc_sNdefSmtCrdFmt_t;

NFCSTATUS
phFriNfc_NdefSmtCrd_Reset(phFriNfc_sNdefSmtCrdFmt_t* NdefSmtCrdFmt,
                          void* LowerDevice,
                          phHal_sRemoteDevInformation_t* psRemoteDevInfo,
                          uint8_t* SendRecvBuffer, uint16_t* SendRecvBuffLen);

NFCSTATUS phFriNfc_NdefSmtCrd_SetCR(phFriNfc_sNdefSmtCrdFmt_t* NdefSmtCrdFmt,
                                    uint8_t FunctionID,
                                    pphFriNfc_Cr_t CompletionRoutine,
                                    void* CompletionRoutineContext);

void phFriNfc_NdefSmtCrd_Process(void* Context, NFCSTATUS Status);

void phFriNfc_SmtCrdFmt_HCrHandler(phFriNfc_sNdefSmtCrdFmt_t* NdefSmtCrdFmt,
                                   NFCSTATUS Status);

#endif /* PHFRINFC_SMTCRDFMT_H */
