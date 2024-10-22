/******************************************************************************
 *
 *  Copyright 2019 NXP
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
#pragma once

/*include files*/
#include <phNfcStatus.h>
#include <phNfcTypes.h>

#define NxpMfcReaderInstance (NxpMfcReader::getInstance())

#define MAX_MFC_BUFF_SIZE 32

#define MFC_4K_BLK128 128  /*Block number 128 for Mifare 4k */
#define MFC_SECTOR_NO32 32 /* Sector 32 for Mifare 4K*/
#define MFC_BYTES_PER_BLOCK 16
#define MFC_BLKS_PER_SECTOR (0x04)

#define MFC_EXTN_ID_SIZE (0x01U)     /* Size of Mfc Req/Rsp Id */
#define MFC_EXTN_STATUS_SIZE (0x01U) /* Size of Mfc Resp Status Byte */

#define MFC_AUTHKEYLEN 0x06 /* Authentication key length */
#define MFC_AUTHENTICATION_KEY                                                 \
  (0x00U) /* Authentication key passed in extension                            \
             command header of authentication command */
#define MFC_ENABLE_KEY_B (0x80U)
#define MFC_EMBEDDED_KEY (0x10)
#define MFC_NUM_OF_KEYS (0x03U)
#define MFC_KEY_SIZE (0x06U)
#define MFC_KEYS                                                               \
  {                                                                            \
    {0xA0, 0XA1, 0xA2, 0XA3, 0xA4, 0XA5},                                      \
        {0xD3, 0XF7, 0xD3, 0XF7, 0xD3, 0XF7},                                  \
        {0xFF, 0XFF, 0xFF, 0XFF, 0xFF, 0XFF},                                  \
  } /* Key used during NDEF format */

typedef enum MifareCmdList {
  eMifareRaw = 0x00U,         /* This command performs raw transcations */
  eMifareAuthentA = 0x60U,    /* This command performs an authentication with
                                       KEY A for a sector. */
  eMifareAuthentB = 0x61U,    /* This command performs an authentication with
                                       KEY B for a sector. */
  eMifareRead16 = 0x30U,      /* Read 16 Bytes from a Mifare Standard block */
  eMifareRead = 0x30U,        /* Read Mifare Standard */
  eMifareWrite16 = 0xA0U,     /* Write 16 Bytes to a Mifare Standard block */
  eMifareWrite4 = 0xA2U,      /* Write 4 bytes. */
  eMifareInc = 0xC1U,         /* Increment */
  eMifareDec = 0xC0U,         /* Decrement */
  eMifareTransfer = 0xB0U,    /* Transfer */
  eMifareRestore = 0xC2U,     /* Restore.   */
  eMifareReadSector = 0x38U,  /* Read Sector.   */
  eMifareWriteSector = 0xA8U, /* Write Sector.   */
} MifareCmdList_t;

/*
 * Request Id for different commands
 */
typedef enum MfcCmdReqId {
  eMfRawDataXchgHdr = 0x10,   /* MF Raw Data Request from DH */
  eMfWriteNReq = 0x31,        /* MF N bytes write request from DH */
  eMfReadNReq = 0x32,         /* MF N bytes read request from DH */
  eMfSectorSelReq = 0x33,     /* MF Block select request from DH */
  eMfPlusProxCheckReq = 0x28, /* MF + Prox check request for NFCC from DH */
  eMfcAuthReq = 0x40,         /* MFC Authentication request for NFCC from DH */
  eInvalidReq                 /* Invalid ReqId */
} MfcCmdReqId_t;

/*
 * Response Ids for different command response
 */
typedef enum MfcRespId {
  eMfXchgDataRsp = 0x10,      /* DH gets Raw data from MF on successful req */
  eMfWriteNRsp = 0x31,        /* DH gets write status */
  eMfReadNRsp = 0x32,         /* DH gets N Bytes read from MF, if successful */
  eMfSectorSelRsp = 0x33,     /* DH gets the Sector Select cmd status */
  eMfPlusProxCheckRsp = 0x29, /* DH gets the MF+ Prox Check cmd status */
  eMfcAuthRsp = 0x40,         /* DH gets the authenticate cmd status */
  eInvalidRsp                 /* Invalid RspId */
} MfcRespId_t;

typedef struct MfcTagCmdIntfData {
  uint8_t byAddr;      /* Start address to perform operation*/
  uint16_t sendBufLen; /* Holds the length of the received data. */
  uint8_t sendBuf[MAX_MFC_BUFF_SIZE]; /*Holds the ack of some initial commands*/
} MfcTagCmdIntfData_t;

class NxpMfcReader {
private:
  MfcTagCmdIntfData_t mMfcTagCmdIntfData;
  void BuildMfcCmd(uint8_t *pData, uint16_t *pLength);
  void BuildAuthCmd();
  void BuildReadCmd();
  void BuildWrite16Cmd();
  void BuildRawCmd();
  void BuildIncDecCmd();
  void CalcSectorAddress();
  void AuthForWrite();
  void SendIncDecRestoreCmdPart2(const uint8_t *mfcData);

public:
  int Write(uint16_t mfcDataLen, const uint8_t *pMfcData);
  NFCSTATUS AnalyzeMfcResp(uint8_t *pBuff, uint16_t *pBufflen);
  NFCSTATUS CheckMfcResponse(uint8_t *pTransceiveData,
                             uint16_t transceiveDataLen);
  static NxpMfcReader &getInstance();
};