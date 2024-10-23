/*
 * Copyright (C) 2010-2014,2020 NXP Semiconductors
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
 * Firmware Download Interface File
 */
#ifndef PHDNLDNFC_H
#define PHDNLDNFC_H

#include <phNfcStatus.h>

/*
 *
 * Callback for handling the received data/response from PN54X.
 * Parameters to be passed/registered to download context during respective
 * download function call:
 *      pContext - Upper layer context
 *      wStatus  - Status of the transaction
 *      pInfo    - Contains the Transaction Info
 */
typedef void (*pphDnldNfc_RspCb_t)(void* pContext, NFCSTATUS wStatus,
                                   void* pInfo);

#define PHLIBNFC_FWDNLD_SESSNOPEN (0x01U)   /* download session is Open */
#define PHLIBNFC_FWDNLD_SESSNCLOSED (0x00U) /* download session is Closed */

#define PHDNLDNFC_HWVER_MRA1_0 (0x01U) /* ChipVersion MRA1.0 */
#define PHDNLDNFC_HWVER_MRA1_1 (0x02U) /* ChipVersion MRA1.1 */
#define PHDNLDNFC_HWVER_MRA2_0 (0x03U) /* ChipVersion MRA2.0 */
#define PHDNLDNFC_HWVER_MRA2_1 (0x04U) /* ChipVersion MRA2.1 */
#define PHDNLDNFC_HWVER_MRA2_2 (0x05U) /* ChipVersion MRA2.2 */

/* PN548AD ChipVersion MRA1.0 */
#define PHDNLDNFC_HWVER_PN548AD_MRA1_0 (0x08U)
/* PN551 ChipVersion MRA1.0 */
#define PHDNLDNFC_HWVER_PN551_MRA1_0 (0x08U)
/* PN553-NCI1.0 ChipVersion MRA1.0 */
#define PHDNLDNFC_HWVER_PN553_MRA1_0 (0x0BU)
/* PN553 A0 -> 0x40 hardware version
   PN553 B0 -> 0x41
   PN80T A0 -> 0x50
   PN80T B0 -> 0x51 */
#define PHDNLDNFC_HWVER_PN553_MRA1_0_UPDATED (0x40U)
#define PHDNLDNFC_HWVER_PN557_MRA1_0 (0x01U)
/*
 * Enum definition contains Download Life Cycle States
 */
typedef enum phDnldNfc_LC {
  phDnldNfc_LCCreat = 11, /* Life Cycle Creation*/
  phDnldNfc_LCInit = 13,  /* Life Cycle Initializing */
  phDnldNfc_LCOper = 17,  /* Life Cycle Operational */
  phDnldNfc_LCTerm = 19   /* Life Cycle Termination */
} phDnldNfc_LC_t;

/*
 * Enum definition contains Clk Source Options for Force command request
 */
typedef enum phDnldNfc_ClkSrc {
  phDnldNfc_ClkSrcXtal = 1U, /* Crystal */
  phDnldNfc_ClkSrcPLL = 2U,  /* PLL output */
  phDnldNfc_ClkSrcPad = 3U   /* Directly use clk on CLK_IN Pad */
} phDnldNfc_ClkSrc_t;

/*
 * Enum definition contains Clk Frequency value for Force command request
 */
typedef enum phDnldNfc_ClkFreq {
  phDnldNfc_ClkFreq_13Mhz = 0U,   /* 13Mhz Clk Frequency */
  phDnldNfc_ClkFreq_19_2Mhz = 1U, /* 19.2Mhz Clk Frequency */
  phDnldNfc_ClkFreq_24Mhz = 2U,   /* 24Mhz Clk Frequency */
  phDnldNfc_ClkFreq_26Mhz = 3U,   /* 26Mhz Clk Frequency */
  phDnldNfc_ClkFreq_38_4Mhz = 4U, /* 38.4Mhz Clk Frequency */
  phDnldNfc_ClkFreq_52Mhz = 5U    /* 52Mhz Clk Frequency */
} phDnldNfc_ClkFreq_t;

/*
 * Struct contains buffer where user payload shall be stored
 */
typedef struct phDnldNfc_Buff {
  uint8_t* pBuff; /*pointer to the buffer where user payload shall be stored*/
  uint16_t wLen;  /*Buffer length*/
} phDnldNfc_Buff_t, *pphDnldNfc_Buff_t; /* pointer to #phDnldNfc_Buff_t */

/*
*********************** Function Prototype Declaration *************************
*/

extern NFCSTATUS phDnldNfc_Reset(pphDnldNfc_RspCb_t pNotify, void* pContext);
extern NFCSTATUS phDnldNfc_GetVersion(pphDnldNfc_Buff_t pVersionInfo,
                                      pphDnldNfc_RspCb_t pNotify,
                                      void* pContext);
extern NFCSTATUS phDnldNfc_CheckIntegrity(uint8_t bChipVer,
                                          pphDnldNfc_Buff_t pCRCData,
                                          pphDnldNfc_RspCb_t pNotify,
                                          void* pContext);
extern NFCSTATUS phDnldNfc_GetSessionState(pphDnldNfc_Buff_t pSession,
                                           pphDnldNfc_RspCb_t pNotify,
                                           void* pContext);
extern NFCSTATUS phDnldNfc_Force(pphDnldNfc_Buff_t pInputs,
                                 pphDnldNfc_RspCb_t pNotify, void* pContext);
extern NFCSTATUS phDnldNfc_Read(pphDnldNfc_Buff_t pData, uint32_t dwRdAddr,
                                pphDnldNfc_RspCb_t pNotify, void* pContext);
extern NFCSTATUS phDnldNfc_ReadLog(pphDnldNfc_Buff_t pData,
                                   pphDnldNfc_RspCb_t pNotify, void* pContext);
extern NFCSTATUS phDnldNfc_Write(bool_t bRecoverSeq, pphDnldNfc_Buff_t pData,
                                 pphDnldNfc_RspCb_t pNotify, void* pContext);
extern NFCSTATUS phDnldNfc_Log(pphDnldNfc_Buff_t pData,
                               pphDnldNfc_RspCb_t pNotify, void* pContext);
extern void phDnldNfc_SetHwDevHandle(void);
void phDnldNfc_ReSetHwDevHandle(void);
extern NFCSTATUS phDnldNfc_ReadMem(void* pHwRef, pphDnldNfc_RspCb_t pNotify,
                                   void* pContext);
extern NFCSTATUS phDnldNfc_RawReq(pphDnldNfc_Buff_t pFrameData,
                                  pphDnldNfc_Buff_t pRspData,
                                  pphDnldNfc_RspCb_t pNotify, void* pContext);
extern NFCSTATUS phDnldNfc_InitImgInfo(void);
extern NFCSTATUS phDnldNfc_LoadRecInfo(void);
extern NFCSTATUS phDnldNfc_LoadPKInfo(void);
extern void phDnldNfc_CloseFwLibHandle(void);
extern NFCSTATUS phDnldNfc_LoadFW(uint8_t** pImgInfo, uint16_t* pImgInfoLen);
extern NFCSTATUS phDnldNfc_LoadBinFW(uint8_t** pImgInfo, uint16_t* pImgInfoLen);
extern NFCSTATUS phDnldNfc_LoadRecoveryFW(uint8_t** pImgInfo,
                                          uint16_t* pImgInfoLen);
extern NFCSTATUS phDnldNfc_UnloadFW(void);
#endif /* PHDNLDNFC_H */
