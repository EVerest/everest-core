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
 * Mifare Standard Format implementation
 */

#ifndef PHFRINFC_MIFSTDFORMAT_H
#define PHFRINFC_MIFSTDFORMAT_H

#include <phFriNfc.h>
#include <phFriNfc_SmtCrdFmt.h>
#include <phNfcStatus.h>
#include <phNfcTypes.h>

/********************* Definitions and structures *****************************/

/*
 * Mifare standard -progress states
 */
#define PH_FRINFC_MFSTD_FMT_RESET_INIT 0 /* Reset state */
#define PH_FRINFC_MFSTD_FMT_AUTH_SECT \
  1                                   /* Sector authentication is in progress */
#define PH_FRINFC_MFSTD_FMT_DIS_CON 2 /* Disconnect is in progress */
#define PH_FRINFC_MFSTD_FMT_CON 3     /* Connect is in progress */
#define PH_FRINFC_MFSTD_FMT_POLL 4    /* Poll is in progress */
#define PH_FRINFC_MFSTD_FMT_RD_SECT_TR \
  5 /* Read sector trailer is in progress */
#define PH_FRINFC_MFSTD_FMT_WR_SECT_TR \
  6                                  /* Write sector trailer is in progress */
#define PH_FRINFC_MFSTD_FMT_WR_TLV 7 /* Write sector trailer is in progress */
#define PH_FRINFC_MFSTD_FMT_WR_MAD_BLK 8  /* Write MAD is in progress */
#define PH_FRINFC_MFSTD_FMT_UPD_MAD_BLK 9 /* Write MAD is in progress */

/*
 * Mifare standard -Authenticate states
 */
#define PH_FRINFC_MFSTD_FMT_AUTH_DEF_KEY \
  0 /* Trying to authenticate with the default key */
#define PH_FRINFC_MFSTD_FMT_AUTH_NFC_KEY \
  1 /* Trying to authenticate with the MAD key */
#define PH_FRINFC_MFSTD_FMT_AUTH_MAD_KEY \
  2 /* Trying to authenticate with the NFC forum key */
#define PH_FRINFC_MFSTD_FMT_AUTH_KEYB    \
  3 /* Trying to authenticate with key B \
     */
#define PH_FRINFC_MFSTD_FMT_AUTH_SCRT_KEYB \
  4 /* Trying to authenticate with secret key B */

/*
 * Mifare standard - Update MAD block flag
 */
#define PH_FRINFC_MFSTD_FMT_NOT_A_MAD_BLK 0 /* Not a MAD block */
#define PH_FRINFC_MFSTD_FMT_MAD_BLK_1 1     /* MAD block number 1 */
#define PH_FRINFC_MFSTD_FMT_MAD_BLK_2 2     /* MAD block number 2 */
#define PH_FRINFC_MFSTD_FMT_MAD_BLK_64 \
  64 /* MAD block number 64 (only used for Mifare 4k card) */
#define PH_FRINFC_MFSTD_FMT_MAD_BLK_65 \
  65 /* MAD block number 65 (only used for Mifare 4k card) */
#define PH_FRINFC_MFSTD_FMT_MAD_BLK_66 \
  66 /* MAD block number 66 (only used for Mifare 4k card) */

/*
 * Mifare standard - Update MAD block flag
 */
#define PH_FRINFC_SMTCRDFMT_MSTD_MADSECT_KEYA_ACS_BIT_1K \
  { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0x78, 0x77, 0x88, 0xC1 }
#define PH_FRINFC_SMTCRDFMT_MSTD_MADSECT_KEYA_ACS_BIT_2K \
  { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0x78, 0x77, 0x88, 0xC1 }
#define PH_FRINFC_SMTCRDFMT_MSTD_MADSECT_KEYA_ACS_BIT_4K \
  { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0x78, 0x77, 0x88, 0xC2 }
#define PH_FRINFC_SMTCRDFMT_NFCFORUMSECT_KEYA_ACS_BIT \
  { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7, 0x7F, 0x07, 0x88, 0x40 }

/*
 * Mifare standard - Key and access bit constants
 */
#define PH_FRINFC_MFSTD_FMT_NFC_SECT_KEYA0 0xD3 /* NFC forum sector key A */
#define PH_FRINFC_MFSTD_FMT_NFC_SECT_KEYA1 0xF7 /* NFC forum sector key A */

#define PH_FRINFC_MFSTD_FMT_MAD_SECT_KEYA0 0xA0 /* MAD sector key A */
#define PH_FRINFC_MFSTD_FMT_MAD_SECT_KEYA1 0xA1 /* MAD sector key A */
#define PH_FRINFC_MFSTD_FMT_MAD_SECT_KEYA2 0xA2 /* MAD sector key A */
#define PH_FRINFC_MFSTD_FMT_MAD_SECT_KEYA3 0xA3 /* MAD sector key A */
#define PH_FRINFC_MFSTD_FMT_MAD_SECT_KEYA4 0xA4 /* MAD sector key A */
#define PH_FRINFC_MFSTD_FMT_MAD_SECT_KEYA5 0xA5 /* MAD sector key A */

#define PH_FRINFC_MFSTD_FMT_DEFAULT_KEY 0xFF /* Default key A or B */

#define PH_FRINFC_MFSTD_FMT_MAD_SECT_ACS6 0x78 /* MAD sector access bits 6 */
#define PH_FRINFC_MFSTD_FMT_MAD_SECT_ACS7 0x77 /* MAD sector access bits 7 */
#define PH_FRINFC_MFSTD_FMT_MAD_SECT_ACS8 0x88 /* MAD sector access bits 8 */
#define PH_FRINFC_MFSTD_FMT_MAD_SECT_GPB 0xC1  /* MAD sector GPB */

#define PH_FRINFC_MFSTD_FMT_NFC_SECT_ACS_RW6 \
  0x7F /* NFC forum sector access bits 6 for read write */
#define PH_FRINFC_MFSTD_FMT_NFC_SECT_ACS_RW7 \
  0x07 /* NFC forum sector access bits 7 for read write */
#define PH_FRINFC_MFSTD_FMT_NFC_SECT_ACS_RW8 \
  0x88 /* NFC forum sector access bits 8 for read write */
#define PH_FRINFC_MFSTD_FMT_NFC_SECT_GPB_RW \
  0x40 /* NFC forum sector GPB for read write */

#define PH_FRINFC_MFSTD_FMT_NFC_SECT_ACS_RO6 \
  0x07 /* NFC forum sector access bits 6 for read only */
#define PH_FRINFC_MFSTD_FMT_NFC_SECT_ACS_RO7 \
  0x8F /* NFC forum sector access bits 7 for read only */
#define PH_FRINFC_MFSTD_FMT_NFC_SECT_ACS_RO8 \
  0x0F /* NFC forum sector access bits 8 for read only */
#define PH_FRINFC_MFSTD_FMT_NFC_SECT_GPB_R0 \
  0x43 /* NFC forum sector GPB for read only */

/*
 * Enum definition contains Mifare standard values
 */
typedef enum {
  PH_FRINFC_MFSTD_FMT_VAL_0,
  PH_FRINFC_MFSTD_FMT_VAL_1,
  PH_FRINFC_MFSTD_FMT_VAL_2,
  PH_FRINFC_MFSTD_FMT_VAL_3,
  PH_FRINFC_MFSTD_FMT_VAL_4,
  PH_FRINFC_MFSTD_FMT_VAL_5,
  PH_FRINFC_MFSTD_FMT_VAL_6,
  PH_FRINFC_MFSTD_FMT_VAL_7,
  PH_FRINFC_MFSTD_FMT_VAL_8,
  PH_FRINFC_MFSTD_FMT_VAL_9,
  PH_FRINFC_MFSTD_FMT_VAL_10,
  PH_FRINFC_MFSTD_FMT_VAL_11
} phFriNfc_MfStdVal;

/*
 * Mifare standard - NDEF information constants
 */
#define PH_FRINFC_MFSTD_FMT_NON_NDEF_COMPL                               \
  0                                      /* Sector is not ndef compliant \
                                          */
#define PH_FRINFC_MFSTD_FMT_NDEF_COMPL 1 /* Sector is ndef compliant */
#define PH_FRINFC_MFSTD_FMT_NDEF_INFO1                     \
  0x03 /* If sector is ndef compliant, then one of the MAD \
         sector byte is 0x03 */
#define PH_FRINFC_MFSTD_FMT_NDEF_INFO2                     \
  0xE1 /* If sector is ndef compliant, then one of the MAD \
         sector byte is 0xE1 */

/*
 * Mifare standard - constants
 */
#define PH_FRINFC_MFSTD_FMT_MAX_RECV_LENGTH 252 /* Maximum receive length */
#define PH_FRINFC_MFSTD_FMT_WR_SEND_LENGTH 17   /* Send length for write */
#define PH_FRINFC_MFSTD_FMT_MAX_SECT_IND_1K \
  16 /* Maximum sector index for Mifare 1k = 16 */
#define PH_FRINFC_MFSTD_FMT_MAX_SECT_IND_2K \
  32 /* Maximum sector index for Mifare 2k = 32 */
#define PH_FRINFC_MFSTD_FMT_MAX_SECT_IND_4K \
  40 /* Maximum sector index for Mifare 4k = 40 */
#define PH_FRINFC_MFSTD_FMT_MAX_BLOCKS_1K \
  64 /* Maximum Number of Blocks for Mifare 1k = 64 */
#define PH_FRINFC_MFSTD_FMT_MAX_BLOCKS_2K \
  128 /* Maximum Number of Blocks for Mifare 2k = 128 */
#define PH_FRINFC_MFSTD_FMT_MAX_BLOCKS_4K \
  256 /* Maximum Number of Blocks for Mifare 4k = 256*/

/*
 * Copy default keyA to send buffer
 */
#define PH_FRINFC_MFSTD_FMT_AUTH_SEND_BUF_DEF(mem)                             \
  do {                                                                         \
    memset(&NdefSmtCrdFmt->SendRecvBuf[PH_FRINFC_MFSTD_FMT_VAL_1],             \
           PH_FRINFC_MFSTD_FMT_DEFAULT_KEY, PH_FRINFC_MFSTD_FMT_VAL_6);        \
    NdefSmtCrdFmt->Cmd.MfCmd = ((NdefSmtCrdFmt->AddInfo.MfStdInfo.AuthState == \
                                 PH_FRINFC_MFSTD_FMT_AUTH_DEF_KEY)             \
                                    ? phHal_eMifareAuthentA                    \
                                    : phHal_eMifareAuthentB);                  \
    NdefSmtCrdFmt->SendLength = PH_FRINFC_MFSTD_FMT_VAL_7;                     \
  } while (0)

/*
 * NFC forum sector keyA to send buffer
 */
#define PH_FRINFC_MFSTD_FMT_AUTH_SEND_BUF_NFCSECT_KEYA(mem)        \
  do {                                                             \
    memcpy(&NdefSmtCrdFmt->SendRecvBuf[PH_FRINFC_MFSTD_FMT_VAL_1], \
           NdefSmtCrdFmt->AddInfo.MfStdInfo.NFCForumSect_KeyA,     \
           PH_FRINFC_MFSTD_FMT_VAL_6);                             \
    NdefSmtCrdFmt->Cmd.MfCmd = phHal_eMifareAuthentA;              \
    NdefSmtCrdFmt->SendLength = PH_FRINFC_MFSTD_FMT_VAL_7;         \
  } while (0)

/*
 * Copy MAD sector keyA to send buffer
 */
#define PH_FRINFC_MFSTD_FMT_AUTH_SEND_BUF_MADSECT_KEYA(mem)        \
  do {                                                             \
    memcpy(&NdefSmtCrdFmt->SendRecvBuf[PH_FRINFC_MFSTD_FMT_VAL_1], \
           NdefSmtCrdFmt->AddInfo.MfStdInfo.MADSect_KeyA,          \
           PH_FRINFC_MFSTD_FMT_VAL_6);                             \
    NdefSmtCrdFmt->Cmd.MfCmd = phHal_eMifareAuthentA;              \
    NdefSmtCrdFmt->SendLength = PH_FRINFC_MFSTD_FMT_VAL_7;         \
  } while (0)

/*
 * Copy MAD sector keyB to send buffer
 */
#define PH_FRINFC_MFSTD_FMT_AUTH_SEND_BUF_SCRT_KEY(mem)                  \
  do {                                                                   \
    (void)memcpy(&NdefSmtCrdFmt->SendRecvBuf[PH_FRINFC_MFSTD_FMT_VAL_1], \
                 NdefSmtCrdFmt->AddInfo.MfStdInfo.ScrtKeyB,              \
                 PH_FRINFC_MFSTD_FMT_VAL_6);                             \
    NdefSmtCrdFmt->Cmd.MfCmd = phHal_eMifareAuthentB;                    \
    NdefSmtCrdFmt->SendLength = PH_FRINFC_MFSTD_FMT_VAL_7;               \
  } while (0)

/*
 * Get the next block
 */
#define PH_FRINFC_MFSTD_FMT_CUR_BLK_INC()          \
  NdefSmtCrdFmt->AddInfo.MfStdInfo.CurrentBlock += \
      ((NdefSmtCrdFmt->AddInfo.MfStdInfo.CurrentBlock >= 127) ? 16 : 4)

/*
 * Get the sector index
 */
#define PH_FRINFC_MFSTD_FMT_SECT_INDEX_CALC                                  \
  ((NdefSmtCrdFmt->AddInfo.MfStdInfo.CurrentBlock >= 128)                    \
       ? (32 + ((NdefSmtCrdFmt->AddInfo.MfStdInfo.CurrentBlock - 128) / 16)) \
       : (NdefSmtCrdFmt->AddInfo.MfStdInfo.CurrentBlock / 4))

/*
 * Check the sector block
 */
#define PH_FRINFC_MFSTD_FMT_CUR_BLK_CHK                              \
  (((NdefSmtCrdFmt->CardType == PH_FRINFC_SMTCRDFMT_MFSTD_1K_CRD) && \
    (NdefSmtCrdFmt->AddInfo.MfStdInfo.CurrentBlock >=                \
     PH_FRINFC_MFSTD_FMT_MAX_BLOCKS_1K)) ||                          \
   ((NdefSmtCrdFmt->CardType == PH_FRINFC_SMTCRDFMT_MFSTD_4K_CRD) && \
    (NdefSmtCrdFmt->AddInfo.MfStdInfo.CurrentBlock >=                \
     PH_FRINFC_MFSTD_FMT_MAX_BLOCKS_4K)) ||                          \
   ((NdefSmtCrdFmt->CardType == PH_FRINFC_SMTCRDFMT_MFSTD_2K_CRD) && \
    (NdefSmtCrdFmt->AddInfo.MfStdInfo.CurrentBlock >=                \
     PH_FRINFC_MFSTD_FMT_MAX_BLOCKS_2K)))

/*
 * Get the next authenticate state
 */
#define PH_FRINFC_MFSTD_FMT_NXT_AUTH_STATE()                           \
  do {                                                                 \
    switch (NdefSmtCrdFmt->AddInfo.MfStdInfo.AuthState) {              \
      case PH_FRINFC_MFSTD_FMT_AUTH_DEF_KEY: {                         \
        NdefSmtCrdFmt->AddInfo.MfStdInfo.AuthState = (uint8_t)(        \
            (((NdefSmtCrdFmt->AddInfo.MfStdInfo.CurrentBlock <= 3) ||  \
              ((NdefSmtCrdFmt->AddInfo.MfStdInfo.CurrentBlock > 63) && \
               (NdefSmtCrdFmt->AddInfo.MfStdInfo.CurrentBlock < 67)))) \
                ? PH_FRINFC_MFSTD_FMT_AUTH_MAD_KEY                     \
                : PH_FRINFC_MFSTD_FMT_AUTH_NFC_KEY);                   \
      } break;                                                         \
      case PH_FRINFC_MFSTD_FMT_AUTH_NFC_KEY: {                         \
        NdefSmtCrdFmt->AddInfo.MfStdInfo.AuthState =                   \
            PH_FRINFC_MFSTD_FMT_AUTH_KEYB;                             \
      } break;                                                         \
      case PH_FRINFC_MFSTD_FMT_AUTH_MAD_KEY: {                         \
        NdefSmtCrdFmt->AddInfo.MfStdInfo.AuthState =                   \
            PH_FRINFC_MFSTD_FMT_AUTH_NFC_KEY;                          \
      } break;                                                         \
      case PH_FRINFC_MFSTD_FMT_AUTH_KEYB: {                            \
        NdefSmtCrdFmt->AddInfo.MfStdInfo.AuthState =                   \
            PH_FRINFC_MFSTD_FMT_AUTH_SCRT_KEYB;                        \
      } break;                                                         \
      case PH_FRINFC_MFSTD_FMT_AUTH_SCRT_KEYB:                         \
      default: {                                                       \
        NdefSmtCrdFmt->AddInfo.MfStdInfo.AuthState =                   \
            PH_FRINFC_MFSTD_FMT_AUTH_DEF_KEY;                          \
      } break;                                                         \
    }                                                                  \
  } while (0)

/*
 * Increment the sector index
 */
#define PH_FRINFC_MFSTD_FMT_INCR_SECT                                         \
  do {                                                                        \
    SectIndex++;                                                              \
    SectIndex =                                                               \
        (uint8_t)((SectIndex == 16) ? (SectIndex + PH_FRINFC_MFSTD_FMT_VAL_1) \
                                    : SectIndex);                             \
  } while (0)

/*
 * Increment the sector index
 */
#define PH_FRINFC_MFSTD_FMT_CHK_SECT_ARRAY                                \
  do {                                                                    \
    while ((index < PH_FRINFC_MFSTD_FMT_MAX_SECT_IND_4K) &&               \
           (memcompare != PH_FRINFC_MFSTD_FMT_VAL_0)) {                   \
      /* Compare any one among the sectors is NDEF COMPLIANT */           \
      memcompare = (uint32_t)phFriNfc_MfStd_MemCompare(                   \
          &Buffer[PH_FRINFC_MFSTD_FMT_VAL_0],                             \
          &NdefSmtCrdFmt->AddInfo.MfStdInfo.SectCompl[index],             \
          PH_FRINFC_MFSTD_FMT_VAL_1);                                     \
      /* increment the index */                                           \
      index += (uint8_t)((index == (PH_FRINFC_MFSTD_FMT_MAX_SECT_IND_1K - \
                                    PH_FRINFC_MFSTD_FMT_VAL_1))           \
                             ? PH_FRINFC_MFSTD_FMT_VAL_2                  \
                             : PH_FRINFC_MFSTD_FMT_VAL_1);                \
    }                                                                     \
  } while (0)

/*
 * Complete the sector
 */
#define PH_FRINFC_MFSTD_FMT_CHK_END_OF_CARD()                                  \
  do {                                                                         \
    phFriNfc_MfStd_H_NdefComplSect(                                            \
        NdefSmtCrdFmt->CardType, NdefSmtCrdFmt->AddInfo.MfStdInfo.SectCompl);  \
    PH_FRINFC_MFSTD_FMT_CHK_SECT_ARRAY;                                        \
    if (memcompare == PH_FRINFC_MFSTD_FMT_VAL_0) {                             \
      phFriNfc_MfStd_H_StrNdefData(NdefSmtCrdFmt);                             \
      NdefSmtCrdFmt->AddInfo.MfStdInfo.CurrentBlock =                          \
          PH_FRINFC_MFSTD_FMT_VAL_1;                                           \
      NdefSmtCrdFmt->AddInfo.MfStdInfo.UpdMADBlk =                             \
          PH_FRINFC_MFSTD_FMT_MAD_BLK_1;                                       \
      NdefSmtCrdFmt->AddInfo.MfStdInfo.AuthState =                             \
          PH_FRINFC_MFSTD_FMT_AUTH_SCRT_KEYB;                                  \
      NdefSmtCrdFmt->State = PH_FRINFC_MFSTD_FMT_AUTH_SECT;                    \
      Result = phFriNfc_MfStd_H_WrRdAuth(NdefSmtCrdFmt);                       \
    } else {                                                                   \
      Result = PHNFCSTVAL(CID_FRI_NFC_NDEF_SMTCRDFMT, NFCSTATUS_FORMAT_ERROR); \
    }                                                                          \
  } while (0)

void phFriNfc_MfStd_Reset(phFriNfc_sNdefSmtCrdFmt_t* NdefSmtCrdFmt);
NFCSTATUS phFriNfc_MfStd_Format(phFriNfc_sNdefSmtCrdFmt_t* NdefSmtCrdFmt,
                                const uint8_t* ScrtKeyB);
void phFriNfc_MfStd_Process(void* Context, NFCSTATUS Status);

#endif /* PHFRINFC_MIFSTDFMT_H */
