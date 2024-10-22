/*
 * Copyright (C) 2010-2014 NXP Semiconductors
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
 * Internal Download Management routines
 * Download Component
 */

#include <phDnldNfc_Internal.h>
#include <phDnldNfc_Utils.h>
#include <phNxpLog.h>
#include <phNxpNciHal_utils.h>
#include <phTmlNfc.h>

/* Minimum length of payload including 1 byte CmdId */
#define PHDNLDNFC_MIN_PLD_LEN (0x04U)

/* Offset of Length byte within the frame */
#define PHDNLDNFC_FRAME_HDR_OFFSET (0x00)
/* Offset of FrameId within the frame */
#define PHDNLDNFC_FRAMEID_OFFSET (PHDNLDNFC_FRAME_HDR_LEN)
/* Offset of status byte within the frame */
#define PHDNLDNFC_FRAMESTATUS_OFFSET PHDNLDNFC_FRAMEID_OFFSET
/* Offset within frame where payload starts*/
#define PHDNLDNFC_PLD_OFFSET (PHDNLDNFC_MIN_PLD_LEN - 1)

#define PHDNLDNFC_FRAME_RDDATA_OFFSET \
  ((PHDNLDNFC_FRAME_HDR_LEN) +        \
   (PHDNLDNFC_MIN_PLD_LEN)) /* recvd frame offset where data starts */

/* Size of first secure write frame Signature */
#define PHDNLDNFC_FRAME_SIGNATURE_SIZE (0xC0U)
/* Size of first secure write frame payload */
#define PHDNLDNFC_FIRST_FRAME_PLD_SIZE (0xE4U)

/* Status response for first fragmented write frame */
#define PHDNLDNFC_FIRST_FRAGFRAME_RESP (0x2DU)
/* Status response for subsequent fragmented write frame */
#define PHDNLDNFC_NEXT_FRAGFRAME_RESP (0x2EU)

#define PHDNLDNFC_SET_HDR_FRAGBIT(n) \
  ((n) | (1 << 10)) /* Header chunk bit set macro */
#define PHDNLDNFC_CLR_HDR_FRAGBIT(n) \
  ((n) & ~(1U << 10)) /* Header chunk bit clear macro */
#define PHDNLDNFC_CHK_HDR_FRAGBIT(n) \
  ((n)&0x04) /* macro to check if frag bit is set in Hdr */

/* Timeout value to wait for response from NFCC */
#define PHDNLDNFC_RSP_TIMEOUT (2500)
/* Timeout value to wait before resending the last frame */
#define PHDNLDNFC_RETRY_FRAME_WRITE (50)

/* size of EEPROM user data length */
#define PHDNLDNFC_USERDATA_EEPROM_LENSIZE (0x02U)
/* size of EEPROM offset */
#define PHDNLDNFC_USERDATA_EEPROM_OFFSIZE (0x02U)

#define PH_LIBNFC_VEN_RESET_ON_DOWNLOAD_TIMEOUT (1)

/* Function prototype declarations */
static void phDnldNfc_ProcessSeqState(void* pContext,
                                      phTmlNfc_TransactInfo_t* pInfo);
static void phDnldNfc_ProcessRWSeqState(void* pContext,
                                        phTmlNfc_TransactInfo_t* pInfo);
static NFCSTATUS phDnldNfc_ProcessFrame(void* pContext,
                                        phTmlNfc_TransactInfo_t* pInfo);
static NFCSTATUS phDnldNfc_ProcessRecvInfo(void* pContext,
                                           phTmlNfc_TransactInfo_t* pInfo);
static NFCSTATUS phDnldNfc_BuildFramePkt(pphDnldNfc_DlContext_t pDlContext);
static NFCSTATUS phDnldNfc_CreateFramePld(pphDnldNfc_DlContext_t pDlContext);
static NFCSTATUS phDnldNfc_SetupResendTimer(pphDnldNfc_DlContext_t pDlContext);
static NFCSTATUS phDnldNfc_UpdateRsp(pphDnldNfc_DlContext_t pDlContext,
                                     phTmlNfc_TransactInfo_t* pInfo,
                                     uint16_t wPldLen);
static void phDnldNfc_RspTimeOutCb(uint32_t TimerId, void* pContext);
static void phDnldNfc_ResendTimeOutCb(uint32_t TimerId, void* pContext);

/*
*************************** Function Definitions ***************************
*/

/*******************************************************************************
**
** Function         phDnldNfc_CmdHandler
**
** Description      Download Command Handler Mechanism
**                  - holds the sub states for each command processing
**                  - coordinates with TML download thread to complete a
**                    download command request
**                  - calls the user callback on completion of a cmd
**
** Parameters       pContext  - pointer to the download context structure
**                  TrigEvent - event requested by user
**
** Returns          NFC status:
**                  NFCSTATUS_PENDING - download request sent to NFCC
**                                      successfully,response pending
**                  NFCSTATUS_BUSY - handler is busy processing a download
**                                   request
**                  NFCSTATUS_INVALID_PARAMETER - one or more of the supplied
**                                                parameters could not be
**                                                interpreted properly
**                  Other errors
**
*******************************************************************************/
NFCSTATUS phDnldNfc_CmdHandler(void* pContext, phDnldNfc_Event_t TrigEvent) {
  NFCSTATUS status = NFCSTATUS_SUCCESS;
  pphDnldNfc_DlContext_t pDlCtxt = (pphDnldNfc_DlContext_t)pContext;

  if (NULL == pDlCtxt) {
    NXPLOG_FWDNLD_E("Invalid Input Parameter!!");
    status = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_INVALID_PARAMETER);
  } else {
    switch (TrigEvent) {
      case phDnldNfc_EventReset:
      case phDnldNfc_EventGetVer:
      case phDnldNfc_EventIntegChk:
      case phDnldNfc_EventGetSesnSt:
      case phDnldNfc_EventRaw: {
        if (phDnldNfc_EventInvalid == (pDlCtxt->tCurrEvent)) {
          NXPLOG_FWDNLD_D("Processing Normal Sequence..");
          pDlCtxt->tCurrEvent = TrigEvent;
          pDlCtxt->tDnldInProgress = phDnldNfc_TransitionBusy;

          phDnldNfc_ProcessSeqState(pDlCtxt, NULL);

          status = pDlCtxt->wCmdSendStatus;
        } else {
          NXPLOG_FWDNLD_E("Prev Norml Sequence not completed/restored!!");
          status = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_FAILED);
        }
        break;
      }
      case phDnldNfc_EventWrite:
      case phDnldNfc_EventRead:
      case phDnldNfc_EventLog:
      case phDnldNfc_EventForce: {
        if (phDnldNfc_EventInvalid == (pDlCtxt->tCurrEvent)) {
          NXPLOG_FWDNLD_D("Processing R/W Sequence..");
          pDlCtxt->tCurrEvent = TrigEvent;
          pDlCtxt->tDnldInProgress = phDnldNfc_TransitionBusy;

          phDnldNfc_ProcessRWSeqState(pDlCtxt, NULL);

          status = pDlCtxt->wCmdSendStatus;
        } else {
          NXPLOG_FWDNLD_E("Prev R/W Sequence not completed/restored!!");
          status = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_FAILED);
        }
        break;
      }
      default: {
        /* Unknown Event */
        NXPLOG_FWDNLD_E("Unknown Event Parameter!!");
        status = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_INVALID_PARAMETER);
        break;
      }
    }
  }

  return status;
}

/*******************************************************************************
**
** Function         phDnldNfc_ProcessSeqState
**
** Description      Processes all cmd/resp sequences except read & write
**
** Parameters       pContext - pointer to the download context structure
**                  pInfo - pointer to the Transaction buffer updated by TML
**                          Thread
**
** Returns          None
**
*******************************************************************************/
static void phDnldNfc_ProcessSeqState(void* pContext,
                                      phTmlNfc_TransactInfo_t* pInfo) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;
  NFCSTATUS wIntStatus;
  uint32_t TimerId;
  pphDnldNfc_DlContext_t pDlCtxt = (pphDnldNfc_DlContext_t)pContext;

  if (NULL == pDlCtxt) {
    NXPLOG_FWDNLD_E("Invalid Input Parameter!!");
    wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_INVALID_PARAMETER);
  } else {
    switch (pDlCtxt->tCurrState) {
      case phDnldNfc_StateInit: {
        NXPLOG_FWDNLD_D("Initializing Sequence..");
        wStatus = phTmlNfc_UpdateReadCompleteCallback (
            (pphTmlNfc_TransactCompletionCb_t)&phDnldNfc_ProcessSeqState);
        if (NFCSTATUS_SUCCESS != wStatus) {
          NXPLOG_FWDNLD_D(
              "Registering phDnldNfc_ProcessSeqState for readComplete "
              "Failed!!");
        }
        if (0 == (pDlCtxt->TimerInfo.dwRspTimerId)) {
          TimerId = phOsalNfc_Timer_Create();

          if (0 == TimerId) {
            NXPLOG_FWDNLD_W("Response Timer Create failed!!");
            wStatus = NFCSTATUS_INSUFFICIENT_RESOURCES;
            pDlCtxt->wCmdSendStatus = wStatus;
            break;
          } else {
            NXPLOG_FWDNLD_D("Response Timer Created Successfully");
            (pDlCtxt->TimerInfo.dwRspTimerId) = TimerId;
            (pDlCtxt->TimerInfo.TimerStatus) = 0;
            (pDlCtxt->TimerInfo.wTimerExpStatus) = 0;
          }
        }
        pDlCtxt->tCurrState = phDnldNfc_StateSend;
      }
      [[fallthrough]];
      case phDnldNfc_StateSend: {
        wStatus = phDnldNfc_BuildFramePkt(pDlCtxt);

        if (NFCSTATUS_SUCCESS == wStatus) {
          pDlCtxt->tCurrState = phDnldNfc_StateRecv;

          wStatus = phTmlNfc_Write(
              (pDlCtxt->tCmdRspFrameInfo.aFrameBuff),
              (uint16_t)(pDlCtxt->tCmdRspFrameInfo.dwSendlength),
              (pphTmlNfc_TransactCompletionCb_t)&phDnldNfc_ProcessSeqState,
              pDlCtxt);
        }
        pDlCtxt->wCmdSendStatus = wStatus;
        break;
      }
      case phDnldNfc_StateRecv: {
        wStatus = phDnldNfc_ProcessRecvInfo(pContext, pInfo);

        if (NFCSTATUS_SUCCESS == wStatus) {
          wStatus = phOsalNfc_Timer_Start((pDlCtxt->TimerInfo.dwRspTimerId),
                                          PHDNLDNFC_RSP_TIMEOUT,
                                          &phDnldNfc_RspTimeOutCb, pDlCtxt);

          if (NFCSTATUS_SUCCESS == wStatus) {
            NXPLOG_FWDNLD_D("Response timer started");
            pDlCtxt->TimerInfo.TimerStatus = 1;
            pDlCtxt->tCurrState = phDnldNfc_StateTimer;
          } else {
            NXPLOG_FWDNLD_W("Response timer not started");
            pDlCtxt->tCurrState = phDnldNfc_StateResponse;
          }
          /* Call TML_Read function and register the call back function */
          wStatus = phTmlNfc_Read(
              pDlCtxt->tCmdRspFrameInfo.aFrameBuff,
              (uint16_t)PHDNLDNFC_CMDRESP_MAX_BUFF_SIZE,
              (pphTmlNfc_TransactCompletionCb_t)&phDnldNfc_ProcessSeqState,
              (void*)pDlCtxt);

          /* set read status to pDlCtxt->wCmdSendStatus to enable callback */
          pDlCtxt->wCmdSendStatus = wStatus;
          break;
        } else {
          /* Setting TimerExpStatus below to avoid frame processing in response
           * state */
          (pDlCtxt->TimerInfo.wTimerExpStatus) = NFCSTATUS_RF_TIMEOUT;
          pDlCtxt->tCurrState = phDnldNfc_StateResponse;
        }
      }
      [[fallthrough]];
      case phDnldNfc_StateTimer: {
        if (1 == (pDlCtxt->TimerInfo.TimerStatus)) /*Is Timer Running*/
        {
          /*Stop Timer*/
          (void)phOsalNfc_Timer_Stop(pDlCtxt->TimerInfo.dwRspTimerId);
          (pDlCtxt->TimerInfo.TimerStatus) = 0; /*timer stopped*/
        }
        pDlCtxt->tCurrState = phDnldNfc_StateResponse;
      }
      [[fallthrough]];
      case phDnldNfc_StateResponse: {
        if (NFCSTATUS_RF_TIMEOUT != (pDlCtxt->TimerInfo.wTimerExpStatus)) {
          /* Process response */
          wStatus = phDnldNfc_ProcessFrame(pContext, pInfo);
        } else {
          if (phDnldNfc_EventReset != pDlCtxt->tCurrEvent) {
            wStatus = (pDlCtxt->TimerInfo.wTimerExpStatus);
          } else {
            wStatus = NFCSTATUS_SUCCESS;
          }
          (pDlCtxt->TimerInfo.wTimerExpStatus) = 0;
        }

        /* Abort TML read operation which is always kept open */
        wIntStatus = phTmlNfc_ReadAbort();

        if (NFCSTATUS_SUCCESS != wIntStatus) {
          /* TODO:-Action to take in this case:-Tml read abort failed!? */
          NXPLOG_FWDNLD_W("Tml Read Abort failed!!");
        }

        pDlCtxt->tCurrEvent = phDnldNfc_EventInvalid;
        pDlCtxt->tDnldInProgress = phDnldNfc_TransitionIdle;
        pDlCtxt->tCurrState = phDnldNfc_StateInit;

        /* Delete the timer & reset timer primitives in context */
        (void)phOsalNfc_Timer_Delete(pDlCtxt->TimerInfo.dwRspTimerId);
        (pDlCtxt->TimerInfo.dwRspTimerId) = 0;
        (pDlCtxt->TimerInfo.TimerStatus) = 0;
        (pDlCtxt->TimerInfo.wTimerExpStatus) = 0;

        if ((NULL != (pDlCtxt->UserCb)) && (NULL != (pDlCtxt->UserCtxt))) {
          pDlCtxt->UserCb((pDlCtxt->UserCtxt), wStatus,
                          &(pDlCtxt->tRspBuffInfo));
        }
        break;
      }
      default: {
        pDlCtxt->tCurrEvent = phDnldNfc_EventInvalid;
        pDlCtxt->tDnldInProgress = phDnldNfc_TransitionIdle;
        break;
      }
    }
  }

  return;
}

/*******************************************************************************
**
** Function         phDnldNfc_ProcessRWSeqState
**
** Description      Processes read/write cmd/rsp sequence
**
** Parameters       pContext - pointer to the download context structure
**                  pInfo - pointer to the Transaction buffer updated by TML
**                             Thread
**
** Returns          None
**
*******************************************************************************/
static void phDnldNfc_ProcessRWSeqState(void* pContext,
                                        phTmlNfc_TransactInfo_t* pInfo) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;
  NFCSTATUS wIntStatus = wStatus;
  uint32_t TimerId;
  pphDnldNfc_DlContext_t pDlCtxt = (pphDnldNfc_DlContext_t)pContext;

  if (NULL == pDlCtxt) {
    NXPLOG_FWDNLD_E("Invalid Input Parameter!!");
    wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_INVALID_PARAMETER);
  } else {
    switch (pDlCtxt->tCurrState) {
      case phDnldNfc_StateInit: {
        if (0 == (pDlCtxt->TimerInfo.dwRspTimerId)) {
          TimerId = phOsalNfc_Timer_Create();

          if (0 == TimerId) {
            NXPLOG_FWDNLD_E("Response Timer Create failed!!");
            wStatus = NFCSTATUS_INSUFFICIENT_RESOURCES;
          } else {
            NXPLOG_FWDNLD_D("Response Timer Created Successfully");
            (pDlCtxt->TimerInfo.dwRspTimerId) = TimerId;
            (pDlCtxt->TimerInfo.TimerStatus) = 0;
            (pDlCtxt->TimerInfo.wTimerExpStatus) = 0;
          }
        }
        pDlCtxt->tCurrState = phDnldNfc_StateSend;
      }
      [[fallthrough]];
      case phDnldNfc_StateSend: {
        if (pDlCtxt->bResendLastFrame == false) {
          wStatus = phDnldNfc_BuildFramePkt(pDlCtxt);
        } else {
          pDlCtxt->bResendLastFrame = false;
        }

        if (NFCSTATUS_SUCCESS == wStatus) {
          pDlCtxt->tCurrState = phDnldNfc_StateRecv;

          wStatus = phTmlNfc_Write(
              (pDlCtxt->tCmdRspFrameInfo.aFrameBuff),
              (uint16_t)(pDlCtxt->tCmdRspFrameInfo.dwSendlength),
              (pphTmlNfc_TransactCompletionCb_t)&phDnldNfc_ProcessRWSeqState,
              pDlCtxt);
        }
        pDlCtxt->wCmdSendStatus = wStatus;
        break;
      }
      case phDnldNfc_StateRecv: {
        wStatus = phDnldNfc_ProcessRecvInfo(pContext, pInfo);

        if (NFCSTATUS_SUCCESS == wStatus) {
          /* processing For Pipelined write before calling timer below */
          wStatus = phOsalNfc_Timer_Start((pDlCtxt->TimerInfo.dwRspTimerId),
                                          PHDNLDNFC_RSP_TIMEOUT,
                                          &phDnldNfc_RspTimeOutCb, pDlCtxt);

          if (NFCSTATUS_SUCCESS == wStatus) {
            NXPLOG_FWDNLD_D("Response timer started");
            pDlCtxt->TimerInfo.TimerStatus = 1;
            pDlCtxt->tCurrState = phDnldNfc_StateTimer;
          } else {
            NXPLOG_FWDNLD_W("Response timer not started");
            pDlCtxt->tCurrState = phDnldNfc_StateResponse;
            /* Todo:- diagnostic in this case */
          }
          /* Call TML_Read function and register the call back function */
          wStatus = phTmlNfc_Read(
              pDlCtxt->tCmdRspFrameInfo.aFrameBuff,
              (uint16_t)PHDNLDNFC_CMDRESP_MAX_BUFF_SIZE,
              (pphTmlNfc_TransactCompletionCb_t)&phDnldNfc_ProcessRWSeqState,
              (void*)pDlCtxt);

          /* set read status to pDlCtxt->wCmdSendStatus to enable callback */
          pDlCtxt->wCmdSendStatus = wStatus;
          break;
        } else {
          /* Setting TimerExpStatus below to avoid frame processing in reponse
           * state */
          (pDlCtxt->TimerInfo.wTimerExpStatus) = NFCSTATUS_RF_TIMEOUT;
          pDlCtxt->tCurrState = phDnldNfc_StateResponse;
        }
      }
      [[fallthrough]];
      case phDnldNfc_StateTimer: {
        if (1 == (pDlCtxt->TimerInfo.TimerStatus)) /*Is Timer Running*/
        {
          /* Stop Timer */
          (void)phOsalNfc_Timer_Stop(pDlCtxt->TimerInfo.dwRspTimerId);
          (pDlCtxt->TimerInfo.TimerStatus) = 0; /*timer stopped*/
        }
        pDlCtxt->tCurrState = phDnldNfc_StateResponse;
      }
      [[fallthrough]];
      case phDnldNfc_StateResponse: {
        if (NFCSTATUS_RF_TIMEOUT != (pDlCtxt->TimerInfo.wTimerExpStatus)) {
          /* Process response */
          wStatus = phDnldNfc_ProcessFrame(pContext, pInfo);

          if (NFCSTATUS_BUSY == wStatus) {
            /* store the status for use in subsequent processing */
            wIntStatus = wStatus;

            /* setup the resend wait timer */
            wStatus = phDnldNfc_SetupResendTimer(pDlCtxt);

            if (NFCSTATUS_SUCCESS == wStatus) {
              /* restore the last mem_bsy status to avoid re-building frame
               * below */
              wStatus = wIntStatus;
            }
          }
        } else {
          wStatus = (pDlCtxt->TimerInfo.wTimerExpStatus);
          (pDlCtxt->TimerInfo.wTimerExpStatus) = 0;
        }

        if ((0 != (pDlCtxt->tRWInfo.wRemBytes)) &&
            (NFCSTATUS_SUCCESS == wStatus)) {
          /* Abort TML read operation which is always kept open */
          wIntStatus = phTmlNfc_ReadAbort();

          if (NFCSTATUS_SUCCESS != wIntStatus) {
            NXPLOG_FWDNLD_W("Tml read abort failed!");
          }

          wStatus = phDnldNfc_BuildFramePkt(pDlCtxt);

          if (NFCSTATUS_SUCCESS == wStatus) {
            pDlCtxt->tCurrState = phDnldNfc_StateRecv;
            wStatus = phTmlNfc_Write(
                (pDlCtxt->tCmdRspFrameInfo.aFrameBuff),
                (uint16_t)(pDlCtxt->tCmdRspFrameInfo.dwSendlength),
                (pphTmlNfc_TransactCompletionCb_t)&phDnldNfc_ProcessRWSeqState,
                pDlCtxt);

            /* TODO:- Verify here if TML_Write returned NFC_PENDING status &
               take appropriate
                  action otherwise ?? */
          }
        } else if (NFCSTATUS_BUSY == wStatus) {
          /* No processing to be done,since resend wait timer should have
           * already been started */
        } else {
          (pDlCtxt->tRWInfo.bFramesSegmented) = false;
          /* Abort TML read operation which is always kept open */
          wIntStatus = phTmlNfc_ReadAbort();

          if (NFCSTATUS_SUCCESS != wIntStatus) {
            NXPLOG_FWDNLD_W("Tml read abort failed!");
          }

          pDlCtxt->tCurrEvent = phDnldNfc_EventInvalid;
          pDlCtxt->tDnldInProgress = phDnldNfc_TransitionIdle;
          pDlCtxt->tCurrState = phDnldNfc_StateInit;
          pDlCtxt->bResendLastFrame = false;

          /* Delete the timer & reset timer primitives in context */
          (void)phOsalNfc_Timer_Delete(pDlCtxt->TimerInfo.dwRspTimerId);
          (pDlCtxt->TimerInfo.dwRspTimerId) = 0;
          (pDlCtxt->TimerInfo.TimerStatus) = 0;
          (pDlCtxt->TimerInfo.wTimerExpStatus) = 0;

          if ((NULL != (pDlCtxt->UserCb)) && (NULL != (pDlCtxt->UserCtxt))) {
            pDlCtxt->UserCb((pDlCtxt->UserCtxt), wStatus,
                            &(pDlCtxt->tRspBuffInfo));
          }
        }
        break;
      }
      default: {
        pDlCtxt->tCurrEvent = phDnldNfc_EventInvalid;
        pDlCtxt->tDnldInProgress = phDnldNfc_TransitionIdle;
        break;
      }
    }
  }

  return;
}

/*******************************************************************************
**
** Function         phDnldNfc_BuildFramePkt
**
** Description      Forms the frame packet
**
** Parameters       pDlContext - pointer to the download context structure
**
** Returns          NFC status
**
*******************************************************************************/
static NFCSTATUS phDnldNfc_BuildFramePkt(pphDnldNfc_DlContext_t pDlContext) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;
  uint16_t wFrameLen = 0;
  uint16_t wCrcVal;
  uint8_t* pFrameByte;

  if (NULL == pDlContext) {
    NXPLOG_FWDNLD_E("Invalid Input Parameter!!");
    wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_INVALID_PARAMETER);
  } else {
    if (phDnldNfc_FTWrite == (pDlContext->FrameInp.Type)) {
      if ((0 == (pDlContext->tUserData.wLen)) ||
          (NULL == (pDlContext->tUserData.pBuff))) {
        NXPLOG_FWDNLD_E("Invalid Input Parameter(s) for Write!!");
        wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_INVALID_PARAMETER);
      } else {
        if ((pDlContext->tRWInfo.bFirstWrReq) == true) {
          (pDlContext->tRWInfo.wRemBytes) = (pDlContext->tUserData.wLen);
          (pDlContext->tRWInfo.wOffset) = 0;
        }
      }
    } else if (phDnldNfc_FTRead == (pDlContext->FrameInp.Type)) {
      if ((0 == (pDlContext->tRspBuffInfo.wLen)) ||
          (NULL == (pDlContext->tRspBuffInfo.pBuff))) {
        NXPLOG_FWDNLD_E("Invalid Input Parameter(s) for Read!!");
        wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_INVALID_PARAMETER);
      } else {
        if ((pDlContext->tRWInfo.bFramesSegmented) == false) {
          NXPLOG_FWDNLD_D("Verifying RspBuffInfo for Read Request..");
          wFrameLen = (pDlContext->tRspBuffInfo.wLen) + PHDNLDNFC_MIN_PLD_LEN;

          (pDlContext->tRWInfo.wRWPldSize) =
              (PHDNLDNFC_CMDRESP_MAX_PLD_SIZE - PHDNLDNFC_MIN_PLD_LEN);
          (pDlContext->tRWInfo.wRemBytes) = (pDlContext->tRspBuffInfo.wLen);
          (pDlContext->tRWInfo.dwAddr) = (pDlContext->FrameInp.dwAddr);
          (pDlContext->tRWInfo.wOffset) = 0;
          (pDlContext->tRWInfo.wBytesRead) = 0;

          if (PHDNLDNFC_CMDRESP_MAX_PLD_SIZE < wFrameLen) {
            (pDlContext->tRWInfo.bFramesSegmented) = true;
          }
        }
      }
    } else if (phDnldNfc_FTLog == (pDlContext->FrameInp.Type)) {
      if ((0 == (pDlContext->tUserData.wLen)) ||
          (NULL == (pDlContext->tUserData.pBuff))) {
        NXPLOG_FWDNLD_E("Invalid Input Parameter(s) for Log!!");
        wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_INVALID_PARAMETER);
      }
    } else {
    }

    if (NFCSTATUS_SUCCESS == wStatus) {
      wStatus = phDnldNfc_CreateFramePld(pDlContext);
    }

    if (NFCSTATUS_SUCCESS == wStatus) {
      wFrameLen = 0;
      wFrameLen = (pDlContext->tCmdRspFrameInfo.dwSendlength);
      if (wFrameLen > PHDNLDNFC_CMDRESP_MAX_BUFF_SIZE) {
        NXPLOG_FWDNLD_D("wFrameLen exceeds the limit");
        return NFCSTATUS_FAILED;
      }

      if (phDnldNfc_FTRaw != (pDlContext->FrameInp.Type)) {
        if (phDnldNfc_FTWrite != (pDlContext->FrameInp.Type)) {
          pFrameByte = (uint8_t*)&wFrameLen;

          pDlContext->tCmdRspFrameInfo.aFrameBuff[PHDNLDNFC_FRAME_HDR_OFFSET] =
              pFrameByte[1];
          pDlContext->tCmdRspFrameInfo
              .aFrameBuff[PHDNLDNFC_FRAME_HDR_OFFSET + 1] = pFrameByte[0];

          NXPLOG_FWDNLD_D("Inserting FrameId ..");
          pDlContext->tCmdRspFrameInfo.aFrameBuff[PHDNLDNFC_FRAMEID_OFFSET] =
              (pDlContext->tCmdId);

          wFrameLen += PHDNLDNFC_FRAME_HDR_LEN;
        } else {
          if (0 != (pDlContext->tRWInfo.wRWPldSize)) {
            if ((pDlContext->tRWInfo.bFramesSegmented) == true) {
              /* Turning ON the Fragmentation bit in FrameLen */
              wFrameLen = PHDNLDNFC_SET_HDR_FRAGBIT(wFrameLen);
            }

            pFrameByte = (uint8_t*)&wFrameLen;

            pDlContext->tCmdRspFrameInfo
                .aFrameBuff[PHDNLDNFC_FRAME_HDR_OFFSET] = pFrameByte[1];
            pDlContext->tCmdRspFrameInfo
                .aFrameBuff[PHDNLDNFC_FRAME_HDR_OFFSET + 1] = pFrameByte[0];

            /* To ensure we have no frag bit set for crc calculation */
            wFrameLen = PHDNLDNFC_CLR_HDR_FRAGBIT(wFrameLen);

            wFrameLen += PHDNLDNFC_FRAME_HDR_LEN;
          }
        }
        /*Check whether enough space is left for 2 bytes of CRC append*/
        if (wFrameLen > (PHDNLDNFC_CMDRESP_MAX_BUFF_SIZE - 2)) {
          NXPLOG_FWDNLD_D("wFrameLen exceeds the limit");
          return NFCSTATUS_FAILED;
        }
        /* calculate CRC16 */
        wCrcVal = phDnldNfc_CalcCrc16((pDlContext->tCmdRspFrameInfo.aFrameBuff),
                                      wFrameLen);

        pFrameByte = (uint8_t*)&wCrcVal;

        /* Insert the computed Crc value */
        pDlContext->tCmdRspFrameInfo.aFrameBuff[wFrameLen] = pFrameByte[1];
        pDlContext->tCmdRspFrameInfo.aFrameBuff[wFrameLen + 1] = pFrameByte[0];

        wFrameLen += PHDNLDNFC_FRAME_CRC_LEN;
      }

      (pDlContext->tCmdRspFrameInfo.dwSendlength) = wFrameLen;
      NXPLOG_FWDNLD_D("Frame created successfully");
    } else {
      NXPLOG_FWDNLD_E("Frame creation failed!!");
      wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_FAILED);
    }
  }

  return wStatus;
}

/*******************************************************************************
**
** Function         phDnldNfc_CreateFramePld
**
** Description      Forms the frame payload
**
** Parameters       pDlContext - pointer to the download context structure
**
** Returns          NFC status
**
*******************************************************************************/
static NFCSTATUS phDnldNfc_CreateFramePld(pphDnldNfc_DlContext_t pDlContext) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;
  uint16_t wBuffIdx = 0;
  uint16_t wChkIntgVal = 0;
  uint16_t wFrameLen = 0;

  if (NULL == pDlContext) {
    NXPLOG_FWDNLD_E("Invalid Input Parameter!!");
    wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_INVALID_PARAMETER);
  } else {
    memset((pDlContext->tCmdRspFrameInfo.aFrameBuff), 0,
           PHDNLDNFC_CMDRESP_MAX_BUFF_SIZE);
    (pDlContext->tCmdRspFrameInfo.dwSendlength) = 0;

    if (phDnldNfc_FTNone == (pDlContext->FrameInp.Type)) {
      (pDlContext->tCmdRspFrameInfo.dwSendlength) += PHDNLDNFC_MIN_PLD_LEN;
    } else if (phDnldNfc_ChkIntg == (pDlContext->FrameInp.Type)) {
      (pDlContext->tCmdRspFrameInfo.dwSendlength) += PHDNLDNFC_MIN_PLD_LEN;

      wChkIntgVal = nfcFL._PHDNLDNFC_USERDATA_EEPROM_OFFSET;

      memcpy(&(pDlContext->tCmdRspFrameInfo
                   .aFrameBuff[PHDNLDNFC_FRAME_RDDATA_OFFSET]),
             &wChkIntgVal, sizeof(wChkIntgVal));

      wChkIntgVal = nfcFL._PHDNLDNFC_USERDATA_EEPROM_LEN;
      memcpy(&(pDlContext->tCmdRspFrameInfo
                   .aFrameBuff[PHDNLDNFC_FRAME_RDDATA_OFFSET +
                               PHDNLDNFC_USERDATA_EEPROM_OFFSIZE]),
             &wChkIntgVal, sizeof(wChkIntgVal));

      (pDlContext->tCmdRspFrameInfo.dwSendlength) +=
          PHDNLDNFC_USERDATA_EEPROM_LENSIZE;
      (pDlContext->tCmdRspFrameInfo.dwSendlength) +=
          PHDNLDNFC_USERDATA_EEPROM_OFFSIZE;
    } else if (phDnldNfc_FTWrite == (pDlContext->FrameInp.Type)) {
      wBuffIdx = (pDlContext->tRWInfo.wOffset);

      if ((pDlContext->tRWInfo.bFramesSegmented) == false) {
        wFrameLen = (pDlContext->tUserData.pBuff[wBuffIdx]);
        wFrameLen <<= 8;
        wFrameLen |= (pDlContext->tUserData.pBuff[wBuffIdx + 1]);

        (pDlContext->tRWInfo.wRWPldSize) = wFrameLen;
      }

      if ((pDlContext->tRWInfo.wRWPldSize) > PHDNLDNFC_CMDRESP_MAX_PLD_SIZE) {
        if ((pDlContext->tRWInfo.bFirstChunkResp) == false) {
          (pDlContext->tRWInfo.wRemChunkBytes) = wFrameLen;
          (pDlContext->tRWInfo.wOffset) += PHDNLDNFC_FRAME_HDR_LEN;
          wBuffIdx = (pDlContext->tRWInfo.wOffset);
        }

        if (PHDNLDNFC_CMDRESP_MAX_PLD_SIZE <
            (pDlContext->tRWInfo.wRemChunkBytes)) {
          (pDlContext->tRWInfo.wBytesToSendRecv) =
              PHDNLDNFC_CMDRESP_MAX_PLD_SIZE;
          (pDlContext->tRWInfo.bFramesSegmented) = true;
        } else {
          (pDlContext->tRWInfo.wBytesToSendRecv) =
              (pDlContext->tRWInfo.wRemChunkBytes);
          (pDlContext->tRWInfo.bFramesSegmented) = false;
        }

        memcpy(&(pDlContext->tCmdRspFrameInfo
                     .aFrameBuff[PHDNLDNFC_FRAMEID_OFFSET]),
               &(pDlContext->tUserData.pBuff[wBuffIdx]),
               (pDlContext->tRWInfo.wBytesToSendRecv));
      } else {
        (pDlContext->tRWInfo.wRWPldSize) = 0;
        (pDlContext->tRWInfo.wBytesToSendRecv) =
            (wFrameLen + PHDNLDNFC_FRAME_HDR_LEN);

        memcpy(&(pDlContext->tCmdRspFrameInfo.aFrameBuff[0]),
               &(pDlContext->tUserData.pBuff[wBuffIdx]),
               (pDlContext->tRWInfo.wBytesToSendRecv));
      }
      (pDlContext->tCmdRspFrameInfo.dwSendlength) +=
          (pDlContext->tRWInfo.wBytesToSendRecv);
    } else if (phDnldNfc_FTRead == (pDlContext->FrameInp.Type)) {
      (pDlContext->tRWInfo.wBytesToSendRecv) =
          ((pDlContext->tRWInfo.wRemBytes) > (pDlContext->tRWInfo.wRWPldSize))
              ? (pDlContext->tRWInfo.wRWPldSize)
              : (pDlContext->tRWInfo.wRemBytes);

      wBuffIdx = (PHDNLDNFC_PLD_OFFSET +
                  ((sizeof(pDlContext->tRWInfo.wBytesToSendRecv)) %
                   PHDNLDNFC_MIN_PLD_LEN) -
                  1);

      memcpy(&(pDlContext->tCmdRspFrameInfo.aFrameBuff[wBuffIdx]),
             &(pDlContext->tRWInfo.wBytesToSendRecv),
             (sizeof(pDlContext->tRWInfo.wBytesToSendRecv)));

      wBuffIdx += sizeof(pDlContext->tRWInfo.wBytesToSendRecv);

      memcpy(&(pDlContext->tCmdRspFrameInfo.aFrameBuff[wBuffIdx]),
             &(pDlContext->tRWInfo.dwAddr), sizeof(pDlContext->tRWInfo.dwAddr));

      (pDlContext->tCmdRspFrameInfo.dwSendlength) +=
          (PHDNLDNFC_MIN_PLD_LEN + (sizeof(pDlContext->tRWInfo.dwAddr)));
    } else if (phDnldNfc_FTLog == (pDlContext->FrameInp.Type)) {
      (pDlContext->tCmdRspFrameInfo.dwSendlength) += PHDNLDNFC_MIN_PLD_LEN;

      wBuffIdx = (PHDNLDNFC_MIN_PLD_LEN + PHDNLDNFC_FRAME_HDR_LEN);

      memcpy(&(pDlContext->tCmdRspFrameInfo.aFrameBuff[wBuffIdx]),
             (pDlContext->tUserData.pBuff), (pDlContext->tUserData.wLen));

      (pDlContext->tCmdRspFrameInfo.dwSendlength) +=
          (pDlContext->tUserData.wLen);
    } else if (phDnldNfc_FTForce == (pDlContext->FrameInp.Type)) {
      (pDlContext->tCmdRspFrameInfo.dwSendlength) += PHDNLDNFC_MIN_PLD_LEN;

      wBuffIdx = PHDNLDNFC_PLD_OFFSET;

      memcpy(&(pDlContext->tCmdRspFrameInfo.aFrameBuff[wBuffIdx]),
             (pDlContext->tUserData.pBuff), (pDlContext->tUserData.wLen));
    } else if (phDnldNfc_FTRaw == (pDlContext->FrameInp.Type)) {
      if ((0 == (pDlContext->tUserData.wLen)) ||
          (NULL == (pDlContext->tUserData.pBuff))) {
        NXPLOG_FWDNLD_E("Invalid Input Parameter(s) for Raw Request!!");
        wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_INVALID_PARAMETER);
      } else {
        memcpy(&(pDlContext->tCmdRspFrameInfo.aFrameBuff[wBuffIdx]),
               (pDlContext->tUserData.pBuff), (pDlContext->tUserData.wLen));

        (pDlContext->tCmdRspFrameInfo.dwSendlength) +=
            (pDlContext->tUserData.wLen);
      }
    } else {
    }
  }

  return wStatus;
}

/*******************************************************************************
**
** Function         phDnldNfc_ProcessFrame
**
** Description      Processes response frame received
**
** Parameters       pContext - pointer to the download context structure
**                  pInfo - pointer to the Transaction buffer updated by TML
**                          Thread
**
** Returns          NFCSTATUS_SUCCESS - parameters successfully validated
**                  NFCSTATUS_INVALID_PARAMETER - invalid parameters
**
*******************************************************************************/
static NFCSTATUS phDnldNfc_ProcessFrame(void* pContext,
                                        phTmlNfc_TransactInfo_t* pInfo) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;
  uint16_t wCrcVal, wRecvdCrc, wRecvdLen, wPldLen;
  pphDnldNfc_DlContext_t pDlCtxt = (pphDnldNfc_DlContext_t)pContext;

  if ((NULL == pDlCtxt) || (NULL == pInfo)) {
    NXPLOG_FWDNLD_E("Invalid Input Parameters!!");
    wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_INVALID_PARAMETER);
  } else {
    if ((PH_DL_STATUS_OK != pInfo->wStatus) || (0 == pInfo->wLength) ||
        (NULL == pInfo->pBuff)) {
      NXPLOG_FWDNLD_E("Dnld Cmd Request Failed!!");
      wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_FAILED);
    } else {
      if (phDnldNfc_FTRaw == (pDlCtxt->FrameInp.Type)) {
        if ((0 != (pDlCtxt->tRspBuffInfo.wLen)) &&
            (NULL != (pDlCtxt->tRspBuffInfo.pBuff))) {
          memcpy((pDlCtxt->tRspBuffInfo.pBuff), (pInfo->pBuff),
                 (pInfo->wLength));

          (pDlCtxt->tRspBuffInfo.wLen) = (pInfo->wLength);
        } else {
          NXPLOG_FWDNLD_E("Cannot update Response buff with received data!!");
        }
      } else {
        /* calculate CRC16 */
        wCrcVal = phDnldNfc_CalcCrc16(
            (pInfo->pBuff), ((pInfo->wLength) - PHDNLDNFC_FRAME_CRC_LEN));

        wRecvdCrc = 0;
        wRecvdCrc = (((uint16_t)(pInfo->pBuff[(pInfo->wLength) - 2]) << 8U) |
                     (pInfo->pBuff[(pInfo->wLength) - 1]));

        if (wRecvdCrc == wCrcVal) {
          wRecvdLen =
              (((uint16_t)(pInfo->pBuff[PHDNLDNFC_FRAME_HDR_OFFSET]) << 8U) |
               (pInfo->pBuff[PHDNLDNFC_FRAME_HDR_OFFSET + 1]));

          wPldLen = ((pInfo->wLength) -
                     (PHDNLDNFC_FRAME_HDR_LEN + PHDNLDNFC_FRAME_CRC_LEN));

          if (wRecvdLen != wPldLen) {
            NXPLOG_FWDNLD_E("Invalid frame payload length received");
            wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_FAILED);
          } else {
            wStatus = phDnldNfc_UpdateRsp(pDlCtxt, pInfo, (wPldLen - 1));
          }
        } else {
          NXPLOG_FWDNLD_E("Invalid frame received");
          wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_FAILED);
        }
      }
    }
  }

  return wStatus;
}

/*******************************************************************************
**
** Function         phDnldNfc_ProcessRecvInfo
**
** Description      Processes the response during the state phDnldNfc_StateRecv
**
** Parameters       pContext - pointer to the download context structure
**                  pInfo - pointer to the Transaction buffer updated by TML
**                          Thread
**
** Returns          NFCSTATUS_SUCCESS - parameters successfully validated
**                  NFCSTATUS_INVALID_PARAMETER - invalid parameters
**
*******************************************************************************/
static NFCSTATUS phDnldNfc_ProcessRecvInfo(void* pContext,
                                           phTmlNfc_TransactInfo_t* pInfo) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;

  if (NULL != pContext) {
    if (NULL == pInfo) {
      NXPLOG_FWDNLD_E("Invalid pInfo received from TML!!");
      wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_FAILED);
    } else {
      wStatus = PHNFCSTATUS(pInfo->wStatus);

      if (NFCSTATUS_SUCCESS == wStatus) {
        NXPLOG_FWDNLD_D("Send Success");
      } else {
        NXPLOG_FWDNLD_E("Tml Write error!!");
        wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_FAILED);
      }
    }
  } else {
    NXPLOG_FWDNLD_E("Invalid context received from TML!!");
    wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_FAILED);
  }

  return wStatus;
}

/*******************************************************************************
**
** Function         phDnldNfc_SetupResendTimer
**
** Description      Sets up the timer for resending the previous write frame
**
** Parameters       pDlContext - pointer to the download context structure
**
** Returns          NFC status
**
*******************************************************************************/
static NFCSTATUS phDnldNfc_SetupResendTimer(pphDnldNfc_DlContext_t pDlContext) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;

  wStatus = phOsalNfc_Timer_Start((pDlContext->TimerInfo.dwRspTimerId),
                                  PHDNLDNFC_RETRY_FRAME_WRITE,
                                  &phDnldNfc_ResendTimeOutCb, pDlContext);

  if (NFCSTATUS_SUCCESS == wStatus) {
    NXPLOG_FWDNLD_D("Frame Resend wait timer started");
    (pDlContext->TimerInfo.TimerStatus) = 1;
    pDlContext->tCurrState = phDnldNfc_StateTimer;
  } else {
    NXPLOG_FWDNLD_W("Frame Resend wait timer not started");
    (pDlContext->TimerInfo.TimerStatus) = 0; /*timer stopped*/
    pDlContext->tCurrState = phDnldNfc_StateResponse;
    /* Todo:- diagnostic in this case */
  }

  return wStatus;
}

#if !defined(PH_LIBNFC_VEN_RESET_ON_DOWNLOAD_TIMEOUT)
#error PH_LIBNFC_VEN_RESET_ON_DOWNLOAD_TIMEOUT has to be defined
#endif

/*******************************************************************************
**
** Function         phDnldNfc_RspTimeOutCb
**
** Description      Callback function in case of timer expiration
**
** Parameters       TimerId  - expired timer id
**                  pContext - pointer to the download context structure
**
** Returns          None
**
*******************************************************************************/
static void phDnldNfc_RspTimeOutCb(uint32_t TimerId, void* pContext) {
  pphDnldNfc_DlContext_t pDlCtxt = (pphDnldNfc_DlContext_t)pContext;

  if (NULL != pDlCtxt) {
    UNUSED(TimerId);

    if (1 == pDlCtxt->TimerInfo.TimerStatus) {
      /* No response received and the timer expired */
      pDlCtxt->TimerInfo.TimerStatus = 0; /* Reset timer status flag */

      NXPLOG_FWDNLD_D("%x", pDlCtxt->tLastStatus);

#if (PH_LIBNFC_VEN_RESET_ON_DOWNLOAD_TIMEOUT == TRUE)
      if (PH_DL_STATUS_SIGNATURE_ERROR == pDlCtxt->tLastStatus) {
        /* Do a VEN Reset of the chip. */
        NXPLOG_FWDNLD_E("Performing a VEN Reset");
        phTmlNfc_IoCtl(phTmlNfc_e_EnableNormalMode);
        phTmlNfc_IoCtl(phTmlNfc_e_EnableDownloadMode);
        NXPLOG_FWDNLD_E("VEN Reset Done");
      }
#endif

      (pDlCtxt->TimerInfo.wTimerExpStatus) = NFCSTATUS_RF_TIMEOUT;

      if ((phDnldNfc_EventRead == pDlCtxt->tCurrEvent) ||
          (phDnldNfc_EventWrite == pDlCtxt->tCurrEvent)) {
        phDnldNfc_ProcessRWSeqState(pDlCtxt, NULL);
      } else {
        phDnldNfc_ProcessSeqState(pDlCtxt, NULL);
      }
    }
  }

  return;
}

/*******************************************************************************
**
** Function         phDnldNfc_ResendTimeOutCb
**
** Description      Callback function in case of Frame Resend Wait timer
**                  expiration
**
** Parameters       TimerId  - expired timer id
**                  pContext - pointer to the download context structure
**
** Returns          None
**
*******************************************************************************/
static void phDnldNfc_ResendTimeOutCb(uint32_t TimerId, void* pContext) {
  pphDnldNfc_DlContext_t pDlCtxt = (pphDnldNfc_DlContext_t)pContext;

  if (NULL != pDlCtxt) {
    UNUSED(TimerId);

    if (1 == pDlCtxt->TimerInfo.TimerStatus) {
      /* No response received and the timer expired */
      pDlCtxt->TimerInfo.TimerStatus = 0; /* Reset timer status flag */

      (pDlCtxt->TimerInfo.wTimerExpStatus) = 0;

      pDlCtxt->tCurrState = phDnldNfc_StateSend;

      /* set the flag to trigger last frame re-transmission */
      pDlCtxt->bResendLastFrame = true;

      phDnldNfc_ProcessRWSeqState(pDlCtxt, NULL);
    }
  }

  return;
}

/*******************************************************************************
**
** Function         phDnldNfc_UpdateRsp
**
** Description      verifies the payload status byte and copies data
**                  to response buffer if successful
**
** Parameters       pDlContext - pointer to the download context structure
**                  pInfo - pointer to the Transaction buffer updated by TML
**                          Thread
**                  wPldLen - Length of the payload bytes to copy to response
**                            buffer
**
** Returns          NFC status
**
*******************************************************************************/
static NFCSTATUS phDnldNfc_UpdateRsp(pphDnldNfc_DlContext_t pDlContext,
                                     phTmlNfc_TransactInfo_t* pInfo,
                                     uint16_t wPldLen) {
  NFCSTATUS wStatus = NFCSTATUS_SUCCESS;
  uint16_t wReadLen = 0;

  if ((NULL == pDlContext) || (NULL == pInfo)) {
    NXPLOG_FWDNLD_E("Invalid Input Parameters!!");
    wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_INVALID_PARAMETER);
  } else {
    if (PH_DL_CMD_WRITE == (pDlContext->tCmdId)) {
      if (PH_DL_STATUS_OK == (pInfo->pBuff[PHDNLDNFC_FRAMESTATUS_OFFSET])) {
        /* first write frame response received case */
        if ((pDlContext->tRWInfo.bFirstWrReq) == true) {
          NXPLOG_FWDNLD_D("First Write Frame Success Status received!!");
          (pDlContext->tRWInfo.bFirstWrReq) = false;
        }

        if ((pDlContext->tRWInfo.bFirstChunkResp) == true) {
          if ((pDlContext->tRWInfo.bFramesSegmented) == false) {
            NXPLOG_FWDNLD_D("Chunked Write Frame Success Status received!!");
            (pDlContext->tRWInfo.wRemChunkBytes) -=
                (pDlContext->tRWInfo.wBytesToSendRecv);
            (pDlContext->tRWInfo.bFirstChunkResp) = false;
          } else {
            NXPLOG_FWDNLD_E("UnExpected Status received!!");
            wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_FAILED);
          }
        }

        if (NFCSTATUS_SUCCESS == wStatus) {
          (pDlContext->tRWInfo.wRemBytes) -=
              (pDlContext->tRWInfo.wBytesToSendRecv);
          (pDlContext->tRWInfo.wOffset) +=
              (pDlContext->tRWInfo.wBytesToSendRecv);
        }
      } else if (((pDlContext->tRWInfo.bFirstChunkResp) == false) &&
                 ((pDlContext->tRWInfo.bFramesSegmented) == true) &&
                 (PHDNLDNFC_FIRST_FRAGFRAME_RESP ==
                  (pInfo->pBuff[PHDNLDNFC_FRAMESTATUS_OFFSET]))) {
        (pDlContext->tRWInfo.bFirstChunkResp) = true;
        (pDlContext->tRWInfo.wRemChunkBytes) -=
            (pDlContext->tRWInfo.wBytesToSendRecv);
        (pDlContext->tRWInfo.wRemBytes) -=
            ((pDlContext->tRWInfo.wBytesToSendRecv) + PHDNLDNFC_FRAME_HDR_LEN);
        (pDlContext->tRWInfo.wOffset) += (pDlContext->tRWInfo.wBytesToSendRecv);

        /* first write frame response received case */
        if ((pDlContext->tRWInfo.bFirstWrReq) == true) {
          NXPLOG_FWDNLD_D("First Write Frame Success Status received!!");
          (pDlContext->tRWInfo.bFirstWrReq) = false;
        }
      } else if (((pDlContext->tRWInfo.bFirstChunkResp) == true) &&
                 ((pDlContext->tRWInfo.bFramesSegmented) == true) &&
                 (PHDNLDNFC_NEXT_FRAGFRAME_RESP ==
                  (pInfo->pBuff[PHDNLDNFC_FRAMESTATUS_OFFSET]))) {
        (pDlContext->tRWInfo.wRemChunkBytes) -=
            (pDlContext->tRWInfo.wBytesToSendRecv);
        (pDlContext->tRWInfo.wRemBytes) -=
            (pDlContext->tRWInfo.wBytesToSendRecv);
        (pDlContext->tRWInfo.wOffset) += (pDlContext->tRWInfo.wBytesToSendRecv);
      } else if (PH_DL_STATUS_FIRMWARE_VERSION_ERROR ==
                 (pInfo->pBuff[PHDNLDNFC_FRAMESTATUS_OFFSET])) {
        NXPLOG_FWDNLD_E(
            "FW version Error !!!could be either due to FW major version "
            "mismatch or Firmware Already Up To Date !!");
        (pDlContext->tRWInfo.bFirstWrReq) = false;
        /* resetting wRemBytes to 0 to avoid any further write frames send */
        (pDlContext->tRWInfo.wRemBytes) = 0;
        (pDlContext->tRWInfo.wOffset) = 0;
        wStatus = NFCSTATUS_FW_VERSION_ERROR;
      } else if (PH_DL_STATUS_PLL_ERROR ==
                 (pInfo->pBuff[PHDNLDNFC_FRAMESTATUS_OFFSET])) {
        NXPLOG_FWDNLD_E("PLL Error Status received!!");
        (pDlContext->tLastStatus) = PH_DL_STATUS_PLL_ERROR;
        wStatus = NFCSTATUS_WRITE_FAILED;
      } else if (PH_DL_STATUS_SIGNATURE_ERROR ==
                 (pInfo->pBuff[PHDNLDNFC_FRAMESTATUS_OFFSET])) {
        NXPLOG_FWDNLD_E("Signature Mismatch Error received!!");
        /* save the status for use in loading the relevant recovery image
         * (either signature or platform) */
        (pDlContext->tLastStatus) = PH_DL_STATUS_SIGNATURE_ERROR;
        wStatus = NFCSTATUS_REJECTED;
      } else if (PH_DL_STATUS_MEM_BSY ==
                 (pInfo->pBuff[PHDNLDNFC_FRAMESTATUS_OFFSET])) {
        NXPLOG_FWDNLD_E("Mem Busy Status received!!");
        (pDlContext->tLastStatus) = PH_DL_STATUS_MEM_BSY;
        wStatus = NFCSTATUS_BUSY;
      } else {
        NXPLOG_FWDNLD_E("Unsuccessful Status received!!");
        wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_FAILED);
      }
    } else if (PH_DL_CMD_READ == (pDlContext->tCmdId)) {
      if (PH_DL_STATUS_OK == (pInfo->pBuff[PHDNLDNFC_FRAMESTATUS_OFFSET])) {
        wReadLen = (((uint16_t)(pInfo->pBuff[PHDNLDNFC_FRAMESTATUS_OFFSET + 3])
                     << 8U) |
                    (pInfo->pBuff[PHDNLDNFC_FRAMESTATUS_OFFSET + 2]));

        if (wReadLen != (pDlContext->tRWInfo.wBytesToSendRecv)) {
          NXPLOG_FWDNLD_E("Desired Length bytes not received!!");
          wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_FAILED);
        } else {
          memcpy(
              &(pDlContext->tRspBuffInfo.pBuff[(pDlContext->tRWInfo.wOffset)]),
              &(pInfo->pBuff[PHDNLDNFC_FRAME_RDDATA_OFFSET]), wReadLen);

          (pDlContext->tRWInfo.wBytesRead) += wReadLen;

          (pDlContext->tRspBuffInfo.wLen) = (pDlContext->tRWInfo.wBytesRead);

          (pDlContext->tRWInfo.wRemBytes) -=
              (pDlContext->tRWInfo.wBytesToSendRecv);
          (pDlContext->tRWInfo.dwAddr) +=
              (pDlContext->tRWInfo.wBytesToSendRecv);
          (pDlContext->tRWInfo.wOffset) +=
              (pDlContext->tRWInfo.wBytesToSendRecv);
        }
      } else {
        NXPLOG_FWDNLD_E("Unsuccessful Status received!!");
        wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_FAILED);
      }
    } else {
      if (PH_DL_STATUS_OK == (pInfo->pBuff[PHDNLDNFC_FRAMESTATUS_OFFSET])) {
        if ((0 != (pDlContext->tRspBuffInfo.wLen)) &&
            (NULL != (pDlContext->tRspBuffInfo.pBuff))) {
          memcpy((pDlContext->tRspBuffInfo.pBuff),
                 &(pInfo->pBuff[PHDNLDNFC_FRAMESTATUS_OFFSET + 1]), wPldLen);

          (pDlContext->tRspBuffInfo.wLen) = wPldLen;
        }
      } else {
        NXPLOG_FWDNLD_E("Unsuccessful Status received!!");
        wStatus = PHNFCSTVAL(CID_NFC_DNLD, NFCSTATUS_FAILED);
      }
    }
  }

  return wStatus;
}
