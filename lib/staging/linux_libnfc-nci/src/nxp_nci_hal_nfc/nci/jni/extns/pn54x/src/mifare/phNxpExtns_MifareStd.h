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

#ifndef _PHNXPEXTNS_MFCRF_H_
#define _PHNXPEXTNS_MFCRF_H_

#include <phFriNfc_MifStdFormat.h>
#include <phFriNfc_MifareStdMap.h>
#include <phNciNfcTypes.h>
#include <phNfcTypes.h>
#include <phNxpExtns.h>
#include <pthread.h>
#include <semaphore.h>

extern uint8_t current_key[];
/* Enable this macro to set key configuration for mifare classic Tag */
#define PHLIBNFC_NXPETENSION_CONFIGURE_MFKEYS 1

#define MAX_BUFF_SIZE (512)

#define PHLIBNFC_MIFARESTD4K_BLK128 128   /*Block number 128 for Mifare 4k */
#define PHLIBNFC_MIFARESTD_SECTOR_NO32 32 /* Sector 32 for Mifare 4K*/
#define PHLIBNFC_MIFARESTD_BLOCK_BYTES \
  16 /* Bytes per block after block 32 for Mifare 4K*/

#define PHLIBNFC_NO_OF_BLKPERSECTOR         \
  (0x04) /* Number of blocks per sector for \
          * Mifare Clsssic Tag*/

#define PHLIBNFC_MFCUIDLEN_INAUTHCMD \
  0x04                               /* UID length in Authentication command */
#define PHLIBNFC_MFC_AUTHKEYLEN 0x06 /* Authentication key length */

#define PHNCINFC_AUTHENTICATION_KEY                 \
  (0x00U) /* Authentication key passed in extension \
             command header of authentication command */
#define PHNCINFC_AUTHENTICATION_KEYB (0x61U) /* Authentication Key B */
#define PHNCINFC_ENABLE_KEY_B (0x80U)        /* Enable Key B */
#define PH_NCINFC_MIFARECLASSIC_EMBEDDED_KEY \
  (0x10) /* MIFARE Classic use Embedded Key*/

#define PH_NCINFC_STATUS_OK (0x0000) /* Status OK */
#define PHNCINFC_EXTNID_SIZE (0x01U) /* Size of Mifare Extension Req/Rsp Id */
#define PHNCINFC_EXTNSTATUS_SIZE \
  (0x01U) /* Size of Mifare Extension Resp Status Byte */

#define PH_NCINFC_EXTN_INVALID_PARAM_VAL \
  (0xFFU) /* Initial value of Req/Resp Param/Status */

#define PH_FRINFC_NDEF_READ_REQ (0x00U)  /* NDEF Read Request */
#define PH_FRINFC_NDEF_WRITE_REQ (0x01U) /* NDEF Write Request */

#define PH_LIBNFC_INTERNAL_CHK_NDEF_NOT_DONE \
  (0x02U) /* Status for check NDEF not done */

#define NDEF_SENDRCV_BUF_LEN 252U /* Send receive buffer length */

#define NXP_NUMBER_OF_MFC_KEYS (0x03U)
#define NXP_MFC_KEY_SIZE (0x06U)

#define NXP_MFC_KEYS                            \
  {                                             \
    {0xA0, 0XA1, 0xA2, 0XA3, 0xA4, 0XA5},       \
    {0xD3, 0XF7, 0xD3, 0XF7, 0xD3, 0XF7},       \
    {0xFF, 0XFF, 0xFF, 0XFF, 0xFF, 0XFF}        \
  } /* Key used during NDEF format */

#ifndef NCI_MAX_DATA_LEN
#define NCI_MAX_DATA_LEN 300
#endif

/*
 * Request Id for different commands
 */
typedef enum phNciNfc_ExtnReqId {
  phNciNfc_e_T1TXchgDataReq = 0x10,   /* T1T Raw Data Request from DH */
  phNciNfc_e_T1TWriteNReq = 0x20,     /* T1T N bytes write request from DH */
  phNciNfc_e_MfRawDataXchgHdr = 0x10, /* MF Raw Data Request from DH */
  phNciNfc_e_MfWriteNReq = 0x31,      /* MF N bytes write request from DH */
  phNciNfc_e_MfReadNReq = 0x32,       /* MF N bytes read request from DH */
  phNciNfc_e_MfSectorSelReq = 0x33,   /* MF Block select request from DH */
  phNciNfc_e_MfPlusProxCheckReq =
      0x28, /* MF + Prox check request for NFCC from DH */
  phNciNfc_e_MfcAuthReq =
      0x40,             /* MFC Authentication request for NFCC from DH */
  phNciNfc_e_InvalidReq /* Invalid ReqId */
} phNciNfc_ExtnReqId_t;

/*
 * Response Ids for different command response
 */
typedef enum phNciNfc_ExtnRespId {
  phNciNfc_e_T1TXchgDataRsp =
      0x10, /* DH gets Raw data from T1T on successful req */
  phNciNfc_e_T1TWriteNRsp = 0x20, /* DH gets write status */
  phNciNfc_e_MfXchgDataRsp =
      0x10, /* DH gets Raw data from MF on successful req */
  phNciNfc_e_MfWriteNRsp = 0x31, /* DH gets write status */
  phNciNfc_e_MfReadNRsp =
      0x32, /* DH gets N Bytes read from MF, if successful */
  phNciNfc_e_MfSectorSelRsp = 0x33, /* DH gets the Sector Select cmd status */
  phNciNfc_e_MfPlusProxCheckRsp =
      0x29,                     /* DH gets the MF+ Prox Check cmd status */
  phNciNfc_e_MfcAuthRsp = 0x40, /* DH gets the authenticate cmd status */
  phNciNfc_e_InvalidRsp         /* Invalid RspId */
} phNciNfc_ExtnRespId_t;

/* Data buffer */
typedef struct phNciNfc_Buff {
  uint8_t*
      pBuff; /* pointer to the buffer where received payload shall be stored*/
  uint16_t wLen; /* Buffer length*/
} phNciNfc_Buff_t, *pphNciNfc_Buff_t;

/*
 * Structure for NCI Extension information
 */
typedef struct phNciNfc_ExtnInfo {
  union {
    phNciNfc_ExtnReqId_t ExtnReqId;  /* NCI extension reqid sent */
    phNciNfc_ExtnRespId_t ExtnRspId; /* NCI extension respid received */
  } ActivExtnId;                     /* Active Req/Rsp Id */
  uint8_t bParamsNumsPresent; /* Holds number of params available for this
                                 Req/Rsp Id */
  uint8_t bParam[2]; /* Req/Res: Param[0] = Param1, Param[1] = Param2 */
} phNciNfc_ExtnInfo_t;

/*
 * NDEF related data structures
 */
typedef struct phLibNfc_NdefInfo {
  uint8_t NdefContinueRead;
  uint32_t NdefActualSize;
  uint32_t AppWrLength;
  phFriNfc_NdefMap_t* psNdefMap;
  uint16_t NdefSendRecvLen;
  phNfc_sData_t* psUpperNdefMsg;
  uint32_t dwWrLength;
  uint32_t NdefLength;
  uint8_t is_ndef;
  phFriNfc_sNdefSmtCrdFmt_t* ndef_fmt;
} phLibNfc_NdefInfo_t;

/*
 * NDEF offsets
 */
typedef enum {
  phLibNfc_Ndef_EBegin = 0x01, /**<  Start from the beginning position */
  phLibNfc_Ndef_ECurrent       /**<  Start from the current position */
} phLibNfc_Ndef_EOffset_t;

/*
 * Extns module status
 */
typedef enum { EXTNS_STATUS_OPEN = 0, EXTNS_STATUS_CLOSE } phNxpExtns_Status;

/*
 * NCI Data
 */
typedef struct nci_data_package {
  uint16_t len;
  uint8_t p_data[NCI_MAX_DATA_LEN];
} nci_rsp_data_t;

/*
 * Mifare Callback function definition
 */
typedef void (*CallBackMifare_t)(void*, uint16_t);

/*
 * Auth Cmd Data
 */
typedef struct nci_mfc_package {
  bool_t auth_status;
  bool_t auth_sent;
  sem_t semPresenceCheck;
  pthread_mutex_t syncmutex;
  NFCSTATUS status;
  phNfc_sData_t* pauth_cmd;
} phNci_mfc_auth_cmd_t;
/*
 * Structure of callback functions from different module.
 * It includes the status also.
 */
typedef struct phNxpExtns_Context {
  phNxpExtns_Status Extns_status;
  tNFA_DM_CBACK* p_dm_cback;
  tNFA_CONN_CBACK* p_conn_cback;
  tNFA_NDEF_CBACK* p_ndef_cback;
  uint8_t writecmdFlag;
  uint8_t RawWriteCallBack;
  void* CallBackCtxt;
  CallBackMifare_t CallBackMifare;
  bool_t ExtnsConnect;
  bool_t ExtnsDeactivate;
  bool_t ExtnsCallBack;
  uint8_t incrdecflag;
  uint8_t incrdecstatusflag;
} phNxpExtns_Context_t;

NFCSTATUS phFriNfc_ExtnsTransceive(phNfc_sTransceiveInfo_t* pTransceiveInfo,
                                   phNfc_uCmdList_t Cmd, uint8_t* SendRecvBuf,
                                   uint16_t SendLength,
                                   uint16_t* SendRecvLength);
NFCSTATUS phNxpExtns_MfcModuleInit(void);
NFCSTATUS phNxpExtns_MfcModuleDeInit(void);
NFCSTATUS Mfc_WriteNdef(uint8_t* p_data, uint32_t len);
NFCSTATUS Mfc_CheckNdef(void);
NFCSTATUS Mfc_ReadNdef(void);
NFCSTATUS Mfc_FormatNdef(uint8_t* secretkey, uint8_t len);
NFCSTATUS Mfc_Transceive(uint8_t* p_data, uint32_t len);
NFCSTATUS Mfc_SetReadOnly(uint8_t* secrtkey, uint8_t len);
void Mfc_DeactivateCbackSelect(void);
void Mfc_ActivateCback(void);
NFCSTATUS Mfc_RecvPacket(uint8_t* buff, uint8_t buffSz);
NFCSTATUS phNxNciExtns_MifareStd_Reconnect(void);
NFCSTATUS Mfc_PresenceCheck(void);

#endif /* _PHNXPEXTNS_MFCRF_H_ */
