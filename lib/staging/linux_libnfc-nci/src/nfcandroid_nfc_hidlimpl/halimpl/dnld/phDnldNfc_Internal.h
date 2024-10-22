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
 * Internal Primitives (Functions + Variables) used for Firmware Download
 */
#ifndef PHDNLDNFC_INTERNAL_H
#define PHDNLDNFC_INTERNAL_H

#include <phDnldNfc.h>
#include <phDnldNfc_Cmd.h>
#include <phDnldNfc_Status.h>

#define PHDNLDNFC_CMDRESP_MAX_BUFF_SIZE                 \
  (0x100U) /* DL Host Frame Buffer Size for all CMD/RSP \
                except pipelined WRITE */
#if (PHDNLDNFC_CMDRESP_MAX_BUFF_SIZE > PHNFC_I2C_FRAGMENT_SIZE)
#undef PHDNLDNFC_CMDRESP_MAX_BUFF_SIZE
#define PHDNLDNFC_CMDRESP_MAX_BUFF_SIZE (PHNFC_I2C_FRAGMENT_SIZE)
#endif

/* DL Host Short Frame Buffer Size for pipelined WRITE RSP */
#define PHDNLDNFC_WRITERSP_BUFF_SIZE (0x08U)

/* DL Host Frame Buffer Header Length */
#define PHDNLDNFC_FRAME_HDR_LEN (0x02U)
/* DL Host Frame Buffer CRC Length */
#define PHDNLDNFC_FRAME_CRC_LEN (PHDNLDNFC_FRAME_HDR_LEN)
#define PHDNLDNFC_FRAME_ID_LEN (0x01U) /* Length of Cmd Id */

/* size of EEPROM/Flash address */
#define PHDNLDNFC_EEFL_ADDR_SIZE (0x03U)
/* 2 Byte size of data */
#define PHDNLDNFC_DATA_SIZE (PHDNLDNFC_FRAME_HDR_LEN)

/* Start of EEPROM address for log */
#define PHDNLDNFC_EEPROM_LOG_START_ADDR (0x201F80U)
/* End of EEPROM address for log */
#define PHDNLDNFC_EEPROM_LOG_END_ADDR (0x201FBFU)

#define PHDNLDNFC_MAX_LOG_SIZE \
  ((PHDNLDNFC_EEPROM_LOG_END_ADDR - PHDNLDNFC_EEPROM_LOG_START_ADDR) + 1)

/* DL Max Payload Size */
#define PHDNLDNFC_CMDRESP_MAX_PLD_SIZE \
  ((PHDNLDNFC_CMDRESP_MAX_BUFF_SIZE) - \
   (PHDNLDNFC_FRAME_HDR_LEN + PHDNLDNFC_FRAME_CRC_LEN))

/*
 * Enum definition contains Download Event Types
 */
typedef enum phDnldNfc_Event {
  phDnldNfc_EventInvalid = 0x00, /*Invalid Event Value*/
  phDnldNfc_EventReset,          /* Reset event */
  phDnldNfc_EventGetVer,         /* Get Version event*/
  phDnldNfc_EventWrite,          /* Write event*/
  phDnldNfc_EventRead,           /* Read event*/
  phDnldNfc_EventIntegChk,       /* Integrity Check event*/
  phDnldNfc_EventGetSesnSt,      /* Get Session State event*/
  phDnldNfc_EventLog,            /* Log event*/
  phDnldNfc_EventForce,          /* Force event*/
  phDnldNfc_EventRaw, /* Raw Req/Rsp event,used currently for sending NCI RESET
                         cmd */
  phDnldNfc_EVENT_INT_MAX /* Max Event Count*/
} phDnldNfc_Event_t;

/*
 * Enum definition contains Download Handler states for each event requested
 */
typedef enum phDnldNfc_State {
  phDnldNfc_StateInit = 0x00, /* Handler init state */
  phDnldNfc_StateSend,        /* Send frame to NFCC state */
  phDnldNfc_StateRecv,        /* Recv Send complete State */
  phDnldNfc_StateTimer, /* State to stop prev set timer on Recv or handle timed
                           out scenario */
  phDnldNfc_StateResponse,  /* Process response from NFCC state */
  phDnldNfc_StatePipelined, /* Write requests to be pipelined state */
  phDnldNfc_StateInvalid    /* Invalid Handler state */
} phDnldNfc_State_t;

/*
 * Enum definition contains Download Handler Transition
 */
typedef enum phDnldNfc_Transition {
  phDnldNfc_TransitionIdle =
      0x00, /* Handler in Idle state - No Download in progress */
  phDnldNfc_TransitionBusy,   /* Handler is busy processing download request */
  phDnldNfc_TransitionInvalid /* Invalid Handler Transition */
} phDnldNfc_Transition_t;

/*
 * Enum definition contains the Frame input type for CmdId in process
 */
typedef enum {
  phDnldNfc_FTNone = 0, /* input type None */
  phDnldNfc_ChkIntg, /* user eeprom offset & len to be added for Check Integrity
                        Request */
  phDnldNfc_FTWrite, /* Frame inputs for Write request */
  phDnldNfc_FTLog,   /* Frame inputs for Log request */
  phDnldNfc_FTForce, /* Frame input for Force cmd request */
  phDnldNfc_FTRead,  /* Addr input required for read request */
  phDnldNfc_FTRaw    /* Raw Req/Rsp type */
} phDnldNfc_FrameInputType_t;

/*
 * Enum definition contains Firmware file format
 */
typedef enum phDnldNfc_FwFormat {
  FW_FORMAT_UNKNOWN = 0x00,
  FW_FORMAT_SO = 0x01,
  FW_FORMAT_BIN = 0x02,
} phDnldNfc_FwFormat_t;

/*
 * Contains Host Frame Buffer information.
 */
typedef struct phDnldNfc_FrameInfo {
  uint16_t dwSendlength; /* length of the payload  */
  uint8_t
      aFrameBuff[PHDNLDNFC_CMDRESP_MAX_BUFF_SIZE]; /* Buffer to store command
                                                      that needs to be sent*/
} phDnldNfc_FrameInfo_t,
    *pphDnldNfc_FrameInfo_t; /* pointer to #phDnldNfc_FrameInfo_t */

/*
 * Frame Input Type & Value for CmdId in Process
 */
typedef struct phDnldNfc_FrameInput {
  phDnldNfc_FrameInputType_t
      Type;        /* Type of frame input required for current cmd in process */
  uint32_t dwAddr; /* Address value required for Read/Write Cmd*/
} phDnldNfc_FrameInput_t,
    *pphDnldNfc_FrameInput_t; /* pointer to #phDnldNfc_FrameInput_t */

/*
 * Context for the response timeout
 */
typedef struct phDnldNfc_RspTimerInfo {
  uint32_t dwRspTimerId;     /* Timer for Core to handle response */
  uint8_t TimerStatus;       /* 0 = Timer not running 1 = timer running*/
  NFCSTATUS wTimerExpStatus; /* Holds the status code on timer expiry */
} phDnldNfc_RspTimerInfo_t;

/*
 * Read/Write Processing Info
 */
typedef struct phDnldNfc_RWInfo {
  uint32_t dwAddr;    /* current Addr updated for read/write */
  uint16_t wOffset;   /* current offset within the user buffer to read/write */
  uint16_t wRemBytes; /* Remaining bytes to read/write */
  uint16_t wRemChunkBytes; /* Remaining bytes within the chunked frame */
  uint16_t wRWPldSize;     /* Size of the read/write payload per transaction */
  uint16_t wBytesToSendRecv; /* Num of Bytes being written/read currently */
  uint16_t wBytesRead;       /* Bytes read from read cmd currently */
  bool_t bFramesSegmented;   /* Flag to indicate if Read/Write frames are
                                segmented */
  bool_t bFirstWrReq; /* Flag to indicate if this is the first write frame being
                         sent */
  bool_t
      bFirstChunkResp; /* Flag to indicate if we got the first chunk response */
} phDnldNfc_RWInfo_t, *pphDnldNfc_RWInfo_t; /* pointer to #phDnldNfc_RWInfo_t */

/*
 * Download context structure
 */
typedef struct phDnldNfc_DlContext {
  const uint8_t* nxp_nfc_fw; /* Pointer to firmware version from image */
  const uint8_t*
      nxp_nfc_fwp; /* Pointer to firmware version from get_version cmd */
  uint16_t nxp_nfc_fwp_len; /* Length of firmware image length */
  uint16_t nxp_nfc_fw_len;  /* Firmware image length */
  bool_t bResendLastFrame;  /* Flag to resend the last write frame after MEM_BSY
                               status */
  phDnldNfc_Transition_t
      tDnldInProgress; /* Flag to indicate if download request is ongoing */
  phDnldNfc_Event_t tCurrEvent; /* Current event being processed */
  phDnldNfc_State_t tCurrState; /* Current state being processed */
  pphDnldNfc_RspCb_t UserCb;    /* Upper layer call back function */
  void* UserCtxt;               /* Pointer to upper layer context */
  phDnldNfc_Buff_t tUserData;   /* Data buffer provided by caller */
  phDnldNfc_Buff_t
      tRspBuffInfo; /* Buffer to store payload field of the received response*/
  phDnldNfc_FrameInfo_t tCmdRspFrameInfo; /* Buffer to hold the cmd/resp frame
                                             except pipeline write */
  phDnldNfc_FrameInfo_t
      tPipeLineWrFrameInfo; /* Buffer to hold the pipelined write frame */
  NFCSTATUS
  wCmdSendStatus; /* Holds the status of cmd request made to cmd handler */
  phDnldNfc_CmdId_t tCmdId; /* Cmd Id of the currently processed cmd */
  phDnldNfc_FrameInput_t
      FrameInp; /* input value required for current cmd in process */
  phDnldNfc_RspTimerInfo_t
      TimerInfo;              /* Timer context handled into download context*/
  phDnldNfc_Buff_t tTKey;     /* Defualt Transport Key provided by caller */
  phDnldNfc_RWInfo_t tRWInfo; /* Read/Write segmented frame info */
  phDnldNfc_Status_t tLastStatus; /* saved status to distinguish signature or
                                     pltform recovery */
  phDnldNfc_FwFormat_t FwFormat;  /*FW file format either lib or bin*/
} phDnldNfc_DlContext_t,
    *pphDnldNfc_DlContext_t; /* pointer to #phDnldNfc_DlContext_t structure */

/* The phDnldNfc_CmdHandler function declaration */
extern NFCSTATUS phDnldNfc_CmdHandler(void* pContext,
                                      phDnldNfc_Event_t TrigEvent);

#endif /* PHDNLDNFC_INTERNAL_H */
